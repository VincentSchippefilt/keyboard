/*
 * Activates the Linux base layer (layer 4) automatically when BT profile 1
 * (channel 2) is selected, and deactivates it on any other profile.
 *
 * Layer 4 swaps the GUI/Ctrl modifiers and points the LOWER thumb key at the
 * Linux symbol layer (layer 5).  All other layers (RAISE, ADJUST) are shared
 * between macOS and Linux.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/keymap.h>

#define LINUX_PROFILE 1  /* BT channel 2, 0-indexed */
#define LINUX_LAYER   4

static void apply_profile(uint8_t index) {
    if (index == LINUX_PROFILE) {
        zmk_keymap_layer_activate(LINUX_LAYER);
    } else {
        zmk_keymap_layer_deactivate(LINUX_LAYER);
    }
}

static int profile_layer_listener(const zmk_event_t *eh) {
    const struct zmk_ble_active_profile_changed *ev = as_zmk_ble_active_profile_changed(eh);
    if (!ev) return ZMK_EV_EVENT_BUBBLE;
    apply_profile(ev->index);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(profile_layer, profile_layer_listener);
ZMK_SUBSCRIPTION(profile_layer, zmk_ble_active_profile_changed);

static int profile_layer_init(void) {
    apply_profile(zmk_ble_active_profile_index());
    return 0;
}

SYS_INIT(profile_layer_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
