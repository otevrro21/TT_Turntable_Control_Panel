//* LIBRARIES: ---------------------------------------------------------------------------------

#include <Arduino.h>

#include <LcdMenu.h>
#include <MenuScreen.h>
#include <MenuItem.h>
#include <ItemCommand.h>
#include <ItemValue.h>
#include <ItemSubMenu.h>
#include <widget/WidgetRange.h>

#include <display/LiquidCrystal_I2CAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>
#include <input/SimpleRotaryAdapter.h>

#include <SimpleRotary.h>
#include <LiquidCrystal_I2C.h>

//* PINS: --------------------------------------------------------------------------------------

#define BTN_1 2
#define BTN_2 3
#define ENC_1 5
#define ENC_2 4

//! GLOBAL VARIABLES: --------------------------------------------------------------------------

int currentBridgePosition = 0; // Current position of the bridge (side A)
int futurePosition = 0; // Future position of the turntable set by the user
bool estopSwitch = false; // for sending stop message to the turntable if interrupt is triggered
char turntableStatus[20] = "Waiting"; // Status of the turntable, cycles between 'Waiting' and 'Turning'

unsigned long previousTime = 0;
unsigned long currentTime = millis();

//* FUNCTION PROTOTYPES: -----------------------------------------------------------------------

void incomingSerial(), switchScreen(int screen), estop();

//* OBJECTS: -----------------------------------------------------------------------------------

LiquidCrystal_I2C lcd(0x27, 20, 4);
SimpleRotary rotary(ENC_1, ENC_2, BTN_2);

LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, 20, 4);
LcdMenu menu(renderer);
SimpleRotaryAdapter knob(&menu, &rotary);

//* SCREENS: -----------------------------------------------------------------------------------
MENU_SCREEN(rotationScreen, rotationItems,
    ITEM_BASIC("Rotate the turntable?"),
    ITEM_COMMAND("Yes", [](){
        Serial.print(String("P") + futurePosition);
        strcpy(turntableStatus, "Turning");
        switchScreen(0);
        }),
    ITEM_COMMAND("No", [](){switchScreen(0);})
);

MENU_SCREEN(selectionScreen, selectionItems,
    ITEM_BASIC("Select position"),
    ITEM_COMMAND("Position 1", [](){futurePosition = 0; menu.setScreen(rotationScreen);}),
    ITEM_COMMAND("Position 2", [](){futurePosition = 1; menu.setScreen(rotationScreen);}),
    ITEM_COMMAND("Position 3", [](){futurePosition = 2; menu.setScreen(rotationScreen);}),
    ITEM_COMMAND("Position 4", [](){futurePosition = 3; menu.setScreen(rotationScreen);}),
    ITEM_COMMAND("Position 5", [](){futurePosition = 4; menu.setScreen(rotationScreen);}),
    ITEM_COMMAND("Position 6", [](){futurePosition = 5; menu.setScreen(rotationScreen);}),
    ITEM_COMMAND("Position 7", [](){futurePosition = 6; menu.setScreen(rotationScreen);}),
    ITEM_COMMAND("Position 8", [](){futurePosition = 7; menu.setScreen(rotationScreen);}),
    ITEM_COMMAND("Position 9", [](){futurePosition = 8; menu.setScreen(rotationScreen);})
);

MENU_SCREEN(mainScreen, mainItems,
    ITEM_VALUE("Position", currentBridgePosition, "%d"),
    ITEM_VALUE("Status", turntableStatus, "%s"),
    ITEM_SUBMENU("Select position", selectionScreen),
    ITEM_COMMAND("Home", [](){switchScreen(3);})
);

MENU_SCREEN(homePromptScreen, homePromptItems,
    ITEM_BASIC("Home the turntable?"),
    ITEM_COMMAND("Yes", [](){
        Serial.print('H');
        strcpy(turntableStatus, "Homing");
        menu.setScreen(mainScreen);
        }),
    ITEM_COMMAND("No", [](){menu.setScreen(mainScreen);})
);

MENU_SCREEN(stopScreen, stopItems,
    ITEM_BASIC("E-STOP TRIGGERED!"),
    ITEM_COMMAND("Reset", [](){
        Serial.print('R');
        strcpy(turntableStatus, "Waiting");
        menu.setScreen(mainScreen);
    }),
    ITEM_COMMAND("Continue", [](){
        Serial.print('C');
        strcpy(turntableStatus, "Waiting");
        menu.setScreen(mainScreen);
    })
);

//* MAIN CODE: ---------------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);

    pinMode(BTN_1, INPUT);
    attachInterrupt(digitalPinToInterrupt(BTN_1), estop, FALLING);

    renderer.begin();
    menu.setScreen(homePromptScreen);
}

void loop() {
    currentTime = millis();
    knob.observe();
    menu.poll();
    incomingSerial();
    if (estopSwitch) {
        if (currentTime - previousTime > 500) {
            previousTime = currentTime;
            Serial.print('S');
        }
        strcpy(turntableStatus, "Stopped");
        estopSwitch = false;
        menu.setScreen(stopScreen);
    }
}

void incomingSerial() {
    if (Serial.available() > 0) {
        char command = Serial.read();
        if (command == 'H') {
            strcpy(turntableStatus, "Home");
        } else if (command == 'P') {
            // Wait for the second character
            delay(10); // Small delay to ensure next character arrives (im so sorry)
            if (Serial.available() > 0) {
                char posChar = Serial.read();
                int pos = posChar - '0';
                if (pos >= 0 && pos <= 8) {
                    currentBridgePosition = pos + 1; // + 1 for better user readability (1-9 instead of 0-8)
                    strcpy(turntableStatus, "Waiting");
                }
            }
        }
    }
}

void switchScreen(int screen) {
    switch (screen) {
        case 0:
            menu.setScreen(mainScreen);
            break;
        case 1:
            menu.setScreen(rotationScreen);
            break;
        case 2:
            menu.setScreen(selectionScreen);
            break;
        case 3:
            menu.setScreen(homePromptScreen);
            break;
        default:
            break;
    }
}

void estop() {
    estopSwitch = true;
}