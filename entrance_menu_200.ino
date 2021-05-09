//DENNE KONSOLLEN ER MONTERT VED INNGANGSPARTIET (SENDER)

//----------------------------------------------------------------------------------------------------------------------------------------------BIBLIOTEKER---------------
#include <Keypad.h> //tar med biblioteket for 4x3 keypaden
#include <LiquidCrystal_I2C.h>  //biblioteket til LCD-en
#include <CircusESP32Lib.h> //inkluderer CoT-biblioteket til ESP32
#include <esp_now.h> //biblioteker for ESPNOW
#include <WiFi.h> //wifi-bibliotek
//---------------------------------------------------------------------------------------------------------------------------------------ESPNOW (SENDER) OPPSETT--------------

//Mottakeradressen (booking/receiver)
//uint8_t broadcastAddress[] = {0x10, 0x52, 0x1C, 0x68, 0x9A, 0x7C}; //gammel ESP32
uint8_t broadcastAddress[] = {0xE8, 0xDB, 0x84, 0x02, 0x4A, 0x80}; //ny ESP32
int buzzerSent = 0;

/*nøkkelordet struct brukes for å definere en struktur og inneholder typen data man ønsker å sende,
  må være lik mottakeren (receiver) sin. struct gjør at message_structure definereres i sturkturnavnsrommet.
  struct kan brukes til å definere variabler eller funksjonsargumenter av strukturen ved å definere
  typen av argumentet som struct message_structure. Mens typedef gir et alternativt navn til eksisterende
  datatyper. Istedenfor å skrive struct message_stucture, kan man bare skrive message_structure. Typedef definerer
  message_structure globalt, altså er identifikatorenes name space/scope forkjellig*/

//det skal sendes et heltall mellom inngangen og bookingkonsollet til den enkelte beboer
typedef struct message_structure {
  int a;
} message_structure;

//lager variablen dataPackage av strukturen message_structure som skal lagre sendeverdiene
message_structure dataPackage;


//tilbakemedlingsfunksjon der mac_addr er adressen til den andre ESP32-en og status er om sendingen var suksess eller feil
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");

}

void sendingBuzzer() { //sender dataPackage via ESP-NOW til mottaker
  //verdi som skal sendes
  dataPackage.a = 1;
  esp_err_t result; //esp_err_t brukes for å initialisere ESP-NOW funksjoner

  //buzzerSent sørger for at dataPackage kun blir sendt en gang for å ringe på døra
  if (buzzerSent == 0) {
    result = esp_now_send(broadcastAddress, (uint8_t*)&dataPackage, sizeof(dataPackage));

    //print om sendingen var vellykket eller ikke
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    buzzerSent = 1;
    return;
  }
}

//---------------------------------------------------------------------------------------------------------------------------------------------COT OPPSETT---------------

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

char pplRoomKey5[] = "10125"; //4
char pplRoomKey4[] = "31088"; //5
char pplRoomKey3[] = "24553"; //6
char pplRoomKey2[] =  "29482"; //7
char pplRoomKey1[] = "21571"; //8

//keys på ntnu-bruker
char guestsKey1[] = "28195"; //15

//keys på outlook-bruker
char guestsKey2[] = "22435"; //16
char guestsKey3[] = "13076";  //17
char guestsKey4[] = "17029";  //18
char guestsKey5[] = "21010";  //19
char pplAwayKey[] = "11096";

//rfid-er
char rfidKey1[] = "24169"; //27
char rfidKey2[] = "17840"; //28
char rfidKey3[] = "14083"; //29
char rfidKey4[] = "4746"; //30
char rfidKey5[] = "7508"; //31


//---------------------------------------------------------------------------------------------------------------------------------------------------COT VERDIER DEKLARERING--------------------------

int pplHouseValue;
int pplRoomValue5;
int pplRoomValue4;
int pplRoomValue3;
int pplRoomValue2;
int pplRoomValue1;

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


//-----------------------------------------------------------------------------------------------------------------------------------------------OPPSETT FOR LCD----------------

//lagrer antall kolonner og rader som lcd-en har i globale variabler
int lcdColumns = 16;
int lcdRows = 2;

//lager et lcd-objekt med adresse sant antall rader og kolonner. Dersom man ikke vet I2C-adressen, kjør skanner-sketchen
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);


