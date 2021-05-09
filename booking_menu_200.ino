//BOOKINGKONSOLL, TILHØRER EN BESTEMT PERSON n (RECEIVER)

//----------------------------------------------------------------------------------------------------------------------------------------------BIBLIOTEKER---------------
#include <Keypad.h> //tar med biblioteket for 4x3 keypaden
#include <LiquidCrystal_I2C.h>  //biblioteket til LCD-en
#include <CircusESP32Lib.h> //inkluderer CoT-biblioteket til ESP32
#include <esp_now.h> //biblioteker for ESP-NOW
#include <WiFi.h> //wifi-biblioteket
#include <Tone32.h> //biblioteker for buzzer
#include <pitches.h> //biblioteket for toner/noter

//----------------------------------------------------------------------------------------------------------------------------------------OPPSETT TIL CIRCUS OF THINGS--------------

//mobilnett
char ssid[] = "AndroidAP";
char password[] = "qwerty123";

char server[] = "www.circusofthings.com"; //her ligger serveren/nettadressen

//ikke faktiske tokens
char gmailToken[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdTA07S_SgeYO_D0QXOwSk5JKHFu4cfgHkPDQ2In0.OJqdGnIn0My17voT1zcjyn"; //token for gmail-brukeren min,
char ntnuToken[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGk.gPpKqs0MikVE9GvIJV7xLOiySY04iNCarv-sm291VtuoX4Y84oeSi"; //ntnu-brukeren min
char outlookToken[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1OTe1Yn67Jun3dTStuiI1I0OcyIOWPTdrn4I6sSq0gEj6vdi-mmdn0E5"; //og outlook-brukeren min

//andre sine tokens
char czarnockajuliaToken[] = "eyJhbGciOiJIUzI1NiJ9.eypO01kiO.fp0OTGk5k927lEFwHrYI68rG7FD9-AyIn0.GxiiOiIUuotd";

CircusESP32Lib circusESP32(server, ssid, password); //lager objektet CircusESP32 som inneholder server, ssid og passord-informasjon

//----------------------------------------------------------------------------------------------------------------------------------------------------------COT KEYS--------------

//keys på gmail-bruker
char pplHouseKey[] = "12984"; //1
char pplBathroomKey[] = "29825"; //2
char pplKitchenKey[] = "23724"; //3

char pplRoomKey5[] = "10125"; //4
char pplRoomKey4[] = "31088"; //5
char pplRoomKey3[] = "24553"; //6
char pplRoomKey2[] =  "29482"; //7
char pplRoomKey1[] = "21571"; //8

//keys på ntnu-bruker
char kitchenQueueKey[] = "22880"; //9
char kitchenTimerKey1[] = "17548"; //10
char kitchenTimerKey2[] = "1146"; //11
char kitchenTimerKey3[] = "6838"; //12

char bathQueueKey[] = "32349"; //13
char bathroomTimerKey[] = "16655"; //14

char guestsKey1[] = "28195"; //15

//keys på outlook-bruker
char guestsKey2[] = "22435"; //16
char guestsKey3[] = "13076";  //17
char guestsKey4[] = "17029";  //18
char guestsKey5[] = "21010";  //19

//ventelisteplasser med tid lagret
char kitchenQueueTimeKey1[] = "23933"; //20
char kitchenQueueTimeKey2[] = "3196"; //21

char bathQueueTimeKey1[] = "15932"; //22
char bathQueueTimeKey2[] = "10031"; //23
char bathQueueTimeKey3[] = "22177"; //24
char bathQueueTimeKey4[] = "21336"; //25

char pplAwayKey[] = "11096"; //26

//rfid-er
char rfidKey1[] = "24169"; //27
char rfidKey2[] = "17840"; //28
char rfidKey3[] = "14083"; //29
char rfidKey4[] = "4746"; //30
char rfidKey5[] = "7508"; //31


//---------------------------------------------------------------------------------------------------------------------------------------------------COT VERDIER DEKLARERING--------------------------
int pplHouseValue;
int pplBathroomValue;
int pplKitchenValue;
int pplOutsideValue;

int pplRoomValue5;
int pplRoomValue4;
int pplRoomValue3;
int pplRoomValue2;
int pplRoomValue1;

int kitchenTimerValue1;
int kitchenTimerValue2;
int kitchenTimerValue3;
int kitchenQueueValue;
int kitchenQueueTimeValue1;
int kitchenQueueTimeValue2;

int bathQueueValue;
int bathroomTimerValue;
int bathQueueTimeValue1;
int bathQueueTimeValue2;
int bathQueueTimeValue3;
int bathQueueTimeValue4;

int guestsValue1;
int guestsValue2;
int guestsValue3;
int guestsValue4;
int guestsValue5;
int pplAwayValue;

int rfidValue1;
int rfidValue2;
int rfidValue3;
int rfidValue4;
int rfidValue5;

//------------------------------------------------------------------------------------------------------------------------------------------LCD OPPSETT------------------

//lagrer antall kolonner og rader som lcd-en har i globale variabler
int lcdColumns = 16;
int lcdRows = 2;

//lager et lcd-objekt med adresse samt antall rader og kolonner. Dersom man ikke vet I2C-adressen, kjør skanner-sketchen
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);


//---------------------------------------------------------------------------------------------------------------------------------------------KEYPAD OPPSETT----------------

//antall rader og kolonner på keypaden
const byte ROWS = 4;
const byte COLS = 3;

//oversikt over alle keys på keypaden, må bruke enkle ' for char
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

//nummer på rad- og kolonnepinner
byte rowPins[ROWS] = {17, 18, 3, 19};
byte colPins[COLS] = {33, 32, 23};

//lager keypad-objekt som inneholder key-ene, rad- og kolonnepinner samt antallet rader og kolonner
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


//variabler for knappetrykk på keypaden som skal styre menyen
char activityKey;
char firstChoiceKey;
char secondChoiceKey;

//----------------------------------------------------------------------------------------------------------------------------------TEKST TIL MENYEN (GLOBALE)--------------------------------
//kun de beskjedene som brukes flere ganger, f. eks. innen booking, lagres som globale her oppe

//navn på de som bor der
String name1 = "Eline";
String name2 = "Julia";
String name3 = "Mathilde";
String name4 = "Sara";
String name5 = "Renate";
String peopleInHouse = "1: " + name1 + "2: " + name2 + "3: " + name3 + "4: " + name4 + "5: " + name5;

//Booking
String chooseRoomTitle = "Choose room";
String chooseRooms = "1:Kitchen 2:Bathroom";
String howLongToUse = "Time needed?";
String timeOptions = "1:15 min 2:30 min 3:45 min 4:60 min";

//disse tidene brukes både for bad og kjøkken
String min15 = "You got 15 min"; //tror ikke det funker å skrive tall først i variabelnavn
String min30 = "You got 30 min";
String min45 = "You got 45 min";
String min60 = "You got 60 min";


//-------------------------------------------------------------------------------------------------------------------------------------------------EGENDEFINERTE FUNKSJONER--------------------------
void resettingKeys3() { //sender brukeren tilbake til hovedmenyen
  activityKey = 0;
  firstChoiceKey = 0;
  secondChoiceKey = 0; //tas med, selv om ikke alle valg fører til tredje valgknappetrykk
}

void resettingKeys2() { //backspacing eller hovedmeny for kortere valgstrukturer
  activityKey = 0;
  firstChoiceKey = 0;
}

void resettingKeys1() { //backspacing
  firstChoiceKey = 0;
  secondChoiceKey = 0;
}

//har kun scrolling tekst på en linje, fordi det er mye å følge med på to linjer som kjører samtidig

void scrollingText(String message) { //tekst som scroller på andre linje

  for (int i = 0; i < message.length(); i++) {
    //Serial.println(i);
    lcd.clear(); //Kanskje ikke nødvendig
    for (int j = 0; j < lcdColumns; j++) {
      lcd.setCursor(j, 1);
      if (i + j >= message.length()) {
        lcd.print(" ");
      }
      lcd.print(message[i + j]);
    }
    delay(150);
  }
}

void twoStaticLines(String message1, String message2) { //to linjer som står stille på LCD-skjermen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message1);
  lcd.setCursor(0, 1);
  lcd.print(message2);
  delay(200);
}


void oneStaticLine(String message) { //statisk tekst på øverste linje på LCD-skjermen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(200);
}

//----------------------------------------------------------------------------------------------------------------------------------------ESPNOW MED BUZZER OPPSETT------------------------------

const int buzzerPin = 4; //pinnen buzzeren er koblet på
const int buzzerChannel = 0; //antakelig pwm-kanal

/*nøkkelordet struct brukes for å definere en struktur og inneholder typen data man ønsker å sende,
  må være lik mottakeren (receiver) sin. struct gjør at message_structure definereres i sturkturnavnsrommet.
  struct kan brukes til å definere variabler eller funksjonsargumenter av strukturen ved å definere
  typen av argumentet som struct message_structure. Mens typedef gir et alternativt navn til eksisterende
  datatyper. Istedenfor å skrive struct message_structure, kan man bare skrive message_structure. Typedef definerer
  message_structure globalt, altså er identifikatorenes name space/scope forkjellig*/

typedef struct message_structure { //verdien som skal mottas er et heltall
  int a;
} message_structure;

//mottakeren skal også ha variablen dataPackage med strukturen message_structure
message_structure dataPackage;

//tilbakemeldingsfunksjonen onDataRecv for ESP32-en som mottar dataPackage
//mac-adressen til den andre ESP32-en, dataene som mottas og lengden på dem er parametrene dens
void onDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  /*funksjonen void *memcpy (void* dest, const void* src, size_t n) står for antakelig for memory copy. incomingData kopieres
    fra minneområdet src til minnneområdet dest (&dataPackage) i tillegg til at den innholder størrelsen av dataPackage */
  memcpy(&dataPackage, incomingData, sizeof(dataPackage));

  //her printes dataene som er blitt motatt samt størrelsen til sammen (bytes)
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Int: ");
  Serial.println(dataPackage.a);
}

void beep(int note, int duration) { //lager bare kjapp funksjon slik at man slipper å skrive inn buzzerPin og buzzerChannel hver gang
  tone(buzzerPin, note, duration, buzzerChannel); //det er et innebygd delay i denne funksjonen som vil stoppe alle kodekjøring
}


void buzzerFunction() { //funksjon som får buzzeren til å spille og forteller personen som eier konsollen at han/hun har gjester
  String guestsComing = "You have guests";
  if (dataPackage.a == 1) { //det som sendes fra den andre ESP32-en er 1

    for (int i = 0; i < 5; i++) { //ringeklokkelyden (buzzeren) skal kjøres fem ganger før den stopper
      scrollingText(guestsComing);
      beep(NOTE_C6, 200);
      beep(NOTE_D6, 200);
      beep(NOTE_E6, 200);
      beep(NOTE_F6, 200);
      beep(NOTE_G6, 200);
      beep(NOTE_A6, 200);
      beep(NOTE_B6, 200);
    }

    dataPackage.a = 0; //da settes også dataPackage.a til 0 igjen
    Serial.println(dataPackage.a);
  }
}


//-----------------------------------------------------------------------------------------------------------------------------SETUP-----------------------------------------------------

void setup() {
  Serial.begin(115200); //starter den serielle monitoren
  pinMode(buzzerPin, OUTPUT); //buzzeren er en utgang
  WiFi.mode(WIFI_AP_STA); //setter enheten som både Wifi-stasjon og access point
  circusESP32.begin(); //initialiserer oppkobling mot CoT
  lcd.init(); //initialiserer LCD
  lcd.backlight(); //skrur på baklyset til LCD-en

  if (esp_now_init() != ESP_OK) { //initialisere ESP-NOW
    Serial.println("Error initializing ESP-NOW"); //hvis det ikke går, print feilmelding
    return;
  }

  //etter at ESP-NOW er initialisert, putter man inn onDataRecv-funksjonen over for å printe resultatet
  esp_now_register_recv_cb(onDataRecv);
}


//------------------------------------------------------------------------------------------------------------------------------------------------------------ROM STATUS-------------------------------------------
void roomStatus() { //roomStatus-funksjonen viser hvor mange som er i hvert rom
  String roomStatusTitle = "Room status";
  String roomStatusOptions = "1:Common rooms 2:People's rooms 3:Home/away";
  switch (firstChoiceKey) {
    case '1': {
        pplBathroomValue = circusESP32.read(pplBathroomKey, gmailToken); //leser verdier på fellesrommene
        pplKitchenValue = circusESP32.read(pplKitchenKey, gmailToken);

        String commonRooms = "Common rooms";
        int commonRoomsTotal = pplBathroomValue + pplKitchenValue;
        String commonRoomsNumbers = " Kitchen:" + String(pplKitchenValue) + " Bathroom:" + String(pplBathroomValue) + " Total:" + String(commonRoomsTotal);

        oneStaticLine(commonRooms);
        scrollingText(commonRoomsNumbers); //printer antallet folk i fellesrommene på LCD-en ut fra infoen over

        resettingKeys3(); //kommer tilbake til hovedmenyen
      }
      break;


    case '2': { //samme som over, bare for soverommene til hver av personene i kollektivet
        pplRoomValue5 = circusESP32.read(pplRoomKey5, gmailToken);
        pplRoomValue4 = circusESP32.read(pplRoomKey4, gmailToken);
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
        pplRoomValue2 = circusESP32.read(pplRoomKey2, gmailToken);
        pplRoomValue1 = circusESP32.read(pplRoomKey1, gmailToken);

        String peopleRooms = "People's rooms";
        int pplRoomsTotal = int(pplRoomValue1 + pplRoomValue2 + pplRoomValue3 + pplRoomValue4 + pplRoomValue5);
        String peopleRoomsNumbers = " 1:" + String(pplRoomValue1) + " 2:" + String(pplRoomValue2) + " 3:" + String(pplRoomValue3) + " 4:" + String(pplRoomValue4) + " 5:" + String(pplRoomValue5) + " Total: " + String(pplRoomsTotal);

        oneStaticLine(peopleRooms);
        scrollingText(peopleRoomsNumbers);
        resettingKeys3();
      }
      break;


    case '3': {
        pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
        rfidValue1 = circusESP32.read(rfidKey1, czarnockajuliaToken);
        rfidValue2 = circusESP32.read(rfidKey2, czarnockajuliaToken);
        rfidValue3 = circusESP32.read(rfidKey3, czarnockajuliaToken);
        rfidValue4 = circusESP32.read(rfidKey4, czarnockajuliaToken);
        rfidValue5 = circusESP32.read(rfidKey5, czarnockajuliaToken);
        //regner ut og kalibrerer antallet beboerne som ikke er hjemme
        int pplAway = 5 - int(rfidValue1 + rfidValue2 + rfidValue3 + rfidValue4 + rfidValue5); //dersom alle er borte er verdien fem, verdien 1 på rfiden betyr at folk er hjemme

        String homeAway = "Home/away"; //home gjelder alle i huset, mens away kun er beboerne som er borte
        String homeAwayNumbers = " House:" + String(pplHouseValue) + " Away:" + String(pplAway);
        oneStaticLine(homeAway);
        scrollingText(homeAwayNumbers);
        circusESP32.write(pplAwayKey, pplAway, outlookToken);

        resettingKeys3();
      }

      break;

    case '0':
      resettingKeys2(); //mulig backspacing

    default: {
        buzzerFunction(); //sjekker om noen ringer på, og hvis det er tilfellet spilles lyd på buzzer og man får beskjed på LCD-skjermen
        oneStaticLine(roomStatusTitle); //overskrift til romstatus

        for (int i = 0; i < roomStatusOptions.length(); i++) { //man kan velge å se antall personer i fellesrom, soverom eller som er borte/hjemme
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= roomStatusOptions.length()) {
              lcd.print(" ");
            }
            lcd.print(roomStatusOptions[i + j]);
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

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------KJØKKEN BOOKING-------------------------
void kitchenBooking() {
  String kitchenBooked = "Kitchen booked";
  switch (secondChoiceKey) {

      pplKitchenValue = circusESP32.read(pplKitchenKey, gmailToken); //skaffer verdien på antallet folk på kjøkkenet


    case '1': { //tilsvarer 15 minutter

        twoStaticLines(kitchenBooked, min15); //dette skal vises først, utenfor if-statementene på LCD-en ettersom jeg tror CoT fører til lagg
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken); //må  ha den under casen for å backspace uten å endre CoT-verdier

        circusESP32.write(pplRoomKey3, int(pplRoomValue3) - 1, gmailToken); //det blir en person mindre i rom 3, antar at vedkommende er på rommet sitt

        if (pplKitchenValue == 0) { //hvis det ikke er noen på kjøkkenet
          circusESP32.write(kitchenTimerKey1, 15, ntnuToken); //lagrer 15 min til person 1 på kjøkkenet, ettersom det er knappetrykk-kombinasjonen
          circusESP32.write(pplKitchenKey, 1, gmailToken); //legger til en person på kjøkkenet

        }
        else if (pplKitchenValue == 1) { //hvis det allerede er en person på kjøkkenet
          circusESP32.write(kitchenTimerKey2, 15, ntnuToken); //lagrer 15 min til person 2 på kjøkkenet
          circusESP32.write(pplKitchenKey, 2, gmailToken); //legger til en person på kjøkkenet

        }

        else if (pplKitchenValue == 2) { //hvis det allerede er to personer på kjøkkenet
          circusESP32.write(kitchenTimerKey3, 15, ntnuToken); //lagrer 15 min til person 3 på kjøkkenet
          circusESP32.write(pplKitchenKey, 3, gmailToken); //legger til at det er enda en person på kjøkkenet

        }

        //Serial.println("Du har booket kjøkkenet i 15 minutter");
        resettingKeys3();
      }
      break;

    case '2': { //tilsvarer 30 minutter
        twoStaticLines(kitchenBooked, min30);
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken); //må  ha den under casen for å backspace uten å endre CoT-verdier
        circusESP32.write(pplRoomKey3, int(pplRoomValue3) - 1, gmailToken); //det blir en person mindre i rom 3, antar at vedkommende er på rommet sitt

        if (pplKitchenValue == 0) { //hvis det ikke er noen personer på kjøkkenet
          circusESP32.write(kitchenTimerKey1, 30, ntnuToken); //lagrer 30 min til person 1 på kjøkkenet
          circusESP32.write(pplKitchenKey, 1, gmailToken); //legger til at det er en person på kjøkkenet
        }
        else if (pplKitchenValue == 1) { //hvis det allerede er en person på kjøkkenet
          circusESP32.write(kitchenTimerKey2, 30, ntnuToken); //legger til at person 2 skal bruke 30 min
          circusESP32.write(pplKitchenKey, 2, gmailToken); //sier at det er to person på kjøkkenet

        }

        else if (pplKitchenValue == 2) { //hvis det allerede er to personer på kjøkkenet
          circusESP32.write(kitchenTimerKey3, 30, ntnuToken); //legger til at person 3 skal bruke 30 min
          circusESP32.write(pplKitchenKey, 3, gmailToken); //sier at det er tre personer på kjøkkenet
        }

        //Serial.println("Du har booket kjøkkenet i 30 minutter");

        resettingKeys3();
      }
      break;

    case '3': { //tilsvarer 45 minutter
        twoStaticLines(kitchenBooked, min45);
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
        circusESP32.write(pplRoomKey3, int(pplRoomValue3) - 1, gmailToken);

        if (pplKitchenValue == 0) { //hvis det ikke er noen på kjøkkenet
          circusESP32.write(kitchenTimerKey1, 45, ntnuToken); //legger til at person 1 skal bruke 45 min på kjøkkenet
          circusESP32.write(pplKitchenKey, 1, gmailToken); //sier at det er en person på kjøkkenet

        }
        else if (pplKitchenValue == 1) { //hvis det er en person på kjøkkenet
          circusESP32.write(kitchenTimerKey2, 45, ntnuToken); //legger til at person 2 skal bruke 45 min på kjøkkenet
          circusESP32.write(pplKitchenKey, 2, gmailToken); //sier at det er to personer på kjøkkenet
        }

        else if (pplKitchenValue == 2) { //hvis allerede er tre personer på kjøkkenet
          circusESP32.write(kitchenTimerKey3, 45, ntnuToken); //legger til at person 3 skal bruke 45 min på kjøkkenet
          circusESP32.write(pplKitchenKey, 3, gmailToken); //sier at det er tre personer på kjøkkenet
        }

        //Serial.println("Du har booket kjøkkenet i 45 minutter");

        resettingKeys3();
      }
      break;

    case '4': { //tilsvarer 60 minutter
        twoStaticLines(kitchenBooked, min60);
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken); //må  ha den under casen for å backspace uten å endre CoT-verdier
        circusESP32.write(pplRoomKey3, int(pplRoomValue3) - 1, gmailToken); //det blir en person mindre i rom 3, antar at vedkommende er på rommet sitt

        if (pplKitchenValue == 0) { //hvis det ikker er noen på kjøkkenet
          circusESP32.write(kitchenTimerKey1, 60, ntnuToken); //setter 60 min på person 1
          circusESP32.write(pplKitchenKey, 1, gmailToken); //skriver at det er en person på kjøkkenet
        }
        else if (pplKitchenValue == 1) { //hvis det allerede er en person på kjøkkenet
          circusESP32.write(kitchenTimerKey2, 60, ntnuToken); //setter 60 min til person 2
          circusESP32.write(pplKitchenKey, 2, gmailToken); //skriver at det er to personer på kjøkkenet
        }

        else if (pplKitchenValue == 2) { //hvis det allerede er to personer på kjøkkenet
          circusESP32.write(kitchenTimerKey3, 60, ntnuToken); //setter 60 min til person 3
          circusESP32.write(pplKitchenKey, 3, gmailToken); //skriver at det er tre personer på kjøkkenet
        }

        //Serial.println("Du har booket kjøkkenet i 60 minutter");

        resettingKeys3(); //resetter alle knappetrykkene slik at man kommer tilbake til hovedmenyen igjen, gjøres på slutten av valgstrukturen
      }
      break;

    case '0': {
        resettingKeys1();
      }
      break;

    default: {
        buzzerFunction(); //den kommer opp også når man er i cases og trykker noe på begge konsollene samtidig med litt delay da for kan ikke skje helt samtidig
        oneStaticLine("Kitchen booking"); //bare for å kjapt vise hvilket rom man har valgt, ettersom bookingskjermene for kjøkken og bad er like
        lcd.clear();

        oneStaticLine(howLongToUse); //antall minutter man kan velge å booke kjøkkenet

        for (int i = 0; i < timeOptions.length(); i++) {
          lcd.clear(); //Kanskje ikke nødvendig
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= timeOptions.length()) {
              lcd.print(" ");
            }
            lcd.print(timeOptions[i + j]);
          }
          secondChoiceKey = keypad.getKey();
          if (!secondChoiceKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("secondChoiceKey pressed");
            break;
          }
        } //end if statement
      }
  } //end switch case
} //end kitchenBooking-funksjon
//------------------------------------------------------------------------------------------------------------------------------------------------------------BADEROM BOOKING-------------------------

