#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DFRobot_ESP_EC.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>  
const int potPin=A0;
float ph;
float Value=0;

#define ONE_WIRE_BUS 14                // this is the gpio pin 13 on esp32.
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
 
DFRobot_ESP_EC ec;
float voltage, ecValue, temperature = 25;

String apiKey = "";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "";    // replace with your wifi ssid and wpa2 key
const char *pass =  "";
const char* server = "api.thingspeak.com";
 
#define DHTPIN 15          //pin where the dht11 is connected
 
DHT dht(DHTPIN, DHT22);
 
WiFiClient client;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(potPin,INPUT);
  EEPROM.begin(32);//needed EEPROM.begin to store calibration k in eeprom
  ec.begin();
  sensors.begin();
  dht.begin();
  delay(1000);
  

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(2000);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}
 
void loop() 
{
  voltage = analogRead(35); // A0 is the gpio 36
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);  // read your temperature sensor to execute temperature compensation
  ecValue = ec.readEC(voltage, temperature); // convert voltage to EC with temperature compensation
 
  Serial.print("Temperature:");
  Serial.print(temperature, 2);
  Serial.println("ºC");
 
  Serial.print("EC:");
  Serial.println(ecValue, 2);
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      float voltages=Value*(3.3/4095.0);
        // put your main code here, to run repeatedly:
      Value= analogRead(potPin);
      Serial.print(Value);
      Serial.print(" | ");
      ph=(3.3*voltages);
      Serial.println(ph);
      Serial.print("Temperature: ");
      Serial.println(t);
      Serial.print("Humidity: ");
      Serial.println(h);
      delay(500);
 ec.calibration(voltage, temperature); // calibration process by Serail CMD
              if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 

 if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
   {
      String postStr = apiKey;
      postStr += "&field1=";
      postStr += String(t);
      postStr += "&field2=";
      postStr += String(h);
      postStr += "&field3=";
      postStr += String(ph);
      postStr += "&field5=";
      postStr += String(temperature, 2);
      postStr += "&field4=";
      postStr += String(ecValue, 2);
      postStr += "\r\n\r\n\r\n\r\n\r\n";
    
      client.print("POST /update HTTP/1.1\n");
      client.print("Host: api.thingspeak.com\n");
      client.print("Connection: close\n");
      client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
      client.print("Content-Type: application/x-www-form-urlencoded\n");
      client.print("Content-Length: ");
      client.print(postStr.length());
      client.print("\n\n");
      client.print(postStr); 
    }    
}
