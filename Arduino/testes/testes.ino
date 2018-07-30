#include <Arduino.h>
//#include <Stepper.h>

//Stepper myStepper = Stepper(400, 8, 9, 10, 11);

const int ENDSTOP = 4;
const int MOTOR = 2;
const int ENCODER = 3;
const int LASER = 13;

int cont = 0;
int ticks;
bool change = false;
int times = 0;
int numero = 4;

void setup() {
    pinMode(ENDSTOP, INPUT); // Entrada do Endstop
    pinMode(MOTOR, OUTPUT);
    digitalWrite(MOTOR, LOW);

    ticks = 2245; // 2245 = 1 volta

    pinMode(ENCODER, INPUT);
    attachInterrupt(digitalPinToInterrupt(3), pulso, CHANGE);

    pinMode(LASER, OUTPUT);
    digitalWrite(LASER, LOW);

    Serial.begin(38400);
/*
    //myStepper.setSpeed(80);

    while (!digitalRead(ENDSTOP)) {
        myStepper.step(1);
    }

    //delay(1000);
    //myStepper.step(-355);
    //delay(1000);
    */
}

void loop() {
    if (Serial.available()) {
        numero = Serial.readString().toInt();
        Serial.print("Mudado para ");
        Serial.println(numero);
    }

    if (digitalRead(ENDSTOP)) {
        digitalWrite(MOTOR, HIGH);
    }
    else {
        digitalWrite(MOTOR, LOW);
    }
}

void pulso() {
    cont++;
    cont = cont % ticks;
    if (cont == 0) {
        digitalWrite(LASER, HIGH);
        delay(50);
        digitalWrite(LASER, LOW);
        
        /*if (ticks == 2246 && change) {
            ticks = 2245;
            Serial.println("Mudado para 2245");
        }
        else {
            change = !change;
            ticks = 2246;
            Serial.println("Mudado para 2246");
        }*/

        times = times % numero;
        if (times == 0) {
            ticks = 2246;
            Serial.println("Mudado para 2246");
        }
        else {
            ticks = 2245;
            Serial.println("Mudado para 2245");
        }
        times++;
    }
    //Serial.println(cont);
}