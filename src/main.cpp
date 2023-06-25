#include <Arduino.h>
#include "DHT.h"
#include <ESP32Time.h>
#include <iostream>
#define DHTPIN 21
#define DHTTYPE DHT11
const int AC = 13;
const int pir = 35;
unsigned long lastTrigger = 0;
bool startTimer = false;
bool motion = false;
bool action = false;
const int Time = 600;
unsigned long now = millis();
float temp;
void AC_Control();
DHT dht(DHTPIN, DHTTYPE);
ESP32Time rtc(3600);
String Now;
String Start = "07:00:00";
String Stop = "19:00:00";

void IRAM_ATTR detectsMovement(){
  if(temp < 25 && action ){
  digitalWrite(AC, HIGH);
  startTimer = true;
  lastTrigger = millis();
  }
}

void readweather(void *parameter){
    for(;;){
        temp = dht.readTemperature();
        Serial.print(F(" Temperature : "));
        Serial.print(temp);
        Serial.println(F("°C "));
        vTaskDelay(30000 / portTICK_PERIOD_MS);
    }
}

void gettime(void *parameter){
  for(;;){
    Now = rtc.getTime();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    if(Now == Start){
     action = true;
    }
    if(Now == Stop){
     action = false;
    }
  }
}



void setup() {
  Serial.begin(9600);
  dht.begin();
  rtc.setTime(30, 59, 05, 25, 6, 2023);  // 25th June 2023 05:26:30
  xTaskCreate(readweather,"Temperature",5000,NULL,4,NULL);
  xTaskCreate(gettime,"Time ",5000,NULL,2,NULL);
  pinMode(pir, INPUT_PULLUP);
  pinMode(AC, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pir), detectsMovement, RISING);
  digitalWrite(AC, LOW);
}

void loop() {
  now = millis();
  AC_Control();
  Serial.println(rtc.getTime("%A, %B %d %Y %H:%M:%S"));   // (String) returns time with specified format 
  struct tm timeinfo = rtc.getTimeStruct();
}

void AC_Control(){
  if((digitalRead(AC) == HIGH) && (motion == false)) {
    motion = true;
  }
  if(startTimer && (now - lastTrigger > (Time)) && temp < 25){
    digitalWrite(AC, LOW);
    startTimer = false;
    motion = false;
  }
  if(startTimer && temp <= 20 ){
    digitalWrite(AC, LOW);
    startTimer = false;
    motion = false;
  }
  if(startTimer && !action) {
    digitalWrite(AC, LOW);
    startTimer = false;
    motion = false;
  }
  
}






