# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is a ZMK firmware configuration repository for a **Sofle Choc Pro BT** split wireless keyboard with a nice!view display. It does not contain a local ZMK build environment — firmware is compiled via GitHub Actions CI.

## Building Firmware

Firmware is built automatically by GitHub Actions on every push. To trigger a build manually:
- Push a commit, or
- Go to GitHub Actions → "Build ZMK Firmware" → "Run workflow"

The build matrix is defined in [build.yaml](build.yaml) and produces 4 artifacts:
- Left half + nice_view_disp + studio-rpc-usb-uart (`CONFIG_ZMK_STUDIO=y` on left only)
- Right half + nice_view_disp + studio-rpc-usb-uart
- Left half + settings_reset (for resetting BLE bonds)
- Right half + settings_reset

There is no local build command — the GitHub Actions workflow ([.github/workflows/build.yml](.github/workflows/build.yml)) delegates to ZMK's official `build-user-config` action.

## Repository Structure

- [config/sofle_choc_pro.keymap](config/sofle_choc_pro.keymap) — Main keymap (AZERTY Belgian layout, layers, combos, behaviors)
- [config/sofle_choc_pro.conf](config/sofle_choc_pro.conf) — ZMK feature flags (mouse emulation, sleep, idle)
- [config/sofle_choc_pro.json](config/sofle_choc_pro.json) — ZMK Studio physical layout export
- [config/keys_fr_belgian.h](config/keys_fr_belgian.h) — Custom key definitions for Belgian French characters
- [config/west.yml](config/west.yml) — ZMK dependency manifest (pins ZMK to v0.3)
- [boards/arm/sofle_choc_pro/](boards/arm/sofle_choc_pro/) — nRF52840 board definitions (device tree, Kconfig, CMake)
- [boards/shields/nice_view_disp/](boards/shields/nice_view_disp/) — Custom nice!view display shield with status widgets

## Keymap Architecture

The keymap uses 5 layers (0–4), with layer 4 currently unused:

| Layer | `#define` | `display-name` | Purpose |
|-------|-----------|----------------|---------|
| 0     | `BASE`    | "Azerty"       | AZERTY Belgian layout for macOS |
| 1     | `LOWER`   | "Symbol"       | Symbols, function keys, brackets |
| 2     | `RAISE`   | "Nav."         | Navigation, mouse buttons, clipboard shortcuts |
| 3     | `ADJUST`  | "adjust"       | Bluetooth profiles, RGB controls, power |

ADJUST is activated automatically via `conditional_layers` when both LOWER and RAISE are held simultaneously.

**Custom behaviors:**
- `hold_tap` (Shift key): hold = Caps Lock, tap = Caps Word
- `layer_hold_tap`: hold = activate layer (`&mo`), tap = toggle layer (`&tog`)

**Combos:**
- D+F (positions 27+28) → Stack Left (Ctrl+Cmd+Left, macOS window snapping)
- J+K (positions 31+32) → Stack Right (Ctrl+Cmd+Right)

**Encoders:**
- Left: Left/Right arrows (Symbol layer: Vol Down/Up)
- Right: Vol Down/Up (Symbol/Nav layers: Prev/Next track)

## Display Shield

The nice!view display has a custom status screen implemented in C under [boards/shields/nice_view_disp/](boards/shields/nice_view_disp/).

**Widget layout** (`widgets/status.c`) — three stacked 68×68 LVGL canvases:
- **Top**: battery level, BT/USB connection icon, WPM graph (or idle/sleep indicator)
- **Middle**: 5 BT profile circles (active profile filled)
- **Bottom**: active layer name

Display state reacts to ZMK events via `ZMK_DISPLAY_WIDGET_LISTENER` / `ZMK_SUBSCRIPTION` macros for battery, output, layer, WPM, and activity.

**Additional C modules in the shield:**
- `layer_rgb.c` — Automatically sets RGB underglow color on layer activation: Symbol (layer 1) → green, Nav (layer 2) → blue. Restores off-state when returning to base if RGB was off before.
- `sleep_display.c` — Virtual PM device that flushes LVGL to the Sharp LCD before the display is suspended (ensures the sleep indicator is rendered before power-off).

**Display config options** (add to `sofle_choc_pro.conf`):
- `CONFIG_NICE_VIEW_DISP_ROTATE_180=y` — rotate widget 180°
- `CONFIG_ZMK_DISPLAY_STATUS_SCREEN_BUILT_IN=y` — use ZMK built-in widget instead of the custom one

## macOS / Linux dual-OS support

BT profile 1 (channel 2, 0-indexed) is the Linux profile. `profile_layer.c` auto-activates layer 4 when that profile is selected and deactivates it otherwise.

| Layer | Name | Active on |
|-------|------|-----------|
| 4 | `LINUX_BASE` | Linux — swaps Ctrl↔GUI in thumb cluster, uses `FRL_AT` for `@`, points LOWER at layer 5 |
| 5 | `LINUX_LOWER` | Linux — same as LOWER but with `FRL_*` symbols from `keys_fr_belgian_linux.h` |
| 2 | `RAISE` | Shared — navigation and clipboard keys are OS-agnostic |
| 3 | `ADJUST` | Shared — triggered by holding LOWER+RAISE **or** LINUX_LOWER+RAISE |

**Key differences between macOS and Linux Belgian** (all handled by `keys_fr_belgian_linux.h`):

| Symbol | macOS (`FR_*`) | Linux (`FRL_*`) |
|--------|---------------|-----------------|
| `@` | AltGr+µ key | AltGr+é (key 2) |
| `[` | Left-Alt+Shift+( | AltGr+( (key 5) |
| `]` | Left-Alt+Shift+) | AltGr+) (key −) |
| `{` | AltGr+( (key 5) | AltGr+' (key 4) |
| `}` | AltGr+) (key −) | AltGr+à (key 0) |
| `\` | AltGr+Shift+> | AltGr+§ (key 6) |
| `\|` | AltGr+Shift+L | AltGr+& (key 1) |
| `~` | AltGr+N | AltGr+Shift+é — verify on your distro |

Window-snap combos are restricted by layer: D+F / J+K send `Ctrl+Cmd+←/→` on macOS layers and `Super+←/→` (GNOME/KDE tiling) on Linux layers.

## ZMK Version

Pinned to ZMK `v0.3` via [config/west.yml](config/west.yml). Upgrading requires updating the `revision` field there and verifying API compatibility in the keymap and board definitions.
