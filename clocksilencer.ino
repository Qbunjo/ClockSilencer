#include <NTPClient.h>
// change next line to use with another board/shield
#include <WiFi.h>
//#include <WiFi.h> // for WiFi shield
//#include <WiFi101.h> // for WiFi 101 shield or MKR1000
#include <WiFiUdp.h>
#include <ESP32Servo.h>

const char *ssid     = "SSID";
const char *password = "PASS";

Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
int servoPin = 18;
int tDelta=0;
int t=0,tH=0 tM=0;


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

void setup() {
  Serial.begin(115200);
  myservo.attach(servoPin);
  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }

  timeClient.begin();
}

void loop() {
  timeClient.update();
  tH=timeClient.getHours();
  tM=timeClient.getMinutes();
  t = tH* 100 + tM; 
  Serial.println(t);
  delay(1000);
 
  //TODO
  // if time <8.00
// Servo to 0 off
//Else check procedure time
  // procedure time
// if time > 20.00 move servo to 0
  // else move servo to 90
}
void sweetDreams(){
  if (tM >15){
    tDelta=30-tM;
    tDelta=15-tDelta;
  deepsleep(hour-tDelta)}
  if (tM <15){
    tDelta=15-tM;
  deepSleep(hour+tDelta)}
  if (tM=15){
    deepSleep (hour);
  }
    
