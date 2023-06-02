#include "DFPlayerMini_Fast.h"
//#include "DFRobotDFPlayerMini.h"
#include "HardwareSerial.h"
//#include <ESP32Time.h> //we quit using this crappy internal RTC
#include <WiFi.h>
#include <RTClib.h>


RTC_DS3231 rtc;
HardwareSerial hwSerial(1);
DFPlayerMini_Fast myDFPlayer;
//DFRobotDFPlayerMini myDFPlayer;
const char *ssid = "SSID";
const char *password = "PASS";
RTC_DATA_ATTR bool rtcTimeWasAlreadySetFromNTP = false;
bool noWifi;
int myDow, mySec, myMin, myHour;
int sleepHelper;
void setup() {
  pinMode(4, OUTPUT);
  Serial.begin(115200);
  hwSerial.begin(9600, SERIAL_8N1, 16,
                 17);  // second serial port to serve mp3 player
   if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (rtcTimeWasAlreadySetFromNTP == false) {
    synchroniseExternalTime(); 
  }
}

void loop() {
  DateTime now = rtc.now();
  Serial.print("It is ");
  //--------------------
   Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
//==========================
  myDow = now.dayOfTheWeek();
  mySec = now.second();
  myMin = now.minute();
  myHour = now.hour();
  if (myMin > 31) {
    sleepHelper = 59 - myMin;
  } else {
    sleepHelper = 29 - myMin;
  };
  if (mySec == 50 and sleepHelper == 0) {  //according to the DFPlayer documentation, it needs 3-5 seconds to start
    startPlayer();
  }

  //---------ringing conditions----------

  if (mySec == 0 and myMin == 0) {     // at full hour
    if (myHour > 7 and myHour < 19) {  // between 8 and 18
      // here another condition
      ringer(myHour);
    }
  }
  if (mySec == 0 and myMin == 30) {    // at half hour
    if (myHour > 7 and myHour < 19) {  // between 8 and 18
      // here another condition
      ringer(1);
    }
  }

  
  // -----deep sleep conditions--------
  if ((sleepHelper > 2)) {
    deepsleep(sleepHelper);
  }
  delay(1000);  //if there is a short time to chime, just work with delay
}
void ringer(int myhours2) {
  //run mosfet here

  Serial.print("Ringing:");
  Serial.println(myhours2);
  if (myhours2 > 12) {
    myhours2 -= 12;
  }
  for (int n = 1; n <= myhours2; n++) {
    Serial.print("Ring nr:");
    Serial.println(n);
    if (n == 1) {
      myDFPlayer.playFromMP3Folder(1);
      delay(2250);

    }  // ring the chime
    else {
      myDFPlayer.playFromMP3Folder(2);
      delay(2250);
    }
    
  }
  while (myDFPlayer.isPlaying()) {
    };
  digitalWrite(4, LOW);
  Serial.println("Mosfet low");
  //maybe here rtc synchronisation?
}
void deepsleep(int sleepHelper) {
  int countMicro = ((sleepHelper - 1) * 1000000 * 60);
  esp_sleep_enable_timer_wakeup(countMicro);
  Serial.println("Deepsleep");
  Serial.print("Wake up in ");
  Serial.print(sleepHelper);
  Serial.print(" minutes, at ");
  Serial.print(myHour);
  Serial.print(":");
  Serial.println(myMin += sleepHelper);
  esp_deep_sleep_start();
}
void synchroniseExternalTime() {
  WiFi.begin(ssid, password);
  Serial.println("Looking for the WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    // if wifi found, break loop
  }
  Serial.println("WiFi connected");
  //---------set with NTP--------------- 
  // we have to rebuild this function to synchronise ext rtc
  configTime(3600, 3600, "europe.pool.ntp.org");/
  //==== //this need to be further modified to meet the requirements of RTClib
    struct tm timeinfo;
  getLocalTime(&timeinfo);

  yr = timeinfo.tm_year + 1900;
  mt = timeinfo.tm_mon + 1;
  dy = timeinfo.tm_mday;
  hr = timeinfo.tm_hour;
  mi = timeinfo.tm_min;
  se = timeinfo.tm_sec;

  rtc.adjust(DateTime(yr, mt, dy, hr, mi, se));
    struct tm timeinfo;//
  
  Serial.println("Attempt to update RTC");
  
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);                 // disconnect after synchronisation to save power
  rtcTimeWasAlreadySetFromNTP = true;  // time was updated
  Serial.println("WiFi turned off");
}

void startPlayer() {
  digitalWrite(4, HIGH);
  Serial.println("Mosfet high");

  hwSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Hardware Serial started");
  delay(100);
  while (!myDFPlayer.begin(hwSerial)) {

  };  //initializing mp3 player
  myDFPlayer.volume(25);
  delay(10);
}