//---------------------------------------------------------------------------------------------------------------------------------------------OPPSETT FOR KEYPAD----------------

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
char guestNumber;
int guestNumberInt = 0; //lagrer guestNumber-verdien som heltall, int('1') != 1 hvis man ser ASCII-tabellen


//----------------------------------------------------------------------------------------------------------------------------------TEKST TIL MENYEN (GLOBALE)--------------------------------

//navn på de som bor der
String name1 = "Eline";
String name2 = "Julia";
String name3 = "Mathilde";
String name4 = "Sara";
String name5 = "Renate";
String peopleInHouse = "1:" + name1 + " 2:" + name2 + " 3:" + name3 + " 4:" + name4 + " 5:" + name5;


//----------------------------------------------------------------------------------------------------------------------------------------------------------SETUP-------------------------------

void setup() {
  Serial.begin(115200); //starter den serielle monitoren
  circusESP32.begin(); //initialiserer oppkobling mot CoT
  lcd.init(); //initialiserer LCD
  lcd.backlight(); //skrur på baklyset til LCD-en

  //-------------------------------------------------------------------------------------------------------------------------------------------------------ESPNOW SETUP--------------------------------
  WiFi.mode(WIFI_STA); //setter enheten som en wifi-stasjon

  if (esp_now_init() != ESP_OK) { //initialisere ESP-NOW
    Serial.println("Error initializing ESP-NOW"); //hvis det ikke går, print feilmelding
    return;
  }

  //putter inn den funksjonen man vil skal fortelle hvordan sendingen gikk i esp_now_register_send_cb
  esp_now_register_send_cb (onDataSent);

  //parer ESP32-enhetene
  esp_now_peer_info_t peerInfo;

  /*funksjonen void *memcpy (void* dest, const void* src, size_t n) står for antakelig for memory copy og
    gjør at det kopieres seks karakterer (broadcastAddress) fra minneområdet src til minnneområdet dest (peerInfo.peer_addr)*/
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);

  //velger kanal 0 og at det ikke skal krypteres
  peerInfo.channel = 0;
  peerInfo.encrypt = false;


  //hvis det å pare enhetene ved bruk av minneaddressen &peerInfo ikke går som den skal, send feilmelding
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

}


//-----------------------------------------------------------------------------------------------------------------------------------------------------EGENDEFINERTE FUNKSJONER--------------------------
void resettingKeys3() { //sender brukeren tilbake til hovedmenyen
  activityKey = 0;
  firstChoiceKey = 0;
  secondChoiceKey = 0; //brukes noen ganger istedenfor resettingKeys2, selv om ikke alle valg fører til tredje valgknappetrykk
}

void resettingKeys2() { //backspacing eller helt tilbake til hovedmeny avhengig av antall delvalg
  activityKey = 0;
  firstChoiceKey = 0;
}


