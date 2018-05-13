#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <unistd.h>

#include "lcd.h"
#include "ruota.h"
//#include "ruota.c"

#define FOOD  1
#define HUG 2
#define WALK 3
#define WASH 4
#define HAPPY 0

#define LEFT 1
#define RIGHT 2
#define FRONT 0

/* drawing methods*/
void fish_screen();		// main screen
void draw_fish(uint8_t need, uint8_t side);
void draw_sad_fish(uint8_t side);
void draw_happy_fish(uint8_t side);
void draw_buttons();
void draw_timer();	// draws the timer in green or red
void message(char message[]);		// draws a message from the fish
void highlight_current_need();		// highlights the button for the current need
void outline_selected_button();			// outlines the currently selected button
void unoutline_button(uint8_t button); 	// removes outline of previously selected button

void random_need();


uint8_t current_need;
uint8_t current_side;
uint8_t timer;
uint8_t selected_button;

int main() {
	init_lcd();
	set_orientation(West);
	//intro_screen();
	fish_screen();
}

void fish_screen() {
	current_need = WASH;
	timer = 300000;
	current_side = 0;
	selected_button = FOOD;
	draw_buttons(current_need, selected_button);

	for(;;) {
		if(timer == 0) {
			//current_need = random_need();
		}
		draw_fish(current_need, current_side);
		draw_timer(timer, current_need);

		scan_switches(0);

		if (get_switch_press(_BV(SWE))) {
			unoutline_button(selected_button);
			selected_button++;
			selected_button = selected_button % 4;
			outline_selected_button;
			display_string_xy("test", 50, 50);
		}

		if (get_switch_press(_BV(SWW))) {
			unoutline_button(selected_button);
			selected_button--;
			if(selected_button == 0) { selected_button = 4; }
			outline_selected_button;
			display_string_xy("test", 100, 100);
		}

		sleep(1000); //java

		timer -= 1;		// count down the timer
		current_side += 1;
		if(current_side == 3) { current_side = 0; }		//loop the current side to the front of the fish
	}
}

void draw_timer() {
	if(current_need == HAPPY) {
		display_color(WHITE,GREEN);
	} else {
		display_color(WHITE,RED);
	}

	char timer_string[6];
	itoa(timer, timer_string, 10);		//converting int to string
	display_string_xy(timer_string, LCDHEIGHT/2 - 10, LCDWIDTH - 60);

}
void draw_fish(uint8_t need, uint8_t side) {}
void draw_sad_fish(uint8_t side) {}
void draw_happy_fish(uint8_t side) {}

void draw_buttons() {

	display_color(YELLOW, DEEP_PINK);

	rectangle rect;
	rect.top = LCDWIDTH - 50;
	rect.bottom = LCDWIDTH - 10;
	rect.left = LCDHEIGHT/2 - 115;
	rect.right = LCDHEIGHT/2 - 65;
	fill_rectangle(rect, DEEP_PINK);
	display_string_xy("FEED", (rect.left + 15), (rect.top + 10));
	display_string_xy("ME!!", (rect.left + 15), (rect.top + 23));

	rect.left = LCDHEIGHT/2 - 55;
	rect.right = LCDHEIGHT/2 - 5;
	fill_rectangle(rect, DEEP_PINK);
	display_string_xy("HUG", (rect.left + 15), (rect.top + 10));
	display_string_xy("ME!!", (rect.left + 15), (rect.top + 23));

	rect.left = LCDHEIGHT/2 + 5;
	rect.right = LCDHEIGHT/2 + 55;
	fill_rectangle(rect, DEEP_PINK);
	display_string_xy("WALK", (rect.left + 15), (rect.top + 10));
	display_string_xy("ME!!", (rect.left + 15), (rect.top + 23));

	rect.left = LCDHEIGHT/2 + 65;
	rect.right = LCDHEIGHT/2 + 115;
	fill_rectangle(rect, DEEP_PINK);
	display_string_xy("WASH", (rect.left + 15), (rect.top + 10));
	display_string_xy("ME!!", (rect.left + 15), (rect.top + 23));

	highlight_current_need();
	outline_selected_button();
}

