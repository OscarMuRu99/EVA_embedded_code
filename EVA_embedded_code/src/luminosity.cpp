#include "Arduino.h"
#include "includes/luminosity.h"

BaseType_t xLuminosityTaskStatus;
TaskHandle_t xLuminosityTaskHandle;
QueueHandle_t xLuminosityQueue;

float readLuminosityLevel() {
  return map(analogRead(LDRPin), 4095, 0, 0, 100);
}

void luminosityTask(void *luminosity_params) {
  float luminosity_level = readLuminosityLevel();

  xLuminosityQueue = xQueueCreate(1, sizeof(&luminosity_level));
  if (xLuminosityQueue == nullptr) {
    esp_restart();
  }

  uint32_t ulEventValue;
  BaseType_t xEventOccured;

  while (true) {
    luminosity = readLuminosityLevel();
    delay(1000);

    xEventOccured = xTaskNotifyWait(pdFALSE, luminosity_task_system_status_bit, &ulEventValue, pdMS_TO_TICKS(1UL));
    if (xEventOccured == pdPASS) {
      if ((ulEventValue & luminosity_task_system_status_bit) != 0) {
        float *xPointerLuminosityLevel = &luminosity;
        if (xQueueSendToBack(xLuminosityQueue, (void*)&xPointerLuminosityLevel, (TickType_t)0) != pdPASS) {
          Serial.println("Failed to write to xLuminosityQueue");
        }
      }
    }

    vTaskDelay(luminosity_task_delay_ms);
  }
}