// Pour accelerometre 
#include<Wire.h>
#define ACX_S 19000
#define ACY_S 250
#define ACZ_S 400
const int MPU_addr=0x68; 
int16_t AcX,AcY,AcZ;
int16_t AcX2,AcY2,AcZ2;
// Pour dht11
#include "DHT.h"
#define DHTPIN 4     
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);

// Pour bmp280
#include <Arduino.h>
#include <Adafruit_BMP280.h>
#define BMP_SDA 21
#define BMP_SCL 22
Adafruit_BMP280 bmp280;

// Pour mqtt
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>
const char* ssid = "mingdaWiFi";
const char* password = "wmd97128";
//const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_server = "172.20.10.8";
WiFiClient espClient;
PubSubClient client(espClient);

//// Pour CCS811
//#include "Adafruit_CCS811.h"
//Adafruit_CCS811 ccs;

// Pour serveur
#include<string.h>
const char* host = "172.20.10.8";
const int httpPort = 8888;
WiFiClient espServeur;

// Pour Potentiometer
const int potPin = 34;
//const int DpotPin = 5;
#define S_D 4096


#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
//Led

#define ledPin 5

// Wifi setup
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


// Mqtt callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if((char)payload[0] == '4')
    digitalWrite(ledPin, HIGH);
  else if((char)payload[0] == '5')
    digitalWrite(ledPin, LOW);
}


// Wifi connecte
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("ledOnOff");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Fonction du busser
void readSonore(void *p){
  // Reading potentiometer value
  int delta;
  int potValue = 0;
  int preValue = 3600;
  int loopcnt = 0;
  int deltacnt;          // le nombre du delta > seuil
  int detected;
  for(;;)
  {
    
  
    potValue = analogRead(potPin);
    Serial.println(potValue);
   
    delta = abs(preValue-potValue);
    preValue = potValue;
    loopcnt++;
  
    if(loopcnt > 10){
      loopcnt = 0;
      deltacnt = 0;
      }
    else{
      if(delta > 25){
        deltacnt++;
        if(deltacnt > 3){
          detected=1;
          deltacnt = 0;
          Serial.println(delta);
          Serial.println("detected");
          snprintf (msg, MSG_BUFFER_SIZE, "%d", detected);
          detected = 0;
          client.publish("cryBaby", "0");
          }
        }
      }
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true); // request a total of 14 registers
    AcX2=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
    AcY2=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ2=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  
    
    
    
    Serial.print(F("\n"));
  
    if(AcX2 > ACX_S)
    {
      Serial.print(F("Bouge X "));
      Serial.print(F("\n"));
      client.publish("cryBaby", "1");
      Serial.print("AcX = "); Serial.print(abs(AcX2));
    }
    else if(AcY2 > ACY_S)
    {
      Serial.print(F("Bouge Y "));
      Serial.print(F("\n"));
      client.publish("cryBaby", "2");
      Serial.print(" | AcY = "); Serial.print(abs(AcY2));
    }
    else if (AcZ2 > ACZ_S)
    {
      Serial.print(F("Bouge Y "));
      Serial.print(F("\n"));
      client.publish("cryBaby", "3");
      Serial.print(" | AcZ = "); Serial.print(abs(AcZ2));
    }
      vTaskDelay(200);
    }
  
  }

void setup() {
//  pinMode(LED_NUM, OUTPUT);
  Serial.begin(115200);
  Serial.println(F("All begin!"));
// Configuration du dth11
  dht.begin();
  
// Configuration du accelerometre
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); 
  Wire.write(0); 
  Wire.endTransmission(true);

// Configuration du bmp280
  boolean status = bmp280.begin(0x76);
  if (!status) {
    Serial.println("Not connected");
  }
// Mqtt et Wifi
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  
// Task pour busser
  xTaskCreate(readSonore,"task1",S_D,NULL,3,NULL);
//// CCS811
//if(!ccs.begin()){
//  Serial.println("Failed to start sensor! Please check your wiring.");
//  while(1);
//}
//while(!ccs.available());
//  float temp = ccs.calculateTemperature();
//  ccs.setTempOffset(temp - 25.0);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  
  delay(2000);

// Utilisation du Wifi
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  

  
// Utilisation du dht11, mesurer la tmperature et huminite

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F(" Temperature: "));
  Serial.print(t);
  Serial.print(F("\n"));

  
// Utilisation du accelerometre

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true); // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

  Serial.print("AcX = "); Serial.print(abs(AcX));
  Serial.print(" | AcY = "); Serial.print(abs(AcY));
  Serial.print(" | AcZ = "); Serial.print(abs(AcZ));
  Serial.print(F("\n"));

