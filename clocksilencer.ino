#include <ESP32Time.h>
#include <WiFi.h>
#include "HardwareSerial.h"
#include "DFPlayerMini_Fast.h"

ESP32Time rtc;
HardwareSerial hwSerial(1);
DFPlayerMini_Fast myDFPlayer;

const char *ssid = "SSID";
const char *password = "PASS";
RTC_DATA_ATTR int bootCount = 0;
bool noWifi;
int myDow, mySec, myMin, myHour;
void setup() {
  Serial.begin(115200);
  hwSerial.begin(9600, SERIAL_8N1, 16, 17);  //second serial port to serve mp3 player
  ++bootCount;

  myDFPlayer.begin(hwSerial);  //initializing mp3 player
  Serial.println("Boot number: " + String(bootCount));
  if (bootCount == 1) {
    WiFi.begin(ssid, password);
    Serial.println("Looking for the WiFi");


    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      // if wifi found, break loop
    }
    Serial.println("WiFi connected");
    //---------set with NTP---------------
    configTime(3600, 3600, "europe.pool.ntp.org");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      rtc.setTimeStruct(timeinfo);
    }
    Serial.println("Attempt to update RTC");
    while (rtc.getYear() == 1970) {
      Serial.println("RTC updating in progress");
    }
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);  //disconnect after synchronisation to save power
    Serial.println("WiFi turned off");
    //play chime when everything ready
    myDFPlayer.volume(5);  //Set volume value. From 0 to 30
    myDFPlayer.play(1);
  } else {
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 3);
    tzset();}
}


void loop() {

  Serial.println(
    rtc.getTimeDate(true));  //  (String) 15:24:38 Sunday, January 17 2021

  myDow = rtc.getDayofWeek();
  mySec = rtc.getSecond();
  myMin = rtc.getMinute();
  myHour = rtc.getHour(true);

  if (mySec == 0 and myMin == 0) {     //at full hour
    if (myHour > 7 and myHour < 19) {  //between 9 and 18
      //here another condition
      ringer(myHour);
    }
  }
  if (mySec == 0 and myMin == 30) {    //at half hour
    if (myHour > 7 and myHour < 19) {  //between 9 and 18
      //here another condition
      ringer(1);
    }
  }
  struct tm timeinfo = rtc.getTimeStruct();
  esp_sleep_enable_timer_wakeup(500000); //in microseconds, but not the full second (1000000)
  if (bootCount > 2147483645){
    bootCount=2;// when bootcount approaches to integer max, reset it
  }
  Serial.println("Deepsleep");
  esp_deep_sleep_start();
}
void ringer(int myhours2) {
  Serial.print("Ringing:");
  Serial.println(myhours2);
  if (myhours2 > 12) {
    myhours2 - 12;
  }
  myDFPlayer.wakeUp();
  myDFPlayer.volume(25);
  for (int n = 1; n <= myhours2; n++) {
    if (n == 1) {
      myDFPlayer.play(1);
    }  //ring the chime}
    else { myDFPlayer.play(2); }
    Serial.print("Ring nr:");
    Serial.println(n);
    delay(2250);
  }
  myDFPlayer.sleep();
}
