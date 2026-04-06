/*
 * menu.h
 *
 *  Created on: 22 Feb 2026
 *      Author: dontk
 */

#ifndef MAIN_MENU_MENU_H_
#define MAIN_MENU_MENU_H_

#include <stdio.h>
#include <stdint.h>

typedef struct MenuItem MenuItem;

struct MenuItem {
	const char *name;		// Display name
	void (*action)(void);	// Function Pointer - Function to be executed
	
	MenuItem *parent;		// Pointer to parent node
	
	MenuItem **children;	// Pointer to multiple children nodes
	uint8_t num_children;	// Number of children
};

void menu_init(void);
void menu_next(void);
void menu_prev(void);
void menu_enter(void);
void menu_back(void);
void menu_render(void);

void button_event_task(void *args);

MenuItem* menu_get_current(void);
uint8_t menu_get_cursor_index(void);

#endif /* MAIN_MENU_MENU_H_ */
