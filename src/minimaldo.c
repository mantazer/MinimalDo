#include "pebble.h"

// Global variable declaration

Window *window;
TextLayer *date_layer;
TextLayer *time_layer;
TextLayer *todo_layer;
Layer *upper_border;
Layer *date_time_divider;
Layer *time_weather_divider;
Layer *lower_border;

// Line layer callback

void update_line_cb(Layer *layer, GContext* context) {
  graphics_context_set_fill_color(context, GColorWhite);
  graphics_fill_rect(context, layer_get_bounds(layer), 0, GCornerNone);
}

// Update on the minute

void tick(struct tm *tick_time, TimeUnits units_changed) {

  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";
  static char todo_text[] = "X X X X";
  static char *time_format;

  // Format and push to layer

  strftime(date_text, sizeof(date_text), "%B %e", tick_time);
  text_layer_set_text(date_layer, date_text);

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Format for non-24 hour displays (removing zero-padding)

  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  // Push to layer

  text_layer_set_text(time_layer, time_text);

  // TODO: Make dynamic

  text_layer_set_text(todo_layer, "do: Present Hack");
}

// Deallocation

void deinit(void) {

  tick_timer_service_unsubscribe();

  // Do NOT deallocate any layers:
  // Reason: watchface persistence

}

void init(void) {

  window = window_create();
  window_stack_push(window, true);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  GRect upper_border_frame = GRect(8, 2, 139, 2);
  upper_border = layer_create(upper_border_frame);
  layer_set_update_proc(upper_border, update_line_cb);
  layer_add_child(window_layer, upper_border);
  tick_timer_service_subscribe(MINUTE_UNIT, tick);

  date_layer = text_layer_create(GRect(8, 6, 144-8, 168-68));
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_font(date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_21)));
  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  GRect date_time_divider_frame = GRect(8, 35, 139, 2);
  date_time_divider = layer_create(date_time_divider_frame);
  layer_set_update_proc(date_time_divider, update_line_cb);
  layer_add_child(window_layer, date_time_divider);

  time_layer = text_layer_create(GRect(7, 35, 144-7, 168-92));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_font(time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_49)));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  GRect time_weather_frame = GRect(8, 95, 139, 2);
  time_weather_divider = layer_create(time_weather_frame);
  layer_set_update_proc(time_weather_divider, update_line_cb);
  layer_add_child(window_layer, time_weather_divider);

  todo_layer = text_layer_create(GRect(7, 115, 144-7, 168-92));
  text_layer_set_text_color(todo_layer, GColorWhite);
  text_layer_set_background_color(todo_layer, GColorClear);
  text_layer_set_font(todo_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_21)));
  layer_add_child(window_layer, text_layer_get_layer(todo_layer));

  GRect lower_border_frame = GRect(8, 162, 139, 2);
  lower_border = layer_create(lower_border_frame);
  layer_set_update_proc(lower_border, update_line_cb);
  layer_add_child(window_layer, lower_border);
  tick_timer_service_subscribe(MINUTE_UNIT, tick);
  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
