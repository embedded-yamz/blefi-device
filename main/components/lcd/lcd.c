/*
 * lcd.c
 *
 *  Created on: 13 Feb 2026
 *      Author: dontk
 */

#include "lcd.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include <stdint.h>

spi_device_handle_t lcd_spi;

void spi_bus_init(void){
	
	gpio_config_t io_conf = {
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = (1ULL << PIN_NUM_DC)|(1ULL << PIN_NUM_RST),
		// maybe add backlight pin
	};
	gpio_config(&io_conf);
	
	spi_bus_config_t bus_config = {
		.miso_io_num = -1, // could possibly leave out
		.mosi_io_num = PIN_NUM_MOSI,
		.sclk_io_num = PIN_NUM_CLK,
		.quadhd_io_num = -1, 
		.quadwp_io_num = -1,
		.max_transfer_sz = LCD_HEIGHT * 80 * sizeof(uint16_t),
	};
	
	ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &bus_config, SPI_DMA_CH_AUTO));

	spi_device_interface_config_t dev_config = {
		.mode = 0,
		.clock_speed_hz = 1*1000*1000,
		.queue_size = 7,
		.spics_io_num = PIN_NUM_CS,
	};
	
	ESP_ERROR_CHECK(spi_bus_add_device(LCD_HOST, &dev_config, &lcd_spi));

}

void lcd_send_cmd(uint8_t cmd){
	
	gpio_set_level(PIN_NUM_DC, 0);
	spi_transaction_t t = {
		.tx_buffer = &cmd,
		.length = 8,
	} ;
	
	ESP_ERROR_CHECK(spi_device_transmit(lcd_spi, &t));
	
}

void lcd_send_data(uint8_t* data, int len){
	
	if(len == 0) return;
	
	gpio_set_level(PIN_NUM_DC, 1);
	spi_transaction_t t = {
		.tx_buffer = data,
		.length = 8 * len,
	} ;
	
	ESP_ERROR_CHECK(spi_device_transmit(lcd_spi, &t));
	
}

void ili9341_init(void){
	
	gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
		
	lcd_send_cmd(0xEF);
    uint8_t data[] = {0x03, 0x80, 0x02};
    lcd_send_data(data, 3);

    lcd_send_cmd(0xCF);
    uint8_t data_cf[] = {0x00, 0xC1, 0x30};
    lcd_send_data(data_cf, 3);

    lcd_send_cmd(0xED);
    uint8_t data_ed[] = {0x64, 0x03, 0x12, 0x81};
    lcd_send_data(data_ed, 4);

    lcd_send_cmd(0xE8);
    uint8_t data_e8[] = {0x85, 0x00, 0x78};
    lcd_send_data(data_e8, 3);

    lcd_send_cmd(0xCB);
    uint8_t data_cb[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
    lcd_send_data(data_cb, 5);

    lcd_send_cmd(0xF7);
    uint8_t data_f7 = 0x20;
    lcd_send_data(&data_f7, 1);

    lcd_send_cmd(0xEA);
    uint8_t data_ea[] = {0x00, 0x00};
    lcd_send_data(data_ea, 2);

    lcd_send_cmd(0xC0); // Power control
    uint8_t data_c0 = 0x23;
    lcd_send_data(&data_c0, 1);

    lcd_send_cmd(0xC1); // Power control
    uint8_t data_c1 = 0x10;
    lcd_send_data(&data_c1, 1);

    lcd_send_cmd(0xC5); // VCM control
    uint8_t data_c5[] = {0x3e, 0x28};
    lcd_send_data(data_c5, 2);

    lcd_send_cmd(0xC7); // VCM control2
    uint8_t data_c7 = 0x86;
    lcd_send_data(&data_c7, 1);

    lcd_send_cmd(0x36); // Memory Access Control
    //uint8_t data_36 = 0x48;
    uint8_t data_36 = 0x40;
    lcd_send_data(&data_36, 1);

    lcd_send_cmd(0x3A);
    uint8_t data_3a = 0x55;
    lcd_send_data(&data_3a, 1);

    lcd_send_cmd(0xB1);
    uint8_t data_b1[] = {0x00, 0x18};
    lcd_send_data(data_b1, 2);

    lcd_send_cmd(0xB6);
    uint8_t data_b6[] = {0x08, 0x82, 0x27};
    lcd_send_data(data_b6, 3);

    lcd_send_cmd(0xF2);
    uint8_t data_f2 = 0x00;
    lcd_send_data(&data_f2, 1);

    lcd_send_cmd(0x26);
    uint8_t data_26 = 0x01;
    lcd_send_data(&data_26, 1);

    lcd_send_cmd(0xE0);
    uint8_t data_e0[] = {0x0F, 0x31, 0x2B, 0x0C,
                         0x0E, 0x08, 0x4E, 0xF1,
                         0x37, 0x07, 0x10, 0x03,
                         0x0E, 0x09, 0x00};
    lcd_send_data(data_e0, 15);

    lcd_send_cmd(0xE1);
    uint8_t data_e1[] = {0x00, 0x0E, 0x14, 0x03,
                         0x11, 0x07, 0x31, 0xC1,
                         0x48, 0x08, 0x0F, 0x0C,
                         0x31, 0x36, 0x0F};
    lcd_send_data(data_e1, 15);

    lcd_send_cmd(0x11);
    vTaskDelay(pdMS_TO_TICKS(120));

    lcd_send_cmd(0x29);
    //ESP_LOGI(TAG, "LCD Initialized");
	
	
}

void lcd_set_addr_window(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end){
	
	/*
		This function set your playground on the LCD's memory for you to draw
		nonsense on.
	*/
	uint8_t window_data[4];
	
	lcd_send_cmd(0x2A); // Sending through Column Address Set Command
	window_data[0] = (x_start >> 8);
	window_data[1] = (x_start & 0xFF);
	window_data[2] = (x_end >> 8);
	window_data[3] = (x_end & 0xFF);
	lcd_send_data(window_data, 4);
	
	lcd_send_cmd(0x2B); // Sending through Page Address Set Command
	window_data[0] = (y_start >> 8);
	window_data[1] = (y_start & 0xFF);
	window_data[2] = (y_end >> 8);
	window_data[3] = (y_end & 0xFF);
	lcd_send_data(window_data, 4);
	
	lcd_send_cmd(0x2C); // Send to memory... affect colours after
	
} 

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t colour){
	
	lcd_set_addr_window(x, x, y, y);
	// lcd_send_cmd(0x2C); // Memory Write Command
	// Send Colour info... also in uint16_t
	// This data is for one individual pixel
	uint8_t colour_data[2] = {colour >> 8,colour & 0xFF};
	lcd_send_data(colour_data, 2);
	
}