void scrollingText(String message) { //kode for å skrolle tekst på linje to med over 16 karakterer

  for (int i = 0; i < message.length(); i++) {
    //Serial.println(i);
    lcd.clear();
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


void twoStaticLines(String message1, String message2) { //statisk tekst på begge linjer av LCD-en
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message1);
  lcd.setCursor(0, 1);
  lcd.print(message2);
  delay(200);
}

void oneStaticLine(String message) { //statisk tekst på øverste LCD-linje
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  delay(200);
}

void resettingKeys1() { //virker kun som tilbakeknapp
  firstChoiceKey = 0;
  secondChoiceKey = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------GJESTER SOM ANKOMMER------------------------

void guestsComing() { //funksjon som kalles når det kommer gjester
  String comeBackLater = "Come back later"; //lokale strenger til undermenyen
  String personVisiting = "Person visiting?"; //når gjesten kommer


  switch (secondChoiceKey) { //knappetrykket lagres i secondChoiceKey (2.orden) og gir ulike cases


    case '1': { //dersom man skal besøke person 1
        guestsValue1 = circusESP32.read(guestsKey1, ntnuToken); //leser antallet gjester til person 1
        rfidValue1 = circusESP32.read(rfidKey1, czarnockajuliaToken); //sjekker om person 1 er hjemme

        if ( ((guestsValue1 + guestNumberInt) <= 2) && (rfidValue1 == 1)  ) { //person det ringes på hos må være hjemme og kan ikke ha over to gjester
          String callingName1 = "Calling " + name1 + "..";
          scrollingText(callingName1); //står på LCD-en at personen ringes
          sendingBuzzer(); //tallet 1 blir sendt til bookingkonsollet på rommet til person 1, som tilsvarer at buzzeren skal ringe
          buzzerSent = 0; //buzzerSent blir resatt, er kun for å unngå at signalet blir sendt flere ganger om gangen

          circusESP32.write(guestsKey1, int(guestsValue1 + guestNumberInt), ntnuToken); //dette blir nytt antall gjester for person 1
          pplRoomValue1 = circusESP32.read(pplRoomKey1, gmailToken); //må lese av antall person på rom 1 først
          circusESP32.write(pplRoomKey1, int(pplRoomValue1 + guestNumberInt), gmailToken); //anta at gjesten i utgangspunket skal rett på rommet til den han/hun besøker og legger til
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + guestNumberInt), gmailToken); //i tillegg blir det flere i huset
        }
        else {
          oneStaticLine(comeBackLater); //man må komme tilbake senere dersom gjesteantallet er fullt eller person man vil besøke ikke er hjemme
        }

        resettingKeys3(); //resetter keyene slik at man kommer tilbake til hovedmenyen, ettersom det er siste valg
        guestNumber = 0; //i tillegg resettes guestNumber-knappetrykket
      }
      break;

    case '2': {
        guestsValue2 = circusESP32.read(guestsKey2, outlookToken);
        rfidValue2 = circusESP32.read(rfidKey2, czarnockajuliaToken);

        if ( ((guestsValue2 + guestNumberInt) <= 2) && (rfidValue2 == 1) ) { //personen må være hjemme for å kunne åpne døra for gjest
          String callingName2 = "Calling " + name2 + "..";
          scrollingText(callingName2);
          sendingBuzzer();
          buzzerSent = 0;

          circusESP32.write(guestsKey2, int(guestsValue2 + guestNumberInt), outlookToken);
          pplRoomValue2 = circusESP32.read(pplRoomKey2, gmailToken);
          circusESP32.write(pplRoomKey2, int(pplRoomValue2 + guestNumberInt), gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + guestNumberInt), gmailToken); //i tillegg blir det flere i huset
        }
        else {
          oneStaticLine(comeBackLater); //dersom det er fullt med gjester, må han/hun komme tilbake senere. Tenker det blir unødvendig å booke tid mtp. reisevei og at vedkommende ikke har noe sted å være mens venter
        }

        resettingKeys3();
        guestNumber = 0;
      }
      break;

    case '3': {

        guestsValue3 = circusESP32.read(guestsKey3, outlookToken);
        rfidValue3 = circusESP32.read(rfidKey3, czarnockajuliaToken);

        if ( ((guestsValue3 + guestNumberInt) <= 2) && (rfidValue3 == 1) ) {

          String callingName3 = "Calling " + name3 + "..";
          scrollingText(callingName3);
          sendingBuzzer();
          buzzerSent = 0;

          circusESP32.write(guestsKey3, int(guestsValue3 + guestNumberInt), outlookToken);
          pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
          circusESP32.write(pplRoomKey3, int(pplRoomValue3 + guestNumberInt), gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + guestNumberInt), gmailToken); //i tillegg blir det flere i huset

        }
        else {
          oneStaticLine(comeBackLater);
        }

        resettingKeys3();
        guestNumber = 0;
      }
      break;

    case '4': {
        guestsValue4 = circusESP32.read(guestsKey4, outlookToken);
        rfidValue4 = circusESP32.read(rfidKey4, czarnockajuliaToken);

        if ( ((guestsValue4 + guestNumberInt) <= 2) && (rfidValue4 == 1) ) {
          String callingName4 = "Calling " + name4 + "..";
          scrollingText(callingName4);
          sendingBuzzer();
          buzzerSent = 0;

          circusESP32.write(guestsKey4, int(guestsValue4 + guestNumberInt), outlookToken);
          pplRoomValue4 = circusESP32.read(pplRoomKey4, gmailToken);
          circusESP32.write(pplRoomKey4, int(pplRoomValue4 + guestNumberInt), gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + guestNumberInt), gmailToken); //i tillegg blir det flere i huset
        }
        else {
          oneStaticLine(comeBackLater);
        }

        resettingKeys3();
        guestNumber = 0;
      }
      break;

    case '5': {
        guestsValue5 = circusESP32.read(guestsKey5, outlookToken);
        rfidValue5 = circusESP32.read(rfidKey5, czarnockajuliaToken);

        if ( ((guestsValue5 + guestNumberInt) <= 2) && (rfidValue5 == 1) ) {

          String callingName5 = "Calling " + name5 + "..";
          scrollingText(callingName5);
          sendingBuzzer();
          buzzerSent = 0; //buzzerSent resettes slik at andre kan ringe på en gang senere

          circusESP32.write(guestsKey5, int(guestsValue5 + guestNumberInt), outlookToken);
          pplRoomValue5 = circusESP32.read(pplRoomKey5, gmailToken);
          circusESP32.write(pplRoomKey5, int(pplRoomValue5 + guestNumberInt), gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + guestNumberInt), gmailToken);


        }
        else {
          oneStaticLine(comeBackLater);
        }

        resettingKeys3();
        guestNumber = 0;
      }
      break;
    case '0':
      //0 er ikke noen av casene og derfor havner det i default
      resettingKeys1();
      guestNumber = 0; //setter også guestNumber til null her, ettersom valgstrukturen her er litt annerledes og man ikke husker tidligere tall som er lagret når backspacer


    default: { //på LCD-skjermen kommer det opp personer man kan besøke
        oneStaticLine(personVisiting);

        for (int i = 0; i < peopleInHouse.length(); i++) {
          //Serial.println(i);
          lcd.clear(); //Kanskje ikke nødvendig
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= peopleInHouse.length()) {
              lcd.print(" ");
            }
            lcd.print(peopleInHouse[i + j]);
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
  } //end switch case
} //end funksjon

