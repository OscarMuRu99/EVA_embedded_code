#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "includes/water_pump.h"
#include "includes/humidity.h"
#include "includes/luminosity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

bool bomba{false};

BaseType_t xWaterPumpTaskStatus;
TaskHandle_t xWaterPumpTaskHandle;
QueueHandle_t xWaterPumpQueue;

void callback(char* topic, byte* message, unsigned int length);
void reconnect();
void processSensorData();
void publishSensorData();
void processWaterPumpStatus();
void processLDRCondition(unsigned long interval, const char *message);

void waterPumpTask(void *water_pump_params) {
  bool water_pump_switch_status = false;

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  esp_netif_init();

  while (true) {
    processSensorData();

    if (!client.connected()) {
      reconnect();
    }

    client.loop();

    publishSensorData();

    processWaterPumpStatus();

    if (!LDR_time) {
      LDR_prev_time = millis();
      LDR_time = true;
    }

    if (luminosity <= 25 && !toggle) {
      processLDRCondition(LDR_night_interval, "La planta necesita sol");
    } else if (luminosity >= 50 && toggle) {
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

    vTaskDelay(water_pump_task_delay_ms);
  }
}

void processSensorData() {
  if (client.connected()) {
    xTaskNotify(xHumidityTaskHandle, humidity_task_system_status_bit, eSetBits);
    float *humidityPtr;
    if (xQueueReceive(xHumidityQueue, &humidityPtr, Queue_humidity_wait_time) == pdPASS) {
      valhumsuelo = *humidityPtr;
    }

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

    char humString[8];
    dtostrf(valhumsuelo, 1, 2, humString);
    client.publish("esp32/humidity", humString);

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
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32")) {
      Serial.println("connected");
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void processLDRCondition(unsigned long interval, const char *message) {
  if (!flag) {
    LDR_time = false;
    flag = true;
  }

  if (millis() - LDR_prev_time >= interval) {
    if (millis() - printStartTime >= printDelay) {
      Serial.println(message);
      client.publish("esp32/status", message);
      printStartTime = millis();
      toggle = !toggle;
      flag = false;
    }
  }
}