#!/bin/bash
# Locate script directory dynamically
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "=== SP1DER GAMES Installer Wrapper ==="

# Clean Windows line endings (\r) if present and grant execution
sed -i 's/\r$//' ./install.sh 2>/dev/null || true
chmod +x ./install.sh

# Execute install.sh
if ./install.sh; then
    echo ""
    echo "=== Installation Successful! ==="
else
    echo ""
    echo "=== ERROR: Installation Failed ==="
fi

echo ""
read -p "Press Enter to exit..."