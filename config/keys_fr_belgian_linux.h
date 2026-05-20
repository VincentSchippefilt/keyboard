/*
 * Linux overrides for Belgian French key combinations.
 *
 * On macOS, both left and right Option keys act as AltGr-equivalent character
 * modifiers.  On Linux, only Right Alt (AltGr) is used for character composition,
 * and the AltGr combinations differ from macOS for several symbols.
 *
 * This file defines FRL_* replacements for the FR_* entries that do not work
 * correctly on Linux with a Belgian keyboard layout.  All other FR_* symbols
 * from keys_fr_belgian.h are identical on both platforms and can be reused as-is.
 *
 * Verified against the standard X11 xkb "be" layout (Ubuntu / Debian default).
 * If your distro ships a variant layout, test and adjust the positions marked
 * "verify" below.
 */

#pragma once

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/modifiers.h>

/* @ — AltGr+é  (key 2)    macOS: RA(backslash) */
#define FRL_AT    (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_2_AND_AT)))

/* # — AltGr+"  (key 3)    macOS: LS(RA(backslash)) */
#define FRL_HASH  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_3_AND_HASH)))

/* { — AltGr+'  (key 4)    macOS: RA(key5) */
#define FRL_LBRC  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_4_AND_DOLLAR)))

/* [ — AltGr+(  (key 5)    macOS: LA(LS(key5)) */
#define FRL_LBKT  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_5_AND_PERCENT)))

/* \ — AltGr+§  (key 6)    macOS: RA(LS(period)) */
#define FRL_BSLH  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_6_AND_CARET)))

/* | — AltGr+&  (key 1)    macOS: RA(LS(L)) */
#define FRL_PIPE  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION)))

/* } — AltGr+à  (key 0)    macOS: RA(minus) */
#define FRL_RBRC  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_0_AND_RIGHT_PARENTHESIS)))

/* ] — AltGr+)  (key -)    macOS: LA(LS(minus)) */
#define FRL_RBKT  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_MINUS_AND_UNDERSCORE)))

/* ~ — AltGr+Shift+é (key 2) — verify on your system */
#define FRL_TILDE (RA(LS(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_2_AND_AT))))
