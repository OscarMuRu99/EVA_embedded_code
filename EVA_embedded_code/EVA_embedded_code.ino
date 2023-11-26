#include "src/includes/humidity.h"
#include "src/includes/luminosity.h"
#include "src/includes/water_pump.h"
#include "src/includes/wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

String FIRMWARE_VERSION = "1.0.0";

#if CONFIG_FREERTOS_UNICORE
  #define ARDUINO_RUNNING_CORE 0
#else
  #define ARDUINO_RUNNING_CORE 1
#endif

constexpr unsigned long baud_rate = 9600;

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

void setupSerialDebugger() {
  Serial.begin(baud_rate);
  Serial.println("Hello team! Welcome to the Firmware Version: " + String(FIRMWARE_VERSION));
}

void initSystemRoutines() {
  // Humidity routine task
  xHumidityTaskStatus = xTaskCreatePinnedToCore(
    &humidityTask, "humidityTask", humidity_stack_size, NULL, 1, &xHumidityTaskHandle, ARDUINO_RUNNING_CORE
  );

  // Luminosity routine task
  xLuminosityTaskStatus = xTaskCreatePinnedToCore(
    &luminosityTask, "luminosityTask", luminosity_stack_size, NULL, 1, &xLuminosityTaskHandle, ARDUINO_RUNNING_CORE
  );

  // WaterPump routine task
  xWaterPumpTaskStatus = xTaskCreatePinnedToCore(
    &waterPumpTask, "waterPumpTask", water_pump_stack_size, NULL, 1, &xWaterPumpTaskHandle, ARDUINO_RUNNING_CORE
  );

  // WiFi routine task
  xWiFiTaskStatus = xTaskCreatePinnedToCore(
    &wifiTask, "wifiTask", wifi_stack_size, NULL, 1, &xWiFiTaskHandle, ARDUINO_RUNNING_CORE
  );

  if (xHumidityTaskStatus == pdPASS && xLuminosityTaskStatus == pdPASS && xWaterPumpTaskStatus == pdPASS && xWiFiTaskStatus == pdPASS) {
    Serial.println("All RTOS tasks created");
  } else {
    Serial.println("An error occurred while creating the RTOS tasks");
  }
}

void setup() {
  setupSerialDebugger();
  if (xMutex != NULL) {
    initSystemRoutines();
  }
}

void loop() {}