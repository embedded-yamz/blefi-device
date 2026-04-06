/*
 * wifi.c
 *
 *  Created on: 01 Mar 2026
 *      Author: dontk
 */

#include "wifi_sniffer.h"
#include "../ui/ui.h"
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "freertos/portmacro.h"
#include "esp_log.h"
#include <stdio.h>
#include <string.h>

static TaskHandle_t sniffer_task_handle = NULL;

static QueueHandle_t wifi_cb_queue = NULL;

static const char *WIFI_TAG = "WIFI_TASK";

//static const char *WIFI_TAG = "WIFI";
wifi_sniffer_msg_t latest_sniffer_data = {0};

static void sniffer_packet_cb(void *buf, wifi_promiscuous_pkt_type_t type){
	//handle the packets
	const wifi_promiscuous_pkt_t *packet = (wifi_promiscuous_pkt_t*)buf;
	const uint8_t *payload = packet->payload;
	const wifi_frame_ctrl_t *fc = (wifi_frame_ctrl_t*)payload;
	
	wifi_sniffer_msg_t msg = {0};
	
	msg.rssi = packet->rx_ctrl.rssi;
	msg.type = fc->type;
    msg.subtype = fc->subtype;
    msg.channel = packet->rx_ctrl.channel;
	
	memcpy(msg.mac_addr1, payload + 4, 6);
	memcpy(msg.mac_addr2, payload + 10, 6);
	memcpy(msg.mac_addr3, payload + 16, 6);
	   
    // Isolate Management Frames type -> 00
    if(fc->type == 0){
		
		if(fc->subtype == 8){ // Beacon frame
			
			const uint8_t *ie = payload + 36; // Directly to Frame Body to SSID; check beacon layout @ https://howiwifi.com/2020/07/13/802-11-frame-types-and-formats/
			uint8_t ssid_len = ie[1];
			
			if(ssid_len > 0 && ssid_len < 32){
				memcpy(msg.ssid, &ie[2], ssid_len);
				msg.ssid[ssid_len] = '\0';
			}
			
		}else if (fc->subtype == 4){ // Probe Request frame
			
			const uint8_t *ie = payload + 24; // Check link 
			if (ie[0] == 0){
				uint8_t ssid_len = ie[1];
			
				if(ssid_len > 0 && ssid_len < 32){
					memcpy(msg.ssid, &ie[2], ssid_len);		
					msg.ssid[ssid_len] = '\0';		
				} 
			}		
		// maybe add more stuff later
		} 
		if(fc->subtype == 4 || fc->subtype == 8){
			xQueueSendFromISR(wifi_cb_queue, &msg, NULL);
		}
	}
	
}

// Task that switches between channels and sniffs
/*void channel_hopper(void *arg) {
    uint8_t channel = 1;
    while (1) {
        esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
        printf("\n--- Switched to Channel %d ---\n", channel);

        channel++;
        if (channel > 13) channel = 1;
        vTaskDelay(pdMS_TO_TICKS(2000)); // hop every x sec
    }
}
*/

void wifi_sniffer_task(void *params){
	
	wifi_sniffer_msg_t msg;
	
	while(1){
		
		if (xQueueReceive(wifi_cb_queue, &msg, portMAX_DELAY)) {
			latest_sniffer_data = msg;
			ESP_LOGI(WIFI_TAG,"Detected Network: %s", msg.ssid);
			ui_set_mode(UI_MODE_SNIFFER);	
			// Give a bit of delay, let queue overflow... shits too fast
			vTaskDelay(500/portTICK_PERIOD_MS);	
		}
	}
}

void wifi_sniffer_init(void){
	
	if(sniffer_task_handle != NULL){ // Check if init called >once
		ESP_LOGW(WIFI_TAG,"Wifi Init already called");
		return;
	}
	esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    ESP_ERROR_CHECK(ret);
	
	if (wifi_cb_queue == NULL) {	// check in since menu action is linked to init... 
    wifi_cb_queue = xQueueCreate(10, sizeof(wifi_sniffer_msg_t));
	}
	
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_NULL));
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_set_channel(7, WIFI_SECOND_CHAN_NONE));
	
	const wifi_promiscuous_filter_t filter = {
		.filter_mask = 
				WIFI_PROMIS_FILTER_MASK_MGMT |
				WIFI_PROMIS_FILTER_MASK_DATA |
				WIFI_PROMIS_FILTER_MASK_CTRL 
	};
	
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous_filter(&filter));
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(sniffer_packet_cb));
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous(1));
	
	// Create wifi sniffer task
	xTaskCreate(wifi_sniffer_task, "wifi_sniffer_task", 4096, NULL, 4, &sniffer_task_handle);
	
	//ui_set_mode(UI_MODE_SNIFFER);	
		
}

void wifi_sniffer_deinit(void){
	ESP_ERROR_CHECK(esp_wifi_set_promiscuous(0));	// Exit promiscuous mode
	ESP_ERROR_CHECK(esp_wifi_stop());		// Stop
	ESP_ERROR_CHECK(esp_wifi_deinit());		// Free resources allocated to wifi_init
	
	if(sniffer_task_handle != NULL){
		vTaskDelete(sniffer_task_handle);
		sniffer_task_handle = NULL;
	}
	
	if(wifi_cb_queue != NULL){
		vQueueDelete(wifi_cb_queue);
		wifi_cb_queue = NULL;
	}
	
}

QueueHandle_t wifi_cb_get_queue(void){
	return wifi_cb_queue;
}

