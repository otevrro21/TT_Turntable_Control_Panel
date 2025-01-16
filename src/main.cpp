//* LIBRARIES AND HEADERS

#include <Arduino.h>

#include <LcdMenu.h>
#include <MenuScreen.h>
#include <MenuItem.h>
#include <ItemCommand.h>
#include <ItemValue.h>
#include <ItemWidget.h>
#include <widget/WidgetRange.h>

#include <display/LiquidCrystal_I2CAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>
#include <input/SimpleRotaryAdapter.h>

#include <SimpleRotary.h>
#include <LiquidCrystal_I2C.h>

//* PINS

#define BTN_1 2
#define BTN_2 3
#define ENC_1 5
#define ENC_2 4

//* GLOBAL VARIABLES

int turntablePosition = 0;

//* OBJECTS

LiquidCrystal_I2C lcd(0x27, 20, 4);
SimpleRotary rotary(ENC_1, ENC_2, BTN_2);

LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, 20, 4);
LcdMenu menu(renderer);
SimpleRotaryAdapter knob(&menu, &rotary);

//* SCREENS

MENU_SCREEN(mainScreen, mainItems,
    ITEM_VALUE("Pozice", x, "%d"),
    ITEM_COMMAND("increment", [](){x++;}),
    ITEM_COMMAND("decrement", [](){x--;}),
    ITEM_COMMAND("SEND A", [](){Serial.print("A");}),
    ITEM_COMMAND("SEND B", [](){Serial.print("B");})
    );

//* FUNCTIONS


void setup()
{
    Serial.begin(115200);
    renderer.begin();
    menu.setScreen(mainScreen);
}

void loop()
{
   knob.observe();
   menu.poll();
   if (Serial.available() > 0)
   {
       char incomingByte = Serial.read();
   }
}