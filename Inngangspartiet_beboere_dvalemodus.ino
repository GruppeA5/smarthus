//______________________________________________________________________

//inkluderer bibliotekene vi trenger
#include <SPI.h>  
#include <MFRC522.h>
#include <CircusESP32Lib.h>
#include <driver/rtc_io.h>



//______________________________________________________________________

//Definerer funksjonen som printer hvorfor ESP32 har blitt vekket

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

#define BUTTON_PIN_BITMASK 0x200000000     //pinne 33 må omgjøres til heksadesimaltall
//_______________________________________________________________________

// Definerer nettverksinnstillinger og personlige opplysninger på CoT

char ssid[] = "juliasiphone";
char password[] = "nyttpassord";
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ2In0.qs91VynV7MiI6sSqJKHFu4cfgH67dHrYI6OwSk5ySY0";
char server[] = "www.circusofthings.com";
char key_orange[] = "24169";
char key_yellow[] = "17840";
char key_pink[] = "14083";

CircusESP32Lib circusESP32(server,ssid,password); 


bool hjemme = 1;       //definerer hjemme- og bortetilstander
bool borte = 0;

//______________________________________________________________________
  
#define SS_PIN 21 //SS pin på RFID er koblet til pin 21 på ESP32
#define RST_PIN 22 //RST pin på RFID er koblet til pin 22 på ESP32
#define redLED 32  //Rød led er koblet til pin 32 på ESP32 
#define greenLED 12 //Grønn led er koblet til pin 12 på ESP32

//________________________________________________________________________

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Lager en forekomst med MFRC522 rfid. 
MFRC522::MIFARE_Key key; //Brukes i autentisering 
//_________________________________________________________________________
void setup(){

  Serial.begin(115200);    //Initierer Seriell overvåker
  delay(1000); 
  print_wakeup_reason();   //Printer måten ESP32 ble vekket på
  Serial.println("Scan your chip");
  Serial.println();
//__________________________________________________________________________________________________________________________________________
  pinMode(redLED, OUTPUT);     //LED for å vise om du får låst opp. Grønn hvis brikken/kortet ditt er godkjent, rød hvis du ikke har adgang.
  pinMode(greenLED, OUTPUT);
 //_________________________________________________________________________________________________________________________________________
  digitalWrite(redLED, HIGH);   //LEDene skal blinke ved oppstart for å vise at koden ble lastet opp riktig
  delay(400);
  digitalWrite(redLED, LOW);
  delay(400);
  digitalWrite(greenLED, HIGH);
  delay(400);
  digitalWrite(greenLED, LOW);
//___________________________________________________________________________________________________________________________________________
  /*
  Her definerer vi hvordan ESP32 skal vekkes. Vi velger da ext0 som betyr ekstern påtrykk, i dette tilfelle en knapp på pinne 33.
  Dette skal huskes av RTC som ikke går i dvalemodus
  */
  rtc_gpio_deinit(GPIO_NUM_33);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_33,1); 

//_____________________________________________________________________________________________________________________________________________

  circusESP32.begin();   //setter i gang kommunikasjonen med CoT
  SPI.begin();          // setter i gang  SPI bus
  mfrc522.PCD_Init();   // setter i gang MFRC522

