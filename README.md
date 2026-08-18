# SP1DER GAMES gaming emulator
A gaming emulator frontend to organize and run old games through libretro and other external applications. Written to work on a Raspberry Pi 5 (4 or 8 Gb), but should work on any Linux Aarch64 system that can handle the consoles.

## Overview & Features
* **Proximity Sensing:** Uses a sonar range finder to track user distance.
* **Flower Actuation:** Controls 4 servos via a 16-channel PCA9685 PWM driver over I2C to open/close petals with fishing lines.
* **State Machine:** Transitions between states for closing servos, opening servos, and powering off.
* **LCD Display:** Displays custom messages on LCD1602 display.

## Requirements
* **System:** Rapsberry Pi 5 (4 or 8 Gb) or any other Linux Aarch64 device (Note: Less powerful systems may have more trouble running certain consoles)
* **OS:** Raspberry Pi OS or Linux distro with apt
* **Internet Connection:** Needed to install raylib and Flatpak applications
* **Permissions:** sudo access
* **Storage:** 
* **Display:** Some type of monitor, TV, or other display set to 1080p, 60 FPS
* **ROMS:** Game files for the games included that the user wishes to play (placed in assets/roms/)
* **BIOS Files:** For each console that requires it (placed in assets/system/ for libretro games and other locations for Flatpak applications)
* **Controller:** Some type of Xbox Controller and a keyboard/mouse

## Repository Structure

```text
.
├── .vscode/
│   └── c_cpp_properties.json  # 
│   └── settings.json  # 
├── assets/
│   ├── cores/         # 
│   ├── emulator-configs/         # 
│   │   ├── dolphin/         # 
│   │   ├── flycast/         #
│   │   ├── melonds/         #
│   │   ├── ppsspp/         #
│   │   └── saturn/         #
│   ├── fonts/         # 
│   ├── images/         # 
│   │   ├── arcade/         # 
│   │   ├── handheld/         # 
│   │   ├── ninte_3d/         # 
│   │   ├── nint_ret/         # 
│   │   ├── other/         # 
│   │   ├── pc_indie/         # 
│   │   ├── playstation/         # 
│   │   └── sega/         # 
│   ├── roms/         #          # 
│   ├── saves/         #          # 
│   ├── system/         #          # 
│   └── txt/         # 
├── src/
│   └── main.cpp         # 
├── .gitignore 
├── install.sh 
├── Makefile       # 
└── README.md            # 
```

## Getting Started
1. Install [VS Code](https://code.visualstudio.com/) and the **PlatformIO IDE** extension
2. Clone this repository
3. Build hardware using parts listed above
4. Connect Arduino Nano via USB-C
5. Use PlatformIO upload button (or press `Ctrl` + `Alt` + `U`) to compile and flash to hardware

## Configuration & Calibration
All user-configurable parameters are located in `include/config.h`.
* `SERVO_MIN/SERVO_MAX`: Sets boundaries for servos which control how much the petals open.
* `RANGE`: Controls how far the user can be (in cm) for the flower to still open.