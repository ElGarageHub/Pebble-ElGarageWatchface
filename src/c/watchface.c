#include <pebble.h>

static Window *main_window;
static TextLayer *time_layer;
static TextLayer *date_layer;
static GFont time_font;
static GFont date_font;
static BitmapLayer *background_layer;
static GBitmap *background_bitmap;

char toUpperIfLower(char c) {
  if(c >= 97 && c <= 122) {
    return c - 32;
  }
  return c;
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char buffer[8];
  strftime(buffer, sizeof(buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M",
      tick_time);
  text_layer_set_text(time_layer, buffer);
  static char date_buffer[8];
  strftime(date_buffer, sizeof(date_buffer), "%a %d", tick_time);
  for(unsigned int i = 0; i < strlen(date_buffer); i++)
    date_buffer[i] = toUpperIfLower(date_buffer[i]);
  text_layer_set_text(date_layer, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ELGARAGE);
  background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(background_layer, background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));
  time_layer = text_layer_create(GRect(0, 80, bounds.size.w, 50));
  date_layer = text_layer_create(GRect(0, 115, bounds.size.w, 20));
  text_layer_set_background_color(time_layer, GColorClear);
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_text_color(date_layer, GColorWhite);
  time_font = fonts_load_custom_font(resource_get_handle(
        RESOURCE_ID_FONT_XOLONIUM_35));
  date_font = fonts_load_custom_font(resource_get_handle(
        RESOURCE_ID_FONT_XOLONIUM_15));
  text_layer_set_font(time_layer, time_font);
  text_layer_set_font(date_layer, date_font);
  text_layer_set_text_color(time_layer, GColorOxfordBlue);
  text_layer_set_text_color(date_layer, GColorOxfordBlue);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  fonts_unload_custom_font(time_font);
  fonts_unload_custom_font(date_font);
  gbitmap_destroy(background_bitmap);
  bitmap_layer_destroy(background_layer);
}

static void init() {
  main_window = window_create();
  window_set_background_color(main_window, GColorOxfordBlue);
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  update_time();
}

static void deinit() {
  window_destroy(main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