void highlight_current_need() {

	display_color(YELLOW, RED);

	rectangle rect;
	rect.top = LCDWIDTH - 50;
	rect.bottom = LCDWIDTH - 10;

	if(current_need == FOOD) {
		rect.left = LCDHEIGHT/2 - 115;
		rect.right = LCDHEIGHT/2 - 65;
		fill_rectangle(rect, RED);
		display_string_xy("FEED", (rect.left + 15), (rect.top + 10));
		display_string_xy("ME!!", (rect.left + 15), (rect.top + 23));
	} else if(current_need == HUG) {
		rect.left = LCDHEIGHT/2 - 55;
		rect.right = LCDHEIGHT/2 - 5;
		fill_rectangle(rect, RED);
		display_string_xy("HUG", (rect.left + 15), (rect.top + 10));
		display_string_xy("ME!!", (rect.left + 15), (rect.top + 23));
	} else if(current_need == WALK) {
		rect.left = LCDHEIGHT/2 + 5;
		rect.right = LCDHEIGHT/2 + 55;
		fill_rectangle(rect, RED);
		display_string_xy("WALK", (rect.left + 15), (rect.top + 10));
		display_string_xy("ME!!", (rect.left + 15), (rect.top + 23));
	} else if(current_need == WASH) {
		rect.left = LCDHEIGHT/2 + 65;
		rect.right = LCDHEIGHT/2 + 115;
		fill_rectangle(rect, RED);
		display_string_xy("WASH", (rect.left + 15), (rect.top + 10));
		display_string_xy("ME!!", (rect.left + 15), (rect.top + 23));
	}
}

void outline_selected_button() {
	uint8_t left;
	uint8_t right;

	if(selected_button == FOOD) {
		left = LCDHEIGHT/2 - 118;
		right = LCDHEIGHT/2 - 62;
	} else if(selected_button == HUG) {
		left = LCDHEIGHT/2 - 58;
		right = LCDHEIGHT/2 - 2;
	} else if(selected_button == WALK) {
		left = LCDHEIGHT/2 + 2;
		right = LCDHEIGHT/2 + 58;
	} else if(selected_button == WASH) {
		left = LCDHEIGHT/2 - 62;
		right = LCDHEIGHT/2 + 118;
	}

	rectangle rect;
	// top line
	rect.top = LCDWIDTH - 53;
	rect.bottom = LCDWIDTH - 52;
	rect.left = left;
	rect.right = right;
	fill_rectangle(rect, YELLOW);

	// left line
	rect.bottom = LCDWIDTH - 7;
	rect.right = left + 1;
	fill_rectangle(rect, YELLOW);

	// bottom line
	rect.top = LCDWIDTH - 8;
	rect.right = right;
	fill_rectangle(rect, YELLOW);

	// right line
	rect.top = LCDWIDTH - 53;
	rect.left = right - 1;
	fill_rectangle(rect, YELLOW);
}

void unoutline_button(uint8_t button) {}

void message(char message[]) {}

void need(uint8_t need) {
	if(need == FOOD) {
		current_need = FOOD;
		message("I'M STARVING!! I'M GONNA FAINT ;(");
	} else if (need == HUG) {
		current_need = HUG;
		message("i'm sooo lonely and sad... love me!!");
	} else if (need == WALK) {
		current_need = WALK;
		message("i'm getting fat and bored in this aquarium");
	} else if (need == WASH) {
		current_need = WASH;
		message("aaa i'm so sweaty and gross!!");
	}
}

void action(uint8_t action) {
	current_need = HAPPY;

	if (action == FOOD) {
		message("i feel fuller than a pufferfish");
		timer += 3;
	} else if (action == HUG) {
		message("i love cuddles more than bubbles!");
		timer += 3;
	} else if (action == WALK) {
		message("woof!");
		timer += 2;
	} else if (action == WASH) {
		message("shine bright like a diamond!");
		timer += 2;
	}
}