//  if(AcX > ACX_S)
//  {
//    Serial.print(F("Bouge X "));
//    Serial.print(F("\n"));
//    client.publish("cryBaby", "1");
//  }
//  else if(AcY > ACY_S)
//  {
//    Serial.print(F("Bouge Y "));
//    Serial.print(F("\n"));
//    client.publish("cryBaby", "2");
//  }
//  else if (AcZ > ACZ_S)
//  {
//    Serial.print(F("Bouge Y "));
//    Serial.print(F("\n"));
//    client.publish("cryBaby", "3");
//  }
  

  
// Utilisation du bmp280
  float press = bmp280.readPressure() / 100;
  Serial.println("Temperature:");
  Serial.println(press);

// Utilisation du mqtt
//  client.publish("mingdaTopicT", "hello");

//
//// Utilsiation du CSS811
//  int CO2 = 0, TVOC = 0;
//  if(ccs.available()){
//    float temp = ccs.calculateTemperature();
//    if(!ccs.readData()){
//      Serial.print("CO2: ");
//      CO2 = ccs.geteCO2();
//      Serial.print(CO2);
//      Serial.print("ppm, TVOC: ");
//      TVOC = ccs.getTVOC();
//      Serial.print(TVOC);
//      Serial.print("ppb Temp:");
//      Serial.println(temp);
//    }
//    else{
//      Serial.println("CSS811 ERROR!");
//      while(1);
//    }
//  }
// Utilisation du serceur------------------------------
  if (!espServeur.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
        }
  String url = "/AirPressure/\?press\=";
  String dbPress((int)press);
         url += dbPress;
         url += "\&idType\=3";
  Serial.print(url);
  Serial.print("\n");
//  String url = "/AirPressure/\?press\=2\&idType\=3";
  espServeur.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  
  while (espServeur.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            espClient.stop();
            return;
        }
  }

  while(espServeur.available()) {
        String line = espServeur.readStringUntil('\r');
        Serial.print(line);
  }

  Serial.println("Send to serveur\n");

  



//-------------------------------------------------------

  if (!espServeur.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
        }
  url = "/TempHumi/\?temp\=";
  String dbT((int)t);
  url += dbT;
  url += "\&humi\=";
  String dbH((int)h);
  url += dbH;
  url += "\&idType\=2";
  Serial.print(url);
  Serial.print("\n");
  espServeur.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  timeout = millis();
  
  while (espServeur.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            espClient.stop();
            return;
        }
  }

  while(espServeur.available()) {
        String line = espServeur.readStringUntil('\r');
        Serial.print(line);
  }

  Serial.println("Send to serveur\n");

//-------------------------------------------------------

  if (!espServeur.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
        }
  url = "/Mouvement/\?x\=";
  String dbX((int)AcX);
  url += dbX;
  url += "\&y\=";
  String dbY((int)AcY);
  url += dbY;
  url += "\&z\=";
  String dbZ((int)AcZ);
  url += dbZ;
  url += "\&idType\=1";
  Serial.print(url);
  Serial.print("\n");
  espServeur.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
  timeout = millis();
  
  while (espServeur.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            espClient.stop();
            return;
        }
  }

  while(espServeur.available()) {
        String line = espServeur.readStringUntil('\r');
        Serial.print(line);
  }

  Serial.println("Send to serveur\n");

 //-------------------------------------------------------

//  if (!espServeur.connect(host, httpPort)) {
//        Serial.println("connection failed");
//        return;
//        }
//  url = "/AirQualite/\?co2\=";
//  String co2((int)CO2);
//  url += co2;
//  url += "\&tvoc\=";
//  String tvoc((int)TVOC);
//  url += tvoc;
//  url += "\&idType\=1";
//  Serial.print(url);
//  Serial.print("\n");
//  espServeur.print(String("POST ") + url + " HTTP/1.1\r\n" +
//                 "Host: " + host + "\r\n" +
//                 "Connection: close\r\n\r\n");
//  timeout = millis();
//  
//  while (espServeur.available() == 0) {
//        if (millis() - timeout > 5000) {
//            Serial.println(">>> Client Timeout !");
//            espClient.stop();
//            return;
//        }
//  }
//
//  while(espServeur.available()) {
//        String line = espServeur.readStringUntil('\r');
//        Serial.print(line);
//  }
//
//  Serial.println("Send to serveur\n");

}
