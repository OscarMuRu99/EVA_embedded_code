//===============================================================================================================================================
/*
Project name: EVA
File name: wifi.h
Description: 
*/
//===============================================================================================================================================

#ifndef WIFI_H
#define WIFI_H

static constexpr unsigned long wifi_stack_size{5000};
static constexpr unsigned int wifi_task_system_status_bit{0x02};
static constexpr TickType_t wifi_task_delay_ms{pdMS_TO_TICKS(1UL)}; // 1ms

static const char* ssid = "INFINITUM8AA4_2.4";
static const char* password = "38544v9kHf";

//static const char* ssid = "#GRANDES_LIDERES";
//static const char* password = NULL;

extern BaseType_t xWiFiTaskStatus;
extern TaskHandle_t xWiFiTaskHandle;
extern QueueHandle_t xWiFiQueue;

void wifiTask(void *wifi_params);

#endif