/*
    SmartHome.h  (Version 1.0.0)

  Implements SmartHome features in your dormitory.

    Created by Julia Czarnocka, May 6th, 2021
*/


#ifndef Gruppe5Bib_h
#define Gruppe5Bib_h

#include "Arduino.h"
#include <analogWrite.h>
#include <Servo.h>  
 
class Gruppe5Bib
{
  public:
    Gruppe5Bib();
    float tempAverage(uint8_t *temp_PIN);
    float LED_Average(uint8_t *lyssensor_PIN);
    void LED_controll();
    void controll();
  private:
    char _key_temp_given;
    char _key_temp;
    char _key_heat;
    char _key_LED;
    int _LED;
    unint8_t *_lyssensor_PIN;
    unint8_t *_temp_PIN;
    unsigned int _LEDPIN_LIGHT;
    unsigned int _LEDPIN_HEAT;
    float _volts; 
    float _temp; 
    int _LED_ValueTotal;
    float _tempValue; 
    float _tempValueFunk; 
    int _LED_state;
    char _token;
    char _server;
    char _ssid;
    char _pass;
};

#endif
