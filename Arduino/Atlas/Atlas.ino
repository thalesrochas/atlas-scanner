#include <Wire.h>
#include <LIDARLite.h>
#include <Stepper.h>
#include <SPI.h>
#include <SD.h>

//* Define o pino do motor.
const byte MOTOR_PIN = 2;

//* Define o pino do encoder do motor.
const byte ENCODER_PIN = 3;

//* Define o pino do endstop do motor.
const byte ENDSTOP_PIN = 4;

//* Define o pino CS do leitor de cartão.
const byte CS_PIN = 10;

// Define a quantidade máxima de passos que o motor pode girar.
const int STEP_LIMIT = 355;

/**
 * Define a quantidade de ticks do encoder por volta.
 * Valor aproximado obtido através de testes.
 * Necessita fazer uma compensação via código para alinhas os
 * pontos a cada volta.
 * ? Feito! Ver interrupção.
 */
const int TICKS_PER_TURN = 2245;

/**
 * Armazena quantos ticks já ocorreram na volta atual.
 * Utilizado no cálculo do theta.
 */
int ticks = 0;

/**
 * * Conexões do Leitor de Cartão:
 * *    CS: 10 -> Pode ser alterado na variável CS_PIN
 * *    SCK: 13
 * *    MOSI: 11
 * *    MISO: 12
 */

/**
 * Cria o objeto do sensor LIDAR.
 * 
 * * Conexões:
 * *    5Vdc (Vermelho): 5V
 * *    I2C SCL (Verde): SCL -> A5 (Arduino Pro Mini)
 * *    I2C SDA (Azul): SDA -> A4 (Arduino Pro Mini)
 * *    GND (Preto): GND
 * *    Capacitor de 1000 uF na entrada do Arduino (para evitar correntes de pico)
 */
LIDARLite lidar;

/**
 * Cria o objeto do motor de passo responsável por inclinar o sensor
 * verticalmente.
 * 
 * Parâmetros:
 *      400 -> Quantidade de passos por volta.
 * *    6, 7, 8 e 9 -> Portas do arduino em que o motor está ligado.
 * *        Vermelho, Amarelo, Laranja, Preto.
 */
Stepper stepper = Stepper(400, 6, 7, 8, 9);

void setup() {
    Serial.begin(38400); // Inicializa a porta Serial.
    Serial.println("Porta Serial inicializada.\n");

    Serial.println("Inicializando cartão SD...");
    if (!SD.begin(CS_PIN)) {
        Serial.println("Falha no cartão.\n");
        while(true);
    }
    else {
        Serial.println("Cartão SD Inicializado.\n");
    }

    /**
     * Configuração do motor.
     * Inicializa o pino como saída.
     * Seta a saída como desligada.
     */
    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(MOTOR_PIN, LOW);

    /**
     * Inicializa um pino de interrupção para o Encoder do motor.
     * A interrupção acontece toda vez que o estado do pino 3 muda -> CHANGE.
     * Cada vez que uma interrupção acontecer, é disparada a função interrupt.
     */
    pinMode(ENCODER_PIN, INPUT); // Define como entrada
    attachInterrupt(digitalPinToInterrupt(ENCODER_PIN), interrupt, CHANGE);
    //// detachInterrupt(digitalPinToInterrupt(ENCODER_PIN));

    /**
     * O método begin inicializa e define a configuração do sensor.
     * 
     * @param configuration Configuração do sensor, podendo variar de 0 a 5,
     * como segue (disponível na documentação do sensor):
     *      0: Default mode, balanced performance.
     *      1: Short range, high speed. Uses 0x1d maximum acquisition count.
     *      2: Default range, higher speed short range. Turns on quick
     *         termination detection for faster measurements at short range
     *         (with decreased accuracy).
     *      3: Maximum range. Uses 0xff maximum acquisition count.
     *      4: High sensitivity detection. Overrides default valid measurement
     *         detection algorithm, and uses a threshold value for high
     *         sensitivity and noise.
     *      5: Low sensitivity detection. Overrides default valid measurement
     *         detection algorithm, and uses a threshold value for low
     *         sensitivity and noise.
     * 
     * @param fasti2c Altera a velocidade padrão de comunicação I2C:
     *      false: 100khz -> Default.
     *      true: 400khz.
     */
    lidar.begin(0, true);
    Serial.println("Sensor LIDAR inicializado.\n");

    pinMode(ENDSTOP_PIN, INPUT); // Define o endstop como entrada
    stepper.setSpeed(80); // Configura o motor para velocidade de 80rpm.
}

// Fator de correção de giro. 14.95 me pareceu o melhor depois de vários testes.
float factor = 14.95;
float acumulator = 0.0; // Usado para compensar o valor de theta.
int turns = 0; // Usado para saber a volta atual.
int J = 0; // J = 1 já é suficiente para fazer uma boa medida.

