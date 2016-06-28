#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <AccelStepper.h>

// Wifi + Server Configuration
const char* ssid = "*****";
const char* password = "*****";
const char* mqtt_server = "*****";

WiFiClient espClient;
PubSubClient client(espClient);
String switch1;
String strTopic;
String strPayload;
long current_position = 0;

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, 15, 14); // 15-PUL,14-DIR
int togglePin = 13; //Switches the driver module on/off

void setup_wifi() {
  delay(10);
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
  payload[length] = '\0';
  strTopic = String((char*)topic);
  if(strTopic == "ha/kitchen_curtains")
    {
    switch1 = String((char*)payload);
    if(switch1 == "ON")
      {
        Serial.println("ON");
        digitalWrite(togglePin, HIGH);
     current_position = stepper.currentPosition();
     if(current_position>=5850) {
      stepper.moveTo(0);
     }
     else if(current_position<=0) {
      stepper.moveTo(5850);
     }
    stepper.run(); 
      }
    else
      {
        Serial.println("OFF");
        digitalWrite(togglePin, HIGH);
     current_position = stepper.currentPosition();
     if(current_position>=5850) {
      stepper.moveTo(0);
     }
     else if(current_position<=0) {
      stepper.moveTo(5850);
     }
    stepper.run();
      }
    }
}
 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Kitchen Curtains")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe("ha/kitchen_curtains");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup() {
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println("READY");

  //configure stepper
  stepper.setAcceleration(1000);
  stepper.setCurrentPosition(0);
  stepper.setMaxSpeed(1500);

  //set pin modes
  pinMode(togglePin, OUTPUT);
  digitalWrite(togglePin, LOW);
}
 
void loop() {
  if (stepper.run() != true) {
    digitalWrite(togglePin, LOW);
  }
  if (WiFi.status() != WL_CONNECTED) {
	setup_wifi();
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
