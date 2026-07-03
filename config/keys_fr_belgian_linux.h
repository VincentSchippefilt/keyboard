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
 * Verified against the upstream xkeyboard-config "be" (basic) xkb symbols file
 * (the standard "Belgian" layout on Fedora/Ubuntu/Debian) and confirmed against
 * live hardware testing on Fedora.
 */

#pragma once

#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/modifiers.h>

/* @ — AltGr+é  (key 2) */
#define FRL_AT    (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_2_AND_AT)))

/* # — AltGr+"  (key 3). Bound via the linux_at_hash mod-morph, not directly. */
#define FRL_HASH  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_3_AND_HASH)))

/* { — AltGr+è  (key 7) */
#define FRL_LBRC  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_7_AND_AMPERSAND)))

/* [ — AltGr + physical [ key */
#define FRL_LBKT  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_LEFT_BRACKET_AND_LEFT_BRACE)))

/* ] — AltGr + physical ] key */
#define FRL_RBKT  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_RIGHT_BRACKET_AND_RIGHT_BRACE)))

/* \ — AltGr + physical - key */
#define FRL_BSLH  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_MINUS_AND_UNDERSCORE)))

/* | — AltGr+&  (key 1) */
#define FRL_PIPE  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION)))

/* } — AltGr+à  (key 0) */
#define FRL_RBRC  (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_0_AND_RIGHT_PARENTHESIS)))

/* ~ — AltGr + physical / key. Dead key (dead_tilde) on Linux; combines with the
 * next keystroke, press + Space for a standalone tilde. Sits on an ISO-only
 * physical position that's ambiguous to map 1:1 from the xkb table — verify on
 * real hardware and adjust if wrong. */
#define FRL_TILDE (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_SLASH_AND_QUESTION_MARK)))

/* ´ — AltGr + physical ; key. Dead key (dead_acute) on Linux, same caveat as ~. */
#define FRL_ACUTE (RA(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_SEMICOLON_AND_COLON)))

/* < — unmodified Non-US-Backslash (ISO 102nd key) */
#define FRL_LT    (ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_NON_US_BACKSLASH_AND_PIPE))

/* > — Shift + Non-US-Backslash (ISO 102nd key) */
#define FRL_GT    (LS(ZMK_HID_USAGE(HID_USAGE_KEY, HID_USAGE_KEY_KEYBOARD_NON_US_BACKSLASH_AND_PIPE)))
