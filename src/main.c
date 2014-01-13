#include <pebble.h>

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
char hour_a_buffer[] = "00";
char hour_b_buffer[] = "00";
char hour_c_buffer[] = "00";

char buffer[] = "00";
int hours12[] = {12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
int hours24[] = {00, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

const int CONTAINER_HEIGHT = 250;
const int SPACER = 130;
int font_size = 49;
int base_y;

void container_update_proc(struct Layer *layer, GContext *ctx) {
	int center_y = CONTAINER_HEIGHT / 2;
	
	graphics_context_set_stroke_color(ctx, GColorBlack);
	for (int line_i = -2; line_i < 3; line_i += 1) {
		// draw hour-boxes (3px tall)
		graphics_fill_rect(ctx, GRect(144-30,  center_y - 1 - (SPACER*line_i), 30, 3), 0, GCornerNone );
		graphics_fill_rect(ctx, GRect(0,  center_y - 1 - (SPACER*line_i), 30, 3), 0, GCornerNone );
		
		// 15 min
		graphics_fill_rect(ctx, GRect(144-10, center_y - 1 - (SPACER*(line_i+.25)), 10, 3), 0, GCornerNone );
		graphics_fill_rect(ctx, GRect(0,  center_y - 1 - (SPACER*(line_i+.25)), 10, 3), 0, GCornerNone );
		
		// 30 min 
		graphics_fill_rect(ctx, GRect(144-20,  center_y - 1 - (SPACER*(line_i+.5)), 20, 3), 0, GCornerNone );
		graphics_fill_rect(ctx, GRect(0,  center_y - 1 - (SPACER*(line_i+.5)), 20, 3), 0, GCornerNone );
		
		// 45 min
		graphics_fill_rect(ctx, GRect(144-10,  center_y - 1 - (SPACER*(line_i+.75)), 10, 3), 0, GCornerNone );
		graphics_fill_rect(ctx, GRect(0,  center_y - 1 - (SPACER*(line_i+.75)), 10, 3), 0, GCornerNone );
		
		// 5 = 0.083
		float multiplier;
		multiplier = line_i + .083;
		graphics_draw_line(ctx, GPoint(144-5, center_y - (SPACER * multiplier)), GPoint(144, center_y - (SPACER * multiplier)));
		graphics_draw_line(ctx, GPoint(0, center_y - (SPACER * multiplier)), GPoint(5, center_y - (SPACER * multiplier)));

		// 10 = 0.166
		multiplier = line_i + .166;
		graphics_draw_line(ctx, GPoint(144-5, center_y - (SPACER * multiplier)), GPoint(144, center_y - (SPACER * multiplier)));
		graphics_draw_line(ctx, GPoint(0, center_y - (SPACER * multiplier)), GPoint(5, center_y - (SPACER * multiplier)));
		
		// 20 = 0.333
		multiplier = line_i + .333;
		graphics_draw_line(ctx, GPoint(144-5, center_y - (SPACER * multiplier)), GPoint(144, center_y - (SPACER * multiplier)));
		graphics_draw_line(ctx, GPoint(0, center_y - (SPACER * multiplier)), GPoint(5, center_y - (SPACER * multiplier)));
		
		// 25 = 0.416
		multiplier = line_i + .416;
		graphics_draw_line(ctx, GPoint(144-5, center_y - (SPACER * multiplier)), GPoint(144, center_y - (SPACER * multiplier)));
		graphics_draw_line(ctx, GPoint(0, center_y - (SPACER * multiplier)), GPoint(5, center_y - (SPACER * multiplier)));
		
		// 35 = 0.583
		multiplier = line_i + .583;
		graphics_draw_line(ctx, GPoint(144-5, center_y - (SPACER * multiplier)), GPoint(144, center_y - (SPACER * multiplier)));
		graphics_draw_line(ctx, GPoint(0, center_y - (SPACER * multiplier)), GPoint(5, center_y - (SPACER * multiplier)));

		// 40 = 0.666
		multiplier = line_i + .666;
		graphics_draw_line(ctx, GPoint(144-5, center_y - (SPACER * multiplier)), GPoint(144, center_y - (SPACER * multiplier)));
		graphics_draw_line(ctx, GPoint(0, center_y - (SPACER * multiplier)), GPoint(5, center_y - (SPACER * multiplier)));
		
		// 50 = 0.833
		multiplier = line_i + .833;
		graphics_draw_line(ctx, GPoint(144-5, center_y - (SPACER * multiplier)), GPoint(144, center_y - (SPACER * multiplier)));
		graphics_draw_line(ctx, GPoint(0, center_y - (SPACER * multiplier)), GPoint(5, center_y - (SPACER * multiplier)));
		
		// 55 = 0.916
		multiplier = line_i + .916;
		graphics_draw_line(ctx, GPoint(144-5, center_y - (SPACER * multiplier)), GPoint(144, center_y - (SPACER * multiplier)));
		graphics_draw_line(ctx, GPoint(0, center_y - (SPACER * multiplier)), GPoint(5, center_y - (SPACER * multiplier)));
	}

}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	int i = tick_time->tm_hour; // add an extra <cycle> so the -1 value works. We modulo it anyway. 

	int minutes = tick_time->tm_min;
	float hour_percent = (float)minutes/60.0;
	
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
	APP_LOG(APP_LOG_LEVEL_INFO, "y_offset: %d", y_offset);
	layer_set_bounds(container_layer, GRect(0, y_offset, 144, 168));
}

void handle_init(void) {
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

	text_b30 = text_layer_create(GRect(start_x, (center_y + (SPACER*1.5)), end_x, 30));
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
	invert_screen_layer = inverter_layer_create(GRect(0, 0, 144, 168));
	layer_add_child(window_get_root_layer(my_window), (Layer*) invert_screen_layer);

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
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}
