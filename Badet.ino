
//-------------------------------------------//
  //-----------------BADEROM-------------//
////-------------------------------------////

#include <CircusESP32Lib.h> //inkluderer biblioteket til CoT til ESP32
#include <analogWrite.h>

//-----------CoT-egenskaper-------//

char ssid []="Elines iphone"; //WIFI SSID (eline)
char password[]="nyttpassord"; //WIFI passord
char server[]="www.circusofthings.com";
char key_LED_bath[] = "2616"; // LED-key til Circus Signal som ESP32 skal høre på. 
char key_temp_given_bath[] = "21212"; // Temperatur gitt til Key, CoT til ESP32
char key_temp_bath[] = "21825"; //Temp-key, CoT til ESP32
char key_heat_bath[] = "17656"; // Varmekabel,LED, CoT til ESP32
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ3In0.aBI_z3hUq4yS2ootcD280GTLo35eg-tWXOI9VVLQySE"; // Personlige token, Eline sin


//-----------CoT-egenskaper Renate----//

char gmailToken[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTA0In0.fprG7FD9-Fw5k92717voJun3dTStu0gEjgeYkVE9GvI";
char pplBathroomKey[] = "29825";
char bathroomTimerKey[] = "16655";

CircusESP32Lib circusESP32(server,ssid,password);

//--------PINS--------//

#define LEDPIN_HEAT_bath 21   //blaa led
#define LEDPIN_LIGHT_bath 22 //gul led
#define temp_PIN_bath  36  
#define lyssensor_PIN_bath 39

float volts; //lagrer spenning, for temp
float temp; //temperaturvariabel
float tempValue; //Temperatur fra sensoren
float tempValueFunk; //Lagring av temp verdi som legges sammen
float tempValueTotal; //Temp verdi etter snittutregning
int LED_state;

int LED_ValueFunk;
int LED_ValueTotal;

//-----------Snitt temperatur bad--------//

float tempAverage_bath () {
  tempValueFunk = 0;  //Lik som på soverom
  for (int i=0;i<=4;i++){  
    tempValue = analogRead(temp_PIN_bath);
    volts = tempValue/1023.0;   
    temp = (volts - 0.5) * 100 ;
    tempValueFunk +=temp; 
    delay(1000);
  }
return tempValueFunk/5;
}


//-----OFFISIELLE LYSKONTROLL---//

void controll () {
  int LED = circusESP32.read(pplBathroomKey, gmailToken);   //Bruker CoT som knapp
  if(LED == 1){
    digitalWrite(LEDPIN_LIGHT_bath, HIGH);   //paa, ingen sensorsystem
    Serial.println("The light on");
  } 
  else {
    Serial.println("The light is off"); //på sensorsystemet styrer
    digitalWrite(LEDPIN_LIGHT_bath, LOW); //Lyset er av
  }
}

//------------Varmekabler-----------//

int varmekabel () {
  float temp_given = circusESP32.read(key_temp_given_bath, token); //samme som soverom
  circusESP32.write(key_temp_bath, tempValueTotal, token);
  if (tempValueTotal < key_temp_given_bath){ 
    LED_state = 1; 
    digitalWrite(LEDPIN_HEAT_bath, HIGH);  //Skrur på lys (setter den til HIGH)
  }
    else{
      LED_state = 0;
      digitalWrite(LEDPIN_HEAT_bath, LOW); //skrur av lys (setter den til LOW);
    }

  Serial.println("Grader");
  Serial.print("Temperatur er: ");
  Serial.print(tempValueTotal);
  Serial.println(" grader C");
  Serial.println(tempValue);
  circusESP32.write(key_heat_bath, LED_state, token); //Sier ifra til CoT om varmekablene har slått på varme til CoT
}

//-----VOID SETUP-----//

void setup() {
  Serial.begin(115200); //starter på 115200 baud rate
  pinMode(LEDPIN_HEAT_bath, OUTPUT); //LEDPIN er nå OUTPUT
  pinMode(LEDPIN_LIGHT_bath, OUTPUT); //LEDPIN er nå OUTPUT
  circusESP32.begin(); //Starter CoT, den setter så påå SSL/Secure connection
}

//---------VOID LOOP------//

void loop() { //kjører alle kommandoene som står ovenfor i rett rekkefølge
  Serial.println ();
  tempValueTotal = tempAverage_bath ();
  controll();
  varmekabel();
}
