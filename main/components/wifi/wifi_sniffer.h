/*
 * wifi.h
 *
 *  Created on: 01 Mar 2026
 *      Author: dontk
 */

#ifndef MAIN_COMPONENTS_WIFI_WIFI_SNIFFER_H_
#define MAIN_COMPONENTS_WIFI_WIFI_SNIFFER_H_

#include <string.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_wifi_types.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"

//#define CONNECTED_WIFI_SSID		"DEFAULT"
//#define WIFI_PASSWORD			"DEFAULT"

typedef struct {
    uint8_t subtype : 4;
    uint8_t type    : 2;
    uint8_t version : 2;
    uint8_t flags;
} wifi_frame_ctrl_t;

typedef struct {
	int8_t rssi; // Signal Strength
	uint8_t type; // type of signal : management frame, etc
	uint8_t subtype; // can either be beacon or probe request frame
	
	uint8_t mac_addr1[6];
	uint8_t mac_addr2[6];
	uint8_t mac_addr3[6];
	
	char ssid[33]; // null-terminated
	uint8_t channel;
	
}wifi_sniffer_msg_t;

extern wifi_sniffer_msg_t latest_sniffer_data;

//static EventGroupHandle_t wifi_event_group;
void channel_hopper(void *arg);
void wifi_sniffer_init(void);
void wifi_sniffer_deinit(void);
QueueHandle_t wifi_cb_get_queue(void);
void wifi_sniffer_task(void *params);

#endif /* MAIN_COMPONENTS_WIFI_WIFI_SNIFFER_H_ */
