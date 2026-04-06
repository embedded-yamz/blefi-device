/*
 * buttons.c
 *
 *  Created on: 14 Feb 2026
 *      Author: dontk
 */

#include "buttons.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "hal/gpio_types.h"

static QueueHandle_t button_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void *args){
	
	button_event_t event = (button_event_t)args;
	xQueueSendFromISR(button_evt_queue, &event, NULL);
	
}

QueueHandle_t buttons_get_queue(){
	
	return button_evt_queue;

}

void buttons_init(void){
	
	// Create queue to send button events
	button_evt_queue = xQueueCreate(10, sizeof(button_event_t));
	
	gpio_config_t io_config = {
		// Check for falling edge, poll if needed to see if button is held down
		.intr_type = GPIO_INTR_NEGEDGE,
		.mode = GPIO_MODE_INPUT,
		.pin_bit_mask = ((1ULL << BUTTON_UP)|(1ULL << BUTTON_DOWN)
						|(1ULL << BUTTON_LEFT)|(1ULL << BUTTON_RIGHT)),
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_ENABLE,
	}; // Resistors will be set in hardware for the most part
	// ESP_OK if there are no issues configuring
	ESP_ERROR_CHECK(gpio_config(&io_config));
	
	// Allocate ISR to GPIO inputs
	ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_IRAM));
	ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_UP, gpio_isr_handler, (void*)BUTTON_UP_PRESSED));
	ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_DOWN, gpio_isr_handler, (void*)BUTTON_DOWN_PRESSED));
	ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_LEFT, gpio_isr_handler, (void*)BUTTON_LEFT_PRESSED));
	ESP_ERROR_CHECK(gpio_isr_handler_add(BUTTON_RIGHT, gpio_isr_handler, (void*)BUTTON_RIGHT_PRESSED));
	
}
