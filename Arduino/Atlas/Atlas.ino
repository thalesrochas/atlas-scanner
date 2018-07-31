#include <Wire.h>
#include <LIDARLite.h>
#include <Stepper.h>

/**
 * Cria o objeto do sensor LIDAR.
 * 
 * * Conexões:
 * *    5Vdc (Vermelho): 5V
 * *    I2C SCL (Verde): SCL -> A5 (Arduino Pro Mini)
 * *    I2C SDA (Azul): SDA -> A4 (Arduino Pro Mini)
 * *    GND (Preto): GND
 * *    Capacitor na entrada do Arduino
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

//* Define a porta em que o endstop do motor de passo está ligada.
const int ENDSTOP = 4;

// Define a quantidade máxima de passos que o motor pode girar.
const int STEP_LIMIT = 355;

void setup() {
    Serial.begin(38400);

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

    pinMode(ENDSTOP, INPUT); // Define o endstop como entrada
    stepper.setSpeed(80); // Configura o motor para velocidade de 80rpm.

    // Leva o suporte ao início até o endstop ser ativado.
    //? Há uma negação (!) na expressão devido a lógica invertida do botão.
    while (!digitalRead(ENDSTOP)) {
        // O sinal negativo representa a direção contrária do giro.
        stepper.step(-1);
    }
    //// delay(500);
    //// stepper.step(STEP_LIMIT); // Leva o sensor até a posição inicial de leitura.
}

int distancia = 0;

void loop() {
    /**
     * O método distance dispara o sensor e realiza uma medição.
     * 
     * @param biasCorrection Correção de polarização do receptor.
     *      false: Medição mais rápida, sem correção.
     *      true: Medição com a correção de polarização. Deve ser realizada
     *            periodicamente (1 em cada 100 medições). -> Default
    */
    distancia = lidar.distance();
    Serial.println(distancia);

    for(int i = 0; i < 99; i++) {
        distancia = lidar.distance(false);
        Serial.println(distancia);
    }
}