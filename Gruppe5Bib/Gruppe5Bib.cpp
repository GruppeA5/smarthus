#include "Arduino.h"
#include "Gruppe5Bib.h"
#include "analogWrite.h"

Gruppe5Bib::Gruppe5Bib()  {

}

float Gruppe5Bib::LED_Average(uint8_t *lyssensor_PIN) {    //DEN ER FERDIG
  pinMode(lyssensor_PIN, INPUT);
  float LED_ValueFunk = 0; //Startvariabel er 0
  for (int i=0;i<4;i++) { //samme som ovenfor, fem målinger
    LED_ValueFunk += analogRead(lyssensor_PIN); //leser av sensorpin
    delay (1000);
  }
  return LED_ValueFunk /5; //samme som ovenfor  
}


float Gruppe5Bib::tempAverage(unint8_t *temp_PIN) {
  pinMode(temp_PIN, INPUT);
  float tempValueFunk = 0; //setter startverdien til funskjonen til 0
  for (int i=0;i<=4;i++){ //tar fem maalinger
    float tempValue = analogRead(temp_PIN);   //leser av temp sensoren og lagrer det i tempValue
    float volts = tempValue/1023.0;             // normaliserer max temperaturens raw reading range
    float temp = (volts - 0.5) * 100 ;         //regner ut temperatur (i celcius fra volt i henhold til ligningen som er funnet på sensorens spesifikasjonsark)
    float tempValueFunk +=temp; //Legger målingnee inn i tempValueFunk variabel
    delay(1000);
  }
return tempValueFunk/5; //deler tempValueFunk (delt på 5) for å få rett gjennomsnittstemp
}

void Gruppe5Bib::controll(){    //Denne funksjonen slår av sensorstyring av lys og aktiverer manuell styring gjennom CoT
  _LEDPIN_LIGHT = LEDPIN_LIGHT;
  pinMode(LEDPIN_LIGHT, OUTPUT); 
  _LED = LED;
  if(LED == 0){
    digitalWrite(LEDPIN_LIGHT, LOW);   //av, ingen sensorsystem
    Serial.println("Lys av");  
}
  else {
    Serial.println("The light is on and the sensor is in controll."); //på og sensor styrer
    LED_controll();      //TROR AT DENNE LINJEN KAN FJERNES SÅ LENGE LED_controll KJØRES FRA FØR AV I VOID LOOP. KAN VÆRE PROBLEMATISK MED EN FUNKSJON INNENFOR EN FUNKSJON
  }

}
  
void Gruppe5Bib::LED_controll(){
  _LEDPIN_LIGHT = LEDPIN_LIGHT;
  _LED_ValueTotal = LED_ValueFunk;    //usikker på den, den burde henge sammen med LED_Average
  pinMode(LEDPIN_LIGHT, OUTPUT); 
  
  if(LED_ValueTotal > 2050) {
    digitalWrite(LEDPIN_LIGHT, 200);
  }
    else {
      digitalWrite(LEDPIN_LIGHT, 255);
    }
  Serial.print("Light value is ");
  Serial.println(LED_ValueTotal); //Lysverdi
}