//----------------------------------------------------------------------------------------------------------------------------------------------------------GJESTER SOM DRAR-----------------------


void guestsLeaving() {
  String personVisited = "Person visited ? "; //når gjesten drar igjen
  String seeYouSoon = "See you soon"; //sparer å skrive alle mellomrommene og " "

  switch (secondChoiceKey) {

    case '1': {
        guestsValue1 = circusESP32.read(guestsKey1, ntnuToken); //tror verdiene her er default int
        //guestNumber er lagret fra forrige gang som en char, derfor har man guestNumberInt
        if ((guestsValue1 - guestNumberInt) >= 0) { //for å unngå at det blir et negativt antall gjester
          oneStaticLine(seeYouSoon);

          circusESP32.write(guestsKey1, int(guestsValue1 - guestNumberInt), ntnuToken); //subtrahere antallet gjester med de som drar som blir ny guestValue1
          pplRoomValue1 = circusESP32.read(pplRoomKey1, gmailToken);
          circusESP32.write(pplRoomKey1, int(pplRoomValue1 - guestNumberInt), gmailToken);  //gjesten blir fra fjernet antall personer på det rommet. Gjesten var i hovedsak/utgangspunktet på rommet som han/hun besøker
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue - guestNumberInt), gmailToken); //i tillegg trekkes gjesten fra personer i huset
        }
        resettingKeys3();
        guestNumber = 0; //null er heller ingen av valgene for gjester
      }
      break;

    case '2': { //senere cases går ut på det samme bare for person 2, 3 osv..
        guestsValue2 = circusESP32.read(guestsKey2, outlookToken);
        if ((guestsValue2 - guestNumberInt) >= 0) { //for å unngå at det blir et negativt antall gjester
          oneStaticLine(seeYouSoon);

          circusESP32.write(guestsKey2, int(guestsValue2 - guestNumberInt), outlookToken);
          pplRoomValue2 = circusESP32.read(pplRoomKey2, gmailToken);
          circusESP32.write(pplRoomKey2, int(pplRoomValue2 - guestNumberInt), gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue - guestNumberInt), gmailToken);
        }
        resettingKeys3();
        guestNumber = 0;
      }
      break;

    case '3': {
        guestsValue3 = circusESP32.read(guestsKey3, outlookToken);
        if ((guestsValue3 - guestNumberInt) >= 0) {
          oneStaticLine(seeYouSoon);

          circusESP32.write(guestsKey3, int(guestsValue3 - guestNumberInt), outlookToken);
          pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
          circusESP32.write(pplRoomKey3, int(pplRoomValue3 - guestNumberInt), gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue - guestNumberInt), gmailToken);
        }
        resettingKeys3();
        guestNumber = 0;
      }
      break;

    case '4': {
        guestsValue4 = circusESP32.read(guestsKey4, outlookToken);
        if ((guestsValue4 - guestNumberInt) >= 0) {
          oneStaticLine(seeYouSoon);

          circusESP32.write(guestsKey4, int(guestsValue4 - guestNumberInt), outlookToken);
          pplRoomValue4 = circusESP32.read(pplRoomKey4, gmailToken);
          circusESP32.write(pplRoomKey4, int(pplRoomValue4 - guestNumberInt), gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue - guestNumberInt), gmailToken);
        }
        resettingKeys3();
        guestNumber = 0;
      }
      break;


    case '5': {
        guestsValue5 = circusESP32.read(guestsKey5, outlookToken);
        if ((guestsValue5 - guestNumberInt) >= 0) {
          oneStaticLine(seeYouSoon);

          circusESP32.write(guestsKey5, int(guestsValue5 - guestNumberInt), outlookToken);
          pplRoomValue5 = circusESP32.read(pplRoomKey5, gmailToken);
          circusESP32.write(pplRoomKey5, int(pplRoomValue5 - guestNumberInt), gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue - guestNumberInt), gmailToken);
        }
        resettingKeys3();
        guestNumber = 0;
      }
      break;

    case '0': { //hvis man trykker på '0' i secondChoiceKey blir både det man har gjort i switchen med secondChoiceKey og den for firstChoiceKey satt til 0. Dermed havner man i defaulten til firstChoiceKey og backspacer dermed
        //0 er ikke noen av casene og derfor havner det i default, derfor 0 og ikke '0'
        resettingKeys1();
        guestNumber = 0; //setter også guestNumber til null her, ettersom valgstrukturen her er litt annerledes og man ikke husker tidligere tall som er lagret når backspacer

      }
      break;

    default: { //personene man kan ha besøkt kommer opp på skjermen
        oneStaticLine(personVisited);

        for (int i = 0; i < peopleInHouse.length(); i++) {
          //Serial.println(i);
          lcd.clear();
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= peopleInHouse.length()) {
              lcd.print(" ");
            }
            lcd.print(peopleInHouse[i + j]);
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

  } //end switch case
} //end funksjon



