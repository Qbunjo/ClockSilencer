#include <ESP32Time.h>
#include <WiFi.h>
#include "HardwareSerial.h"
#include "DFPlayerMini_Fast.h"
ESP32Time rtc;
HardwareSerial hwSerial(1);
DFPlayerMini_Fast mp3;
const char *ssid = "SSID";
const char *password = "PASS";
RTC_DATA_ATTR bool rtcTimeWasSetFromNTP = false;
bool noWifi;
int myDow, mySec, myMin, myHour;
void setup() {
  Serial.begin(115200);
  hwSerial.begin(9600, SERIAL_8N1, 16, 17);  //second serial port to serve mp3 player
   mp3.begin(hwSerial);  //initializing mp3 player
  if (rtcTimeWasAlreadySetFromNTP==false) {
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
    Serial.println("RTC update done");
    rtcTimeWasSetFromNTP=true; //time was updated
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);  //disconnect after synchronisation to save power
    Serial.println("WiFi turned off");
    //play chime when everything ready
    //mp3.volume(15);  //Set volume value. From 0 to 30
    mp3.playbackSource(2);
    mp3.play(3); //this does not work. dunno why
    Serial.println("Play OK signal");
    while (mp3.isPlaying()) { Serial.println("Playing!"); } //this also shows that no music is played.
    
  } else {
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 3);  //repair timezone
    tzset();
  }
}


void loop() {

  Serial.println(rtc.getTimeDate(true));  //  (String) 15:24:38 Sunday, January 17 2021

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
  esp_sleep_enable_timer_wakeup(500000);  //in microseconds, but not the full second (1000000)
  Serial.println("Deepsleep");
  esp_deep_sleep_start();
  //delay(1000);
}
void ringer(int myhours2) {
  if (myhours2 > 12) {
    myhours2=myhours2 - 12;
  }
  Serial.print("Ringing:");
  Serial.println(myhours2);
  mp3.volume(27);
  for (int n = 1; n <= myhours2; n++) {
    if (n == 1) {
      mp3.play(1);
    }  //ring the chime}
    else { mp3.play(2); }
    Serial.print("Ring nr:");
    Serial.println(n);
    delay(2250);
  }
}