void bathroomBooking() {
  String bathBooked = "Bath booked";

  switch (secondChoiceKey) {
    case '1': { //tilsvarer 15 minutter
        twoStaticLines(bathBooked, min15); //skriver at man har booket badet i 15 min

        //det er bare en person som kan bruke badet, siden dette ble filtrert bort i forrige funksjon, er det bare å skrive inn verdier
        circusESP32.write(bathroomTimerKey, 15, ntnuToken); //person skal bruke badet i 15 minutter
        circusESP32.write(pplBathroomKey, 1, gmailToken); //det er en person på badet
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken); //må  ha den under casen for å backspace uten å endre CoT-verdier
        circusESP32.write(pplRoomKey3, int(pplRoomValue3) - 1, gmailToken); //det blir en person mindre i rom 3, antar at vedkommende er på rommet sitt

        //Serial.println("Du har booket badet i 15 minutter");
        resettingKeys3();
      }
      break;


    case '2': { //tilsvarer 30 min
        twoStaticLines(bathBooked, min30); //skriver at man har booket badet i 30 min
        //må ha write under casen for å backspace uten å endre CoT-verdier
        circusESP32.write(bathroomTimerKey, 30, ntnuToken); //person skal bruke badet i 30 min, dette sendes til CoT
        circusESP32.write(pplBathroomKey, 1, gmailToken); //skriver til CoT at det er en person på badet
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
        circusESP32.write(pplRoomKey3, int(pplRoomValue3) - 1, gmailToken); //det blir en person mindre i rom 3, antar at vedkommende er på rommet sitt

        //Serial.println("Du har booket badet i 30 minutter");
        resettingKeys3();
      }
      break;

    case '3': { //når man har booket badet i 45 minutter
        twoStaticLines(bathBooked, min45);
        circusESP32.write(bathroomTimerKey, 45, ntnuToken);
        circusESP32.write(pplBathroomKey, 1, gmailToken);
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
        circusESP32.write(pplRoomKey3, int(pplRoomValue3) - 1, gmailToken); //det blir en person mindre i rom 3, antar at vedkommende er på rommet sitt
        //det kan være andre der fra før av

        //Serial.println("Du har booket badet i 45 minutter");

        resettingKeys3();
      }
      break;


    case '4': { //når man har booket badet i 60 minutter
        twoStaticLines(bathBooked, min60);
        //må ha dette under casen for å backspace uten å endre CoT-verdier og mindre lagg
        circusESP32.write(bathroomTimerKey, 60, ntnuToken);
        circusESP32.write(pplBathroomKey, 1, gmailToken);

        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
        circusESP32.write(pplRoomKey3, int(pplRoomValue3) - 1, gmailToken); //det blir en person mindre i rom 3, antar at vedkommende er på rommet sitt

        //Serial.println("Du har booket badet i 60 minutter");

        resettingKeys3();
      }
      break;


    case '0': {
        resettingKeys1(); //kun backspacing, ikke helt til hovedmeny her

      }
      break;

    default: {
        buzzerFunction(); //sjekker for om buzzer/ringing på døra
        oneStaticLine("Bathroom booking");  //bare for å kjapt vise hvilket rom man har valgt, ettersom bookingskjermene for kjøkken og bad er like
        lcd.clear();
        oneStaticLine(howLongToUse);

        for (int i = 0; i < timeOptions.length(); i++) { //antall minutter man kan booke badet
          lcd.clear(); //Kanskje ikke nødvendig
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= timeOptions.length()) {
              lcd.print(" ");
            }
            lcd.print(timeOptions[i + j]);
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
  }//end switch case
}
//end bathroomBooking-funksjon