//---------------------------------------------------------------------------------------------------------------------------------------ANTALL GJESTER----------------------------------------

void howManyGuests() {
  String numberOfGuests = "Number of guests"; //selv om jeg bare bruker det en gang er det greit å ha menyteksten på toppen også mtp. lesing altså
  String numberOfGuestsAnswer = "1 or 2";
  String fullHouse = "The house is full";

  switch (guestNumber) { //guestNumber er en char
    case '1': {

        if (firstChoiceKey == '1') { //guestComing-funksjonen kjøres med en gang dersom innenfor antallet personer i huset, denne verdien er lagret fra forrige valg
          //dersom det er en gjest lagres dette antallet i guestNumberInt = 1
          guestNumberInt = 1; //fordi int('1') = 49 og ikke 1, hvis man ser ASCII-tabellen

          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          if ((pplHouseValue + guestNumberInt) < 16) { //det kan bare komme gjester dersom huset ikke er fullt
            guestsComing();

          }
          else {
            guestNumberInt = 0;
            scrollingText(fullHouse); //får beskjed om at huset er fullt
            resettingKeys3();
          }
        }
        else if (firstChoiceKey == '2') { //guestLeaving-funksjonen kjøres, denne verdien er laget fra forrige valg
          guestsLeaving();
        }
      }
      break;

    case '2': { //setter guestNumber = '2', lagres ved knappetrykket

        if (firstChoiceKey == '1') { //knappetrykket til firstChoiceKey er lagret fra forrige gang

          guestNumberInt = 2; //antallet gjester, int('2') = 50 og ikke 2 hvis man ser ASCII-tabellen
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken); //sjekker at huset ikke blir fullt ved flere gjester
          if ( (pplHouseValue + guestNumberInt) < 16) {
            guestsComing();
          }
          else {
            guestNumberInt = 0; //resetter verdien her
            scrollingText(fullHouse);
            resettingKeys3();

          }
        }
        else if (firstChoiceKey == '2') {
          guestsLeaving(); //kaller på funksjonen for at gjesten drar
        }
      }
      break;

    case '0':
      resettingKeys2();
      guestNumber = 0;
      guestNumberInt = 0;

    default: {
        //printer LCD-skjerm der man skriver inn antallet gjester, maks to stk
        lcd.clear();
        twoStaticLines(numberOfGuests, numberOfGuestsAnswer);
        //her er teksten på andre linje såpass kort at den uoffisielle scrollingText-funksjonen ikke trengs

        guestNumber = keypad.getKey();
        if (!guestNumber) {
          delay(150);
        } //end if statement

        else {
          Serial.println("guestNumber pressed");
          break;

        } //end else
      }
  } //end switch case
} //end howManyGuests-funskjon


