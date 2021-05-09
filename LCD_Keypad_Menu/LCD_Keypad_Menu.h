#ifndef LED_Keypad_Menu_h
#define LCD_Keypad_Menu_h

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

class LCD_Keypad_Menu {
private: //deklarer variabler selv i egen sketch fordi de er private
/*char _keyType1;
char _keyType2;
char _keyType3;
char _keyType4;
const char _message;
const char _message1;
const char _message2;*/


public: //ikke skrive inn variablene her, men kun typen?

LiquidCrystal_I2C lcd(0x27, 16, 2);
LCD_Keypad_Menu::LCD_Keypad_Menu(LiquidCrystal_I2C); //hvor skal objekter?

void scrollingText(char[]);
void resettingKeys2(char, char);
void resettingKeys3(char, char, char);
void resettingKeys4(char, char, char, char);
void oneStaticLine(const char[]);
void twoStaticLines(const char[], const char[]);
}; 
#endif