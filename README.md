# Wolf4SDL-Switch2

A native Nintendo Switch 2 homebrew port of Wolfenstein 3D,
based on Wolf4SDL and Keeganator's original Nintendo Switch port.

This port uses libounce and a custom SDL compatibility layer
adapted for Nintendo Switch 2.

## Current Release

v1.0 Experimental

## Features

- Native Nintendo Switch 2 support
- Dual analogue stick controls
- Working title screen and palette transitions
- Music and sound effects
- Corrected enemy voice playback
- Triple-buffered audio backend
- DOSBox OPL audio emulation
- Saving and loading
- Reproducible build script

## Audio Improvements

Technical details: [AUDIO.md](AUDIO.md)

The original Nintendo Switch port had known issues with
audio popping and crackling.

For the Nintendo Switch 2 port, the audio backend has
been reworked to address these issues.

- Implemented triple-buffered audio playback.
- Fixed audio popping and crackling during tested gameplay.
- Corrected high-pitched and accelerated enemy voices.
- Added proper mono-to-stereo PCM conversion.
- Improved music and sound effect playback.
- Replaced MAME OPL with the GPL-licensed DOSBox OPL emulator.
- Clearer audio and smoother screen fade transitions observed during testing.

These improvements have been tested on real Nintendo Switch 2
hardware. The original Nintendo Switch has not been tested
with these changes.

## Installation

A homebrew-capable Nintendo Switch 2 is required.

You must own a compatible copy of Wolfenstein 3D.

Supported game data:
Wolf3D Full v1.4 GT/ID/Activision.

Create the following directory structure on your SD card:

```
switch/
  Wolf3d.nro
  wolf4sdl/
    wolf3d/
      audiohed.wl6
      audiot.wl6
      gamemaps.wl6
      maphead.wl6
      vgadict.wl6
      vgagraph.wl6
      vgahead.wl6
      vswap.wl6
```

Launch Wolf3d.nro through the Homebrew Menu.

Original commercial game data is not included.

## Controls

| Input | Action |
| --- | --- |
| Left analogue stick | Movement |
| Right analogue stick | Turning |

Additional controller mappings will be documented.

## Known Issues

- Menu music stopping after returning from HOME was observed in earlier builds.
- Changing audio settings caused freezes in earlier builds.
- These two issues have not yet been retested with DOSBox OPL.

This is an experimental release. Further improvements are planned.

## Building from Source

The Switch 2 port requires a compatible devkitA64 and libounce
development environment.

Using a locally configured ounce-devkit Docker image:

```bash
sudo docker run --rm \
  -v "$PWD:/workspace" \
  -w /workspace \
  ounce-devkit:latest \
  bash switch2-test/build.sh
```

The resulting executable is located at:

```
switch2-test/gpl-build/Wolf3d.nro
```

## Credits

- id Software - Original Wolfenstein 3D
- Wolf4SDL developers and contributors
- Keeganator - Original Nintendo Switch port
- devkitPro - Development toolchain
- libounce developers and contributors
- DOSBox and MAME contributors
- Cerebral-Context - Nintendo Switch 2 port

Original Nintendo Switch repository:
https://github.com/keeganatorr/Wolf4SDL-Switch

The original Nintendo Switch README is preserved in
README-Switch1.md.

## Licensing

This project retains the original Wolf4SDL licensing terms.

- Wolfenstein 3D engine: see license-id.txt and license-gpl.txt.
- MAME OPL emulator: see license-mame.txt.
- DOSBox OPL emulator: GPL alternative.

The current Switch 2 build enables USE_GPL and uses the
DOSBox OPL emulator instead of MAME OPL.
The MAME emulator is not compiled into this build.

The engine and DOSBox OPL are used under their GPL terms.
Other components remain subject to their applicable licenses.

Dependency license notices:

- [libnx - ISC](licenses/libnx-ISC.txt)
- [libounce - ISC](licenses/libounce-ISC.txt)
- [mimalloc - MIT](licenses/mimalloc-MIT.txt)

Original commercial game data is not distributed.

See readme-wolf.txt for the upstream licensing declaration.
