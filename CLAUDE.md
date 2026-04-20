# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

This is a ZMK firmware configuration repository for a **Sofle Choc Pro BT** split wireless keyboard with a nice!view display. It does not contain a local ZMK build environment — firmware is compiled via GitHub Actions CI.

## Building Firmware

Firmware is built automatically by GitHub Actions on every push. To trigger a build manually:
- Push a commit, or
- Go to GitHub Actions → "Build ZMK Firmware" → "Run workflow"

The build matrix is defined in [build.yaml](build.yaml) and produces 4 artifacts:
- Left half + nice_view_disp + studio-rpc-usb-uart
- Right half + nice_view_disp + studio-rpc-usb-uart
- Left half + settings_reset (for resetting BLE bonds)
- Right half + settings_reset

There is no local build command — the GitHub Actions workflow ([.github/workflows/build.yml](.github/workflows/build.yml)) delegates to ZMK's official `build-user-config` action.

## Repository Structure

- [config/sofle_choc_pro.keymap](config/sofle_choc_pro.keymap) — Main keymap (AZERTY Belgian layout, layers, combos, behaviors)
- [config/sofle_choc_pro.conf](config/sofle_choc_pro.conf) — ZMK feature flags (mouse emulation, sleep, idle)
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

The nice!view display has a custom status screen implemented in C under [boards/shields/nice_view_disp/](boards/shields/nice_view_disp/). Key files:
- `custom_status_screen.c` — Top-level screen layout
- `widgets/status.c` — Layer name and WPM display
- `widgets/peripheral_status.c` — Bluetooth and battery status
- `widgets/bolt.c` / `art.c` — Visual elements

## ZMK Version

Pinned to ZMK `v0.3` via [config/west.yml](config/west.yml). Upgrading requires updating the `revision` field there and verifying API compatibility in the keymap and board definitions.