//---------------------------------------------------------------------------------------------------------------------------------------------GJESTER-----------------------------

void guests() { //funksjonen guests er et 1.ordens valg under aktiviteten: gjester
  String guestOptionsTitle = "Guest options";
  String guestOptions = "1:Coming 2:Leaving";
  switch (firstChoiceKey) { //uansett om man velger "coming" eller "leaving" blir man sendt til valgskjermen/funksjonen for antallet gjester
    case '1': { //gjester som kommer, her lagres firstChoiceKey = '1'. Skal bruke disse verdiene videre
        howManyGuests(); 
      }
      break;

    case '2': { //firstChoiceKey = '2', gjester som drar

        howManyGuests();
      }
      break;

    case '0':
      resettingKeys2(); //backspacing

    default: {
        //LCD-skjermen med guestOptions tilsvarer om gjesten(e) kommer eller drar
        oneStaticLine(guestOptionsTitle);

        for (int i = 0; i < guestOptions.length(); i++) {
          //Serial.println(i);
          lcd.clear(); 
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= guestOptions.length()) {
              lcd.print(" ");
            }
            lcd.print(guestOptions[i + j]);
          }

          firstChoiceKey = keypad.getKey();
          if (!firstChoiceKey) {
            delay(150);

          } //end if statement
          else {
            Serial.println("firstChoiceKey pressed");
            break;
          } //end else
        }
      }

  } //end switch case

} //end guests-funksjon

//-------------------------------------------------------------------------------------------------------------------------------------------------BEBOERE----------------------

