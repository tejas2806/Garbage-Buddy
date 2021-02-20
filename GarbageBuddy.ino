#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#define anInput     A0                        //analog feed from MQ135
#define co2Zero     55                        //calibrated CO2 0 level
#define RXPin D7
#define TXPin D6

#define FIREBASE_HOST "PRIVATE"
#define FIREBASE_AUTH "PRIVATE"
#define WIFI_SSID "PRIVATE"
#define WIFI_PASSWORD "PRIVATE"
const int trigPin = 2;  //D4
const int echoPin = 0;  //D3

static const uint32_t GPSBaud = 9600;
long duration;
int distance;
SoftwareSerial ss(RXPin, TXPin);
TinyGPSPlus gps;
WiFiClient  client;
void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(anInput, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {
  //Ultrasonic
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);


  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculating the distance
  distance = duration * 0.034 / 2;
  if (distance < 5) {
    Serial.print("Need to be Empty: ");
    Serial.println(distance);
    Firebase.setString("BIN-1/Space", "Warning");
    Firebase.setFloat("BIN-1/Space-Available", distance);
    delay(2000);
  }
  else {
    Serial.print("Their is space to dump");
    Firebase.setString("BIN-1/Space", "Safe");
    Firebase.setFloat("BIN-1/Space-Available", distance);
  }

  // gas sensor

  /*
     Atmospheric CO2 Level..............400ppm
     Average indoor co2.............350-450ppm
     Maxiumum acceptable co2...........1000ppm
     Dangerous co2 levels.............>2000ppm
  */

  int co2now[10];                               //int array for co2 readings
  int co2raw = 0;                               //int for raw value of co2
  int co2ppm = 0;                               //int for calculated ppm
  int zzz = 0;
  for (int x = 0; x < 10; x++) //samplpe co2 10x over 2 seconds
  {
    co2now[x] = analogRead(A0);
    delay(200);
  }

  for (int x = 0; x < 10; x++) //add samples together
  {
    zzz = zzz + co2now[x];
  }

  co2raw = zzz / 10;                          //divide samples by 10
  co2ppm = co2raw - co2Zero;                 //get calculated ppm

  Serial.print("AirQuality=");
  if (co2ppm > 800) {
    Serial.print(co2ppm);  // prints the value read
    Serial.println(" PPM");
    Firebase.setString("BIN-1/CO2", "Danger");
    Firebase.setFloat("BIN-1/CO2-level", co2ppm);

    delay(50);
  }
  else
  {
    Serial.print(co2ppm);  // prints the value read
    Serial.println(" PPM");
    Firebase.setString("BIN-1/CO2", "Safe");
    Firebase.setFloat("BIN-1/CO2-level", co2ppm);
    delay(50);
  }

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true);
  }


}
void displayInfo()
{
  // Serial.print(F("Location: "));
  if (gps.location.isValid())
  {

    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());

    String latbuf;
    latbuf += (String(latitude, 6));
    Serial.println(latbuf);

    String lonbuf;
    lonbuf += (String(longitude, 6));
    Serial.println(lonbuf);
    Firebase.setString("BIN-1/Latitude", latbuf);
    Firebase.setString("BIN-1/Longitude", lonbuf);
  }
}
