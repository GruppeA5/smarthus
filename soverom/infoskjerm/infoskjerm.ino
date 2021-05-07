
//////////////----------------------INFOSKJERM---------------------------------------////////////////////
////----------------------------------------------------------------------///

//----BIBLIOTEK----//

#include <Keypad.h> //tar med biblioteket for 4x3 keypaden
#include <LiquidCrystal_I2C.h>  //biblioteket til LCD-en
#include <CircusESP32Lib.h> //inkluderer CoT-biblioteket til ESP32



char ssid [] = "Elines iphone"; //WIFI SSID (eline)
char password[] = "passordet"; //WIFI passord 

char server[] = "www.circusofthings.com";


//----------------------------------------------------------------------------------------------------------------------------------------------------KEYS-------------------------


//---------------------SOVEROM-KEYS, eline token---------------------//
char key_temp[] = "6069"; // Temp-Key, CoT til ESP32

//-------------------BAD-KEYS, eline token--------------------------//
char key_temp_bath[] = "21825"; //Temp-key, CoT til ESP32

//---------------KJOKKEN-KEYS, julia token----------------------//
char key_temp_kitchen[] = "5843"; //Temp-key, CoT til ESP32

//----------------VER-KEYS, hører til cz token------------------//
char key_temp_outside[] = "1536";
char key_humidity[] = "25756";
char key_windspeed[] = "22794";

//--------------Strømpriser, Julia sin token------//
char key_eluse_price_hour[] = "31254";

//--------------Solcelle keys, Are sin token-----------//
char key_suncell_hour[] = "32404";
char key_suncell_day[] = "11674";
char key_suncell_week[] = "32218";
char key_suncell_month[] = "11494";

//---------------Solcelle keys, mathilde sin token-------------//
char key_suncell_year[] = "14699";

//---------------Strømpris etter solcellens produksjon er trekt fra. Mathilde sin token------------//
char key_elexpense_week[] = "23616";
char key_elexpense_month[] = "25052";
char key_elexpense_year[] = "18360";

//----------------El-use, Are sin token---------------------//
char key_eluse_day[] = "6877";
char key_eluse_week[] = "30188";
char key_eluse_month[] = "22800";

//---------------------------------------------------------------------------------------------------------------------------------------------TOKENS------------------------------

