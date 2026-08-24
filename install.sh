#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$HOME/.local/share/SP1DER-GAMES"
BIN_DIR="$HOME/.local/bin"

echo "=== Installing SP1DER GAMES ==="

# --- System & Build Dependencies ---
echo "Checking system dependencies..."
sudo apt update
sudo apt install -y build-essential git cmake pkg-config flatpak \
    libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev \
    xorg-dev libgl1-mesa-dev libglu1-mesa-dev libwayland-dev libxkbcommon-dev

# --- Raylib ---
if ! pkg-config --exists raylib 2>/dev/null; then
    echo "Building and installing Raylib..."
    git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib
    cd /tmp/raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    sudo ldconfig
    rm -rf /tmp/raylib
    cd "$SCRIPT_DIR"
else
    echo "Raylib is already installed."
fi

# --- Flatpak & Emulators ---
echo "Configuring Flatpak repositories..."
sudo flatpak remote-add --if-not-exists flathub https://dl.flathub.org/repo/flathub.flatpakrepo

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
    fi
done

# --- Build Application ---
echo "Building executable..."
cd "$SCRIPT_DIR"
make clean && make

# --- Deploy Files ---
echo "Installing application files to $APP_DIR..."
mkdir -p "$APP_DIR"
mkdir -p "$BIN_DIR"

# Copy binary and project folders
cp -r SP1DER-GAMES assets roms saves "$APP_DIR/" 2>/dev/null || cp -r SP1DER-GAMES assets "$APP_DIR/"

# Symlink to ~/.local/bin
ln -sf "$APP_DIR/SP1DER-GAMES" "$BIN_DIR/SP1DER-GAMES"

# --- Create Desktop Launcher ---
mkdir -p ~/.local/share/applications

cat > ~/.local/share/applications/SP1DER-GAMES.desktop << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=SP1DER GAMES
Comment=Custom retro gaming console frontend
Exec=${APP_DIR}/SP1DER-GAMES
Path=${APP_DIR}
Icon=${APP_DIR}/assets/images/other/logo.png
Terminal=false
Categories=Game;
EOF

chmod +x ~/.local/share/applications/SP1DER-GAMES.desktop

if [ -d "$HOME/Desktop" ]; then
    cp ~/.local/share/applications/SP1DER-GAMES.desktop ~/Desktop/
    chmod +x ~/Desktop/SP1DER-GAMES.desktop
    gio set ~/Desktop/SP1DER-GAMES.desktop metadata::trusted true 2>/dev/null || true
fi

echo "=== Installation Complete! ==="