// ====> Libraries <====
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


// ====> Variables <====
const String codeVersion ="Version 0.1 - Test framework arduino";
// GPIO settings
const int GPIO2 = 2;          // LED_BUILTIN
const int GPIO4 = 4;          // LED_RED
const int GPIO5 = 5;          // LED_GREEN
//wifi settings
const char* wifi_ssid = "SFR-bc50";
const char* wifi_pwd = "Q4NC4D7KJJ65";
//MQTT server settings
const char* mqtt_server = "192.168.0.33";
const int mqtt_port = 1883;
const char* mqtt_user = "esp";
const char* mqtt_pwd = "bv94gp";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  // Connect to WiFi network
  Serial.println();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  Serial.print("Connecting to ");
  Serial.println(wifi_ssid); 
  
  WiFi.begin(wifi_ssid, wifi_pwd);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println();
  Serial.print("Connected to WiFi with ip : ");
  Serial.print(WiFi.localIP());
  digitalWrite(GPIO4, HIGH);    // LED_RED off
}

void connect_mqtt() {

  // Initialisation connection
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_pwd)) {
      Serial.println("connected"); 
      digitalWrite(GPIO4, LOW);    // LED_RED off
    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
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
  
  if (!strcmp(topic, "homebridge/LIGHT_MQTT/set_on")) {
    if (!strncmp((char *)payload, "true", length)) {
        digitalWrite(GPIO5, HIGH);
        client.publish("homebridge/LIGHT_MQTT/get_on", "true");
    }
    else if (!strncmp((char *)payload, "false", length)) {
        digitalWrite(GPIO5, LOW);
        client.publish("homebridge/LIGHT_MQTT/get_on", "false");
    }
  }
}

// ====> Setup loop <====
void setup() {
   // Serial COM init
  Serial.begin(9600);

  // GPIO setup
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO4, OUTPUT);
  pinMode(GPIO5, OUTPUT);
 
  // Print welcome mesg
  delay(2000);
  Serial.println();
  Serial.println(codeVersion);
  
  setup_wifi();

  //Set server & callback
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  connect_mqtt();

  client.publish("homebridge/LIGHT_MQTT/get_online", "true"); //Topic name
  client.subscribe("homebridge/LIGHT_MQTT/#");
}

// ====> Main loop <====
void loop() {
  client.loop();
}