//-----------------------------------------------------------------------------------------------------------------------------------------------------------ROM BOOKING VENTELISTE--------------------

/*år det gjelder venteliste er det beboerne som prioriteres, altså er antall plasser totalt 5 stk.
  for bad blir det 1 bruksplass og 4 ventelisteplasser, mens for kjøkken blir det 3 bruksplasser og 2 ventelisteplasser*/

void roomQueue() {
  String fullQueue = "The queue is full, try again later";
  String onBathQueue = "You're on the bathroom queue";
  String onKitchenQueue = "You're on the kitchen queue";

  switch (secondChoiceKey) {
    case '1': { //tilsvarer 15 min
        if (firstChoiceKey == '1') { //hvis man valgte kjøkken i forrige meny
          kitchenQueueValue = circusESP32.read(kitchenQueueKey, ntnuToken);
          if (kitchenQueueValue == 0) { //hvis det ikke er noen på venteliste fra før av
            scrollingText(onKitchenQueue); //forteller brukeren at man blir satt på venteliste
            circusESP32.write(kitchenQueueTimeKey1, 15, outlookToken); //skriver inn tiden man vil bruke
            circusESP32.write(kitchenQueueKey, 1, ntnuToken); //og at man er først på ventelista
          }
          else if (kitchenQueueValue == 1) { //hvis det allerede står en annen person på venteliste
            scrollingText(onKitchenQueue); //forteller brukeren at man blir satt på venteliste til kjøkkenet
            circusESP32.write(kitchenQueueTimeKey2, 15, outlookToken); //lagrer tiden man vil bruke
            circusESP32.write(kitchenQueueKey, 2, ntnuToken); //at man er nummer to på ventelista
          }
          else {
            scrollingText(fullQueue); //dersom en det er flere totalt på venteliste til kjøkken enn beboere i huset, dvs. at gjester har tatt køplasser
          }
        }

        else if (firstChoiceKey == '2') { //hvis man valgte badet i forrige meny, samme system for kjøkken bortsett fra at det er flere ventelisteplasser
          bathQueueValue = circusESP32.read(bathQueueKey, ntnuToken);
          if (bathQueueValue == 0) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey1, 15, outlookToken);
            circusESP32.write(bathQueueKey, 1, ntnuToken);
          }
          else if (bathQueueValue == 1) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey2, 15, outlookToken);
            circusESP32.write(bathQueueKey, 2, ntnuToken);
          }
          else if (bathQueueValue == 2) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey3, 15, outlookToken);
            circusESP32.write(bathQueueKey, 3, ntnuToken);
          }

          else if (bathQueueValue == 3) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey4, 15, outlookToken);
            circusESP32.write(bathQueueKey, 4, ntnuToken);
          }
          else {
            scrollingText(fullQueue);
          }
        }

        resettingKeys3(); //resetter keys i sistevalg, til hovedmenyen
      }
      break;

    case '2': { //tilsvarer 30 min
        if (firstChoiceKey == '1') { //for kjøkken
          kitchenQueueValue = circusESP32.read(kitchenQueueKey, ntnuToken);
          if (kitchenQueueValue == 0) {
            scrollingText(onKitchenQueue);
            circusESP32.write(kitchenQueueTimeKey1, 30, outlookToken);
            circusESP32.write(kitchenQueueKey, 1, ntnuToken);
          }
          else if (kitchenQueueValue == 1) {
            scrollingText(onKitchenQueue);
            circusESP32.write(kitchenQueueTimeKey2, 30, outlookToken);
            circusESP32.write(kitchenQueueKey, 2, ntnuToken);
          }
          else {
            scrollingText(fullQueue); //dersom en det er flere totalt på venteliste til kjøkken enn beboere i huset, dvs. at gjester har tatt køplasser
          }
        }

        else if (firstChoiceKey == '2') { //for bad
          bathQueueValue = circusESP32.read(bathQueueKey, ntnuToken);
          if (bathQueueValue == 0) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey1, 30, outlookToken); //det er key for circusESP32.write() og ikke value
            circusESP32.write(bathQueueKey, 1, ntnuToken);
          }
          else if (bathQueueValue == 1) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey2, 30, outlookToken);
            circusESP32.write(bathQueueKey, 2, ntnuToken);
          }
          else if (bathQueueValue == 2) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey3, 30, outlookToken);
            circusESP32.write(bathQueueKey, 3, ntnuToken);
          }

          else if (bathQueueValue == 3) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey4, 30, outlookToken);
            circusESP32.write(bathQueueKey, 4, ntnuToken);
          }
          else {
            scrollingText(fullQueue);
          }
        }

        resettingKeys3();
      }
      break;

    case '3': { //tilsvarer 45 min

        if (firstChoiceKey == '1') { //for kjøkken
          kitchenQueueValue = circusESP32.read(kitchenQueueKey, ntnuToken);
          if (kitchenQueueValue == 0) {
            scrollingText(onKitchenQueue);
            circusESP32.write(kitchenQueueTimeKey1, 45, outlookToken);
            circusESP32.write(kitchenQueueKey, 1, ntnuToken);
          }
          else if (kitchenQueueValue == 1) {
            scrollingText(onKitchenQueue);
            circusESP32.write(kitchenQueueTimeKey2, 45, outlookToken);
            circusESP32.write(kitchenQueueKey, 2, ntnuToken);
          }
          else {
            scrollingText(fullQueue); //dersom en det er flere totalt på venteliste til kjøkken enn beboere i huset, dvs. at gjester har tatt køplasser
          }
        }

        else if (firstChoiceKey == '2') { //for bad
          bathQueueValue = circusESP32.read(bathQueueKey, ntnuToken);
          if (bathQueueValue == 0) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey1, 45, outlookToken);
            circusESP32.write(bathQueueKey, 1, ntnuToken);
          }
          else if (bathQueueValue == 1) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey2, 45, outlookToken);
            circusESP32.write(bathQueueKey, 2, ntnuToken);
          }
          else if (bathQueueValue == 2) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey3, 45, outlookToken);
            circusESP32.write(bathQueueKey, 3, ntnuToken);
          }

          else if (bathQueueValue == 3) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey4, 45, outlookToken);
            circusESP32.write(bathQueueKey, 4, ntnuToken);
          }
          else {
            scrollingText(fullQueue);
          }
        }

        resettingKeys3();
      }
      break;
    case '4': { //tilsvarer 60 min

        if (firstChoiceKey == '1') { //ventelisteplasser for kjøkken
          kitchenQueueValue = circusESP32.read(kitchenQueueKey, ntnuToken);
          if (kitchenQueueValue == 0) {
            scrollingText(onKitchenQueue);
            circusESP32.write(kitchenQueueTimeKey1, 60, outlookToken);
            circusESP32.write(kitchenQueueKey, 1, ntnuToken);
          }
          else if (kitchenQueueValue == 1) {
            scrollingText(onKitchenQueue);
            circusESP32.write(kitchenQueueTimeKey2, 60, outlookToken);
            circusESP32.write(kitchenQueueKey, 2, ntnuToken);
          }
          else {
            scrollingText(fullQueue); //dersom en det er flere totalt på venteliste til kjøkken enn beboere i huset, dvs. at gjester har tatt køplasser
          }
        }

        else if (firstChoiceKey == '2') { //ventelisteplasser for bad
          bathQueueValue = circusESP32.read(bathQueueKey, ntnuToken);
          if (bathQueueValue == 0) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey1, 60, outlookToken);
            circusESP32.write(bathQueueKey, 1, ntnuToken);
          }
          else if (bathQueueValue == 1) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey2, 60, outlookToken);
            circusESP32.write(bathQueueKey, 2, ntnuToken);
          }
          else if (bathQueueValue == 2) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey3, 60, outlookToken);
            circusESP32.write(bathQueueKey, 3, ntnuToken);
          }

          else if (bathQueueValue == 3) {
            scrollingText(onBathQueue);
            circusESP32.write(bathQueueTimeKey4, 60, outlookToken);
            circusESP32.write(bathQueueKey, 4, ntnuToken);
          }
          else {
            scrollingText(fullQueue);
          }
        }

        resettingKeys3();
      }
      break;

    case '0': { //backspacing til firstChoiceKey og roomBooking
        resettingKeys1();
      }
      break;

    default: {
        buzzerFunction(); //sjekkes for buzzer/ringing på døra
        oneStaticLine("Queue booking"); //for å vise at man er sendt til ventelistebooking
        lcd.clear();
        oneStaticLine(howLongToUse);

        for (int i = 0; i < timeOptions.length(); i++) { //antall minutter man kan velge å booke senere
          lcd.clear(); //Kanskje ikke nødvendig
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= timeOptions.length()) {
              lcd.print(" ");
            }
            lcd.print(timeOptions[i + j]);
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
      } //end switch case
  } //end switch case
} //end roomBookingQueue-funksjon

