#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Yingshanwifi";
const char* password = "123456789";
const char* mqtt_server = "broker.hivemq.com";

const char* mqtt_server_homeassis = "192.168.43.62";
const char* mqtt_user = "homeassistant";
const char* mqtt_password = "123456789";
#define LED 5

WiFiClient espClient;
PubSubClient client(espClient);
WiFiClient espClient2;
PubSubClient client2(espClient2);

int freq = 2000;
int channel = 0;
int resolution = 8;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  buzzer();

}

void callback2(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.println("P after convert :");
  Serial.println(p); 

  //message to led
  if(String(topic)=="esp/led"){
    
    if(String(p)=="ON"){
      Serial.println("Turn on the led \n");
      digitalWrite(LED, HIGH);
     }
     else if(String(p)=="OFF"){
      Serial.println("Turn off the led \n");
      digitalWrite(LED, LOW);
     }
   }
}


void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32-buzzer")) {
      Serial.println("connected");
      client.subscribe("cry_detection");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds \n");
      delay(3000);
    }
  }
}

void reconnect2() {
  // Loop until we're reconnected
  while (!client2.connected()) {
    Serial.println("Attempting MQTT connection to homeassistant...");
    
    // Attemp to connect
    if (client2.connect("myESP32",mqtt_user, mqtt_password)) {
      Serial.println("Connected");
      client2.subscribe("esp/led");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client2.state());
      Serial.println("try again in 3 seconds \n");
      delay(3000);
    }
  }
}


void setup() {
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(4, channel);

  client2.setServer(mqtt_server_homeassis, 1883);
  client2.setCallback(callback2); 
  pinMode(LED, OUTPUT);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if (!client2.connected()) {
    reconnect2();
  }
  client.loop();
  client2.loop();
  
  delay(5000);

}

void buzzer(){
  for(int i=0; i<3;i++){
    ledcWriteTone(channel, 2000);
    ledcWrite(channel, 125);
    delay(600);
    ledcWrite(channel, 0);
    delay(400);
    }
  }
