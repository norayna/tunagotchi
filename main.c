#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>

#include "lcd.c"
#include "ruota.c"

#define FOOD  1
#define HUG 2
#define WALK 3
#define WASH 4
#define HAPPY 0

#define LEFT 1
#define RIGHT 2
#define FRONT 0

// drawing methods
void fish_screen();		// main screen
void draw_fish(uint8_t side);
void draw_sad_fish(uint8_t side);
void draw_happy_fish(uint8_t side);
void draw_buttons();
void draw_timer();	// draws the timer in green or red
void message(char message[]);		// draws a message from the fish
void highlight_current_need();		// highlights the button for the current need
void outline_selected_button();			// outlines the currently selected button
void unoutline_button(uint8_t button); 	// removes outline of previously selected button

// other methods
void random_need();
void need(uint8_t need);
void action(uint8_t need);


uint8_t current_need;
uint8_t current_side;
uint8_t timer;
uint8_t selected_button;

int main() {
	init_lcd();
	set_orientation(West);
	os_init_ruota();
	//intro_screen();
	fish_screen();
}

void fish_screen() {
	need(WASH);
	timer = 300000;
	current_side = 0;
	selected_button = FOOD;
	draw_buttons();

	//TODO remove
	draw_happy_fish(0);

	// copied from https://exploreembedded.com/wiki/AVR_Timer_programming
	TCNT0=0x00;
	TCCR0 = (1<<CS00) | (1<<CS02);


	for(;;) {
		if(timer == 0) {
			//current_need = random_need();
			current_need++;
			if (current_need == 5) { current_need = 1;}
		}
		//draw_timer(timer, current_need);
		//sei();
		scan_switches(0);

		if (get_switch_press(_BV(SWE))) {
			outline_selected_button(BLACK);
			selected_button++;
			if(selected_button == 5) { selected_button = 1; }
			outline_selected_button(YELLOW);
		}

		if (get_switch_press(_BV(SWW))) {
			outline_selected_button(BLACK);
			selected_button--;
			if(selected_button == 0) { selected_button = 4; }
			outline_selected_button(YELLOW);
		}

		if(get_switch_press(_BV(SWC))) {
			action(selected_button);
		}


		//sleep(1000); //java

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

void draw_fish(uint8_t side) {
	if(current_need == HAPPY) {
		draw_happy_fish(side);
	} else {
		draw_sad_fish(side);
	}
}

void draw_sad_fish(uint8_t side) {}


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
	outline_selected_button(YELLOW);
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

void outline_selected_button(uint16_t color) {
	uint16_t left;
	uint16_t right;

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
		left = LCDHEIGHT/2 + 62;
		right = LCDHEIGHT/2 + 118;
	}

	rectangle rect;
	// top line
	rect.top = LCDWIDTH - 53;
	rect.bottom = LCDWIDTH - 52;
	rect.left = left;
	rect.right = right;
	fill_rectangle(rect, color);

	// left line
	rect.bottom = LCDWIDTH - 7;
	rect.right = left + 1;
	fill_rectangle(rect, color);

	// bottom line
	rect.top = LCDWIDTH - 8;
	rect.right = right;
	fill_rectangle(rect, color);

	// right line
	rect.top = LCDWIDTH - 53;
	rect.left = right - 1;
	fill_rectangle(rect, color);
}

void unoutline_button(uint8_t button) {
	outline_selected_button(BLACK);
}

void message(char message[]) {
	display_color(BLACK, WHITE);

	rectangle rect;
	rect.top = 15;
	rect.bottom = 40;
	rect.left = 200;
	rect.right = LCDHEIGHT;
	fill_rectangle(rect, WHITE);
	//display_string_xy("            ", 200, 15);
	display_string_xy(message, 210, 23);
	//display_string_xy("            ", 200, 31);

}

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
		message("my fins is stanky");
	}
}

void action(uint8_t action) {
	if(action == current_need) {
		current_need = HAPPY;

		if (action == FOOD) {
			message("mmm chicken!");
			timer += 3;
		} else if (action == HUG) {
			message("*cuddles* *bubbles*");
			timer += 3;
		} else if (action == WALK) {
			message("woof!");
			timer += 2;
		} else if (action == WASH) {
			message("i feel fresh!");
			timer += 2;
		}
	} else if(current_need == HAPPY) {
			message("uhmm.. no thanks");
	} else {
			message("i'm dying here!!");
	}
	draw_buttons();
}