//______________________________________________________________________________________________________________________________________________

 bool orange_val = 1; //circusESP32.read(key_orange, token);  //Sjekker om beboere i utgangspunktet er hjemme 
 bool pink_val = 1; //circusESP32.read(key_pink, token);      //for å kunne avgjøre om de sjekker seg inn eller ut
 bool yellow_val = 1; // circusESP32.read(key_yellow, token);
  // Ser etter nye kort
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  //Velger et av kortene
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  } 
  //Viser ID til brikken
  //Serial.print("ID nummer :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "); //printer i heksadesimal 
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Melding : ");
  content.toUpperCase();
  

  if (content.substring(1) == "5B B1 91 15" && yellow_val == borte) // Dersom det avleste nummeret samsvarer med den gule brikken og personen er ikke hjemme
  {
    Serial.println("Access granted");    //Så kommer det en melding
    circusESP32.write(key_yellow, hjemme, token);   //Og det sendes et signal til CoT om at personen er hjemme
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW);
  }
 else if (content.substring(1) == "5B B1 91 15" && yellow_val == hjemme) //Dersom brikken skannes på vei ut (personen var hjemme fra før av) 
  {
    Serial.println("Bye!");       // Så kommer det en melding     
    circusESP32.write(key_yellow, borte, token);     //Og det sendes et signal til CoT om at personen forlater huset.
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW);
  } 
 else { //Kort som ikke er registrert med id'en får ikke åpnet døra og det viser vi med rødt lys. 
    Serial.println(" Adgang forbudt");
    digitalWrite(redLED, HIGH); //Rød LED lyser for å vise at brikken ikke åpner låsen. 
        delay(500);
        digitalWrite(redLED, LOW);
 }
//________________________________________________________________________________________________________________________________________________

// Gjentar prosedyren med den oransje brikken
if (content.substring(1) == "09 EB 60 63" && orange_val == borte) //Dersom det avleste nummeret samsvarer med den oransje brikken og personen er ikke hjemme 
  {
    Serial.println("Access granted");       //Så kommer det en melding
    circusESP32.write(key_orange, hjemme, token);     //og det sendes set signal til CoT
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW);
  } 
 else if (content.substring(1) == "09 EB 60 63" && orange_val == hjemme) //Dersom brikken skannes på vei ut (personen var hjemme fra før av) 
  {
    Serial.println("Bye!");      //så kommer det en melding
    circusESP32.write(key_orange, borte, token);    //og det sendes et signal til CoT om at personen forlater huset
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW);
  } 
 else { //Kort som ikke er registrert med id'en får ikke åpnet døra og det viser vi med rødt lys. 
    Serial.println(" Adgang forbudt");
    digitalWrite(redLED, HIGH); //Rød LED lyser for å vise at brikken ikke åpner låsen. 
        delay(500);
        digitalWrite(redLED, LOW);
  }
//_____________________________________________________________________________________________________________________________________________

//Følger samme prosedyre med den rosa brikken
if (content.substring(1) == "19 2B 5F 63" && pink_val == borte) // Dersom det avleste nummeret samsvarer med den rosa brikken og personen er ikke hjemme
  {
    Serial.println("Access granted");    //så kommer det en melding 
    circusESP32.write(key_pink, hjemme, token);    // og det sendes et signal til CoT om at personen er hjemme
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW);
  } 
 else if (content.substring(1) == "19 2B 5F 63" && pink_val == hjemme) //Dersom brikken skannes på vei ut (personen var hjemme fra før av) 
  {
    Serial.println("Bye!");       // Så kommer det en melding     
    circusESP32.write(key_pink, borte, token);     //Og det sendes et signal til CoT om at personen forlater huset.
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW);
  } 
 else { //Kort som ikke er registrert med id'en får ikke åpnet døra og det viser vi med rødt lys. 
    Serial.println(" Adgang forbudt");
    digitalWrite(redLED, HIGH); //Rød LED lyser for å vise at brikken ikke åpner låsen. 
        delay(500);
        digitalWrite(redLED, LOW);}
 


//Når vi skanner en brikke som er registrert så oppfylles det alltid tre betingelser: "gjenkjent" på den ene brikken og 
//"ikke gjenkjent" på de to andre.
//Det vil si at selv om vi skanner riktig brikke så får vi både grønt og rødt LED til å lyse. 
//Det får ikke konsekvenser for registrering av personer i huset.


  //Setter ESP32 i dvalemodus på ubegrenset tid, frem til noen trykker på knappen
  Serial.println("Going to sleep now");
  delay(1000);
  esp_deep_sleep_start();
  
}

void loop(){ }
