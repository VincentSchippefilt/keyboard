/*
 *
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>
#include <zephyr/random/random.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/events/activity_state_changed.h>
#include <zmk/usb.h>
#include <zmk/ble.h>

#include "peripheral_status.h"

LV_IMG_DECLARE(balloon);
LV_IMG_DECLARE(mountain);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct peripheral_status_state {
    bool connected;
};

static void draw_top(lv_obj_t *widget, lv_color_t cbuf[], const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 0);

    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_montserrat_16, LV_TEXT_ALIGN_RIGHT);
    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);

    // Fill background
    lv_canvas_draw_rect(canvas, 0, 0, CANVAS_SIZE, CANVAS_SIZE, &rect_black_dsc);

    // Draw battery
    draw_battery(canvas, state);

    // Draw output status
    lv_canvas_draw_text(canvas, 0, 0, CANVAS_SIZE, &label_dsc,
                        state->connected ? LV_SYMBOL_WIFI : LV_SYMBOL_CLOSE);

    if (state->idle) {
        lv_draw_label_dsc_t label_dsc_zzz;
        init_label_dsc(&label_dsc_zzz, LVGL_FOREGROUND, &lv_font_montserrat_16, LV_TEXT_ALIGN_CENTER);
        lv_canvas_draw_text(canvas, 0, 28, 68, &label_dsc_zzz, "ZZZ");
    }

    // Rotate canvas
    rotate_canvas(canvas, cbuf);
}

static void set_battery_status(struct zmk_widget_status *widget,
                               struct battery_status_state state) {
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    widget->state.charging = state.usb_present;
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

    widget->state.battery = state.level;

    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_status(widget, state); }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    return (struct battery_status_state){
        .level = zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

static struct peripheral_status_state get_state(const zmk_event_t *_eh) {
    return (struct peripheral_status_state){.connected = zmk_split_bt_peripheral_is_connected()};
}

static void set_connection_status(struct zmk_widget_status *widget,
                                  struct peripheral_status_state state) {
    widget->state.connected = state.connected;

    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void output_status_update_cb(struct peripheral_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_connection_status(widget, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_status, struct peripheral_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_peripheral_status, zmk_split_peripheral_status_changed);

struct activity_status_state {
    bool idle;
};

static void set_activity_status(struct zmk_widget_status *widget,
                                struct activity_status_state state) {
    widget->state.idle = state.idle;
    if (state.idle) {
        lv_obj_add_flag(widget->art, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(widget->art, LV_OBJ_FLAG_HIDDEN);
    }
    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void activity_status_update_cb(struct activity_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_activity_status(widget, state); }
}

static struct activity_status_state activity_status_get_state(const zmk_event_t *eh) {
    const struct zmk_activity_state_changed *ev = as_zmk_activity_state_changed(eh);
    return (struct activity_status_state){
        .idle = (ev != NULL) ? ev->state != ZMK_ACTIVITY_ACTIVE : false,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_activity_status, struct activity_status_state,
                            activity_status_update_cb, activity_status_get_state)
ZMK_SUBSCRIPTION(widget_activity_status, zmk_activity_state_changed);

#ifdef CONFIG_NICE_VIEW_DISP_ROTATE_180 // sets positions for default and flipped canvases
int art_pos = 20;
int top_pos = 0;
#else
int art_pos = 0;
int top_pos = 92;
#endif

int zmk_widget_status_init(struct zmk_widget_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 160, 68);
    lv_obj_t *top = lv_canvas_create(widget->obj);
    lv_obj_align(top, LV_ALIGN_TOP_LEFT, top_pos, 0);
    lv_canvas_set_buffer(top, widget->cbuf, CANVAS_SIZE, CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);

    lv_obj_t *art = lv_img_create(widget->obj);
    bool random = sys_rand32_get() & 1;
    lv_img_set_src(art, random ? &balloon : &mountain);
    lv_obj_align(art, LV_ALIGN_TOP_LEFT, art_pos, 0);
    widget->art = art;

    sys_slist_append(&widgets, &widget->node);
    widget_battery_status_init();
    widget_peripheral_status_init();
    widget_activity_status_init();

    return 0;
}

lv_obj_t *zmk_widget_status_obj(struct zmk_widget_status *widget) { return widget->obj; }