void lcd_fill_screen(uint16_t colour){
	lcd_set_addr_window(0, LCD_LENGTH - 1, 0, LCD_HEIGHT - 1);
	//lcd_send_cmd(0x2C); // Memory Write Command
	// Send Colour info... also in uint16_t
	// This data needs to be looped for all pixels
	uint8_t line_buf[LCD_LENGTH * 2]; // 2 bytes per pixel... colour is 16 bits
	for (int i = 0; i < LCD_LENGTH; i++){
		line_buf[2 * i] = colour >> 8;
		line_buf[2 * i + 1] = colour & 0xFF;
	}
	// send row 
	for (int y = 0; y < LCD_HEIGHT; y++){
		lcd_send_data(line_buf, sizeof(line_buf));
	}
}

void lcd_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour){
	lcd_set_addr_window(x, x + width-1, y, y + height-1);
	
	uint8_t line_buf[width * 2]; // 2 bytes per pixel... colour is 16 bits
	for (int i = 0; i < width; i++){
		line_buf[2 * i] = colour >> 8;
		line_buf[2 * i + 1] = colour & 0xFF;
	}
	// send row 
	for (int j = 0; j < height; j++){
		lcd_send_data(line_buf, sizeof(line_buf));
	}
}

void lcd_draw_char(uint16_t x, uint16_t y, char c, uint16_t colour, uint16_t bg) {
    if (c < 0x20 || c > 0x7F) c = '?';
    const uint8_t *bitmap = font5x7[c - 0x20];

    for (int col = 0; col < 5; col++) {
        uint8_t line = bitmap[col];
        for (int row = 0; row < 7; row++) {
            if (line & 0x01)
                lcd_draw_pixel(x + col, y + row, colour);
            else
                lcd_draw_pixel(x + col, y + row, bg);
            line >>= 1;
        }
    }
}

void lcd_draw_text(uint16_t x, uint16_t y, const char *text, uint16_t colour, uint16_t bg) {
    while (*text) {
        lcd_draw_char(x, y, *text, colour, bg);
        x += 6; // 5px wide + 1px spacing
        text++;
    }
}

void lcd_sleep(void) {
    // Turn display off
    lcd_send_cmd(0x28); //turn display off
    vTaskDelay(pdMS_TO_TICKS(100)); // Small delay

    // Enter sleep mode
    lcd_send_cmd(0x10); //go to sleep
}

