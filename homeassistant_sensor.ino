#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// DHT11
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
char payload_temperature[10];
char payload_humidity[10];

// détecteur sonore
const int potPin = 34;
const int DpotPin = 5;
int potValue = 0;
int preValue = 3600;
int loopcnt = 0;
int deltacnt;          // le nombre du delta > seuil
int detected;     

// Pour accelerometre 

#include<Wire.h>
const int MPU_addr = 0x68; 
const int PWR_MGMT_1 = 0x6B; 
const int GYRO_CONFIG = 0x1B;
const int ACCE_CONFIG = 0x1C;
const int ACCEL_XOUT_H = 0x3B;
const int GYRO_XOUT_H = 0x43;

long accelX, accelY, accelZ; // accelerometer
float gForceX, gForceY, gForceZ; //
char payload_gForceX[10];
char payload_gForceY[10];
char payload_gForceZ[10];
char payload_acc[10];

long gyroX, gyroY, gyroZ; //gyroscopes
float rotX, rotY, rotZ;  //

//send buzzer
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

// Network params
const char* ssid = "Yingshanwifi";
const char* password = "123456789";
const char* mqtt_server = "192.168.43.62";
const char* mqtt_user = "homeassistant";
const char* mqtt_password = "123456789";

const char* mqtt_hivemq = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

WiFiClient espClient2;
PubSubClient client2(espClient2);

StaticJsonDocument<200> json_doc;
char json_output[100];
DeserializationError json_error;

StaticJsonDocument<200> json_doc2;
char json_output2[100];
 

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  client2.setServer(mqtt_hivemq, 1883);
  client2.setCallback(callback);
  dht.begin();

  // Configuration du accelerometre
  Wire.begin();
  Wire.beginTransmission(MPU_addr);//This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(PWR_MGMT_1); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission(true);
  Wire.beginTransmission(MPU_addr); //I2C address of the MPU
  Wire.write(GYRO_CONFIG); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(true); 
  Wire.beginTransmission(MPU_addr); //I2C address of the MPU
  Wire.write(ACCE_CONFIG); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(true); 
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    //if (client.connect("arduinoClient")) {
    if (client.connect("arduinoClient", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void reconnect2() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  int val;
  if (!client.connected()) {
    reconnect();
  }
  if (!client2.connected()) {
    reconnect2();
  }
  client.loop();
  client2.loop();
  readDht11();
  readAccel();
  readGyros();
  readSonore();
  snprintf (msg, MSG_BUFFER_SIZE, "%d", detected);
  detected = 0;
  client2.publish("cry_detection", msg);
}

void readDht11(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  dtostrf(t, 4, 2, payload_temperature);
  dtostrf(h, 4, 2, payload_humidity);
  
  json_doc["Temperature"] = payload_temperature;
  json_doc["Humidity"] = payload_humidity;
  serializeJson(json_doc, json_output);
  Serial.println( "string to json:" ); 
  Serial.println( json_output ); 
  
  client.publish("esp/sensor1", json_output, true);
   
}

void readSonore(){
  int delta = 0;
  potValue = analogRead(potPin);
  Serial.print("potValue=");
  Serial.print(potValue);
  Serial.print("\n");
  
  delta = abs(preValue-potValue);
  preValue = potValue;
  loopcnt++;

  if(loopcnt > 10){
    loopcnt = 0;
    deltacnt = 0;
    }
  else{
    if(delta > 20){
      deltacnt++;
      if(deltacnt > 2){
        detected=1;
        deltacnt = 0;
        }
      }
    }
}

void readAccel(){
  // Utilisation du accelerometre
  Wire.beginTransmission(MPU_addr);
  Wire.write(ACCEL_XOUT_H); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,6); //Request Accel Registers (3B - 40)
   while(Wire.available() < 6);
  accelX=Wire.read()<<8|Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  accelY=Wire.read()<<8|Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  accelZ=Wire.read()<<8|Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  processAccelData();
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
  gForceX -= 1;
  gForceY -= 0.07;
  gForceZ -= 0.15;
  

  float Abs = abs(gForceX)+abs(gForceY)+abs(gForceZ);
//  Serial.print(" Accel (g)");
//  Serial.print(" absX=");
//  Serial.print(abs(gForceX));
//  Serial.print(" absY=");
//  Serial.print(abs(gForceY));
//  Serial.print(" absZ=");
//  Serial.println(abs(gForceZ));
  dtostrf(Abs, 4, 2, payload_acc);
  
  json_doc2["Accelerate"] = payload_acc;
  serializeJson(json_doc2, json_output2);
  Serial.println( "string to json:" ); 
  Serial.println( json_output2 ); 
  
  client.publish("esp/sensor2", json_output2, true);
}
void readGyros(){
  // Utilisation du Gyroscopes
  Wire.beginTransmission(MPU_addr); //I2C address of the MPU
  Wire.write(0x43); //Starting register for Gyro Readings
  Wire.endTransmission();
  Wire.requestFrom(MPU_addr,6); //Request Gyro Registers (43 - 48)
  while(Wire.available() < 6);
  gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processGyroData();
}

void processGyroData() {
  rotX = gyroX / 131.0;
  rotY = gyroY / 131.0; 
  rotZ = gyroZ / 131.0;
}

void printData() {
  Serial.print("Gyro (deg)");
  Serial.print(" X=");
  Serial.print(rotX);
  Serial.print(" Y=");
  Serial.print(rotY);
  Serial.print(" Z=");
  Serial.print(rotZ);
  Serial.print(" Accel (g)");
  Serial.print(" X=");
  Serial.print(gForceX);
  Serial.print(" Y=");
  Serial.print(gForceY);
  Serial.print(" Z=");
  Serial.println(gForceZ);
}