void residents() {
  //lokale strenger for imHome-menyen som skal vises på LCD-en
  String whoIsThere = "Who's there?"; //konsollen ved inngangsdøra vet i utgangspunktet ikke hvem som er beboer, gjest eller skiller mellom ulike personer
  String scanRFID = "Scan your RFID";
  String welcomeHome = "Welcome home ";
  String anotherDayInLife = "Make the most out of your day";

  switch (firstChoiceKey) {
    case '1': { //hvis man er person 1 i kollektivet trykker man på knapp en
        oneStaticLine(scanRFID); //man får beskjed om å skanne RFID-en sin
        rfidValue1 = circusESP32.read(rfidKey1, czarnockajuliaToken); //leser rfidValue1
        delay(1000); //delay for å få tid til å skanne rfid-en før if statement
        pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
        pplRoomValue1 = circusESP32.read(pplRoomKey1, gmailToken);
        if (rfidValue1 == 1) { //hvis denne verdien blir 1 er rfid-en skannet og verdiene telles

          //velkommen beskjeden med navn burde alltid være med scrolling- for å unngå at lange navn ikke synes
          scrollingText(welcomeHome + name1); //velkommen-beskjed til beboer 1
          circusESP32.write(pplRoomKey1, 1, gmailToken); //det skal i utgangspunktet ikke gå an å være på noens rom uten at den som bor der er hjemme
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + 1), gmailToken); //i tillegg blir det enda en person i huset
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue - 1), outlookToken); //og det blir en mindre person som er borte
        }
        else if ((rfidValue1 == 0) && ((pplHouseValue - pplRoomValue1) >= 0)) { //0 på rfid-en betyr at personen forlater huset
          scrollingText(anotherDayInLife);
          circusESP32.write(pplHouseKey, int(pplHouseValue - pplRoomValue1), gmailToken); //det blir mellom 1 og 3 personer færre i kollektivet
          circusESP32.write(pplRoomKey1, 0, gmailToken); //dersom den som bor der drar, må alle eventuelle gjester også det
          circusESP32.write(guestsKey1, 0, ntnuToken); 
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue + 1), outlookToken);



        }

        resettingKeys3(); //hadde holdt å resette 2 keys her,  men tar med alle
      }
      break;

    case '2': { //hvis man er person 2 i kollektivet trykker man på knapp to
        oneStaticLine(scanRFID); //man får beskjed om å skanne RFID-en sin
        rfidValue2 = circusESP32.read(rfidKey2, czarnockajuliaToken); //leser rfidValue2
        delay(1000);
        pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
        pplRoomValue2 = circusESP32.read(pplRoomKey2, gmailToken);

        if (rfidValue2 == 1) { //hvis denne verdien blir 1 kommer beboeren hjem
          scrollingText(welcomeHome + name2); //velkommen-beskjed til beboer 2
          circusESP32.write(pplRoomKey2, 1, gmailToken);
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + 1), gmailToken); //i tillegg blir det enda en person i huset
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue - 1), outlookToken); //og det blir en mindre person som er borte
        }

        else if ((rfidValue2 == 0) && ((pplHouseValue - pplRoomValue2) >= 0)) {
          scrollingText(anotherDayInLife);
          circusESP32.write(pplHouseKey, int(pplHouseValue - pplRoomValue2), gmailToken); //det blir mellom 1 og 3 personer færre i kollektivet
          circusESP32.write(pplRoomKey2, 0, gmailToken); //dersom den som bor der drar, må alle eventuelle gjester også det
          circusESP32.write(guestsKey2, 0, outlookToken);
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue + 1), outlookToken);
        }

        resettingKeys3();
      }
      break;

    case '3': { //hvis man er person 3 i kollektivet trykker man på knapp tre
        oneStaticLine(scanRFID); //man får beskjed om å skanne RFID-en sin
        rfidValue3 = circusESP32.read(rfidKey3, czarnockajuliaToken); //leser rfidValue3
        delay(1000);
        pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
        pplRoomValue3 = circusESP32.read(pplRoomKey3, gmailToken);
        
        if (rfidValue3 == 1) { //hvis denne verdien blir 1
          scrollingText(welcomeHome + name3);
          circusESP32.write(pplRoomKey3, 1, gmailToken); //det skal i utgangspunktet ikke gå an å være på noens rom uten at den som bor der er hjemme
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + 1), gmailToken); //i tillegg blir det enda en person i huset
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue - 1), outlookToken); //og det blir en mindre person som er borte

        }

        else if ((rfidValue3 == 0) && ((pplHouseValue - pplRoomValue3) >= 0)) {
          scrollingText(anotherDayInLife);

          circusESP32.write(pplHouseKey, int(pplHouseValue - pplRoomValue3), gmailToken); //det blir mellom 1 og 3 personer færre i kollektivet
          circusESP32.write(pplRoomKey3, 0, gmailToken); //dersom den som bor der drar, må alle eventuelle gjester også det
          circusESP32.write(guestsKey3, 0, outlookToken);
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue + 1), outlookToken); //må til for at det pplAway skal telles uten å kjøre roomStatus-funksjonen
        }

        resettingKeys3();
      }
      break;

    case '4': { //hvis man er person 4 i kollektivet trykker man på knapp fire
        oneStaticLine(scanRFID); //man får beskjed om å skanne RFID-en sin
        rfidValue4 = circusESP32.read(rfidKey4, czarnockajuliaToken); //leser rfidValue4
        delay(1000); //delay for at man skal få tid til å skanne ID-en sin, selv om CoT lagger mye mer enn det
        pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
        pplRoomValue4 = circusESP32.read(pplRoomKey4, gmailToken);

        if (rfidValue4 == 1) { //hvis denne verdien blir 1
          scrollingText(welcomeHome + name4);
          circusESP32.write(pplRoomKey4, 1, gmailToken); //det skal i utgangspunktet ikke gå an å være på noens rom uten at den som bor der er hjemme
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + 1), gmailToken); //i tillegg blir det enda en person i huset
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue - 1), outlookToken); //og det blir en mindre person som er borte
        }

        else if ((rfidValue4 == 0) && ((pplHouseValue - pplRoomValue4) >= 0)) {
          scrollingText(anotherDayInLife);

          circusESP32.write(pplHouseKey, int(pplHouseValue - pplRoomValue4), gmailToken); //det blir mellom 1 og 3 personer færre i kollektivet
          circusESP32.write(pplRoomKey4, 0, gmailToken); //dersom den som bor der drar, må alle eventuelle gjester også det
          circusESP32.write(guestsKey4, 0, outlookToken);
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue + 1), outlookToken);
        }

        resettingKeys3();
      }
      break;


    case '5': { //hvis man er person 5 i kollektivet trykker man på knapp fem
        oneStaticLine(scanRFID); //man får beskjed om å skanne RFID-en sin
        rfidValue5 = circusESP32.read(rfidKey5, czarnockajuliaToken); //leser rfidValue5
        delay(1000);
        pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
        pplRoomValue5 = circusESP32.read(pplRoomKey5, gmailToken);


        if (rfidValue5 == 1) { 
          scrollingText(welcomeHome + name5);
          circusESP32.write(pplRoomKey5, 1, gmailToken); //det skal i utgangspunktet ikke gå an å være på noens rom uten at den som bor der er hjemme
          pplHouseValue = circusESP32.read(pplHouseKey, gmailToken);
          circusESP32.write(pplHouseKey, int(pplHouseValue + 1), gmailToken); //i tillegg blir det enda en person i huset
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue - 1), outlookToken); //og det blir en mindre person som er borte
        }

        else if ( (rfidValue5 == 0) && ((pplHouseValue - pplRoomValue5) >= 0)) { //det skal ikke være mulig at flere personer enn det som er i kollektivet forlater huset
          scrollingText(anotherDayInLife);
          circusESP32.write(pplHouseKey, int(pplHouseValue - pplRoomValue5), gmailToken); //det blir mellom 1 og 3 personer færre i kollektivet
          circusESP32.write(pplRoomKey5, 0, gmailToken); //dersom den som bor der drar, må alle eventuelle gjester også det
          circusESP32.write(guestsKey5, 0, outlookToken);
          pplAwayValue = circusESP32.read(pplAwayKey, outlookToken);
          circusESP32.write(pplAwayKey, int(pplAwayValue + 1), outlookToken);
        }

        resettingKeys3();
      }
      break;

    case '0':
      resettingKeys2();

    default: { //konsollen spør hvem av beboerne som er ved døra
        oneStaticLine(whoIsThere);

        for (int i = 0; i < peopleInHouse.length(); i++) {
          lcd.clear(); //kanskje ikke nødvendig
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= peopleInHouse.length()) {
              lcd.print(" ");
            }
            lcd.print(peopleInHouse[i + j]);
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
}//end imHome-funksjon


//------------------------------------------------------------------------------------------------------STARTMENY (LOOP)----------------------------------------------------
void loop() {
  String mainMenuTitle = "By the entrance";
  String mainMenuOptions = "1:Residents 2:Guests"; //hovedmeny for inngangsparti-LCD

  switch (activityKey) { //knappetrykket lagres i activityKey

    case '1': {

        residents(); //funksjon som kalles når de som bor der er ved inngangsdøra
      }
      break;

    case '2': { //funksjon som kalles for gjester
        guests();
      }
      break;

    default: { 

        //statisk tekst med LCD-hovedmenytittelen øverst
        oneStaticLine(mainMenuTitle); 

        for (int i = 0; i < mainMenuOptions.length(); i++) { //her vises de ulike valgene, om man er gjest eller bor der
          //Serial.println(i); 
          lcd.clear(); 
          for (int j = 0; j < lcdColumns; j++) {
            lcd.setCursor(j, 1);
            if (i + j >= mainMenuOptions.length()) {
              lcd.print(" ");
            }
            lcd.print(mainMenuOptions[i + j]);
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
  } //end switch case
} //end void loop
