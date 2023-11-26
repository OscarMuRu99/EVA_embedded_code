#include "Arduino.h"
#include "includes/humidity.h"
#include "includes/luminosity.h"

BaseType_t xHumidityTaskStatus;
TaskHandle_t xHumidityTaskHandle;
QueueHandle_t xHumidityQueue;

float readHumidityLevel() {
  return map(analogRead(humsuelo), 4095, 0, 0, 100);
}

void humidityTask(void *humidity_params) {
  pinMode(motorPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  float humidity_level = readHumidityLevel();

  xHumidityQueue = xQueueCreate(1, sizeof(&humidity_level));
  if (xHumidityQueue == nullptr) {
    esp_restart();
  }

  uint32_t ulEventValue;
  BaseType_t xEventOccured;

  while (true) {
    valhumsuelo = readHumidityLevel();

    xEventOccured = xTaskNotifyWait(pdFALSE, humidity_task_system_status_bit, &ulEventValue, pdMS_TO_TICKS(1UL));
    if (xEventOccured == pdPASS) {
      if ((ulEventValue & humidity_task_system_status_bit) != 0) {
        float *xPointerHumidityLevel = &valhumsuelo;
        if (xQueueSendToBack(xHumidityQueue, (void*)&xPointerHumidityLevel, (TickType_t)0) != pdPASS) {
          Serial.println("Failed to write to xHumidityQueue");
        }
      }
    }

    vTaskDelay(humidity_task_delay_ms);
  }
}