# SP1DER GAMES Console Emulator

## Overview
A gaming emulator frontend to organize and run old games through libretro and other external applications. Written to work on a Raspberry Pi 5 (4 or 8 Gb), but should work on any Linux Aarch64 system that can handle the consoles.

## Features
* Support for 21 consoles via libretro cores and Flatpak-based emulators
* Custom per-console keyboard, mouse, and controller mappings
* Live diagnostics overlay
* Automatic controller detection and support
* Cover art browsing UI with category/game scrolling
* Save/battery-backed game saves, with automatic, periodic saving
* One-command setup script (dependency installation + desktop shortcut)

## Requirements
* **System:** Raspberry Pi 5 (4 or 8 Gb) or any other Linux Aarch64 device (Note: Less powerful systems may have more trouble running certain consoles)
* **OS:** Raspberry Pi OS or Linux distro with apt
* **Internet Connection:** Needed to install raylib and Flatpak applications
* **Permissions:** sudo access
* **Storage:** 
* **Display:** Some type of monitor, TV, or other display set to 1080p, 60 FPS
* **ROMS:** Game files for the games included that the user wishes to play (placed in assets/roms/)
* **BIOS Files:** For each console that requires it (placed in assets/system/ for libretro games and other locations for Flatpak applications)
* **Controller:** Some type of Xbox Controller and a keyboard/mouse

## Download & Installation

### Option 1: Download the latest release (recommended)

1. Go to the [Releases](https://github.com/tywebb1724/SP1DER-GAMES-Console-Emulator-Linux-AArch64/releases) page.
2. Download the latest `xxxxx.zip`
3. Extract it to a folder of your choice

### Option 2: Download the source directly

1. Click the green **Code** button at the top of this page
2. Select **Download ZIP** then extract it.
    *(Or, if you have git installed: `git clone https://github.com/tywebb1724/SP1DER-GAMES-Console-Emulator-Linux-AArch64.git` )*

### Setup

1. Open a terminal in the extracted folder
2. Run the install script:
```bash
 >  chmod +x install.sh
 >  ./install.sh
```

    This will:
    - Install raylib (if not already present)
    - Install Flatpak and the required emulator apps (Dolphin, melonDS, Flycast, PPSSPP, Ymir)
    - Create a desktop shortcut with an icon

    >**Note:** This step requires an internet connection and will prompt for your password (`sudo`). First-time setup may take 15-30 minutes depending on your internet connection.

3. If you downloaded the source (Option 2), build the project
```bash
 >  make
```

4. Add the ROMS and BIOS files (see [Directory Structure](#directory-structure) below for where each console's files go). **ROMS and BIOS files are not included - you must supply your own legally obtained copies.**

5. Launch the app using the desktop icon, or run it directly:
```bash
 >  ./SP1DER-GAMES
```

### A note on display settings
 For best performance, set your display to ***1920x1080 @ 60 Hz.*** Some TVs (especially 4K displays) may auto-negotiate a higher resolution or lower refresh rate over HDMI, which can significantly reduce performance. Check via `raspi-config` → Display Options, or your desktop's display settings.

## Adding ROMS and BIOS files

This project does not include any ROMS, BIOS files, or copyrighted content. You must supply your own legally obtained copies.

### Libretro consoles
Place ROM files in `assets/roms/<console>/` and any required BIOS files in `assets/system/`.

### DOOM / DOOM II music
DOOM music is not included and must be sourced separately as `.mp3` files. Place them alongside your ROM files (`assets/roms/pc/`) or in (`assets/system/prboom/`), named to match each level (e.g. `e1m1.mp3` for DOOM, `runnin.mp3` for DOOM II - see [PrBoom's documentation](https://docs.libretro.com/library/prboom/) for the full naming list).

### External emulator (GameCube, Dreamcast, PSP, Nintendo DS, Sega Saturn)
These run through their own standalone Flatpak apps rather than through this project directly. The only one that needs BIOS files is Ymir (Sega Saturn). BIOS files for this emulator are placed in `~/.var/app/io.github.strikerx3.ymir/data/StrikerX3/Ymir/roms/ipl`.

ROMs for these consoles go in `assets/roms/<console>/`, same as the libretro consoles above - this project passes the path directly to each emulator when launching.

## Directory Structure

```text
.
├── assets/
│   ├── cores/              # Libretro emulator cores (included, open source; see Credits)
│   ├── emulator-configs/   # Configuration files for Flatpak applications
│   │   ├── dolphin/        # File(s) for Dolphin (GameCube)
│   │   ├── flycast/        # File(s) for Flycast (Dreamcast)
│   │   ├── melonds/        # File(s) for MelonDS (DS)
│   │   ├── ppsspp/         # File(s) for PPSSPP (PSP)
│   │   └── saturn/         # File(s) for Ymir (Saturn)
│   ├── fonts/              # Font files
│   ├── images/             # Different images
│   │   ├── arcade/         # Arcade game covers
│   │   ├── handheld/       # Handheld Classics game covers
│   │   ├── ninte_3d/       # 3D Nintendo game covers
│   │   ├── nint_ret/       # Retro Nintendo game covers
│   │   ├── other/          # Other images (backgrounds, logo, controller, etc.)
│   │   ├── pc_indie/       # PC/TurboGrafx/Other game covers
│   │   ├── playstation/    # Playstation game covers
│   │   └── sega/           # Sega game covers
│   ├── roms/               # ROM files for the games, separated by console
│   ├── saves/              # Save files for libretro games
│   ├── system/             # Libretro BIOS files and other system needs
│   └── txt/                # UI settings and controller mapping data
├── src/                    #.c and .h files for the libretro API, UI frontend, and more
├── install.sh              # Shell script for downloading the emulator and other libraries/applications needed
├── Makefile                # File for building code manually
└── README.md               # README file 
```

## Credits & Licenses

This project is built on top of several open-source tools.

### Libretro cores
This project uses precompiled libretro cores from the [RetroArch](https://retroarch.com/) / [libretro.com](https://www.libretro.com) project. Each core is developed independently by its own authors and distributed under its own open-source license. See [libretro.com](https://www.libretro.com) and the individual core respositories for license details.

### External emulators
The following are used for consoles not supported via libretro, and are installed separately via Flatpak (not bundled with this project):

- [Dolphin Emulator](https://dolphin-emu.org/) GameCube
- [melonDS](https://melonds.kuribo64.net/) Nintendo DS
- [Flycast](https://github.com/flyinghead/flycast) Dreamcast
- [PPSSPP](https://www.ppsspp.org/) PSP
- [Ymir](https://github.com/StrikerX3/Ymir) Sega Saturn 

Each of these is a separate open-source project with its own license and contributors.

### Framework & Libraries
- [raylib](https://www.raylib.com/) - the graphics/audio/input library this project's UI and rendering are built on created by Ramon Santamaria and contributors. Licensed under the [zlib/libpng license](https://github.com/raysan5/raylib/blob/master/LICENSE)

### Disclaimer
This project does not include any ROMS, BIOS files, or copyrighted game content. Users are responsible for legally obtaining their own game files and any required BIOS/firmware for the consoles they wish to emulate.