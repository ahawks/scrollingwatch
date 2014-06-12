#include <pebble.h>
#include <math.h>
#include "autoconfig.h"

Window *my_window;
TextLayer *text_now;
TextLayer *text_a;
TextLayer *text_b;
TextLayer *text_c;

TextLayer *text_now30;
TextLayer *text_a30;
TextLayer *text_b30;
Layer *container_layer;
InverterLayer *needle_line;
InverterLayer *invert_screen_layer;

char hour_buffer[] = "00";
char minutes_buffer[] = "00";
char hour_a_buffer[] = "00";
char hour_b_buffer[] = "00";
char hour_c_buffer[] = "00";

char buffer[] = "00";
int hours12[] = {12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
int hours24[] = {00, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
int tick_widths[] = {
					30, 	// hour
					5, 		// 5
					5, 		// 10
					10, 	// 15
					5, 		// 20
					5, 		// 25
					20, 	// 30
					5, 		// 35
					5, 		// 40
					10, 	// 45
					5, 		// 50
					5, 		// 55
				};

int tick_heights[] = {
					3, 	// hour
					1, 	// 5
					1, 	// 10
					3, 	// 15
					1, 	// 20
					1, 	// 25
					3, 	// 30
					1, 	// 35
					1, 	// 40
					3, 	// 45
					1, 	// 50
					1, 	// 55
				};

const int CONTAINER_HEIGHT = 250;
const int SPACER = 130;
int font_size = 49;
int base_y;

// SETTINGS
static void in_received_handler(DictionaryIterator *iter, void *context) {
        // Let Pebble Autoconfig handle received settings
        autoconfig_in_received_handler(iter, context);

        // Here the updated settings are available
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Configuration updated. Background: %s", 
                getBackground() ? "true (black)" : "false (white)"); 
        layer_mark_dirty(container_layer);
}

void draw_tick_line(GContext *ctx, bool left_side, int minute) {
	int center_y = CONTAINER_HEIGHT / 2;
	float tmp = (float)minute / 60.0;
	int distance = floor(SPACER * tmp);

	int width = tick_widths[abs(minute % 60)/5];
	int height = tick_heights[abs(minute % 60)/5];
	int x = left_side ? 0 : 144 - width;
	int y = center_y - 1 - distance; 
	
	// draw the tick
	graphics_fill_rect(ctx, GRect(x, y, width, height),
				  	0, GCornerNone);

}

// rendering function for container layer. 
// Draws all of the lines and rectangles. 
void container_update_proc(struct Layer *layer, GContext *ctx) {
	
	graphics_context_set_stroke_color(ctx, GColorBlack);
	for (int minute = -90; minute < 60; minute += 5) {
		// todo: preferences for left style
		draw_tick_line(ctx, true, minute);

		// todo: preferences for right style
		draw_tick_line(ctx, false, minute);
	}

	//invert screen	
	if (getBackground()) {
		layer_add_child(window_get_root_layer(my_window), (Layer*) invert_screen_layer);
	} else {
		layer_remove_from_parent((Layer*) invert_screen_layer);
	}
}

// tick response function. Updates the position of the 'container_layer' and numeric values. 
void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	int i = tick_time->tm_hour; // add an extra <cycle> so the -1 value works. We modulo it anyway. 

	int minutes = tick_time->tm_min;
	float hour_percent = (float)minutes/60.0;
	snprintf(minutes_buffer, sizeof("00"), "%d", minutes);
	if (clock_is_24h_style()) {
		i += 24;
		snprintf(hour_buffer, sizeof("00"), "%d", hours24[i%24]);
		snprintf(hour_a_buffer, sizeof("00"), "%d", hours24[(i-1)%24]);
		snprintf(hour_b_buffer, sizeof("00"), "%d", hours24[(i+1)%24]);
		snprintf(hour_c_buffer, sizeof("00"), "%d", hours24[(i+2)%24]);
	} else {
		i += 12;
		snprintf(hour_buffer, sizeof("00"), "%d", hours12[i%12]);
		snprintf(hour_a_buffer, sizeof("00"), "%d", hours12[(i-1)%12]);
		snprintf(hour_b_buffer, sizeof("00"), "%d", hours12[(i+1)%12]);
		snprintf(hour_c_buffer, sizeof("00"), "%d", hours12[(i+2)%12]);	
	}
	
	text_layer_set_text(text_now, hour_buffer);
	text_layer_set_text(text_a, hour_a_buffer);
	text_layer_set_text(text_b, hour_b_buffer);
	text_layer_set_text(text_c, hour_c_buffer);
	

	int y_offset =(int) -1 * (hour_percent * SPACER);
	layer_set_bounds(container_layer, GRect(0, y_offset, 144, 168));
}