char token_Julia[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MjM1In0.p0385WG1WdjnypiJ0vfSyO0QRs7nVICkRrpdVCDsnFs"; // Personlige token som liker opp til signalene, Julia
char token_Eline[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ3In0.aBI_z3hUq4yS2ootcD280GTLo35eg-tWXOI9VVLQySE"; // Personlige token som liker opp til signalene, Eline sin
char token_cz[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ2In0.qs91VynV7MiI6sSqJKHFu4cfgH67dHrYI6OwSk5ySY0"; //Julia sin andre token
char token_mat[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1OTAwIn0.ieCdl0qd5EgIahhOKKSkhOOaSYICXKHcJYfjduC2vCo"; //Mathilde sin token
char token_are[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI2MDQwIn0.kta-kE9IMyj9agxtGUrIZ2eFZ_25HpzKNkBpmxSdp5s"; //Are sin token

CircusESP32Lib circusESP32(server, ssid, password);

//lagrer antall kolonner og rader som lcd-en har i globale variabler
int lcdColumns = 16;
int lcdRows = 2;

//lager et lcd-objekt med adresse sant antall rader og kolonner. dersom man ikke vet I2C-adressen, kjør skanner-sketchen
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//---------------------------------------------------------------------------------------------------------------------------------------------OPPSETT FOR KEYPAD----------------

const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {17, 18, 3, 19};
byte colPins[COLS] = {33, 32, 23};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char activityKey;
char firstChoiceKey;
char secondChoiceKey;

//skal gjøre # til clear all og * til tilbake til hovedmeny, mens også clear på slutten av en valgsekvens (???)

//----------------------------------------------------------------------------------------------------------------------------------TEKST TIL MENYEN--------------------------------

//Overskrifter med alternativer til menyen
String info_home = "INFO";
String info_Options = "1:Temprature 2:Weather 3:Smartmeter 4:El-use";


//Temperature ;)
String temp_hello = "Which room?";
String temp_room = "1:Bedroom 2:Bathroom 3:Kitchen";

String bed_temp_Options;
String bath_temp_Options;
String kitchen_temp_Options;

//Weather
String weather_hello = "The weather today!";   
String info_weather = "1: Temperature 2:Humidity 3:Windspeed";

String outside_temp_Options;
String weather_humidity_Options;
String weather_windspeed_Options;

//Suncell
String suncell_hour_Options;
String suncell_day_Options;
String suncell_week_Options;
String suncell_month_Options;
String suncell_year_Options;


//Smartmeter
String smartmeter_hello = "SMARTMETER";   
String smartmeter_info = "1: Suncell 2: El-price this hour 3:El-espanse";

String smartmeter_suncell = "Suncell";
String smartmeter_suncell_Options = "1:Today 2:This week 3:This month 4:This year";

String smartmeter_elexpense = "Electrial expense";
String smartmeter_elexpense_Options = "1:This hour 2:Today 3:This week 4:This month 5:This year";


//El-use price
String eluse_price_hour_Options;

//Elexpense
String elexpense_week_Options;
String elexpense_month_Options;
String elexpense_year_Options;

//El-use
String eluse_hello = "Electrial use ";
String eluse_info = "1:El-use hour 2:El-use today 3:El-use week 4:El-use month 5:El-use year";

String eluse_day_Options;
String eluse_week_Options;
String eluse_month_Options;

void setup() {
  Serial.begin(115200); //starter den serielle monitoren
  lcd.init();   //initialiserer LCD
  lcd.backlight();  //Skrus på LCD sitt baklys
  circusESP32.begin();
} //end setup-funksjon

//-----------------------------------------------------------------------------------------------------------------------------------------------TEMPERATUR CHOICE------------------------
void temp_choice() {
  switch (firstChoiceKey) {

    case '1': {
        int temp_bed = circusESP32.read(key_temp, token_Eline);
        bed_temp_Options = "Temperature bedroom " + String(temp_bed) + "*C";
        
        for (int i = 0; i < bed_temp_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= bed_temp_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(bed_temp_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;

    case '2': {
        int temp_bath = circusESP32.read(key_temp_bath, token_Eline);
        bath_temp_Options = "Temperature bath " + String(temp_bath) + "*C";
        
        for (int i = 0; i < bath_temp_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= bath_temp_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(bath_temp_Options[i + j]);
          }
          delay (250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;

    case '3': {
        int temp_kitchen = circusESP32.read(key_temp_kitchen, token_Julia);
        kitchen_temp_Options = "Temperature kitchen " + String(temp_kitchen) + "*C";
        
        for (int i = 0; i < kitchen_temp_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= kitchen_temp_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(kitchen_temp_Options[i + j]);
          }
          delay (250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;


    default: {
        //statisk tekst med LCD-menytittelen først
        lcd.print(temp_hello);
        delay(250);
        lcd.clear();

        for (int i = 0; i < temp_room.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= temp_room.length()) {
              lcd.print(" ");
            }
            lcd.print(temp_room[i + j]);
          }

          firstChoiceKey = keypad.getKey();
          if (!firstChoiceKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("firstChoice pressed");
            break;
          }
        }
      }
  } //end switch case
} //end temp-funksjon

//---------------------------------------------------------------------------------------------------------------------------------------------Weather choice------------------------

void weather_choice() {
  switch (firstChoiceKey) {

    case '1': {
        int weather_temp_outside = circusESP32.read(key_temp_outside, token_cz);
        outside_temp_Options = "The temperature outside is " + String(weather_temp_outside) + "*C";
 
        for (int i = 0; i < outside_temp_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= outside_temp_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(outside_temp_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;


    case '2': {
        int weather_humidity = circusESP32.read(key_humidity, token_cz);
        weather_humidity_Options = "The humidity is " + String(weather_humidity) + "%";
        
        for (int i = 0; i < weather_humidity_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= weather_humidity_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(weather_humidity_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;


    case '3': {
        int weather_windspeed = circusESP32.read(key_windspeed, token_cz);
        weather_windspeed_Options = "The windspeed is " + String(weather_windspeed) + "km/h";
       
        for (int i = 0; i < weather_windspeed_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= weather_windspeed_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(weather_windspeed_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;


    default: {
        //statisk tekst med LCD-menytittelen først
        lcd.print(weather_hello);
        delay(250);
        lcd.clear();

        for (int i = 0; i < info_weather.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= info_weather.length()) {
              lcd.print(" ");
            }
            lcd.print(info_weather[i + j]);
          }

          firstChoiceKey = keypad.getKey();
          if (!firstChoiceKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("firstChoice pressed");
            break;
          }
        }
      }
  } //end switch case
} //end weather-funksjon

//---------------------------------------------------------------------------------------------------------------------------------------------Solcellepanel----------------------

void suncell_choice () {
  switch (secondChoiceKey) {

    case '1': { //time
        int suncell_hour = circusESP32.read(key_suncell_hour, token_are);
        suncell_hour_Options = "During the last hour the solar panels have produced " + String(suncell_hour) + "Wh";

        for (int i = 0; i < suncell_hour_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= suncell_hour_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(suncell_hour_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    case '2': { //dag
        int suncell_day = circusESP32.read(key_suncell_day, token_are);
        suncell_day_Options = "Today, the solar panels have produced " + String(suncell_day) + "Wh";

        for (int i = 0; i < suncell_day_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= suncell_day_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(suncell_day_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    case '3': { //uke
        int suncell_week = circusESP32.read(key_suncell_week, token_are);
        suncell_week_Options = "This week, the solar panels have produced " + String(suncell_week) + "Wh";

        for (int i = 0; i < suncell_week_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= suncell_week_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(suncell_week_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    case '4': { //mnd
        int suncell_month = circusESP32.read(key_suncell_month, token_are);
        suncell_month_Options = "This month, the solar panels has produced " + String(suncell_month) + "Wh";

        for (int i = 0; i < suncell_month_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= suncell_month_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(suncell_month_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    case '5': { //aar
        int suncell_year = circusESP32.read(key_suncell_year, token_mat);
        suncell_year_Options = "This year, the solar panels has produced " + String(suncell_year) + "kWh";

        for (int i = 0; i < suncell_year_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= suncell_year_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(suncell_year_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    default: {
        //statisk tekst med LCD-menytittelen først
        lcd.print(smartmeter_suncell);
        delay(250);
        lcd.clear();

        for (int i = 0; i < smartmeter_suncell_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= smartmeter_suncell_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(smartmeter_suncell_Options[i + j]);
          }

          secondChoiceKey = keypad.getKey();
          if (!secondChoiceKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("secondChoiceKey pressed");
            break;
          }
        }
      }
  } //end suncel-funksjon
}

//--------------------------------------------------------------------------------------------------------------------------------------------STRØMUTGIFT---------------------------

void el_expense_choice () {
  switch (secondChoiceKey) {

    case '1': { //denne uken
        int elexpense_week = circusESP32.read(key_elexpense_week, token_mat);
        elexpense_week_Options = "The electricity expense this week is " + String(elexpense_week) + "kr";

        for (int i = 0; i < elexpense_week_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= elexpense_week_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(elexpense_week_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    case '2': { //mnd
        int elexpense_month = circusESP32.read(key_elexpense_month, token_mat);
        elexpense_month_Options = "The electricity expense this month is " + String(elexpense_month) + "kr";

        for (int i = 0; i < elexpense_month_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= elexpense_month_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(elexpense_month_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    case '3': { //aar
        int elexpense_year = circusESP32.read(key_elexpense_year, token_mat);
        elexpense_year_Options = "The electricity expense this year is " + String(elexpense_year) + "kr";

        for (int i = 0; i < elexpense_year_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= elexpense_year_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(elexpense_year_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    default: {
        //statisk tekst med LCD-menytittelen først
        lcd.print(smartmeter_elexpense);
        delay(250);
        lcd.clear();

        for (int i = 0; i < smartmeter_elexpense_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= smartmeter_elexpense_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(smartmeter_elexpense_Options[i + j]);
          }

          secondChoiceKey = keypad.getKey();
          if (!secondChoiceKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("secondChoiceKey pressed");
            break;
          }
        }
      }
  }

} //end strømutgift-funksjon

//----------------------------------------------------------------------------------------------------------------------------------------------SMARTMETER---------------------------

void smartmeter_choice () {
  switch (firstChoiceKey) {

    case '1': {
        suncell_choice();
        activityKey = 0;
      }
      break;

    case '2': {
    int eluse_price_hour = circusESP32.read(key_eluse_price_hour, token_Julia);
        eluse_price_hour_Options = "The electricity price this hour is " + String(eluse_price_hour) + "kr per Wh";

        for (int i = 0; i < eluse_price_hour_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= eluse_price_hour_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(eluse_price_hour_Options[i + j]);
          }
          delay(250);
        }
        firstChoiceKey = 0;
        secondChoiceKey = 0;
      }
      break;

    case '3': {
        el_expense_choice();
      }
      break;

    default: {
        //statisk tekst med LCD-menytittelen først
        lcd.print(smartmeter_hello);
        delay(250);
        lcd.clear();

        for (int i = 0; i < smartmeter_info.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= smartmeter_info.length()) {
              lcd.print(" ");
            }
            lcd.print(smartmeter_info[i + j]);
          }

          firstChoiceKey = keypad.getKey();
          if (!firstChoiceKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("firstChoiceKey pressed");
            break;
          }
        }
      }
  }
}//end SMARTMETER

//--------------------------------------------------------------------------------------------------------------------------------------------EL-USE-------------------------------

void el_use () {

  switch (firstChoiceKey) {

    case '1': { //dag
        int eluse_day = circusESP32.read(key_eluse_day, token_are);
        eluse_day_Options = "The electrical use today is " + String(eluse_day) + "kWh";

        for (int i = 0; i < eluse_day_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= eluse_day_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(eluse_day_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;


    case '2': { //uke
        int eluse_week = circusESP32.read(key_eluse_week, token_are);
        eluse_week_Options = "The electrical use this week is  " + String(eluse_week) + "kWh";

        for (int i = 0; i < eluse_week_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= eluse_week_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(eluse_week_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;


    case '3': { //mnd
        int eluse_month = circusESP32.read(key_eluse_month, token_are);
        eluse_month_Options = "The electrical use this month is " + String(eluse_month) + "kWh";

        for (int i = 0; i < eluse_month_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= eluse_month_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(eluse_month_Options[i + j]);
          }
          delay(250);
        }
        activityKey = 0;
        firstChoiceKey = 0;
      }
      break;

    default: {
        //statisk tekst med LCD-menytittelen først
        lcd.print(eluse_hello);
        delay(250);
        lcd.clear();

        for (int i = 0; i < eluse_info.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= eluse_info.length()) {
              lcd.print(" ");
            }
            lcd.print(eluse_info[i + j]);
          }

          firstChoiceKey = keypad.getKey();
          if (!firstChoiceKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("firstChoice pressed");
            break;
          }
        }
      }
  } //end switch case
} //end el-use

//------------------------------------------------------------------------------------------------------------------------------------------INFO-SKJERM START----------------

void loop () {
  switch (activityKey) {

    case '1': {
        temp_choice();
      }
      break;

    case '2':     {
        weather_choice();
      }
      break;

    case '3': {
        smartmeter_choice();
      }
      break;

    case '4': {
        el_use();
      }
      break;

    default: {
        //statisk tekst med LCD-menytittelen først
        lcd.print(info_home);
        delay(250);
        lcd.clear();

        for (int i = 0; i < info_Options.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= info_Options.length()) {
              lcd.print(" ");
            }
            lcd.print(info_Options[i + j]);
          }

          activityKey = keypad.getKey();
          if (!activityKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("activityKey pressed");
            break;
          }
        }
      }
  }

} //end of loop
