#!/bin/bash
# ==============================================================================
# Script Peluncur VaniFlight Web Configurator
# ==============================================================================

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PORT=8080

echo "=========================================================="
echo "   ▲ VANIFLIGHT WEB CONFIGURATOR (Ala iNav / Betaflight)"
echo "=========================================================="
echo "Menjalankan server lokal di port $PORT..."

# Bunuh proses sebelumnya di port 8080 jika ada
lsof -ti:$PORT | xargs kill -9 2>/dev/null

# Jalankan server HTTP lokal di latar belakang
cd "$DIR/configurator"
python3 -m http.server $PORT > /dev/null 2>&1 &
SERVER_PID=$!

sleep 1

URL="http://localhost:$PORT"
echo "Membuka Configurator di browser: $URL"

# Coba buka di Google Chrome jika terpasang di Mac, atau browser default
if [ -d "/Applications/Google Chrome.app" ]; then
    open -a "Google Chrome" "$URL"
elif [ -d "/Applications/Brave Browser.app" ]; then
    open -a "Brave Browser" "$URL"
elif [ -d "/Applications/Microsoft Edge.app" ]; then
    open -a "Microsoft Edge" "$URL"
else
    open "$URL"
fi

echo "Tekan Ctrl+C untuk menghentikan server jika sudah selesai."
wait $SERVER_PID
