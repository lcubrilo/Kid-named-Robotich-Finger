#include <Servo.h>
Servo motoric;

#include <ESP8266WiFi.h>
WiFiClient client;
const char *ssid = "HUAWEI P10 lite";
const char *password = "knin2022";

void setup() {
  // Opening the serial connection
  Serial.begin(9600);
  
  // Connecting to the servo at pin 2
  motoric.attach(15);

  /* Connecting to the WiFi
  WiFiClient client; int retries = 0;
  while ((WiFi.status() != WL_CONNECTED)) {
    retries++;
    delay(1000);
    Serial.print(".");
    if (retries > 14) {
      Serial.println(F("WiFi connection FAILED"));
      break;
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }*/
}

void loop() {
  motoric.write(0);
  delay(3000);
  motoric.write(180);
  delay(3000);
}
