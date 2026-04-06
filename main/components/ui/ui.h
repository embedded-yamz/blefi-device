/*
 * ui.h
 *
 *  Created on: 16 Feb 2026
 *      Author: dontk
 */

#ifndef MAIN_UI_UI_H_
#define MAIN_UI_UI_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

extern TaskHandle_t ui_task_handle;

typedef enum {
	UI_MODE_MENU,
	UI_MODE_SNIFFER,
	UI_MODE_SETTINGS
	//can add some other stuff
}ui_mode_t;


void ui_task(void *args);
void render_menu(void);
void ui_set_mode(ui_mode_t mode_set);
void render_wifi_sniffer(void);

#endif /* MAIN_UI_UI_H_ */
