
///----------------SOVEROM------------///

#include <CircusESP32Lib.h> //inkluderer biblioteket til CoT til ESP32
#include <analogWrite.h>
#include <Servo.h>  //bib for servoen
#include <Gruppe5Bib.h> 

//-----------CoT-egenskaper-------//

char ssid []="Elines iphone"; //WIFI SSID (eline sin iphone)
char password[]="nyttpassord"; //WIFI passord
char server[]="www.circusofthings.com";
char key_temp_given[] = "9183"; // Temperatur-gitt-key til Circus Signal som ESP32 skal høre på. 
char key_temp[] = "6069"; // Temp-Key, CoT til ESP32
char key_heat[] = "6564"; // Varme-Key, CoT til ESP32
char key_LED[] = "6729"; //LED-key, CoT til ESP32
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ3In0.aBI_z3hUq4yS2ootcD280GTLo35eg-tWXOI9VVLQySE"; // Personlige token som liker opp til signalene (eline)

CircusESP32Lib circusESP32(server,ssid,password);
Gruppe5Bib Gruppe5Bib();

//-----VERDIER------//

#define LEDPIN_HEAT 21   //panelovn
#define LEDPIN_LIGHT 22 //lys
uint8_t temp_PIN = 36;  //temperatur pin
uint8_t lyssensor_PIN = 39; //lyssensor

Servo myservo; //for seroven
int servopos = 0; //starter på 0 grader

float volts; //lagrer spenning
float temp; //temperaturvariabel 
float tempValue; //Temperatur fra sensoren
float tempValueFunk; //Lagring av temp verdi som legges sammen
float tempValueTotal; //Temp verdi etter snittutregning
int LED_state; //om LED er av eller på (bruker LED som en "stikkontakt" til vamreovnen. Når led lyser skrus stikkontakt på og varmeovnen skrus på og motsatt)

int LED_ValueFunk;
int LED_ValueTotal;


//--------------"Panelovn" og vindusaapner-------------//
         //----Temp og servo funksjoner-----//
            //---Sender info til CoT----//

int panelovn () {
  float temp_given = circusESP32.read(key_temp_given, token); //Dette er signalet/verdien man angir i CoT
  circusESP32.write(key_temp, tempValueTotal, token); //Har panelovnen slaatt på vamre? Info går til CoT
  if (tempValueTotal < temp_given){ //om tempen som blir maalt er mindre enn den som er gitt
    LED_state = 1; //da skrus led på (aka panelovn skrus på)
    digitalWrite(LEDPIN_HEAT, HIGH);  //Skrur på lys (HIGH)
  }
    else{
      LED_state = 0;
      digitalWrite(LEDPIN_HEAT, LOW); //skrur av lys (LOW);
    }
  if (tempValueTotal > 25) {
    for (servopos =0; servopos <=45; servopos += 1) { //0 grader til 45 grader
      myservo.write(servopos); //servo går i postisjon i variabelen pos
    }
  }
  if (tempValueTotal < 25) {
    for (servopos = 45; servopos >=0; servopos -=1) { //45 grader til 0
      myservo.write(servopos); //er servoen til å å til posisjonen sin i pos
    }
  }
  Serial.print("Servos position is: ");
  Serial.print(servopos); //printer ut posisjonen til servoen
  Serial.println("Degrees");
  Serial.print("Temperature is: ");
  Serial.print(tempValueTotal); //printer ut temp
  Serial.println(" degrees C");
  Serial.println(tempValue);
  circusESP32.write(key_heat, LED_state, token); //Sier ifra til CoT om panelovnen har slaatt på varme til CoT
}


//-----VOID SETUP---//

void setup()
{
  Serial.begin(115200); //starter på 115200 baud rate
  pinMode(LEDPIN_HEAT, OUTPUT); //LEDPIN er nå OUTPUT
  myservo.attach(13); //kobler servo til pin 13
  circusESP32.begin(); //Starter CoT, den setter så påå SSL/Secure connection
}

//-----VOID LOOP----//

void loop() { //kjorer alle kommandoen ovenfor i rett rekkefolge
  Serial.println ();
tempValueTotal = Gruppe5Bib.tempAverage();
LED_ValueTotal = LED_Average();
Gruppe5Bib.controll();
panelovn();
  }
