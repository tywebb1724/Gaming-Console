#!/bin/bash
set -e

# Dynamically locate the script directory regardless of folder name or launch location
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
APP_DIR="$HOME/.local/share/SP1DER-GAMES"
BIN_DIR="$HOME/.local/bin"

echo "=== Installing SP1DER GAMES from $SCRIPT_DIR ==="

# --- System & Build Dependencies ---
echo "Checking system dependencies..."
sudo apt update
sudo apt install -y build-essential git cmake pkg-config flatpak \
    libasound2-dev mesa-common-dev libx11-dev libxrandr-dev libxi-dev \
    xorg-dev libgl1-mesa-dev libglu1-mesa-dev libwayland-dev libxkbcommon-dev \
    libsdl2-dev libsdl2-image-dev

# --- Raylib ---
if ! pkg-config --exists raylib 2>/dev/null; then
    echo "Building and installing Raylib..."
    git config --global http.postBuffer 524288000 2>/dev/null || true

    # Retry clone up to 3 times in case of network drops
    for i in {1..3}; do
        rm -rf /tmp/raylib
        echo "Attempt $i to clone Raylib..."
        if git clone --depth 1 https://github.com/raysan5/raylib.git /tmp/raylib; then
            break
        fi
        sleep 2
    done

    if [ ! -d "/tmp/raylib/src" ]; then
        echo "ERROR: Failed to clone Raylib repository."
        exit 1
    fi

    cd /tmp/raylib/src
    make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=SHARED
    sudo make install RAYLIB_LIBTYPE=SHARED
    sudo ldconfig
    rm -rf /tmp/raylib
    cd "$SCRIPT_DIR"
else
    echo "Raylib is already installed."
fi

sudo ldconfig

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
        flatpak install -y flathub "$app" || true
    fi
done

# --- Build Application ---
echo "Building executable..."
cd "$SCRIPT_DIR"
make clean || true
make

# --- Deploy Files ---
echo "Installing application files to $APP_DIR..."
mkdir -p "$APP_DIR"
mkdir -p "$BIN_DIR"
mkdir -p "$APP_DIR/roms"
mkdir -p "$APP_DIR/saves"

# Copy binary and asset directories
cp -f SP1DER-GAMES "$APP_DIR/"
[ -f "sdl_name_probe" ] && cp -f sdl_name_probe "$APP_DIR/" || true
[ -d "$SCRIPT_DIR/assets" ] && cp -r "$SCRIPT_DIR/assets" "$APP_DIR/"

# Copy initial ROMs and saves if present in installer
[ -d "$SCRIPT_DIR/roms" ] && cp -r "$SCRIPT_DIR/roms/"* "$APP_DIR/roms/" 2>/dev/null || true
[ -d "$SCRIPT_DIR/saves" ] && cp -r "$SCRIPT_DIR/saves/"* "$APP_DIR/saves/" 2>/dev/null || true

# Symlink executable to ~/.local/bin
ln -sf "$APP_DIR/SP1DER-GAMES" "$BIN_DIR/SP1DER-GAMES"

# --- Create Portable Desktop Launcher ---
mkdir -p "$HOME/.local/share/applications"

cat > "$HOME/.local/share/applications/SP1DER-GAMES.desktop" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=SP1DER GAMES
Comment=Custom retro gaming console frontend
Exec=sh -c 'cd "$APP_DIR" && MESA_GL_VERSION_OVERRIDE=2.1 ./SP1DER-GAMES'
Path=$APP_DIR
Icon=$APP_DIR/assets/images/other/logo.png
Terminal=false
Categories=Game;
EOF

chmod +x "$HOME/.local/share/applications/SP1DER-GAMES.desktop"

# Copy launcher to Desktop if ~/Desktop exists
if [ -d "$HOME/Desktop" ]; then
    cp "$HOME/.local/share/applications/SP1DER-GAMES.desktop" "$HOME/Desktop/"
    chmod +x "$HOME/Desktop/SP1DER-GAMES.desktop"
    gio set "$HOME/Desktop/SP1DER-GAMES.desktop" metadata::trusted true 2>/dev/null || true
fi

echo "=== Installation Complete! ==="