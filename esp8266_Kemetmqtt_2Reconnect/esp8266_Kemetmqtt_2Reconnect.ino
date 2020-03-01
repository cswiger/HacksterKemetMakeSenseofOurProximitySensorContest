/*
  esp8266_Kemetmqtt  
  sketch to take signals from Kemet PIR Sensor and control a water valve thru an mqtt server by publishing an ON to turn the valve ON and OFF for OFF. 

  On motion detect signal, which is two 200mSec pulses, publish an ON message to the /watervalve topic for 5 seconds.
  After 5 seconds of open water valve to the sprinkler, turn it off and begin a 30 second cooldown period. During this period no
  pulses from the PIR sensor will be acted on. 

  This arrangement allows flexibility in using the water sprinkler for more than just scaring away moving warn animals 
  from the garden in the night. It can also be used to schedule a morning watering, perhaps depending on fetching a weather
  forecast and not watering if there is a high probability of rain soon, or giving an extra long watering cycle if an 
  unusually hot and dry day is forecast.
  
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "yourwifissid";
const char* password =  "yourwifipassword";
const char* mqttServer = "yourmqttserverIP";
const int mqttPort = 1883;
const char* mqttUser = "yourmqttuser";
const char* mqttPassword = "yourmqttuserpassword";

int IR_sensed = 0;
unsigned long IR_threshold = 198000;

WiFiClient espClient;
PubSubClient client(espClient);
 

// the setup function runs once when you press reset or power the board
void setup() {
  // Serial for debug
  Serial.begin(115200);
  // initialize digital pin 2 GPIO2 as output to an LED
  pinMode(2, OUTPUT);
  // initialize digital pin 14 GPIO14 as input from the Kemet module
  pinMode(14, INPUT);
  // initialize WiFi
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
 
    if (client.connect("mqttKemet", mqttUser, mqttPassword )) {
 
      Serial.println("connected");  
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.publish("/watervalve", "Hello from ESP8266-03");
  client.subscribe("/watervalve");
 
}

long turnonTime = 0;
int ON = 0;
int COOLDOWN = 0;

// the loop function runs over and over again forever
void loop() {
  // get state from Kemet IR motion sensor
  int sig = pulseIn(14, HIGH); //Measure trigger point
  if(sig > IR_threshold){ //Make sure trigger is over 198msec)
    digitalWrite(2, HIGH);   // turn the LED on 
    IR_sensed++; //Mark as a good trigger
    if ( IR_sensed > 1 ) { 
      IR_sensed = 0;
      // if we are not in cooldown period after having run the water sprinkler, then turn it on
      if ( COOLDOWN == 0 ) {
        turnonTime = millis();
        ON = 1;
        COOLDOWN = 1;
        client.publish("/watervalve","ON");
      }
    } 
  } else {
    digitalWrite(2, LOW);   // turn the LED off 
  }
  if (ON == 1) {     // if water is on see if it has been on more than 5 seconds
    if ( millis() > turnonTime + 5L * 1000 ) {
      client.publish("/watervalve","OFF");
      ON = 0;
      turnonTime = millis();
      Serial.println("cooldown started "); 
    }
  }
  if ( COOLDOWN == 1 ){
    if ( millis() > turnonTime + 30L * 1000 ) {
      COOLDOWN = 0; 
      Serial.println("cooldown ended"); 
    }
  }  
  if (!client.connected()) {
    client.connect("mqttKemet", mqttUser, mqttPassword );
  }
  client.loop();
}


// put this in but we should not need to get inbound messages
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

