#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "lcd.h"
#include "ruota.h"


#define FOOD  1
#define HUG 2
#define WALK 3
#define WASH 4
#define HAPPY 0

#define LEFT 1
#define RIGHT 2
#define FRONT 0

/* drawing methods*/
void fish_screen();
void draw_fish(uint8_t need, uint8_t side);
void draw_sad_fish(uint8_t side);
void draw_happy_fish(uint8_t side);
void draw_buttons(uint8_t current_need, uint8_t selected);
void draw_timer(uint8_t timer);
void message(char[] message);
void highlight_button(uint8_t need);
void outline_button(uint8_t need);

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
	current_need = FOOD;
	timer = 30;
	current_side = 0;
	selected_button = FOOD;

	for(;;) {
		if (timer = 0) {
			current_need = random_need();
		}
		draw_fish(current_need, current_side);
		draw_buttons(current_need, selected_button);
		draw_timer(timer);

		//thread.sleep(1000); //java

		timer -= 1;		// count down the timer
		current_side += 1;
		if (current_side = 3) { current_side = 0; }		//loop the current side to the front of the fish
	}
}


void draw_sad_fish() {}

void draw_buttons(uint8_t current_need, uint8_t selected) {

	display_color(YELLOW, PINK);

	rectangle rect;
	rect.top = LCDHEIGHT - 30;
	rect.bottom = LCDHEIGHT - 10;
	rect.left = LCDWIDTH/2 - 75;
	rect.right = LCDWIDTH/2 - 45;
	fill_rectangle(rect, PINK);
	display_string_xy("FEED ME !", (rect.left + 3), (rect.top + 3));

	rect.left = LCDWIDTH/2 - 35;
	rect.right = LCDWIDTH/2 - 5;
	fill_rectangle(rect, PINK);
	display_string_xy("HUG ME !", (rect.left + 3), (rect.top + 3));

	rect.left = LCDWIDTH/2 + 5;
	rect.right = LCDWIDTH/2 - 35;
	fill_rectangle(rect, PINK);
	display_string_xy("WALK ME !", (rect.left + 3), (rect.top + 3));

	rect.left = LCDWIDTH/2 - 45;
	rect.right = LCDWIDTH/2 - 75;
	fill_rectangle(rect, PINK);
	display_string_xy("WASH ME !", (rect.left + 3), (rect.top + 3));

	highlight_button(current_need);
	outline_button(selected_button);
}

void message(char[] message) {}

void need(uint8_t need) {
	if(need = FOOD) {
		current_need = FOOD;
		message("I'M STARVING!! I'M GONNA FAINT ;(");
	} else if (need = HUG) {
		current_need = HUG;
		message("i'm sooo lonely and sad... love me!!");
	} else if (need = WALK) {
		current_need = WALK;
		message("i'm getting fat and bored in this aquarium");
	} else if (need = WASH) {
		current_need = WASH;
		message("aaa i'm so sweaty and gross!!");
	}
}

void action(char[] action) {
	current_need = HAPPY;

	if (action = FOOD) {
		message("i feel fuller than a pufferfish");
		timer += 3;
	} else if (action = HUG) {
		message("i love cuddles more than bubbles!");
		timer += 3;
	} else if (action = WALK) {
		message("woof!");
		timer += 2;
	} else if (action = WASH) {
		message("shine bright like a diamond!");
		timer += 2;
	}
}
