#!/bin/bash
set -e

INSTALL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Checking dependencies..."

# --- raylib ---
if ! pkg-config --exists raylib 2>/dev/null; then
    echo "Installing raylib..."
    sudo apt update
    sudo apt install -y build-essential git cmake libasound2-dev mesa-common-dev \
        libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev

    git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib
    cd /tmp/raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    cd "$INSTALL_DIR"
    rm -rf /tmp/raylib
else
    echo "raylib already installed."
fi

# --- Flatpak itself ---
if ! command -v flatpak &> /dev/null; then
    echo "Installing Flatpak..."
    sudo apt update
    sudo apt install -y flatpak
    sudo flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
else
    echo "Flatpak already installed."
fi

# --- Flatpak apps your project needs ---
FLATPAK_APPS=(
    "org.DolphinEmu.dolphin-emu"
    "net.kuribo64.melonDS"
    "org.flycast.Flycast"
    "org.ppsspp.PPSSPP"
    "io.github.strikerx3.ymir"
)

for app in "${FLATPAK_APPS[@]}"; do
    if ! flatpak list | grep -q "$app"; then
        echo "Installing $app..."
        flatpak install -y flathub "$app"
    else
        echo "$app already installed."
    fi
done

# --- desktop shortcut (from before) ---
mkdir -p ~/.local/share/applications
cat > ~/Desktop/Gaming-Console.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Gaming Console
Comment=Custom retro gaming console frontend
Exec=${INSTALL_DIR}/gaming-console
Icon=${INSTALL_DIR}/assets/icon.png
Terminal=false
Categories=Game;
EOF
cp ~/Desktop/Gaming-Console.desktop ~/.local/share/applications/
chmod +x ~/Desktop/Gaming-Console.desktop
chmod +x ~/.local/share/applications/Gaming-Console.desktop

echo "Setup complete!"