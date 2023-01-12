#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include "ThingSpeak.h"

#include "DHT.h"

const char* ssid = "OnePlus 8T";
const char* pass = "27032503";
WiFiClient client;

unsigned long myChannelNumber = 1992281;
const char* myWriteAPIKey = "DIAG4TYEU9V7M4WJ";

int LDRSensor = 13;
int LEDPin = LED_BUILTIN;

#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701
const int trigPin = 12;
const int echoPin = 14;
long duration;
float distanceCm;
float distanceInch;
float distanceThresh = 10;

int alarmPin = 5;
DHT dht2(3, DHT11);
  
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  ThingSpeak.begin(client);

  pinMode(LDRSensor, INPUT_PULLUP);
  pinMode(LEDPin, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(alarmPin, OUTPUT);

}

float Temperature = 0, Humidity = 0;
void loop()
{
  float temp = dht2.readTemperature( );
  if (!isnan(temp)) {
    Temperature = temp;
  }
  Serial.print("Temperature in C:");
  Serial.println((Temperature));
  float humid = dht2.readHumidity( );
  if (!isnan(Humidity)) {
    Humidity = humid;
  }
  Serial.print("Humidity in C:");
  Serial.println((Humidity));
  //delay(1000);

  int LDRstate = digitalRead(LDRSensor);
  if (LDRstate == LOW) {
    Serial.println("Lights On!");
    //Serial.println("LED PIN : "+String(LED_BUILTIN));
    digitalWrite(LEDPin, HIGH);
  }
  else {
    Serial.println("Lights Off!");
    digitalWrite(LEDPin, LOW);
  }
  
   digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delay(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  Serial.println("Duration : " + String(duration));
  distanceCm = duration * SOUND_VELOCITY / 2;
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  if(distanceCm <= distanceThresh) {
digitalWrite(alarmPin, HIGH);    
  }
  else {
digitalWrite(alarmPin, LOW);    
  }
  

  ThingSpeak.setField(6, LDRstate);
    ThingSpeak.setField(3, distanceCm);
  
  ThingSpeak.setField(1, Temperature);
  ThingSpeak.setField(2, Humidity);

  
  
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  }
  else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(2000);
}