/** 
Initializes the watch face. Creates all layers
**/
void handle_init(void) {
    // Initialize Pebble Autoconfig to register App Message handlers and restores settings
    autoconfig_init();
    // Here the restored or defaulted settings are available
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Configuration restored. Background: %s", 
            getBackground() ? "true" : "false");

    // Register our custom receive handler which in turn will call Pebble Autoconfigs receive handler
    app_message_register_inbox_received(in_received_handler);

	my_window = window_create();

	// set up the text labels
	int center_y = (CONTAINER_HEIGHT/2) - (font_size/1.5);
	int start_x = 30;
	int end_x = 144-30-30;
	container_layer = layer_create(GRect(0, (168/2 - CONTAINER_HEIGHT/2), 144, CONTAINER_HEIGHT));
	layer_set_update_proc(container_layer, container_update_proc);
	layer_set_clips(container_layer, true);

	text_a = text_layer_create(GRect(start_x, (center_y - SPACER), end_x, 50));
	text_layer_set_text_color(text_a, GColorBlack);
	text_layer_set_text_alignment(text_a, GTextAlignmentCenter);
	text_layer_set_font(text_a, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	layer_add_child(container_layer, (Layer*) text_a);	

	text_a30 = text_layer_create(GRect(start_x, (center_y - (SPACER*0.5)), end_x, 30));
	text_layer_set_text_color(text_a30, GColorBlack);
	text_layer_set_text_alignment(text_a30, GTextAlignmentCenter);
	text_layer_set_font(text_a30, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text(text_a30, "30");
	layer_add_child(container_layer, (Layer*) text_a30);	
	
	text_now = text_layer_create(GRect(start_x, center_y, end_x, 50));
	text_layer_set_text_color(text_now, GColorBlack);
	text_layer_set_background_color(text_now, GColorWhite);
	text_layer_set_text_alignment(text_now, GTextAlignmentCenter);
	text_layer_set_font(text_now, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(container_layer, (Layer*) text_now);

	text_now30 = text_layer_create(GRect(start_x, (center_y + (SPACER*0.6)), end_x, 30));
	text_layer_set_text_color(text_now30, GColorBlack);
	text_layer_set_text_alignment(text_now30, GTextAlignmentCenter);
	text_layer_set_font(text_now30, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text(text_now30, "30");
	layer_add_child(container_layer, (Layer*) text_now30);	
	
	text_b = text_layer_create(GRect(start_x, center_y + SPACER+10, end_x, SPACER));
	text_layer_set_background_color(text_b, GColorClear);
	text_layer_set_text_color(text_b, GColorBlack);
	text_layer_set_text_alignment(text_b, GTextAlignmentCenter);
	text_layer_set_font(text_b, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	layer_add_child(container_layer, (Layer*) text_b);

	text_b30 = text_layer_create(GRect(start_x, (center_y + (SPACER*1.6)), end_x, 30));
	text_layer_set_text_color(text_b30, GColorBlack);
	text_layer_set_text_alignment(text_b30, GTextAlignmentCenter);
	text_layer_set_font(text_b30, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text(text_b30, "30");
	layer_add_child(container_layer, (Layer*) text_b30);	
	
	text_c = text_layer_create(GRect(start_x, center_y + (SPACER*2), end_x, SPACER));
	text_layer_set_background_color(text_c, GColorClear);
	text_layer_set_text_color(text_c, GColorBlack);
	text_layer_set_text_alignment(text_c, GTextAlignmentCenter);
	text_layer_set_font(text_c, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	layer_add_child(container_layer, (Layer*) text_c);
	
	// set up inverter layers (last)
	needle_line = inverter_layer_create(GRect(0, (168/2), 144, 1));
	window_set_fullscreen(my_window, true);

	layer_add_child(window_get_root_layer(my_window), container_layer);
	// set up tick handler
	tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
	
	// push window to stack 
	layer_add_child(window_get_root_layer(my_window), (Layer*) needle_line);
	
	// create the invert layer, in case it's used
	invert_screen_layer = inverter_layer_create(GRect(0, 0, 144, 168));

	window_stack_push(my_window, true);
}

void handle_deinit(void) {
	text_layer_destroy(text_now);
	text_layer_destroy(text_a);
	text_layer_destroy(text_b);
	text_layer_destroy(text_c);
	text_layer_destroy(text_now30);
	text_layer_destroy(text_a30);
	text_layer_destroy(text_b30);

	tick_timer_service_unsubscribe();
	layer_destroy(container_layer);
	inverter_layer_destroy(needle_line);
	inverter_layer_destroy(invert_screen_layer);
	window_destroy(my_window);

	// Let Pebble Autoconfig write settings to Pebbles persistant memory
    autoconfig_deinit();
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
