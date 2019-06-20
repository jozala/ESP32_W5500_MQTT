#include "MasterConfig.h"

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

IPAddress ipAddress;
PubSubClient mqttClient;
EthernetClient ethClient;

uint32_t lastMqttPublishTime = 0;

void macCharArrayToBytes(const char* str, byte* bytes) {
    for (int i = 0; i < 6; i++) {
        bytes[i] = strtoul(str, NULL, 16);
        str = strchr(str, ':');
        if (str == NULL || *str == '\0') {
            break;
        }
        str++;
    }
}

/*
 * Wiz W5500 reset function
 */
void ethernetWizReset(const uint8_t resetPin) {
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);
    delay(250);
    digitalWrite(resetPin, LOW);
    delay(50);
    digitalWrite(resetPin, HIGH);
    delay(350);
}

void connectEthernet() {
    delay(500);
    byte* mac = new byte[6];
    macCharArrayToBytes(ETHERNET_MAC, mac);
    ipAddress.fromString(ETHERNET_IP);

    Ethernet.init(ETHERNET_CS_PIN);
    ethernetWizReset(ETHERNET_RESET_PIN);

    Serial.println("Starting ETHERNET connection...");
    Ethernet.begin(mac, ipAddress);
    delay(200);

    Serial.print("Ethernet IP is: ");
    Serial.println(Ethernet.localIP());
}

void setupMqtt(const char* hostname, uint16_t port, Client& _connectionClient) {
    mqttClient.setClient(_connectionClient);
    mqttClient.setServer(hostname, port);
}

void connectToMqtt(const char* deviceName, const char* willTopic, uint8_t willQoS, bool willRetain, const char* willMessage) {
    Serial.println("Connecting to MQTT broker...");
    while(!mqttClient.connect(deviceName, willTopic, willQoS, willRetain, willMessage)) {
        Serial.print("Connecting to MQTT as ");
        Serial.println(deviceName);
        delay(1000);
    }
    Serial.print("Connected to MQTT as ");
    Serial.print(deviceName);
    Serial.print(". LWT: ");
    Serial.print(willTopic);
    Serial.print(" -> ");
    Serial.println(willMessage);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void mqttReconnect() {
  connectToMqtt("MyDevice", "mydevice/status", 1, 1, "lost");
  mqttClient.setCallback(mqttCallback);
  mqttClient.subscribe("mydevice/listen");
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32_W5500_MQTT");
  connectEthernet();
  setupMqtt(MQTT_HOSTNAME, MQTT_PORT, ethClient);
  mqttReconnect();
}

void loop() {
  if (!mqttClient.connected()) {
    Serial.println("ERROR: mqttClient has been disconnected. Reconnecting...");
    mqttReconnect();
  }
  mqttClient.loop();
  uint32_t now = millis();
  if (now - lastMqttPublishTime > MQTT_PUBLISH_INTERVAL_MS) {
    String uptimeInSec = String(now/1000);
    Serial.print("Sending uptime message to MQTT: ");
    Serial.print("mydevice/uptime <- ");
    Serial.println(uptimeInSec);
    mqttClient.publish("mydevice/uptime", uptimeInSec.c_str());
    mqttClient.publish("mydevice/fakemessage", "sending long message to check -> of this influence stability");
    mqttClient.publish("mydevice/fakemessage", "and another to check if -> of this influence stability");
    mqttClient.publish("mydevice/fakemessage", "and even one more to check if -> of this influence stability");
    mqttClient.publish("mydevice/fakemessage2", "sending long message to check -> of this influence stability");
    mqttClient.publish("mydevice/fakemessage2", "and another to check if -> of this influence stability");
    mqttClient.publish("mydevice/fakemessage2", "and even one more to check if -> of this influence stability");
    lastMqttPublishTime = now;
  }
}