void loop() {
    Serial.println("Nova medição");

    // Reinicia as variáveis
    factor = 14.95;
    acumulator = 0.0;
    turns = 0;
    J = 0;

    // Leva o suporte ao início até o endstop ser ativado.
    //? Há uma negação (!) na expressão devido a lógica invertida do botão.
    while (!digitalRead(ENDSTOP_PIN)) {
        // O sinal negativo representa a direção contrária do giro.
        stepper.step(-1);
    }
    //// delay(500);
    //// stepper.step(STEP_LIMIT); // Leva o sensor até a posição inicial de leitura.

    //! Recebe pela porta serial o valor do fator de correção.
    // Serial.print("Fator de correção: ");
    // while (factor == 0.0) {
    //     if (Serial.available()) {
    //         factor = Serial.readString().toFloat();
    //     }
    // }

    //! Recebe pela porta serial o valor de J.
    Serial.print("J: ");
    while (J == 0.0) {
        if (Serial.available()) {
            J = Serial.readString().toInt();
        }
    }

    // Recebe pela porta serial o nome do arquivo de pontos.
    String filename = "";
    Serial.print("Filename: ");
    while (filename == "") {
        if (Serial.available()) {
            filename = Serial.readString();
            filename += ".txt";
        }
    }

    // Inicializa um novo arquivo para armazenar os pontos
    Serial.println("Inicializando arquivo " + filename + "...");
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
        Serial.println("Erro ao criar o arquivo " + filename + "\n");
        while(true);
    }
    
    Serial.println("Iniciando motor...");
    digitalWrite(MOTOR_PIN, HIGH);
    delay(2000); // Delay para aguardar que o motor pegue velocidade.

    Serial.println("Iniciando captura de pontos...");

    /**
     * Captura as distâncias em coordenadas esféricas e insere os
     * dados no arquivo.
     *      0 <=rho < 4000
     *      0 <= theta < 360 (2pi)
     *      0 <= phi <= 90 (pi/2) -> Iniciando no 90, em direção ao zero.
     */

    int rho;
    float theta;
    float phi;

    // Laço que determina o movimento do sensor no sentido de phi
    for (int step = 0; step < STEP_LIMIT; step++) {
        // Laço que determina o movimento do sensor no sentido de theta
        // 3 repetições de 100 medidas. Aproximadamente pouco mais de uma volta.
        for (byte j = 0; j < J; j++) {
            //// Serial.print("Rodada ");
            //// Serial.println(j+1);

            /**
             * O método distance dispara o sensor e realiza uma medição.
             * 
             * @param biasCorrection Correção de polarização do receptor.
             *      false: Medição mais rápida, sem correção.
             *      true: Medição com a correção de polarização. Deve ser realizada
             *            periodicamente (1 em cada 100 medições). -> Default
             */
            rho = lidar.distance();

            // Regra de 3 - acumulador
            theta = float(ticks) * (360.0 / float(TICKS_PER_TURN - 1)) - acumulator;
            // theta pode ficar negativo devido a subtração com o acumulador, dessa forma,
            // remapeia-se o valor de theta para que seu valor sempre seja 0 <= theta < 360.
            theta = theta < 0.0 ? theta + 360.0 : theta;
            
            // O valor calculado é subtraído de 90 para que o ângulo inicie em
            // 90 e chegue até 0.
            phi = 90.0 - (float(step) * (90.0 / float(STEP_LIMIT - 1))); // Regra de 3

            file.print(rho);
            file.print("\t");
            file.print(theta);
            file.print("\t");
            file.println(phi);

            for (byte i = 0; i < 99; i++) {
                rho = lidar.distance(false);
                theta = float(ticks) * (360.0 / float(TICKS_PER_TURN - 1)) - acumulator;
                theta = theta < 0.0 ? theta + 360.0 : theta;
                // theta = float(ticks) * (344.0 / float(TICKS_PER_TURN - 1));
                phi = 90.0 - (float(step) * (90.0 / float(STEP_LIMIT - 1)));

                file.print(rho);
                file.print("\t");
                file.print(theta);
                file.print("\t");
                file.println(phi);
            }
            file.flush(); // Descarrega os dados do buffer no arquivo.
        }
        //// Serial.print("Steps ");
        //// Serial.print(step);
        //// Serial.print("\tGraus ");
        //// Serial.println(phi);
        
        // Aplica um passo ao motor a cada iteração para movimentar o sensor.
        stepper.step(1);
    }
    
    file.close(); // Fecha o acesso ao arquivo
    Serial.println("Fim da captura de pontos.\n");

    Serial.println("Motor desligado.\n");
    digitalWrite(MOTOR_PIN, LOW);
}

void interrupt() {
    ticks++;
    ticks %= TICKS_PER_TURN;
    
    // Se ticks == 0, então é uma nova volta.
    turns = ticks == 0 ? turns + 1 : turns;

    // Calcula um acumulador para remover o erro de deslocamento para cada volta.
    // O Fator de correção é quem faz o ajuste do erro.
    acumulator = factor * (float(ticks) / float(TICKS_PER_TURN) + float(turns));
    
    // Acumulador não deve ser maior que 360.
    // Como o acumulador vai subtrair o angulo atual, é interessante que
    // ele esteja dentro do limite 0 <= acumulador < 360.
    acumulator = acumulator >= 360.0 ? acumulator - 360.0 : acumulator;
}