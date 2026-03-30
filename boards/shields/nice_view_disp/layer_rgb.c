/*
 * Layer RGB indicator.
 * Symbol layer (1) → green, Nav layer (2) → blue.
 * On return to base: turns off if RGB was off before, otherwise leaves it on.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/rgb_underglow.h>

#define SYMBOL_LAYER 1
#define NAV_LAYER    2

// Brightness matches CONFIG_ZMK_RGB_UNDERGLOW_BRT_MAX = 20
#define LAYER_RGB_BRT 20

static const struct zmk_led_hsb SYMBOL_COLOR = {.h = 120, .s = 100, .b = LAYER_RGB_BRT}; // green
static const struct zmk_led_hsb NAV_COLOR    = {.h = 240, .s = 100, .b = LAYER_RGB_BRT}; // blue

static bool symbol_active = false;
static bool nav_active    = false;
static bool was_on        = false;

static int layer_rgb_listener(const zmk_event_t *eh) {
    const struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);
    if (!ev) return ZMK_EV_EVENT_BUBBLE;

    if (ev->layer != SYMBOL_LAYER && ev->layer != NAV_LAYER) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    bool any_was_active = symbol_active || nav_active;

    if (ev->layer == SYMBOL_LAYER) symbol_active = ev->state;
    if (ev->layer == NAV_LAYER)    nav_active    = ev->state;

    bool any_now_active = symbol_active || nav_active;

    if (!any_was_active && any_now_active) {
        // First indicator layer just activated — save RGB on/off state
        zmk_rgb_underglow_get_state(&was_on);
    }

    if (symbol_active && !nav_active) {
        zmk_rgb_underglow_set_hsb(SYMBOL_COLOR);
        zmk_rgb_underglow_on();
    } else if (nav_active && !symbol_active) {
        zmk_rgb_underglow_set_hsb(NAV_COLOR);
        zmk_rgb_underglow_on();
    } else if (!symbol_active && !nav_active) {
        // Back to base layer
        if (!was_on) {
            zmk_rgb_underglow_off();
        }
        // If RGB was already on: leave it on (no color restoration)
    }
    // Both active (ADJUST layer): keep current layer color, no change

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(layer_rgb, layer_rgb_listener);
ZMK_SUBSCRIPTION(layer_rgb, zmk_layer_state_changed);
