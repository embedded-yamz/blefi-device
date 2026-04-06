#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "components/buttons/buttons.h"
#include "components/lcd/lcd.h"
#include "components/menu/menu.h"
#include "components/wifi/wifi_sniffer.h"
#include "components/ui/ui.h"

void app_main(void)
{
	//initialise
	spi_bus_init();
	ili9341_init();
	menu_init();
	buttons_init();
	//wifi_sniffer_init();
	
    xTaskCreate(ui_task, "ui_task", 4096, NULL, 5, &ui_task_handle);
    xTaskCreate(button_event_task, "button_event_task", 4096, NULL, 4, NULL);
    //xTaskCreate(wifi_sniffer_task, "wifi_sniffer_task", 4096, NULL, 4, NULL);
    
    ui_set_mode(UI_MODE_MENU);
}
