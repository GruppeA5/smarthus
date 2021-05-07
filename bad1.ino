
//-------------------------------------------//
  //-----------------BADEROM-------------//
////-------------------------------------////

#include <CircusESP32Lib.h> //inkluderer biblioteket til CoT til ESP32
#include <analogWrite.h>

//-----------CoT-egenskaper-------//

char ssid []="Elines iphone"; //WIFI SSID (eline)
char password[]="passordet"; //WIFI passord
char server[]="www.circusofthings.com";
char key_LED_bath[] = "2616"; // LED-key til Circus Signal som ESP32 skal høre på. 
char key_temp_given_bath[] = "21212"; // Temperatur gitt til Key, CoT til ESP32
char key_temp_bath[] = "21825"; //Temp-key, CoT til ESP32
char key_heat_bath[] = "17656"; // Varmekabel,LED, CoT til ESP32
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ3In0.aBI_z3hUq4yS2ootcD280GTLo35eg-tWXOI9VVLQySE"; // Personlige token som liker opp til signalene, Eline sin


//-----------CoT-egenskaper Renate----//
char gmailToken[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTA0In0.fprG7FD9-Fw5k92717voJun3dTStu0gEjgeYkVE9GvI"; //toke for gmail-brukeren min, Renate
char pplBathroomKey[] = "29825"; //2
char bathroomTimerKey[] = "16655"; //14

CircusESP32Lib circusESP32(server,ssid,password);

//--------PINS--------//

#define LEDPIN_HEAT_bath 21  
#define LEDPIN_LIGHT_bath 22
#define temp_PIN_bath  36  


float volts; //lagrer spenning, for temp
float temp; //temperaturvariabel
float tempValue; //Temperatur fra sensoren
float tempValueFunk; //Lagring av temp verdi som legges sammen
float tempValueTotal; //Temp verdi etter snittutregning
int LED_state; //

int LED_ValueFunk;
int LED_ValueTotal;

//-----------Snitt temperatur bad--------//

float tempAverage_bath () {
  tempValueFunk = 0;  //setter startverdien til funskjonen til 0
  for (int i=0;i<=4;i++){   //tar fem målinger
    tempValue = analogRead(temp_PIN_bath);   //leser av temp sensoren og lagrer det i tempValue
    volts = tempValue/1023.0;             // normaliserer max temperaturens raw reading range
    temp = (volts - 0.5) * 100 ;         //regner ut temperatur (i celcius fra volt i henhold til ligningen som er funnet på sensorens spesifikasjonsark)
    tempValueFunk +=temp; //Legger målingnee inn i tempValueFunk variabel
    delay(1000);
  }
return tempValueFunk/5; //deler tempValueFunk (delt på 5) for å få rett gjennomsnittstemp
}


//-----OFFISIELLE LYSKONTROLL---//

void controll () {
  int LED = circusESP32.read(pplBathroomKey, gmailToken);   //Bruker CoT som knapp
  if(LED == 1){
    digitalWrite(LEDPIN_LIGHT_bath, HIGH);   //på, ingen sensorsystem
    Serial.println("Lys på");
  } 
  else {
    Serial.println("Lyset er av"); //på sensorsystemet styrer
    digitalWrite(LEDPIN_LIGHT_bath, LOW); //Lyset er av
  }
}

//------------Varmekabler-----------//

int varmekabel () {
  float temp_given = circusESP32.read(key_temp_given_bath, token); //Dette er signalet/verdien man angir i CoT
  circusESP32.write(key_temp_bath, tempValueTotal, token); //SKriver ut verdi til CoT
  if (tempValueTotal < temp_given){ //om tempen som blir målt er mindre enn den som er gitt
    LED_state = 1; //da skrus led på (aka pnelovn skrus på)
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
