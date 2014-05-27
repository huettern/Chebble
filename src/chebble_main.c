#include <pebble.h>

Window *window;
TextLayer *text_layer;
TextLayer *hide_cheat_layer;

// This is a scroll layer
static ScrollLayer *scroll_layer;

// Lorem ipsum to have something to scroll
//static char cheat_text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam quam tellus, fermentu  m quis vulputate quis, vestibulum interdum sapien. Vestibulum lobortis pellentesque pretium. Quisque ultricies purus e  u orci convallis lacinia. Cras a urna mi. Donec convallis ante id dui dapibus nec ullamcorper erat egestas. Aenean a m  auris a sapien commodo lacinia. Sed posuere mi vel risus congue ornare. Curabitur leo nisi, euismod ut pellentesque se  d, suscipit sit amet lorem. Aliquam eget sem vitae sem aliquam ornare. In sem sapien, imperdiet eget pharetra a, lacin  ia ac justo. Suspendisse at ante nec felis facilisis eleifend.";
static char cheat_text[] = "default";


static char hide_text[] = "Hidden!";

static const int vert_scroll_text_padding = 168;  //space on the bottom of the text layer when scrolling all way down

static TextLayer *timeLayer; // The clock

static GRect clockFrame = {.origin = {.x = 29, .y = 54}, .size = {.w = 144-40, .h = 168-54}};


#define KEY_INVERT 0
  
static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  
   APP_LOG(APP_LOG_LEVEL_DEBUG, "Long-Click");
}

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  
   APP_LOG(APP_LOG_LEVEL_DEBUG, "Single-Click");
}



//builds the desired button handlers
static void config_provider(Window *window) {
  //const uint16_t repeat_interval_ms = 100;

  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 0, (ClickHandler) select_long_click_handler, NULL);
  
}

static void in_recv_handler(DictionaryIterator *iterator, void *context)
{
  //Get Tuple
  Tuple *t = dict_read_first(iterator);
  if(t)
  {
    switch(t->key)
    {
    case KEY_INVERT:
      //It's the KEY_INVERT key
      if(strcmp(t->value->cstring, "on") == 0)
      {
        //Set and save as inverted
        text_layer_set_text(text_layer, "Inverted!");
 
        persist_write_bool(KEY_INVERT, true);
      }
      else if(strcmp(t->value->cstring, "off") == 0)
      {
        //Set and save as not inverted
        text_layer_set_text(text_layer, "Not inverted!");
 
        persist_write_bool(KEY_INVERT, false);
      }
      break;
    }
  }
}

// Setup the scroll layer on window load
// We do this here in order to be able to get the max used text size
static void window_load(Window *window) {
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = GRect(0, 0, 144, 168); //full screen
  GRect max_text_bounds = GRect(0, 0, bounds.size.w, 2000);
  
  /******************* init hide layer ***********************/
  hide_cheat_layer = text_layer_create(bounds);
  hide_cheat_layer = text_layer_create(max_text_bounds);
  text_layer_set_background_color(hide_cheat_layer, GColorBlack);
  text_layer_set_text_color(hide_cheat_layer, GColorWhite);
  text_layer_set_font(hide_cheat_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(hide_cheat_layer, hide_text);
  
  
  /******************* init text layer ***********************/
  text_layer = text_layer_create(bounds);
  text_layer = text_layer_create(max_text_bounds);
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(text_layer, cheat_text);
  
  
  /******************* Initialize the scroll layer ***********************/
  scroll_layer = scroll_layer_create(bounds);
  
  // This binds the scroll layer to the window so that up and down map to scrolling
  // may use scroll_layer_set_callbacks to add or override interactivity
  scroll_layer_set_click_config_onto_window(scroll_layer, window);

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(text_layer);
  scroll_layer_set_content_size(scroll_layer, GSize(bounds.size.w, max_size.h + vert_scroll_text_padding));
  
  //hide the top and bottom scroll border for more text view
  scroll_layer_set_shadow_hidden(scroll_layer, true);
  
  // Add the layers for display
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(text_layer));
  
	//layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));
}

static void window_unload(Window *window) {
  //inverter_layer_destroy(inverter_layer);
  text_layer_destroy(text_layer);
  scroll_layer_destroy(scroll_layer);
}

void handle_init(void) {
  // Create our app's base window
  window = window_create();
  
  //disable status bar
  window_set_fullscreen(window, true);
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  // Attach our desired button functionality
  window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
  
  //for communication with config
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  window_stack_push(window, true /* Animated */);
}

void handle_deinit(void) {
	// Destroy the text layer
	text_layer_destroy(text_layer);
	
	// Destroy the window
	window_destroy(window);
}

int main(void) {   
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App started");
	handle_init();
	app_event_loop();
  window_destroy(window);
}
