//* LIBRARIES

#include <Arduino.h>
#include <SimpleRotary.h>
#include <LiquidCrystal_I2C.h>

//* PINS

#define BTN_1 2
#define BTN_2 3
#define ENC_1 5
#define ENC_2 4

//* OBJECTS

LiquidCrystal_I2C display(0x27, 20, 4);
SimpleRotary knob(ENC_1, ENC_2, BTN_2);

//* FUNCTIONS



//* GLOBAL VARIABLES

int ctr = 0;
int x = 0;

void setup() {
    Serial.begin(115200);

    knob.setTrigger(LOW);
    display.init();
    display.backlight();
    display.print(ctr);
}

void loop() {
    int r = knob.rotate();
    if (r == 1) {
        ctr++;
        display.clear();
        display.print(ctr);
    }
    if (r == 2) {
        ctr--;
        display.clear();
        display.print(ctr);
    }
    if (knob.push() == 1) {
        display.clear();
        display.print("Pressed");
        switch (x) {
        case 0:
            Serial.print('A');
            x = 1;
            break;
        case 1:
            Serial.print('B');
            x = 0;
            break;
        }
        delay(1000);
        display.clear();
        display.print(ctr);
    }
}