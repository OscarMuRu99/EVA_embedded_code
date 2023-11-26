//===============================================================================================================================================
/*
Project name: EVA
File name: humidity.h
Description: 
*/
//===============================================================================================================================================

#ifndef HUMIDITY_H
#define HUMIDITY_H

static constexpr unsigned long humidity_stack_size{1792};
static constexpr unsigned int humidity_task_system_status_bit{0x02};
static constexpr TickType_t Queue_humidity_wait_time{250}; // 100ms
static constexpr TickType_t humidity_task_delay_ms{pdMS_TO_TICKS(1UL)}; // 1ms

static const int humsuelo = 33; //lectura del sensor
static float valhumsuelo;
static const int motorPin = 12; // Pin de control PWM conectado al puente H
static int in1 = 14;
static int in2 = 27;

extern BaseType_t xHumidityTaskStatus;
extern TaskHandle_t xHumidityTaskHandle;
extern QueueHandle_t xHumidityQueue;

void humidityTask(void *humidity_params);

#endif