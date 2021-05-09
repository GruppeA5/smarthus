#include "Arduino.h"
#include "LCD_Keypad_Menu.h"
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

//lcd er udefinert uten LiquidCrystal_I2C lcd et sted i headeren?
//kan print være alias for print?

LCD_Keypad_Menu::LCD_Keypad_Menu() { //LCD_Keypad_Menu-objektet er egentlig bare et LiquidCrystal_I2C-objekt 
 LiquidCrystal_I2C lcd(0x27, 16, 2);
}

void LCD_Keypad_Menu::resettingKeys3(char _keyType1, char _keyType2, char _keyType3) {
   _keyType1 = 0;
  _keyType2 = 0;
  _keyType3 = 0;
}

void LCD_Keypad_Menu::resettingKeys4(char _keyType1, char _keyType2, char _keyType3, char _keyType4) { 
  _keyType1 = 0;
  _keyType2 = 0;
  _keyType3 = 0;
  _keyType4 = 0;

}

void LCD_Keypad_Menu::resettingKeys2(char _keyType1, char _keyType2) {
  _keyType1 = 0;
  _keyType2 = 0;
}

void LCD_Keypad_Menu::scrollingText(char _message[]) {
  for (int i = 0; i < _message.length(); i++) {
    lcd.clear();
    for (int j = 0; j < 16; j++) { //setter 16 der
      lcd.setCursor(j, 1);
      if (i + j >= _message.length()) {
        lcd.printstr(" ");
      }
      lcd.printstr(_message[i + j]);
    }
    delay(250);
  }

}

void LCD_Keypad_Menu::oneStaticLine(const char _message[]) {
  lcd.setCursor(0, 0);
  lcd.printstr(_message);
  delay(250);
}

void LCD_Keypad_Menu::twoStaticLines(const char _message1[], const char _message2[]) {
  lcd.setCursor(0, 0);
  lcd.printstr(_message1);
  lcd.setCursor(0, 1);
  lcd.printstr(_message2);
  delay(250);
}



