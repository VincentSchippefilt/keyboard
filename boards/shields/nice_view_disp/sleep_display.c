/*
 * Registers a virtual device whose PM suspend action flushes any pending
 * LVGL work to the Sharp memory LCD before the display device is suspended.
 *
 * zmk_pm_suspend_devices() iterates devices in reverse init order, so a
 * device at APPLICATION level is suspended before hardware devices (which
 * init at POST_KERNEL / PRE_KERNEL levels).  By the time the real display
 * device is suspended, the sleep indicator is already written to the panel.
 */

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/pm/device.h>

#include <lvgl.h>
#include <zmk/display.h>

static K_SEM_DEFINE(flush_done_sem, 0, 1);

static void do_flush(struct k_work *work) {
    lv_task_handler();
    k_sem_give(&flush_done_sem);
}
static K_WORK_DEFINE(flush_work, do_flush);

static int sleep_display_pm_action(const struct device *dev, enum pm_device_action action) {
    switch (action) {
    case PM_DEVICE_ACTION_SUSPEND:
        if (zmk_display_is_initialized()) {
            /* Canvas update work was already queued by the activity widget
             * listener.  Submitting flush_work after it ensures the display
             * thread runs: canvas-update → flush, in that order. */
            k_sem_reset(&flush_done_sem);
            k_work_submit_to_queue(zmk_display_work_q(), &flush_work);
            k_sem_take(&flush_done_sem, K_MSEC(500));
        }
        return 0;
    case PM_DEVICE_ACTION_RESUME:
        return 0;
    default:
        return -ENOTSUP;
    }
}

PM_DEVICE_DEFINE(sleep_display_pm, sleep_display_pm_action);

static int sleep_display_init(void) { return 0; }

DEVICE_DEFINE(sleep_display, "sleep_display", sleep_display_init,
              PM_DEVICE_GET(sleep_display_pm), NULL, NULL,
              APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY, NULL);