//------------------------------------------------------------------------------------------------------------------------------------------------------------ROM BOOKING-------------------------

void roomBooking() { //funksjon for booking av rom

  switch (firstChoiceKey) {
    case '1': {
        pplKitchenValue = circusESP32.read(pplKitchenKey, gmailToken); //kun lese CoT-verdier her, kun skrive på slutten av valgstrukturen med tanke på å få til backspacing/cancel av valg
        if (pplKitchenValue < 3) {
          kitchenBooking(); //hvis det er under tre personer på kjøkkenet tas du videre til kitchenBooking undermenyen
        }

        else { //ellers havner de på venteliste
          roomQueue();
        }
      }
      break;

    case '2': {
        pplBathroomValue = circusESP32.read(pplBathroomKey, gmailToken); //CoT-verdien leses før den sjekkes mot kondisjon og settes under cacse for å ikke henge opp den serielle monitoren og få LCD-en til å ikke funke
        if (pplBathroomValue == 0) { //hvis det ikke er noen på badet sendes du til undermenyen for booking av baderom
          bathroomBooking();
        }
        else { //venteliste-funksjonen kalles
          roomQueue();
        }
      }
      break;
    case '0': {
        resettingKeys2(); //tilbake til hovedmeny
      }
      break;

    default: {
        buzzerFunction(); //sjekking av buzzer/gjester som ringer på døra
        oneStaticLine(chooseRoomTitle); //valg av rom kommer opp på LCD-skjermen

        for (int i = 0; i < chooseRooms.length(); i++) {
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= chooseRooms.length()) {
              lcd.print(" ");
            }
            lcd.print(chooseRooms[i + j]);
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
  } //end switch case
}//end roomBooking-funksjon
//--------------------------------------------------------------------------------------------------------------------------------STARTMENY (LOOP)------------------------------

void loop() { //hovedmenyen
  String mainMenuTitle = "Booking and ppl";
  String mainMenuOptions = "1:Room status 2:Booking";

  /* Menu for booking and people-LCD
    1: Room status
    2: Booking
  */

  switch (activityKey) { //0. ordens knappetrykkvariabel activityKey

    case '1': {
        roomStatus(); //funksjon for å se antall personer i hvert av rommene og som er hjemme/borte
      }
      break;

    case'2': {
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
        if ((pplRoomValue3 > 0) && (pplRoomValue3 <= 3)) { //det må være noen i rom 3 for at det skal kunne bookes derfra, kan bare bookes fra rom og det kan ikke være over 3 stk
          roomBooking();
        }
        else {
          scrollingText("The number of people in room 3 are zero, you can't book anything"); //det er det eneste realistiske, ettersom det ikke skal bli et negativt antall eller over tre personer på rommet
          activityKey = 0; //man sendes tilbake til hovedmenyen
        }
      }
      break;

    default: {
        buzzerFunction(); //sjekkes for buzzer/gjester på døra

        //statisk tekst med LCD-hovedmenytittelen øverst
        oneStaticLine(mainMenuTitle);

        for (int i = 0; i < mainMenuOptions.length(); i++) { //aktiviteter man kan velge mellom å gjøre
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= mainMenuOptions.length()) {
              lcd.print(" ");
            }
            lcd.print(mainMenuOptions[i + j]);
          }
          activityKey = keypad.getKey(); //lagrer knappetrykket i variablen
          /*if- og else-statementene er for at teksten skal slutte å scrolle og man skal komme ut av for-løkka en gang, slik at man kan
            bytte LCD-vising. Delayet er satt under if og break under else, avhengig av om det er registrert knappetrykk eller ikke*/

          if (!activityKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("activityKey pressed");
            break;
          }
        }
      }
  } //end switch case
} //end void loop
