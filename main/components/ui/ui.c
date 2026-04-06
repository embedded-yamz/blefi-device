/*
 * ui.c
 *
 *  Created on: 16 Feb 2026
 *      Author: dontk
 */

#include "ui.h"
#include "../menu/menu.h"
#include "../lcd/lcd.h"
#include "../wifi/wifi_sniffer.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include <stdint.h>
#include <stdio.h>

TaskHandle_t ui_task_handle = NULL;

ui_mode_t current_mode = UI_MODE_MENU;

/*
	The idea in ui is to have all the other components functions combined here
	For example, the buttons queue, etc should be waited for here to see what
	I will do for them. In the event that button RIGHT is pressed, then the 
	menu_next function should be called.
*/

void ui_set_mode(ui_mode_t mode_set){
	current_mode = mode_set;
	xTaskNotifyGive(ui_task_handle);
}

void ui_task(void *args){
	
	while(1){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		
		switch (current_mode) {
			case UI_MODE_MENU:
				render_menu();
			break;
			case UI_MODE_SNIFFER:
				render_wifi_sniffer();
			break;
			default:
			break;
		}
		
	}
}

void render_menu(void){
	
	MenuItem *menu = menu_get_current();
	uint8_t cursor = menu_get_cursor_index();
		
	//ui_render(menu, cursor);
	lcd_fill_screen(COLOUR_BLACK);
	for(uint8_t i = 0; i < menu->num_children; i++){
		uint16_t colour = (i == cursor) ? COLOUR_RED : COLOUR_WHITE;
		lcd_draw_text(10, 20 + (i*20), menu->children[i]->name, colour, COLOUR_BLACK);
	}
}

void render_wifi_sniffer(void){

	// lcd_fill_screen(COLOUR_BLACK);
	char string_buf[50] = {0};
	snprintf(string_buf, sizeof(string_buf), "SSID: %.20s", latest_sniffer_data.ssid); // limit .20s to 20 since ssid can be 32 long
	lcd_fill_rect(10, 120, 200, 12, COLOUR_BLACK);
	lcd_draw_text(10, 120,string_buf, COLOUR_WHITE, COLOUR_BLACK);
	sprintf(string_buf, "RSSI: %d",latest_sniffer_data.rssi);
	lcd_fill_rect(10, 135, 200, 12, COLOUR_BLACK);
	lcd_draw_text(10, 135,string_buf, COLOUR_WHITE, COLOUR_BLACK);
	snprintf(string_buf, sizeof(string_buf),
         "MAC1: %02X:%02X:%02X:%02X:%02X:%02X",
         latest_sniffer_data.mac_addr1[0],
         latest_sniffer_data.mac_addr1[1],
         latest_sniffer_data.mac_addr1[2],
         latest_sniffer_data.mac_addr1[3],
         latest_sniffer_data.mac_addr1[4],
         latest_sniffer_data.mac_addr1[5]);
    lcd_fill_rect(10, 150, 200, 12, COLOUR_BLACK);     
	lcd_draw_text(10, 150,string_buf, COLOUR_WHITE, COLOUR_BLACK);
	snprintf(string_buf, sizeof(string_buf),
         "MAC2: %02X:%02X:%02X:%02X:%02X:%02X",
         latest_sniffer_data.mac_addr2[0],
         latest_sniffer_data.mac_addr2[1],
         latest_sniffer_data.mac_addr2[2],
         latest_sniffer_data.mac_addr2[3],
         latest_sniffer_data.mac_addr2[4],
         latest_sniffer_data.mac_addr2[5]);
	lcd_fill_rect(10, 165, 200, 12, COLOUR_BLACK);
	lcd_draw_text(10, 165,string_buf, COLOUR_WHITE, COLOUR_BLACK);
	snprintf(string_buf, sizeof(string_buf),
         "MAC3: %02X:%02X:%02X:%02X:%02X:%02X",
         latest_sniffer_data.mac_addr3[0],
         latest_sniffer_data.mac_addr3[1],
         latest_sniffer_data.mac_addr3[2],
         latest_sniffer_data.mac_addr3[3],
         latest_sniffer_data.mac_addr3[4],
         latest_sniffer_data.mac_addr3[5]);
    lcd_fill_rect(10, 180, 200, 12, COLOUR_BLACK);
	lcd_draw_text(10, 180,string_buf, COLOUR_WHITE, COLOUR_BLACK);
	sprintf(string_buf, "CHANNEL: %d",latest_sniffer_data.channel);
	lcd_fill_rect(10, 195, 200, 12, COLOUR_BLACK);
	lcd_draw_text(10, 195,string_buf, COLOUR_WHITE, COLOUR_BLACK);
		
}
