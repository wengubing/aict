#!/usr/bin/env bash
set -euo pipefail

# Script to configure Ollama to listen on all interfaces via systemd override
# Usage: sudo ./enable_ollama_remote.sh

if ! command -v ollama >/dev/null 2>&1; then
  echo "ERROR: 'ollama' not found in PATH. Install ollama first." >&2
  exit 1
fi

# Check for systemd service presence (non-fatal)
if ! systemctl list-unit-files --type=service | grep -q '^ollama.service'; then
  echo "WARNING: 'ollama.service' not found among systemd unit files. Continuing..." >&2
fi

# Use sudo when not running as root
if [ "$(id -u)" -ne 0 ]; then
  SUDO=sudo
else
  SUDO=
fi

# Create systemd override to set OLLAMA_HOST=0.0.0.0
$SUDO mkdir -p /etc/systemd/system/ollama.service.d
$SUDO bash -c 'cat > /etc/systemd/system/ollama.service.d/override.conf <<'"'EOF'"'
[Service]
Environment="OLLAMA_HOST=0.0.0.0"
EOF'

# Reload systemd and restart service
$SUDO systemctl daemon-reload
$SUDO systemctl restart ollama

# Small delay then report status
sleep 1

echo "--- Listening sockets for port 11434 ---"
ss -tuln | grep 11434 || echo "(no socket found)"

echo
echo "--- Local IP addresses ---"
hostname -I || true

echo
if command -v ufw >/dev/null 2>&1; then
  echo "--- ufw status ---"
  $SUDO ufw status || true
fi

echo
echo "Ollama configured to listen on 0.0.0.0:11434."
echo "Connect from another machine using: http://<HOST_IP>:11434"

echo "If this file is not executable: chmod +x /home/w/enable_ollama_remote.sh"
