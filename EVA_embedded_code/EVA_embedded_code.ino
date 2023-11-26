#include "src/includes/humidity.h"
#include "src/includes/luminosity.h"
#include "src/includes/water_pump.h"
#include "src/includes/wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

double FIRWARE_VERSION = 1.0;

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

constexpr unsigned long baud_rate = 9600;

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

void setupSerialDebugger()
{
	Serial.begin(baud_rate);
	Serial.println("Hello team! Welcome to the ");
	Serial.print("Firmware Version: ");
  Serial.println(FIRWARE_VERSION);
}

void initSystemRoutines()
{
	// Humidity routine task
	xHumidityTaskStatus = xTaskCreatePinnedToCore(
		&humidityTask,		 // Pointer to task
		"humidityTask",		 // Name of the task
		humidity_stack_size,  // Task stack size
		NULL,				 // Parameters to the task
		1,					 // Priority
		&xHumidityTaskHandle, // Handler of the task
		ARDUINO_RUNNING_CORE // Core ID
	);

  // Luminosity routine task
  xLuminosityTaskStatus = xTaskCreatePinnedToCore(
    &luminosityTask, //Pointer to task
    "luminosityTask", // Name of task
    luminosity_stack_size, // Task stack size
    NULL, // Parameters of the task
    1, // Priority
    &xLuminosityTaskHandle, // Handler of the task
    ARDUINO_RUNNING_CORE // Core ID
  );

	// WaterPump routine task
	xWaterPumpTaskStatus = xTaskCreatePinnedToCore(
		&waterPumpTask,		 // Pointer to task
		"waterPumpTask",		 // Name of the task
		water_pump_stack_size,  // Task stack size
		NULL,				 // Parameters to the task
		1,					 // Priority
		&xWaterPumpTaskHandle, // Handler of the task
		ARDUINO_RUNNING_CORE // Core ID
	);

  // WiFi routine task
	xWiFiTaskStatus = xTaskCreatePinnedToCore(
		&wifiTask,		 // Pointer to task
		"wifiTask",		 // Name of the task
		wifi_stack_size,  // Task stack size
		NULL,				 // Parameters to the task
		1,					 // Priority
		&xWiFiTaskHandle, // Handler of the task
		ARDUINO_RUNNING_CORE // Core ID
	);

	if ((xHumidityTaskStatus == pdPASS) &&
		(xLuminosityTaskStatus == pdPASS) &&
		(xWaterPumpTaskStatus == pdPASS) &&
    (xWiFiTaskStatus)
		) 
	{
		Serial.println("All RTOS tasks created");
	}
	else
	{
		Serial.println("An error occurred while creating the RTOS tasks");
	}
}

void setup() {
  setupSerialDebugger();
	if(xMutex != NULL)
  {
		initSystemRoutines();
  }
}

void loop() {}
