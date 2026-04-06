/*
 * menu.c
 *
 *  Created on: 22 Feb 2026
 *      Author: dontk
 */

#include "menu.h"
#include "../buttons/buttons.h"
#include "../ui/ui.h"
#include "../wifi/wifi_sniffer.h"
#include "freertos/portmacro.h"

// Finding where I am in the menu
// static MenuItem *current_menu = NULL;
// static uint8_t cursor_index = 0;

typedef struct {
	MenuItem *current_menu;
	uint8_t cursor_index;
} MenuState;

static MenuState menu_state;

static MenuItem main_menu;
static MenuItem wifi_item;
static MenuItem settings_menu;
static MenuItem brightness_item;
static MenuItem volume_item;
static MenuItem about_item;
static MenuItem wifi_sniffer;
static MenuItem stop_wifi_sniffer;

static MenuItem *main_children[] = {
    &wifi_item,
    &settings_menu,
    &about_item
};

static MenuItem *settings_children[] = {
    &brightness_item,
    &volume_item
};

static MenuItem *wifi_children[] = {
	&wifi_sniffer,
	&stop_wifi_sniffer,
};

static MenuItem main_menu = {
    .name = "Main",
    .action = NULL,
    .parent = NULL,
    .children = main_children,
    .num_children = 3
};

static MenuItem wifi_item = {
    .name = "Wifi",
    .action = NULL,   // some function
    .parent = &main_menu,
    .children = wifi_children,
    .num_children = 2
};

static MenuItem wifi_sniffer = {
	.name = "Wifi Sniffer",
	.children = NULL,
	.num_children = 0,
	.parent = &wifi_item,
	.action = wifi_sniffer_init,
	// link action to task first??
};

static MenuItem stop_wifi_sniffer = {
	.name = "Stop Wifi Sniffer",
	.children = NULL,
	.num_children = 0,
	.parent = &wifi_item,
	.action = wifi_sniffer_deinit,
	// link action to task first??
};

static MenuItem settings_menu = {
    .name = "Settings", 
    .action = NULL,
    .parent = &main_menu,
    .children = settings_children,
    .num_children = 2
};

static MenuItem brightness_item = {
    .name = "Brightness",
    .action = NULL,
    .parent = &settings_menu,
    .children = NULL,
    .num_children = 0
};

static MenuItem volume_item = {
    .name = "Volume",
    .action = NULL,
    .parent = &settings_menu,
    .children = NULL,
    .num_children = 0
};

static MenuItem about_item = {
    .name = "About",
    .action = NULL,
    .parent = &main_menu,
    .children = NULL,
    .num_children = 0
};

void button_event_task(void *args){
	
	static TickType_t last_button_tick = 0;
	const TickType_t debounce_delay = pdMS_TO_TICKS(100); // 50 ms
	
	button_event_t event;
	
	while(1){
		
		if(xQueueReceive(buttons_get_queue(), &event, portMAX_DELAY)){
			
			TickType_t now = xTaskGetTickCount();
    		if (now - last_button_tick < debounce_delay)
        		continue; // ignore bounces
    		last_button_tick = now;
			
			switch (event) {
				case BUTTON_UP_PRESSED:
					menu_prev();
					break;
				case BUTTON_DOWN_PRESSED:
					menu_next();
					break;
				case BUTTON_LEFT_PRESSED:
					menu_back();
					break;
				case BUTTON_RIGHT_PRESSED:
					menu_enter();
					break;
				
				default:
					break;
			}
			//render something or send something for the screen to update...
			ui_set_mode(UI_MODE_MENU);
		}
	}
	
}

void menu_init(void)
{
    menu_state.current_menu = &main_menu;
    menu_state.cursor_index = 0;
}

void menu_next(void)
{
    if (menu_state.current_menu->num_children == 0)
        return;

    menu_state.cursor_index++;

    if (menu_state.cursor_index >= menu_state.current_menu->num_children)
        menu_state.cursor_index = 0; // wrap around
}

void menu_prev(void)
{
    if (menu_state.current_menu->num_children == 0)
        return;

    if (menu_state.cursor_index == 0)
        menu_state.cursor_index = menu_state.current_menu->num_children - 1;
    else
        menu_state.cursor_index--;
}

void menu_enter(void)
{
    if (menu_state.current_menu->num_children == 0)
        return;

    MenuItem *selected = menu_state.current_menu->children[menu_state.cursor_index];

    // If it has children, go into submenu
    if (selected->num_children > 0)
    {
        menu_state.current_menu = selected;
        menu_state.cursor_index = 0;
    }
    // If it has action, execute it
    else if (selected->action != NULL)
    {
        selected->action();
    }
}

void menu_back(void)
{
    if (menu_state.current_menu->parent != NULL)
    {
        menu_state.current_menu = menu_state.current_menu->parent;
        menu_state.cursor_index = 0;
    }
}

MenuItem* menu_get_current(void)
{
    return menu_state.current_menu;
}

uint8_t menu_get_cursor_index(void)
{
    return menu_state.cursor_index;
}

/*	Testing purposes 
void menu_render(void){
	
    MenuItem *menu = menu_state.current_menu;

    lcd_fill_screen(COLOUR_BLACK);

    for (uint8_t i = 0; i < menu->num_children; i++)
    {
        uint16_t colour = (i == menu_state.cursor_index) ? COLOUR_RED : COLOUR_WHITE;

        lcd_draw_text(10, 20 + (i * 20), menu->children[i]->name, colour, COLOUR_BLACK);
    }
}
*/
