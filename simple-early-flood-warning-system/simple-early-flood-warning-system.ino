#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//WiFi Setup
const char* ssid="YOUR_SSID";
const char* pswd="YOUR_SSID_PASSWORD";

//MQTT Server
const char* mqtt_server="YOUR_MQTT_SERVER";
const int mqtt_port=1883;

WiFiClient espClient;
PubSubClient client(espClient);

long duration;
float distance;

long now = millis();
long lastMeasure= 0;


int waterPin = A0;
const int trigPin = D2;
const int echoPin = D1;

boolean water_detected = false;


void setup_wifi(){
  delay(10);
  WiFi.begin(ssid, pswd);
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_pub(){
  Serial.print("Attempting MQTT Connection...");
  if(client.connect("ESP8266Client")){
    Serial.println("Connected");
  }
  else{
    Serial.print("MQTT Connection failed, rc=");
    Serial.print(client.state());
    Serial.println("reconnect in 5 seconds");
    delay(5000);
  }
}

void setup(){
  Serial.begin(115200);
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(mqtt_server,mqtt_port);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
}

void loop(){
  if(!client.connected())
    mqtt_pub();

  if(!client.loop())
    client.connect("ESPClient8266");

  now = millis();
  if(now - lastMeasure > 10000){
    analogRead(waterPin);

    if(waterPin ==17){
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      duration = pulseIn(echoPin, HIGH);
      distance = duration*0.034/2;
      water_detected = true;
    }
    else{
      water_detected = false;
    }

    static char distanceTemp[7];
    dtostrf(distance,7,2,distanceTemp);
    
    //client.publish("/waterSensor/waterDetected", water_detected);
    client.publish("/penang1/watersensor1/waterlevel", distanceTemp);

    //Display in serial monitor only
    Serial.print("Water measure: ");
    Serial.println(waterPin);
    Serial.print("Water Level: ");
    Serial.println(distance);
    delay(5000);
  }
}
