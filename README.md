# HacksterKemetMakeSenseofOurProximitySensorContest
Hackster Kemet Make Sense of Our Proximity Sensor Contest<br/>
<br/>
esp8266_mqttValve - Arduino sketch for esp8266 that subscribes to mqtt topic and turns the solid state relay on / off<br/>
topic:  /watervalve<br/>
On message: ON<br/>
Off message: OFF<br/>
<br/>
esp8266_Kemetmqtt_2Reconnect - Arduino sketch for the esp8266 interfaced with the Kemet PIR sensor, detects pulses of 198mSec, and publishes an 'ON' message to the /watervalve topic. After the water to sprinkler is turned on for 5 seconds, publish an 'OFF' to turn it off, then wait 30 seconds cooldown period before responding to any more pulses from the Kemet Sensor.



