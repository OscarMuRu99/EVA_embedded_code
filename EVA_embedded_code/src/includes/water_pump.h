//===============================================================================================================================================
/*
Project name: EVA
File name: water_pump.h
Description: 
*/
//===============================================================================================================================================

#ifndef WATER_PUMP_H
#define WATER_PUMP_H

static constexpr unsigned long water_pump_stack_size{10000};
static constexpr unsigned int water_pump_task_system_status_bit{0x02};

static constexpr TickType_t water_pump_task_delay_ms{pdMS_TO_TICKS(1UL)}; // 1ms

static const char* mqtt_server = "192.168.1.159";

static bool lumPumpStatus{false};

extern BaseType_t xWaterPumpTaskStatus;
extern TaskHandle_t xWaterPumpTaskHandle;
extern QueueHandle_t xWaterPumpQueue;

void waterPumpTask(void *water_pump_params);

#endif