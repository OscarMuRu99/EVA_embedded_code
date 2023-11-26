#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "includes/wifi.h"
#include "includes/humidity.h"

BaseType_t xWiFiTaskStatus;
TaskHandle_t xWiFiTaskHandle;
QueueHandle_t xWiFiQueue;

void setupWiFi();

void wifiTask(void *wifi_params) {
  setupWiFi();

  bool wifiStatus = false;

  uint32_t ulEventValue;
  BaseType_t xEventOccurred;

  while (true) {
    xEventOccurred = xTaskNotifyWait(pdFALSE, wifi_task_system_status_bit, &ulEventValue, pdMS_TO_TICKS(1UL));

    if (xEventOccurred == pdPASS) {
      if ((ulEventValue & wifi_task_system_status_bit) != 0) {
        bool *xPointerWiFiState = &wifiStatus;
        if (xQueueSendToBack(xWiFiQueue, (void*)&xPointerWiFiState, (TickType_t)0) != pdPASS) {
          Serial.println("Failed to write to xWiFiQueue");
        }
      }
    }

    vTaskDelay(wifi_task_delay_ms);
  }
}

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}