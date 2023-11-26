#include "Arduino.h"
#include "includes/luminosity.h"

BaseType_t xLuminosityTaskStatus;
TaskHandle_t xLuminosityTaskHandle;
QueueHandle_t xLuminosityQueue;

//void processLDRCondition(unsigned long interval, const char *message);

float readLuminosityLevel(){
  return map(analogRead(LDRPin), 4095,0,0,100);
}

void luminosityTask(void *luminosity_params)
{
  // Setting up task I/Os

  // Refresh the humidity level value for the first time
  float luminosity_level = readLuminosityLevel();

  xLuminosityQueue = xQueueCreate(1, sizeof(&luminosity_level));
  if (xLuminosityQueue == nullptr)
  {
      //Serial.println("Error: Humidity Queue could not be created");
      //Serial.println("Rebooting...");
      esp_restart();
  }
  
  // Variable to store how much stack is consuming this task
  //UBaseType_t uxHighWaterMark;
  // Task event parameter
  uint32_t ulEventValue;
  // Task event notification receiver
  BaseType_t xEventOccured;

  while (true)
  {

    // Read luminosity and print it
    luminosity = readLuminosityLevel();
    //Serial.println(luminosity);
    delay(1000);

    xEventOccured = xTaskNotifyWait(pdFALSE, luminosity_task_system_status_bit, &ulEventValue, pdMS_TO_TICKS(1UL));
    // An event occurred?
    if (xEventOccured == pdPASS)
    {
        // Is it a system status event?
        if ((ulEventValue & luminosity_task_system_status_bit) != 0)
        {
            // Serial.println("Sending battery data to xBatteryQueue...");
            float *xPointerLuminosityLevel = &luminosity;
            if (xQueueSendToBack(xLuminosityQueue, (void*) &xPointerLuminosityLevel, (TickType_t) 0) != pdPASS)
            {
              Serial.println("Failed to write to xLuminosityQueue");
            }
            
        }
    }
    
    vTaskDelay(luminosity_task_delay_ms);
  }
}