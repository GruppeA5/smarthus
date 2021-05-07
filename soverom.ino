///----------------SOVEROM------------///

#include <CircusESP32Lib.h> //inkluderer biblioteket til CoT til ESP32
#include <analogWrite.h>
#include <Servo.h>  //bib for servoen

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

//-----VERDIER------//

#define LEDPIN_HEAT 21   //panelovn
#define LEDPIN_LIGHT 22 //lys
#define temp_PIN  36  //temperatur pin
#define lyssensor_PIN 39 //lyssensor

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

//-----------Snitt temperatur--------//

float tempAverage () {
  tempValueFunk = 0; //setter startverdien til funskjonen til 0
  for (int i=0;i<=4;i++){ //tar fem maalinger
    tempValue = analogRead(temp_PIN);   //leser av temp sensoren og lagrer det i tempValue
    volts = tempValue/1023.0;             // normaliserer max temperaturens raw reading range
    temp = (volts - 0.5) * 100 ;         //regner ut temperatur (i celcius fra volt i henhold til ligningen som er funnet på sensorens spesifikasjonsark)
    tempValueFunk +=temp; //Legger målingnee inn i tempValueFunk variabel
    delay(1000);
  }
return tempValueFunk/5; //deler tempValueFunk (delt på 5) for å få rett gjennomsnittstemp
}

//-------------LYSSENSOR----------//

float LED_Average () {
  LED_ValueFunk = 0; //Startvariabel er 0
  for (int i=0;i<4;i++) { //samme som ovenfor, fem målinger
    LED_ValueFunk = analogRead(lyssensor_PIN); //leser av sensorpin
    delay (1000);
  }
  return LED_ValueFunk /5; //samme som ovenfor  
}

//---------LYSSTYRING-----------//

void LED_controll(){
  if(LED_ValueTotal > 2050) {
    digitalWrite(LEDPIN_LIGHT, 200);
  }
    else {
      digitalWrite(LEDPIN_LIGHT, 255);
    }
  Serial.print("Light value is ");
  Serial.println(LED_ValueTotal); //Lysverdi
}


//-----OFFISIELLE LYSKONTROLL---//
void controll () {
  int LED = circusESP32.read(key_LED, token);   //Bruker CoT som knapp
  if(LED == 0){
    digitalWrite(LEDPIN_LIGHT, LOW);   //av, ingen sensorsystem
    Serial.println("The light is off.");
  } 
  else {
    Serial.println("The light is on and the sensor is in controll."); //på og sensor styrer
    LED_controll();
  }
}

//--------------"Panelovn" og vindusaapner-------------//
         //----Temp og servo funksjoner-----//
            //---Sender info til CoT----//

int panelovn () {
  float temp_given = circusESP32.read(key_temp_given, token); //Dette er signalet/verdien man angir i CoT
  circusESP32.write(key_temp, tempValueTotal, token);
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
  pinMode(LEDPIN_LIGHT, OUTPUT); //LEDPIN er nå OUTPUT
  myservo.attach(13); //kobler servo til pin 13
  circusESP32.begin(); //Starter CoT, den setter så påå SSL/Secure connection
}

//-----VOID LOOP----//

void loop() { //kjorer alle kommandoen ovenfor i rett rekkefolge
  Serial.println ();
  tempValueTotal = tempAverage ();
  LED_ValueTotal = LED_Average ();
  controll();
  panelovn();
  }
