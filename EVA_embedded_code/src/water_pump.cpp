#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "includes/water_pump.h"
#include "includes/humidity.h"
#include "includes/luminosity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// Global Variables
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

bool bomba{false};

// Task Status and Handles
BaseType_t xWaterPumpTaskStatus;
TaskHandle_t xWaterPumpTaskHandle;
QueueHandle_t xWaterPumpQueue;

// Function Declarations
void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void processSensorData();
void publishSensorData();
void processWaterPumpStatus();
void processLDRCondition(unsigned long interval, const char *message);

void waterPumpTask(void *water_pump_params)
{ 
  bool water_pump_switch_status = false;

  // Initialize MQTT client
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  esp_netif_init();

  while (true)
  {
    // Process sensor data and update global variables
    processSensorData();

    // Reconnect to MQTT broker if not connected
    if (!client.connected()) {
      reconnect();
    }

    // Handle MQTT messages
    client.loop();

    // Publish sensor data periodically
    publishSensorData();

    // Process water pump status and update if necessary
    processWaterPumpStatus();

    // Reset timing if necessary
    if (!LDR_time) {
      LDR_prev_time = millis();
      LDR_time = true;
    }

     // Check luminosity conditions
    if (luminosity <= 25 && !toggle) { // In the shadow
      processLDRCondition(LDR_night_interval, "La planta necesita sol");
    } else if (luminosity >= 50 && toggle) { // In the light
      processLDRCondition(LDR_day_interval, "La planta ya tuvo suficiente sol");
    }

    if (luminosity <= 30) {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      if (valhumsuelo <= 30) {
        analogWrite(motorPin, 200);
        Serial.println("Motor al 100");
      } else if (valhumsuelo <= 50) {
        analogWrite(motorPin, 100);
        Serial.println("Motor al 50");
      } else {
        analogWrite(motorPin, 0);
        Serial.println("Motor al 0");
      }
    } else {
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
      analogWrite(motorPin, 0);
      Serial.println("Motor al 0");
    }

    // Delay before next iteration
    vTaskDelay(water_pump_task_delay_ms);
  }
}

void processSensorData() {
  if (client.connected()) {
    // Process humidity data
    xTaskNotify(xHumidityTaskHandle, humidity_task_system_status_bit, eSetBits);
    float *humidityPtr;
    if (xQueueReceive(xHumidityQueue, &humidityPtr, Queue_humidity_wait_time) == pdPASS) {
      valhumsuelo = *humidityPtr;
    }

    // Process luminosity data
    xTaskNotify(xLuminosityTaskHandle, luminosity_task_system_status_bit, eSetBits);
    float *luminosityPtr;
    if (xQueueReceive(xLuminosityQueue, &luminosityPtr, Queue_luminosity_wait_time) == pdPASS) {
      luminosity = *luminosityPtr;
    }
  }
}

void publishSensorData() {

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    // Publish humidity data
    char humString[8];
    dtostrf(valhumsuelo, 1, 2, humString);
    client.publish("esp32/humidity", humString);

    // Publish luminosity data
    char lumString[8];
    dtostrf(luminosity, 1, 2, lumString);
    client.publish("esp32/luminosity", lumString);
  }
}

void processWaterPumpStatus() {
  // Your water pump status handling logic here
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Handle MQTT messages based on the topic
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      bomba = true;
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      analogWrite(motorPin, 0);
      Serial.println("Apagado desde MQTT");
      bomba = false;
    }
  }
}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Function to handle LDR conditions
void processLDRCondition(unsigned long interval, const char *message) {
  if (!flag) {
    LDR_time = false;
    flag = true;
  }

  if (millis() - LDR_prev_time >= interval) {
    if (millis() - printStartTime >= printDelay) {
      Serial.println(message);
      // Publish luminosity data
      //char statusString[8];
      //dtostrf(message, 1, 2, statusString);
      client.publish("esp32/status", message);
      printStartTime = millis(); // Record the time when the message is triggered
      toggle = !toggle; // Toggle the state
      flag = false;      // Reset the flag
    }
  }
}