void draw_happy_fish(uint8_t side) {
	uint8_t sq = 6;
	uint8_t top = 15;
	uint8_t left = 50;
	//uint8_t rleft = 51;

	rectangle rect;
	rect.top = top;
	rect.left = left;

	//line 1

	//line 2
	rect.top = top + 1*sq + 1;
	rect.bottom = top + 2*sq;
	rect.left = left + 7*sq + 1;
	rect.right = left + 9*sq;
	fill_rectangle(rect, YELLOW);

	//line 3
	rect.top = top + 2*sq + 1;
	rect.bottom = top + 3*sq;
	rect.left = left + 9*sq + 1;
	rect.right = left + 11*sq;
	fill_rectangle(rect, YELLOW);

	//line 4
	rect.top = top + 3*sq + 1;
	rect.bottom = top + 4*sq;
	rect.left = left + 10*sq + 1;
	rect.right = left + 12*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 17*sq + 1;
	rect.right = left + 18*sq;
	fill_rectangle(rect, YELLOW);

	//line 5
	rect.top = top + 4*sq + 1;
	rect.bottom = top + 5*sq;
	rect.left = left + 11*sq + 1;
	rect.right = left + 13*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 17*sq + 1;
	rect.right = left + 20*sq;
	fill_rectangle(rect, YELLOW);

	//line 6
	rect.top = top + 5*sq + 1;
	rect.bottom = top + 6*sq;
	rect.left = left + 12*sq + 1;
	rect.right = left + 14*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 14*sq + 1;
	rect.right = left + 21*sq;
	fill_rectangle(rect, DARK_BLUE);

	//line 7
	rect.top = top + 6*sq + 1;
	rect.bottom = top + 7*sq;
	rect.left = left + 0*sq + 1;
	rect.right = left + 2*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 12*sq + 1;
	rect.right = left + 13*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 13*sq + 1;
	rect.right = left + 23*sq;
	fill_rectangle(rect, DARK_BLUE);

	//line 8
	rect.top = top + 7*sq + 1;
	rect.bottom = top + 8*sq;
	rect.left = left + 1*sq + 1;
	rect.right = left + 3*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 11*sq + 1;
	rect.right = left + 24*sq;
	fill_rectangle(rect, DARK_BLUE);

	//line 9
	rect.top = top + 8*sq + 1;
	rect.bottom = top + 9*sq;
	rect.left = left + 1*sq + 1;
	rect.right = left + 4*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 10*sq + 1;
	rect.right = left + 13*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 13*sq + 1;
	rect.right = left + 22*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 22*sq + 1;
	rect.right = left + 26*sq;
	fill_rectangle(rect, DARK_BLUE);

	//line 10
	rect.top = top + 9*sq + 1;
	rect.bottom = top + 10*sq;
	rect.left = left + 2*sq + 1;
	rect.right = left + 5*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 9*sq + 1;
	rect.right = left + 11*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 11*sq + 1;
	rect.right = left + 25*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 25*sq + 1;
	rect.right = left + 27*sq;
	fill_rectangle(rect, YELLOW);

	//line 11
	rect.top = top + 10*sq + 1;
	rect.bottom = top + 11*sq;
	rect.left = left + 3*sq + 1;
	rect.right = left + 6*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 8*sq + 1;
	rect.right = left + 10*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 10*sq + 1;
	rect.right = left + 23*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 23*sq + 1;
	rect.right = left + 25*sq;
	fill_rectangle(rect, OLIVE);
	rect.left = left + 25*sq + 1;
	rect.right = left + 26*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 26*sq + 1;
	rect.right = left + 27*sq;
	fill_rectangle(rect, YELLOW);

	//line 12
	rect.top = top + 11*sq + 1;
	rect.bottom = top + 12*sq;
	rect.left = left + 3*sq + 1;
	rect.right = left + 6*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 7*sq + 1;
	rect.right = left + 8*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 8*sq + 1;
	rect.right = left + 11*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 11*sq + 1;
	rect.right = left + 22*sq;
	fill_rectangle(rect, LIGHT_BLUE);
	rect.left = left + 22*sq + 1;
	rect.right = left + 23*sq;
	fill_rectangle(rect, OLIVE);
	rect.left = left + 23*sq + 1;
	rect.right = left + 25*sq;
	fill_rectangle(rect, BLACK);
	rect.left = left + 25*sq + 1;
	rect.right = left + 26*sq;
	fill_rectangle(rect, OLIVE);
	rect.left = left + 26*sq + 1;
	rect.right = left + 27*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 27*sq + 1;
	rect.right = left + 28*sq;
	fill_rectangle(rect, DARK_BLUE);

	//line 13
	rect.top = top + 12*sq + 1;
	rect.bottom = top + 13*sq;
	rect.left = left + 4*sq + 1;
	rect.right = left + 7*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 7*sq + 1;
	rect.right = left + 9*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 9*sq + 1;
	rect.right = left + 22*sq;
	fill_rectangle(rect, LIGHT_BLUE);
	rect.left = left + 22*sq + 1;
	rect.right = left + 23*sq;
	fill_rectangle(rect, OLIVE);
	rect.left = left + 23*sq + 1;
	rect.right = left + 25*sq;
	fill_rectangle(rect, BLACK);
	rect.left = left + 25*sq + 1;
	rect.right = left + 26*sq;
	fill_rectangle(rect, OLIVE);
	rect.left = left + 26*sq + 1;
	rect.right = left + 28*sq;
	fill_rectangle(rect, LIGHT_BLUE);

	//line 14
	rect.top = top + 13*sq + 1;
	rect.bottom = top + 14*sq;
	rect.left = left + 4*sq + 1;
	rect.right = left + 7*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 7*sq + 1;
	rect.right = left + 28*sq;
	fill_rectangle(rect, LIGHT_BLUE);

	//line 15
	rect.top = top + 14*sq + 1;
	rect.bottom = top + 15*sq;
	rect.left = left + 3*sq + 1;
	rect.right = left + 6*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 7*sq + 1;
	rect.right = left + 16*sq;
	fill_rectangle(rect, LIGHT_BLUE);
	rect.left = left + 16*sq + 1;
	rect.right = left + 20*sq;
	fill_rectangle(rect, GREY);
	rect.left = left + 20*sq + 1;
	rect.right = left + 25*sq;
	fill_rectangle(rect, LIGHT_BLUE);
	rect.left = left + 25*sq + 1;
	rect.right = left + 26*sq;
	fill_rectangle(rect, RED);
	rect.left = left + 26*sq + 1;
	rect.right = left + 28*sq;
	fill_rectangle(rect, LIGHT_BLUE);

	//line 16
	rect.top = top + 15*sq + 1;
	rect.bottom = top + 16*sq;
	rect.left = left + 3*sq + 1;
	rect.right = left + 6*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 8*sq + 1;
	rect.right = left + 12*sq;
	fill_rectangle(rect, LIGHT_BLUE);
	rect.left = left + 12*sq + 1;
	rect.right = left + 19*sq;
	fill_rectangle(rect, GREY);
	rect.left = left + 19*sq + 1;
	rect.right = left + 26*sq;
	fill_rectangle(rect, LIGHT_BLUE);
	rect.left = left + 26*sq + 1;
	rect.right = left + 28*sq;
	fill_rectangle(rect, RED);

	//line 17
	rect.top = top + 16*sq + 1;
	rect.bottom = top + 17*sq;
	rect.left = left + 2*sq + 1;
	rect.right = left + 5*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 9*sq + 1;
	rect.right = left + 27*sq;
	fill_rectangle(rect, LIGHT_BLUE);

	//line 18
	rect.top = top + 17*sq + 1;
	rect.bottom = top + 18*sq;
	rect.left = left + 1*sq + 1;
	rect.right = left + 4*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 10*sq + 1;
	rect.right = left + 25*sq;
	fill_rectangle(rect, LIGHT_BLUE);

	//line 19
	rect.top = top + 18*sq + 1;
	rect.bottom = top + 19*sq;
	rect.left = left + 1*sq + 1;
	rect.right = left + 3*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 10*sq + 1;
	rect.right = left + 11*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 11*sq + 1;
	rect.right = left + 24*sq;
	fill_rectangle(rect, LIGHT_BLUE);

	//line 20
	rect.top = top + 19*sq + 1;
	rect.bottom = top + 20*sq;
	rect.left = left + 0*sq + 1;
	rect.right = left + 2*sq;
	fill_rectangle(rect, DARK_BLUE);
	rect.left = left + 10*sq + 1;
	rect.right = left + 13*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 13*sq + 1;
	rect.right = left + 23*sq;
	fill_rectangle(rect, LIGHT_BLUE);

	//line 21
	rect.top = top + 20*sq + 1;
	rect.bottom = top + 21*sq;
	rect.left = left + 9*sq + 1;
	rect.right = left + 12*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 15*sq + 1;
	rect.right = left + 21*sq;
	fill_rectangle(rect, LIGHT_BLUE);
	rect.left = left + 15*sq + 1;
	rect.right = left + 16*sq;
	fill_rectangle(rect, YELLOW);

	//line 22
	rect.top = top + 21*sq + 1;
	rect.bottom = top + 22*sq;
	rect.left = left + 9*sq + 1;
	rect.right = left + 11*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 20*sq + 1;
	rect.right = left + 22*sq;
	fill_rectangle(rect, YELLOW);

	//line 23
	rect.top = top + 22*sq + 1;
	rect.bottom = top + 23*sq;
	rect.left = left + 7*sq + 1;
	rect.right = left + 10*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 20*sq + 1;
	rect.right = left + 21*sq;
	fill_rectangle(rect, YELLOW);

	//line 24
	rect.top = top + 23*sq + 1;
	rect.bottom = top + 24*sq;
	rect.left = left + 6*sq + 1;
	rect.right = left + 8*sq;
	fill_rectangle(rect, YELLOW);
	rect.left = left + 19*sq + 1;
	rect.right = left + 20*sq;
	fill_rectangle(rect, YELLOW);

}
