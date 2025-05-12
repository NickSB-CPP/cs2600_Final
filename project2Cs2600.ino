#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "iPhone (3)";
const char* password = "7ehs17k64dx6j";

// IP of the machine running Mosquitto (on your local network)
const char* mqtt_server = "34.58.166.91";  // Replace with your actual local IP
const int ledPin = 2; // Pin connected to the LED
const char *mqtt_username = "CaptainL";
const char *mqtt_password = "MainingBroker@420";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// MQTT callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();


  // Control the LED based on the message
  if (payload[1] == 78) {
    digitalWrite(ledPin, HIGH);
  } else if (payload[1] == 70) {
    digitalWrite(ledPin, LOW);
  }
}

void setup() {
  // Start Serial Communication
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to MQTT Broker
  mqttClient.setServer(mqtt_server, 1883);  // Mosquitto default port
  mqttClient.setCallback(callback);

  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Connect to MQTT broker (you can use a unique client ID)
    if (mqttClient.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      mqttClient.subscribe("esp32/led");  // Subscribe to topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      delay(2000);
    }
  }
}

void loop() {
  mqttClient.loop();  // Keep the MQTT connection alive
}