//---------------------------------------//
      //--------Kjokken---------//
////----------------------------------////

#include <CircusESP32Lib.h> //inkluderer biblioteket til CoT til ESP32
#include <analogWrite.h>

//-----------CoT-egenskaper-------//

char ssid []="Elines iphone"; //WIFI SSID (eline)
char password[]="passordet"; //WIFI passord
char server[]="www.circusofthings.com";
char key_LED_kitchen[] = "27086"; // Key til Circus Signal som ESP32 skal høre på. 
char key_temp_kitchen[] = "5843"; //Temp-key, CoT til ESP32
char key_OVN_kitchen[] = "14401"; // Ovn, CoT til ESP32
char key_TV_kitchen[]= "31680"; //TV, CoT til ESP32
char key_WASH_kitchen[] = "28400"; //Oppvaskmaskin, CoT ESP32
char token_Julia[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MjM1In0.p0385WG1WdjnypiJ0vfSyO0QRs7nVICkRrpdVCDsnFs"; // Personlige token som liker opp til signalene, Julia

CircusESP32Lib circusESP32(server,ssid,password);

//--------PINS--------//

#define LEDPIN_OVN_kitchen 21  //red led
#define LEDPIN_TV_kitchen 25  //green led
#define LEDPIN_wash_kitchen 23  //blue led
#define LEDPIN_LIGHT_kitchen 22 //yellow led
#define temp_PIN_kitchen  36  
#define lyssensor_PIN_kitchen 39

float volts; //lagrer spenning, for temp
float temp; //temperatur
float tempValue; //Temperatursverdi
float tempValueFunk; //Lagring av temp verdi som legges sammen
float tempValueTotal; //Temp verdi etter snittutregning
int LED_state; //om LED er av eller på (bruker LED som en "stikkontakt)

int LED_ValueFunk;
int LED_ValueTotal;

int totaltemp;

long offAt = 0; //av funkjson

//----------Snitt temperatur kjokken--------//

float tempAverage_kitchen () {  //samme som soverom
  tempValueFunk = 0; 
  for (int i=0;i<=4;i++){ 
    tempValue = analogRead(temp_PIN_kitchen); 
    volts = tempValue/1023.0; 
    temp = (volts - 0.5) * 100 ; 
    tempValueFunk +=temp; 
    delay(1000);
  }
return totaltemp = tempValueFunk/5; //deler tempValueFunk (delt på 5) for å få rett gjennomsnittstemp
}

//-------OVN---------//

void ovn_controll () {
  int OVN = circusESP32.read(key_OVN_kitchen, token_Julia);
  if(OVN == 0){
    digitalWrite(LEDPIN_OVN_kitchen, LOW);   //av, ovnen er av (Styres i COT)
    Serial.println("Stove is off");
}
else {
    Serial.println("Stove is on"); 
    digitalWrite(LEDPIN_OVN_kitchen, HIGH);
  }
}

//---------TV--------//

void TV_controll () {
  int TV = circusESP32.read(key_TV_kitchen, token_Julia);
  if(TV == 0){
    digitalWrite(LEDPIN_TV_kitchen, LOW);   //av, TV er av (Styres i COT)
    Serial.println("TV is off");
}
else {
    Serial.println("TV is on"); 
    digitalWrite(LEDPIN_TV_kitchen, HIGH);
  }
}

//-------OPPVASKMASKIN-----//
bool counting = false;

void washer_controll () {
 
  int washer = circusESP32.read(key_WASH_kitchen, token_Julia);
  if (washer == 1 && counting == false) { // Counting fopr å besørge at denne sekvensen bare kjøres en gang.
    Serial.println("The washer is on"); //sjekk om oppvaskmaskin er på 1 og ikke 0
    digitalWrite(LEDPIN_wash_kitchen, HIGH);
    offAt = millis() + (1000*14,4) ; //maskinen skal skru seg av etter 14 400 sekunder (fire timer)
    counting = true;
  }
  if (millis() >= offAt && counting == true) // skal led skrus av? Sjekker
    {
    digitalWrite(LEDPIN_wash_kitchen, LOW);
    circusESP32.write(key_WASH_kitchen, 0, token_Julia); //skru den av
    counting = false; //Flagges counting her, slik at den er klar for neste runde
  }
}

//------Lyssystem (fordi det er så mye naturlig lys)-------//
              //-------------LYSSENSOR----------//

float LED_Average_kitchen () {
  LED_ValueFunk = 0; //Startvariabel er 0
  for (int i=0;i<4;i++) { //samme som ovenfor, fem maalinger
    LED_ValueFunk += analogRead(lyssensor_PIN_kitchen); //leser av sensorpin
    delay (1000);
  }
  return LED_ValueFunk /5; //samme som ovenfor  
}

//---------LYSMÅLING-----------//

void LED_controll_kitchen(){
  if(LED_ValueTotal > 2050) {
    digitalWrite(LEDPIN_LIGHT_kitchen, 200);
  }
    else {
      digitalWrite(LEDPIN_LIGHT_kitchen, 255);
    }
  Serial.print("The lightvalue is ");
  Serial.println(LED_ValueTotal); //Lysverdi
}


//-----LYSKONTROLL---//
void controll () {
  int LED = circusESP32.read(key_LED_kitchen, token_Julia);   //Bruker CoT som knapp
  if(LED == 0){
    digitalWrite(LEDPIN_LIGHT_kitchen, LOW);   //av, ingen sensorsystem
    Serial.println("The light is off");
  } 
  else {
    Serial.println("The light is on and the servo is controlling"); //på og sensor styrer
    LED_controll_kitchen();
  }

  Serial.println("Degrees");
  Serial.print("Temperature is: ");
  Serial.print(tempValueTotal); //printer ut temperaturen som er på kjokkenet 
  Serial.println(" degrees C");
  Serial.println(tempValue);
}

void setup() {
  Serial.begin(115200); //starter på 115200 baud rate
  pinMode(LEDPIN_LIGHT_kitchen, OUTPUT); //LEDPIN er nå OUTPUT
  pinMode(LEDPIN_TV_kitchen, OUTPUT); //LEDPIN er nå OUTPUT
  pinMode(LEDPIN_OVN_kitchen, OUTPUT); //LEDPIN er nå OUTPUT
  pinMode(LEDPIN_wash_kitchen, OUTPUT); //LEDPIN er nå OUTPUT
  circusESP32.begin(); //Starter CoT, den setter så påå SSL/Secure connection
}

void loop() {
  Serial.println ("Hei this is a kitchen check");
  tempValueTotal = tempAverage_kitchen ();
  LED_ValueTotal = LED_Average_kitchen ();
  circusESP32.write(key_temp_kitchen, totaltemp, token_Julia);
  controll ();
  ovn_controll ();
  washer_controll ();
  TV_controll ();  
}
