# ESPHome Sendspin Remote

ESPHome firmware that turns the Radio Box board into a remote control and "now playing" screen for [Music Assistant](https://music-assistant.io/) using the Sendspin protocol.

The remote does not play audio itself. It joins the Sendspin group of a speaker and shows what that speaker is playing: cover art, title, artist, progress, playback state and volume. The three front buttons control playback.

<img width="804" height="861" alt="image" src="https://github.com/user-attachments/assets/f1735787-e0cf-46d2-b4ba-778f0a3dc129" />

## Features

- Cover art with a cross-fade between tracks
- Top bar: playback state, clock, volume
- Bottom bar: scrolling "title - artist", elapsed time, progress bar, track length
- Large clock with rainbow colours while nothing is playing
- Physical buttons: volume, play/pause, previous/next track, seek
- Lists to browse from the remote: play any SomaFM station, or start an ambient scene (mood audio and lights)
- Lights screen: colour, brightness and on/off for a group of lights
- Automatic switch between the clock and the playback screen
- Display off after a minute and deep sleep after five minutes without button presses
- Splash screen while connecting to Wi-Fi, Home Assistant and Music Assistant
- Hourglass overlay from pressing play until playback starts
- Volume ring overlay for a moment after each volume button press

## Screens

<img width="880" height="894" alt="image" src="https://github.com/user-attachments/assets/954a67df-b5eb-4c88-9853-c32690e420f2" />

<img width="890" height="892" alt="image" src="https://github.com/user-attachments/assets/ea7dc9d3-9693-43c8-b6a4-7c8187954d64" />


## Buttons

| Button | Single click | Double click | Long press |
|---|---|---|---|
| Left | Volume down | Previous track | Seek back while held |
| Middle | Play / pause | – | – |
| Right | Volume up | Next track | Seek forward while held |
| Side | Next [list](#lists) or [screen](#lights), then back to the player | – | – |

A single click on the left or right button fires about 0.3 s after release, because the remote waits to rule out a double click.

## Lists

The side button cycles through the lists and screens: player → SomaFM → Ambient → [Lights](#lights) → player.

| List | Middle button | Through Home Assistant |
|---|---|---|
| SomaFM | Plays the selected station on `sendspin_resume_entity` | `media_player.play_media` |
| Ambient | Starts the selected scene: mood audio and lights. **Stop**, at the end of the list, turns them off again. | `automation.trigger` on the `automation.ambient_*` automations |

On a list:

| Button | Action |
|---|---|
| Left | Previous item |
| Right | Next item |
| Middle | Act on the selected item and go back to the player |
| Side | Next list or screen |

The lists are compiled into the firmware. Everything goes through Home Assistant, so the lists can be opened as soon as Wi-Fi is connected, before Music Assistant has connected to the remote. To change the ambient scenes, edit [packages/ambient-scenes.yaml](packages/ambient-scenes.yaml); its entries match the `automation.ambient_*` automations in Home Assistant.

<img width="894" height="898" alt="image" src="https://github.com/user-attachments/assets/8457e02f-0c7c-41cf-adff-20b3e176e79d" />

<img width="895" height="896" alt="image" src="https://github.com/user-attachments/assets/b8914b9a-00aa-425e-9158-2455f6948e2b" />


## Lights

The Lights screen controls `light.booba`, `light.mati` and `light.mama` together. It shows a 4x4 grid of colours, with the selected one outlined. The title line shows the brightness while the lights are on, and an on/off switch.

| Button | Action |
|---|---|
| Left / right click | Previous / next colour, applied right away (also turns the lights on) |
| Left / right long press | Brightness down / up while held, down to 5% at the lowest |
| Middle | Lights on / off |
| Side | Back to the player |

The colours are warm, neutral and cool white (2700 K, 4000 K, 6500 K), then red, orange, amber, yellow, lime, green, spring green, cyan, sky blue, blue, violet, magenta and pink. The on/off state and brightness shown, and the on/off state the middle button follows, are those of `light.booba`.

<img width="891" height="893" alt="image" src="https://github.com/user-attachments/assets/5352d5f4-bb40-4274-8a88-9cd892c723b4" />


## Power saving

| Time since last button press | State |
|---|---|
| 1 minute | Display and backlight off. Still connected, so any button turns the display back on instantly and does its normal action. |
| 5 minutes | Deep sleep. Any front button wakes the remote. The press that wakes it does nothing else. |

Waking from deep sleep is a full reboot. The splash screen shows while Wi-Fi, Home Assistant and Music Assistant reconnect, usually within 5 to 15 seconds.

In Home Assistant:

- **Display** turns the display on and off manually.
- **Display Sleep Enabled** off keeps the display on.
- **Sleep Enabled** off keeps the remote out of deep sleep, for example for OTA updates or reading logs.

<img width="377" height="257" alt="image" src="https://github.com/user-attachments/assets/22beb52d-5b56-451a-a0d3-c8e559b79fdf" />


## Light variant

[remote-01-light.yaml](remote-01-light.yaml) is a lightweight firmware for the S2 mini, without Sendspin. It keeps:

- the clock,
- the lists: start a SomaFM station, start an ambient scene,
- the Lights screen,
- power saving and the splash screen (Wi-Fi, then Home Assistant).

It has no now-playing screen, cover art, volume or playback buttons, since it is not part of a Sendspin group. On the clock screen only the side button does something: it opens the lists and the Lights screen as usual. Starting a station shows the hourglass briefly to confirm the request was sent.

## Requirements

- Radio Box board with the 240x240 ST7789 display, and one of:
  - LOLIN S3 mini (ESP32-S3FH4R2, 2 MB PSRAM): recommended
  - LOLIN S2 mini (ESP32-S2FH4, no PSRAM): use the [light variant](#light-variant). The full firmware runs, with reduced cover art quality, but has so little spare memory that it can crash when playback starts.
- [ESPHome](https://esphome.io/) 2026.9.0 or newer
- Music Assistant with a Sendspin speaker
- Home Assistant with the ESPHome integration, with **Allow the device to perform Home Assistant actions** enabled for each remote (see [Build and flash](#build-and-flash)). Needed to resume playback after a stop (see [Known limitations](#known-limitations)), for everything on the [lists](#lists), and for the [Lights](#lights) screen.

## Files

| File | Purpose |
|---|---|
| [sendspin-remote-01.yaml](sendspin-remote-01.yaml) | Device config for the LOLIN S2 mini: board, Wi-Fi, memory tuning, list of packages |
| [sendspin-remote-02.yaml](sendspin-remote-02.yaml) | Device config for the LOLIN S3 mini: board, pins, PSRAM, memory-dependent settings |
| [remote-01-light.yaml](remote-01-light.yaml) | Device config for the [light variant](#light-variant) on the LOLIN S2 mini |
| [packages/ui-light.yaml](packages/ui-light.yaml) | Light variant UI: splash screen, clock, hourglass, "Display" switch |
| [packages/gpio.yaml](packages/gpio.yaml) | The four buttons as binary sensors |
| [packages/display-tft.yaml](packages/display-tft.yaml) | SPI bus and ST7789 display |
| [packages/sendspin-screen-only.yaml](packages/sendspin-screen-only.yaml) | Sendspin hub and media player |
| [packages/sendspin-addon-display.yaml](packages/sendspin-addon-display.yaml) | LVGL user interface: splash screen, clock, cover art, top and bottom bars; "Display" switch |
| [packages/artwork_prealloc.h](packages/artwork_prealloc.h) | Creates the cover art decoder at boot (see [Known limitations](#known-limitations)) |
| [packages/sendspin-addon-buttons.yaml](packages/sendspin-addon-buttons.yaml) | What each button does |
| [packages/sendspin_seek.h](packages/sendspin_seek.h) | Seek helper (see [Known limitations](#known-limitations)) |
| [packages/browse.yaml](packages/browse.yaml) | List page and its button handling |
| [packages/browse_list.h](packages/browse_list.h) | List and item types for the list page |
| [packages/somafm-stations.yaml](packages/somafm-stations.yaml) | SomaFM list: titles, stream URLs, icons |
| [packages/ambient-scenes.yaml](packages/ambient-scenes.yaml) | Ambient list: scene names, automations, icons |
| [packages/lights.yaml](packages/lights.yaml) | Lights screen: colour grid, brightness, on/off |
| [packages/sleep.yaml](packages/sleep.yaml) | Display-off and deep sleep timers, wake-up buttons, "Display Sleep Enabled" and "Sleep Enabled" switches |
| [packages/display_power.h](packages/display_power.h) | Turns the display off and re-initialises it when turned back on |
| [packages/remote_sleep.h](packages/remote_sleep.h) | Holds the display reset/backlight pin low and keeps wake-up pin pull-downs during sleep |

## Configuration

Set these substitutions in the device config for your board:

| Substitution | Example | Description |
|---|---|---|
| `name` | `sendspin-remote-01` | Device hostname |
| `friendly_name` | `Remote 01` | Name shown in Home Assistant |
| `sendspin_resume_entity` | `media_player.living_room` | Home Assistant player that the middle button resumes after a stop, and that the SomaFM list plays on |

Optional tuning, with defaults set in the packages:

| Substitution | Default | Description |
|---|---|---|
| `cover_art_size` | `60` | Size of the cover art requested from the server, in px. LVGL scales it up to fill the 240 px screen. The S3 config sets `240`. |
| `lvgl_buffer_size` | `5%` | LVGL draw buffer, as a share of the screen. The S3 config sets `25%`. |
| `task_stack_in_psram` | – | Put Sendspin task stacks in PSRAM. Set in each device config: `false` on the S2, `true` on the S3. |
| `cover_art_fade_time` | `400` | Cover art cross-fade time, in ms |
| `volume_overlay_time` | `1500ms` | How long the volume ring stays after the last volume button press |
| `seek_step_ms` | `10000` | Distance of each seek step while a button is held, in ms |
| `seek_repeat_interval` | `500ms` | Time between seek steps while a button is held |
| `display_off_after` | `60s` | Time without a button press before the display turns off |
| `sleep_after` | `5min` | Time without a button press before deep sleep |
| `somafm_icon_size` | `32x32` | Size of the station icons |
| `lights_entities` | `light.booba, light.mati, light.mama` | Lights the Lights screen controls together |
| `lights_state_entity` | `light.booba` | Light whose on/off state the Lights screen shows and follows |
| `lights_brightness_step` | `10` | Brightness change per step while left / right is held, in % |
| `lights_brightness_interval` | `400ms` | Time between brightness steps while left / right is held |
| `lights_brightness_min` | `5` | Lowest brightness when dimming, in %. Dimming stops there instead of turning the lights off. |

The clock gets its time over SNTP. The time zone is the one of the computer that builds the firmware; set `timezone:` on the `sntp` time platform in [packages/sendspin-addon-display.yaml](packages/sendspin-addon-display.yaml) to override it.

Wi-Fi credentials come from `secrets.yaml`:

```yaml
wifi_ssid: "your-ssid"
wifi_password: "your-password"
```

## Pins

Both boards share the D1 mini footprint, but the same header pin is a different GPIO on each. The packages default to the S2 mini; [sendspin-remote-02.yaml](sendspin-remote-02.yaml) overrides them for the S3 mini.

| Function | Substitution | S2 mini | S3 mini |
|---|---|---|---|
| Side button | `side_button` | GPIO0 | GPIO0 |
| Left button | `left_button` | GPIO12 | GPIO10 |
| Middle button | `middle_button` | GPIO16 | GPIO16 |
| Right button | `right_button` | GPIO9 | GPIO13 |
| SPI clock | `spi_clk_pin` | GPIO7 | GPIO12 |
| SPI MOSI | `spi_mosi_pin` | GPIO11 | GPIO11 |
| Display DC | `display_dc_pin` | GPIO18 | GPIO18 |
| Display reset and backlight | `display_reset_pin` | GPIO5 | GPIO4 |

## Build and flash

```sh
python3 -m venv .venv
.venv/bin/pip install esphome
.venv/bin/esphome run sendspin-remote-02.yaml   # S3 mini
.venv/bin/esphome run remote-01-light.yaml      # S2 mini, light variant
.venv/bin/esphome run sendspin-remote-01.yaml   # S2 mini, full (tight on memory)
```

For the first flash, connect the board over USB and pick the serial port. After that, updates can go over Wi-Fi.

Then in Home Assistant:

1. Add the device through the ESPHome integration.
2. Open **Settings → Devices & Services → ESPHome → the remote → Configure**.
3. Enable **Allow the device to perform Home Assistant actions**.

> [!IMPORTANT]
> Step 3 is off by default, and has to be done for every new remote. Without it, Home Assistant ignores the remote's requests: the middle button can't resume playback after a stop, and nothing on the lists or the Lights screen works.

Finally, add the remote to the speaker's Sendspin group in Music Assistant, so it shows what that speaker plays.

## Troubleshooting

**Play shows the hourglass for about 15 seconds, then nothing plays.** The remote asked Home Assistant to start playback, and nothing came of it. Check that:

- **Allow the device to perform Home Assistant actions** is enabled for this remote (see [Build and flash](#build-and-flash)). In the device log, a line like `resuming media_player.living_room via Home Assistant` or `Playing <station>` is then followed by neither `Home Assistant resumed ...` / `Home Assistant started playback` nor an error.
- `sendspin_resume_entity` is the speaker's current entity ID. Home Assistant reports success even for an entity that doesn't exist, so the log shows `Home Assistant resumed ...` but nothing plays.

If Home Assistant rejects the request, the log shows `Home Assistant could not ...` with its error message.

## Known limitations

**S2 mini: no PSRAM.** The ESP32-S2FH4 has about 170 KB of RAM and no PSRAM. To make room for cover art:

- Cover art is decoded at 60x60 and scaled up by LVGL, so it looks soft. Two 240x240 frames would need 230 KB.
- The cover art JPEG decoder (about 20 KB) is created at boot by [packages/artwork_prealloc.h](packages/artwork_prealloc.h). Created later, when the first cover arrives, it may not find a free block that size, and the device crashes.
- The S2 device config turns off the high-performance networking that Sendspin requests, and resets the Wi-Fi buffers to ESP-IDF defaults.
- The LVGL draw buffer is 5% of the screen.
- Page changes are not animated, on either board.
- Track title, artist and album are not exposed to Home Assistant as sensors.

Even so, only a few KB of heap are left at the moment playback starts. The S3 mini doesn't need any of this: [sendspin-remote-02.yaml](sendspin-remote-02.yaml) enables PSRAM, uses full-size cover art and a larger draw buffer, and keeps Sendspin's network settings.

**Resume after stop.** Pausing a speaker makes Music Assistant stop the group and move the remote into a group of its own. That group has no queue, so the remote can't send `play` over Sendspin, and the Sendspin `switch` command gets no response. When this happens, the middle button calls `media_player.media_play` on `sendspin_resume_entity` through Home Assistant instead.

**ESPHome internals.** Three helpers reach into ESPHome components to do what their public interface doesn't offer, and may break after an ESPHome update:

- [packages/sendspin_seek.h](packages/sendspin_seek.h): ESPHome has no seek action for Sendspin yet, so this sends `seek_relative` commands through the Sendspin hub. Remove it once ESPHome supports seeking.
- [packages/display_power.h](packages/display_power.h): the display component can only initialise the panel once, so this replays its init sequence after the display was turned off.
- [packages/artwork_prealloc.h](packages/artwork_prealloc.h): creates the cover art decoder early.
