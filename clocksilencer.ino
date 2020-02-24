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
int servoPin = 18;
int tDelta=0; //this variable counts the possible variations of sleep clock, 
//as we want the deepsleep to wake the device always at quarter past hour
int t=0,tH=0 tM=0; 
int hour=3600; //hour in seconds


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", hour, 60000);

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
  whatTime();// check on time
  Serial.println(t);
 
  // check if it should be on or off
  if (t >700 and t<1930){ //hours in format HHMM
    myservo.write(90);} //turn it on
  else {myservo.write(0)};
  sweetDreams();

}
void sweetDreams(){
  if (tM >15){
    tDelta=(tM-15)*60;
  ESP.deepsleep((hour-tDelta)*1000000)}
  if (tM <15){
    tDelta=(15-tM)*60;
  ESP.deepSleep((hour+tDelta)*1000000)}
  if (tM==15){
    ESP.deepSleep (hour*1000000);
  }
  void whatTime(){
     timeClient.update();
  tH=timeClient.getHours();
  tM=timeClient.getMinutes();
  t = (tH* 100) + tM; 
  }
    
