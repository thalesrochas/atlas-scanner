#include <Stepper.h>

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

void loop() {
    
}