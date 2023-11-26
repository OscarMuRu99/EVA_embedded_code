//===============================================================================================================================================
/*
Project name: EVA
File name: luminosity.h
Description: 
*/
//===============================================================================================================================================

#ifndef LUMINOSITY_H
#define LUMINOSITY_H

static constexpr unsigned long luminosity_stack_size{2000};
static constexpr unsigned int luminosity_task_system_status_bit{0x02};
static constexpr TickType_t Queue_luminosity_wait_time{250}; // 100ms
static constexpr TickType_t luminosity_task_delay_ms{pdMS_TO_TICKS(1UL)}; // 1ms

// Define constants for LDR pin and intervals
static const int LDRPin = 34; // Luminosity sensor pin
static const unsigned long LDR_night_interval = 5000; // Night interval in milliseconds
static const unsigned long LDR_day_interval = 2500;   // Day interval in milliseconds
static const unsigned long printDelay = 15000;         // 5 seconds delay before printing message

// Variables for LDR and timing
static float luminosity = 0;
static unsigned int LDR_prev_time = 0;
static bool LDR_time = true;
static bool toggle = false;
static bool flag = false;
static unsigned long printStartTime = 0; // Variable to store the time when the message is triggered

extern BaseType_t xLuminosityTaskStatus;
extern TaskHandle_t xLuminosityTaskHandle;
extern QueueHandle_t xLuminosityQueue;

void luminosityTask(void *luminosity_params);

#endif