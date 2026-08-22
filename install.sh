#!/bin/bash
set -e

INSTALL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Checking dependencies..."

# --- raylib ---
if ! pkg-config --exists raylib 2>/dev/null; then
    echo "Installing raylib dependencies and building raylib..."
    sudo apt update
    sudo apt install -y build-essential git cmake libasound2-dev mesa-common-dev \
        libx11-dev libxrandr-dev libxi-dev xorg-dev libgl1-mesa-dev libglu1-mesa-dev \
        libwayland-dev libxkbcommon-dev

    git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib
    cd /tmp/raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    sudo ldconfig
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
    sudo flatpak remote-add --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo
else
    echo "Flatpak already installed."
fi

# --- Flatpak apps ---
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

# --- Build Frontend ---
echo "Building SP1DER GAMES executable..."
cd "$INSTALL_DIR"
make

# --- Desktop Shortcut ---
mkdir -p ~/.local/share/applications
mkdir -p ~/Desktop

cat > ~/.local/share/applications/SP1DER-GAMES.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=SP1DER GAMES
Comment=Custom retro gaming console frontend
Exec=${INSTALL_DIR}/SP1DER-GAMES
Icon=${INSTALL_DIR}/assets/images/other/logo.png
Terminal=false
Categories=Game;
EOF

# Copy to Desktop if Desktop folder exists
if [ -d "$HOME/Desktop" ]; then
    cp ~/.local/share/applications/SP1DER-GAMES.desktop ~/Desktop/
    chmod +x ~/Desktop/SP1DER-GAMES.desktop
fi

chmod +x ~/.local/share/applications/SP1DER-GAMES.desktop

echo "Setup complete! You can run the emulator via ./SP1DER-GAMES or from your application menu."