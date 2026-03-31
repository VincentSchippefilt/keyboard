/*
 * Layer RGB indicator — uses behavior invocation so BEHAVIOR_LOCALITY_GLOBAL
 * broadcasts to the peripheral half automatically.
 *
 * Symbol layer (1) → green, Nav layer (2) → blue.
 * On return to base: turns off if RGB was off before, otherwise leaves it on.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/behavior.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/rgb_underglow.h>
#include <dt-bindings/zmk/rgb.h>

#define SYMBOL_LAYER 1
#define NAV_LAYER    2

#define SYMBOL_HSB RGB_COLOR_HSB_VAL(120, 100, 20) // green
#define NAV_HSB    RGB_COLOR_HSB_VAL(240, 100, 20) // blue

static void invoke_rgb(uint32_t cmd, uint32_t param) {
    struct zmk_behavior_binding binding = {
        .behavior_dev = "RGB_UNDERGLOW",
        .param1 = cmd,
        .param2 = param,
    };
    struct zmk_behavior_binding_event event = {
        .position = 0,
        .timestamp = k_uptime_get(),
    };
    zmk_behavior_invoke_binding(&binding, event, true);
}

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
        zmk_rgb_underglow_get_state(&was_on);
    }

    if (symbol_active && !nav_active) {
        invoke_rgb(RGB_EFS_CMD, 0); // solid effect
        invoke_rgb(RGB_COLOR_HSB_CMD, SYMBOL_HSB);
        invoke_rgb(RGB_ON_CMD, 0);
    } else if (nav_active && !symbol_active) {
        invoke_rgb(RGB_EFS_CMD, 0); // solid effect
        invoke_rgb(RGB_COLOR_HSB_CMD, NAV_HSB);
        invoke_rgb(RGB_ON_CMD, 0);
    } else if (!symbol_active && !nav_active) {
        if (!was_on) {
            invoke_rgb(RGB_OFF_CMD, 0);
        }
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(layer_rgb, layer_rgb_listener);
ZMK_SUBSCRIPTION(layer_rgb, zmk_layer_state_changed);
