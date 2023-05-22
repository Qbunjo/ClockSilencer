#include "DFPlayerMini_Fast.h"
#include "HardwareSerial.h"
#include <ESP32Time.h>
#include <WiFi.h>


ESP32Time rtc;
HardwareSerial hwSerial(1);
DFPlayerMini_Fast myDFPlayer;
const char *ssid = "SSID";
const char *password = "PASS";
RTC_DATA_ATTR bool rtcTimeWasAlreadySetFromNTP = false;
bool noWifi;
int myDow, mySec, myMin, myHour;
int sleepHelper;
void setup() {
  Serial.begin(115200);
  hwSerial.begin(9600, SERIAL_8N1, 16,
                 17);  // second serial port to serve mp3 player
  if (rtcTimeWasAlreadySetFromNTP == false) {
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
    WiFi.mode(WIFI_OFF);                 // disconnect after synchronisation to save power
    rtcTimeWasAlreadySetFromNTP = true;  // time was updated
    Serial.println("WiFi turned off");
  } else {
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 3);  // repair timezone
    tzset();
  }
}


void loop() {


  Serial.print("It is:");
  Serial.println(
    rtc.getTimeDate(true));  //  (String) 15:24:38 Sunday, January 17 2021

  myDow = rtc.getDayofWeek();
  mySec = rtc.getSecond();
  myMin = rtc.getMinute();
  myHour = rtc.getHour(true);
  if (myMin > 31) {
    sleepHelper = 59 - myMin;
  } else {
    sleepHelper = 29 - myMin;
  };
  if (mySec == 0 and myMin == 0) {     // at full hour
    if (myHour > 7 and myHour < 19) {  // between 9 and 18
      // here another condition
      ringer(myHour);
    }
  }
  if (mySec == 0 and myMin == 30) {    // at half hour
    if (myHour > 7 and myHour < 19) {  // between 9 and 18
      // here another condition
      ringer(1);
    }
  }

  struct tm timeinfo = rtc.getTimeStruct();
  if ((sleepHelper > 2)) {
    deepsleep(sleepHelper);
  }
  delay(1000);  //if there is a short time to chime, just work with delay
}
void ringer(int myhours2) {
  //run mosfet here
  digitalWrite(23, 1023);
  hwSerial.begin(9600, SERIAL_8N1, 16, 17);
  myDFPlayer.begin(hwSerial);  //initializing mp3 player
  delay(500);                  // wait for the player to wake
  Serial.print("Ringing:");
  Serial.println(myhours2);
  if (myhours2 > 12) {
    myhours2 -= 12;
  }

  myDFPlayer.volume(25);
  for (int n = 1; n <= myhours2; n++) {
    if (n == 1) {
      myDFPlayer.play(1);
    }  // ring the chime}
    else {
      myDFPlayer.play(2);
    }
    Serial.print("Ring nr:");
    Serial.println(n);
    delay(2250);
  }
  digitalWrite(23, 0);
}
void deepsleep(int sleepHelper) {
  int countMicro = ((sleepHelper - 1) * 1000000 * 60);
  esp_sleep_enable_timer_wakeup(countMicro);
  Serial.println("Deepsleep");
  Serial.print("Wake up in: ");
  Serial.print(sleepHelper);
  Serial.print(" minutes, at ");
  Serial.print(myHour);
  Serial.print(":");
  Serial.println(myMin += sleepHelper);
  esp_deep_sleep_start();
}
