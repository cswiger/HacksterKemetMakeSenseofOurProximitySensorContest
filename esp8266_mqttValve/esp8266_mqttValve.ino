/*
  esp8266_Kemetmqtt  
  sketch to control a water valve thru an mqtt server by publishing an ON to turn the valve ON and OFF for OFF. 
  
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "yourwifissid";
const char* password =  "yourwifipassword";
const char* mqttServer = "yourmqttserverIP";
const int mqttPort = 1883;
const char* mqttUser = "mqttuser";
const char* mqttPassword = "mqttpassword";
 
WiFiClient espClient;
PubSubClient client(espClient);
 

// the setup function runs once when you press reset or power the board
void setup() {
  // Serial for debug
  Serial.begin(115200);
  // initialize digital pin 2 GPIO2 as output to the Crydom Solid State Relay
  pinMode(5, OUTPUT);
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
 
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    // use different name for this one 
    if (client.connect("solenoid", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("/watervalve", "Hello from ESP8266-03 Relay");
  client.subscribe("/watervalve");
 
}


// the loop function runs over and over again forever
void loop() {
  // all the action is in the message queue callback
  client.loop();
}


void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");

  // got ON message in /watervalve topic
  if ( (char)payload[1] == 'N' ) {
    digitalWrite(5,HIGH);
  }
  // got OFF message in /watervalve topic
  if ( (char)payload[1] == 'F' ) {
    digitalWrite(5,LOW);
  }
 
}

