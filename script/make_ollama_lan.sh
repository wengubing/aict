#!/usr/bin/env bash
set -euo pipefail

# Script to make Ollama serve on LAN and persist across reboots.
# Usage: sudo bash /home/w/make_ollama_lan.sh  (sudo only needed for firewall changes)

USER_NAME="$(whoami)"
OLLAMA_BIN="$(command -v ollama || true)"
if [ -z "$OLLAMA_BIN" ]; then
echo "Error: ollama binary not found in PATH. Install ollama or adjust PATH." >&2
exit 1
fi

SERVICE_DIR="$HOME/.config/systemd/user"
SERVICE_FILE="$SERVICE_DIR/ollama.service"
mkdir -p "$SERVICE_DIR"

cat > "$SERVICE_FILE" <<'EOF'
[Unit]
Description=Ollama Service (user)
After=network-online.target

[Service]
ExecStart=%h/.local/bin/ollama serve
Restart=always
RestartSec=3
Environment=OLLAMA_HOST=0.0.0.0:11434
Environment=PATH=%h/.local/bin:%h/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

[Install]
WantedBy=default.target
EOF

# Reload and start the user service
systemctl --user daemon-reload
systemctl --user enable --now ollama

# Enable linger so user services start even without interactive login
loginctl enable-linger "$USER_NAME"

# Open firewall port if common firewalls are present
if command -v ufw >/dev/null 2>&1; then
  if ufw status | grep -qi inactive; then
    echo "ufw inactive or not configured; skipping ufw rules"
  else
    echo "Allowing port 11434/tcp in ufw (may ask for sudo)"
    sudo ufw allow 11434/tcp || true
  fi
fi

if command -v firewall-cmd >/dev/null 2>&1; then
  echo "Adding port 11434/tcp to firewalld (may ask for sudo)"
  sudo firewall-cmd --add-port=11434/tcp --permanent || true
  sudo firewall-cmd --reload || true
fi

# Show listening sockets and a quick API check
ss -ltnp '( sport = :11434 )' || true
curl -sS --max-time 5 http://127.0.0.1:11434/api/tags || true

IP="$(hostname -I 2>/dev/null | awk '{print $1}')"
if [ -z "$IP" ]; then IP="<your-host-ip>"; fi

echo "\nDone. Test from another machine:\n  curl http://$IP:11434/api/tags\n"
