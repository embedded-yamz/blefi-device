/*
 * buttons.h
 *
 *  Created on: 14 Feb 2026
 *      Author: dontk
 */

#ifndef MAIN_BUTTONS_BUTTONS_H_
#define MAIN_BUTTONS_BUTTONS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define BUTTON_UP		GPIO_NUM_16
#define BUTTON_DOWN 	GPIO_NUM_32
#define BUTTON_LEFT		GPIO_NUM_19
#define BUTTON_RIGHT	GPIO_NUM_22

typedef enum {
	BUTTON_UP_PRESSED,
	BUTTON_DOWN_PRESSED,
	BUTTON_LEFT_PRESSED,
	BUTTON_RIGHT_PRESSED,
}button_event_t;

void buttons_init(void);
// Buttons files should just detect button presses
QueueHandle_t buttons_get_queue(void);

#endif /* MAIN_BUTTONS_BUTTONS_H_ */
