//inkluderer bibliotekene vi trenger
#include <SPI.h>  /
#include <MFRC522.h>
#include <CircusESP32Lib.h>

char ssid[] = "juliasiphone";
char password[] = "nyttpassord";
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MDQ2In0.qs91VynV7MiI6sSqJKHFu4cfgH67dHrYI6OwSk5ySY0";
char server[] = "www.circusofthings.com";
char key_orange[] = "24169";
char key_yellow[] = "17840";
char key_pink[] = "14083";
bool Home = 1;
bool Away = 0;


CircusESP32Lib circusESP32(server,ssid,password);
 
#define SS_PIN 21 //SS pin på RFID er koblet til pin 21 på ESP32
#define RST_PIN 22 //RST pin på RFID er koblet til pin 22 på ESP32
#define redLED 32  //Rød led er koblet til pin 32 på ESP32 
#define greenLED 12 //Grønn led er koblet til pin 12 på ESP32

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Lager en forekomst med MFRC522 rfid. 
MFRC522::MIFARE_Key key; //Brukes i autentisering 

void setup() 
{
  Serial.begin(9600);   // Setter i gang en serial kommunikasjon
  SPI.begin();          // setter i gang  SPI bus
  mfrc522.PCD_Init();   // setter i gang MFRC522
 Serial.println('Put yout card to the scanner');
 Serial.println();

  
  pinMode(redLED, OUTPUT);     //LED for å vise om du får låst opp. 
  pinMode(greenLED, OUTPUT);  //Grønn hvis brikken/kortet ditt er godkjent, rød hvis du ikke har adgang.
  digitalWrite(redLED, HIGH);
  delay(400);
  digitalWrite(redLED, LOW);
  delay(400);
  digitalWrite(greenLED, HIGH);
  delay(400);
  digitalWrite(greenLED, LOW);
}
void loop() 
{
  // Ser etter nye kort
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Velger en av kortene
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Viser ID til brikken
  Serial.print("ID number :");
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
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "19 2B 5F 63") //gir Id-brikkene våre adgang. 
  //
  {
    Serial.println("Access granted");  //Hvis brikken din er registrert får du tilgang
    circusESP32.write(key_yellow, Home, token); //Sender signal til CoT om at du er hjemme
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW);
  }
if (content.substring(1) == "09 EB 60 63") // 
  {
    Serial.println("Access granted");
    circusESP32.write(key_orange, Home, token);
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW);
  }
 if (content.substring(1) == "5B B1 91 15") // 
  {
    Serial.println("Access granted");
    circusESP32.write(key_pink, Home, token);
    Serial.println();
    digitalWrite(greenLED, HIGH);    //Grønn LED lyser for å vise at brikken er registrert og vil åpne døra. 
      delay(500);
      digitalWrite(greenLED, LOW); 
  }     
 else   { //Kort som ikke er registrert med id'en får ikke åpnet døra og det viser vi med rødt lys. 
    Serial.println(" Access denied"); //Printer adgang forbudt
    digitalWrite(redLED, HIGH); //Rød LED lyser for å vise at brikken ikke åpner døra. 
        delay(500);
        digitalWrite(redLED, LOW);
  }
} 
  
 
