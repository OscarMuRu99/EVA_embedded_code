#include "Arduino.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "includes/wifi.h"
#include "includes/humidity.h"

BaseType_t xWiFiTaskStatus;
TaskHandle_t xWiFiTaskHandle;
QueueHandle_t xWiFiQueue;

void setup_wifi();

void wifiTask(void *wifi_params)
{ 
  setup_wifi();

  bool wifi_status = false;

  // Variable to store how much stack is consuming this task
  //UBaseType_t uxHighWaterMark;
  // Task event parameter
  uint32_t ulEventValue;
  // Task event notification receiver
  BaseType_t xEventOccured;

  while (true)
  {
    xEventOccured = xTaskNotifyWait(pdFALSE, wifi_task_system_status_bit, &ulEventValue, pdMS_TO_TICKS(1UL));
    // An event occurred?
    if (xEventOccured == pdPASS)
    {
      // Is it a system status event?
      if ((ulEventValue & wifi_task_system_status_bit) != 0)
      {
        bool *xPointerWiFiState = &wifi_status;
        if (xQueueSendToBack(xWiFiQueue, (void*)&xPointerWiFiState, (TickType_t)0) != pdPASS)
        {
          Serial.println("Failed to write to xWiFiQueue");
        }
      }
    }
    vTaskDelay(wifi_task_delay_ms);
  }
}

void setup_wifi() 
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}