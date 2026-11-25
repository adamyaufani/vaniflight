#!/bin/bash
# ==============================================================================
# VANIFLIGHT - FLASH ESP32-C3 VIA RP2040-ZERO USB PASSTHROUGH (GP28 / GP29)
# ==============================================================================
# Cara pakai:
#   ./flying_wing_fc/flash_esp32_via_pico.sh <path_sketch_esp32_atau_file.bin>
#
# Fitur:
# 1. Otomatis mengirim perintah 'e' ke RP2040-Zero via USB (/dev/cu.usbmodem*)
#    agar RP2040 masuk ke mode jembatan SerialPIO (GP29 TX <-> GP28 RX).
# 2. RP2040 otomatis memicu sekuens BOOT (GP26) & RST (GP27) jika tersambung,
#    atau Anda cukup tahan tombol BOOT + klik RST di modul ESP32-C3 Super Mini.
# 3. Melakukan upload firmware ESP32-C3 lewat kabel USB RP2040-Zero yang sama!
# ==============================================================================
set -e

TARGET_ARG="${1:-}"
ARDUINO_CLI="/Applications/Arduino IDE.app/Contents/Resources/app/lib/backend/resources/arduino-cli"
SERIAL_PORT=$(ls /dev/cu.usbmodem* 2>/dev/null | head -n 1 || true)

if [ -z "$SERIAL_PORT" ]; then
  echo "[ERROR] Port USB RP2040-Zero (/dev/cu.usbmodem*) tidak ditemukan!"
  echo "        Pastikan kabel USB-C terhubung ke RP2040-Zero."
  exit 1
fi

echo "============================================================"
echo "  VANIFLIGHT - ESP32-C3 FLASH VIA RP2040 USB PASSTHROUGH"
echo "============================================================"
echo "  Port RP2040 : $SERIAL_PORT"
echo "  Jalur UART  : GP29 (TX) -> RX ESP32 | GP28 (RX) <- TX ESP32"
echo "============================================================"

echo "[1/2] Mengaktifkan Mode ESP32 Passthrough Bridge ('e') di RP2040..."
python3 -c "
import serial, time
s = serial.Serial('$SERIAL_PORT', 115200, timeout=0.5)
s.write(b'e\n')
time.sleep(0.3)
out = s.read(512).decode('utf-8', errors='ignore')
if out:
    print(out.strip())
s.close()
"

echo ""
echo "[2/2] Siap melakukan flashing ke ESP32-C3 lewat $SERIAL_PORT!"
echo "      (Jika GP26/GP27 belum disolder ke BOOT/RST ESP32-C3,"
echo "       tahan tombol BOOT di ESP32-C3, klik RST, lalu lepas BOOT sekarang)"

if [ -z "$TARGET_ARG" ]; then
  echo ""
  echo "[OK] RP2040 sekarang berada dalam Mode Jembatan USB-to-UART (GP28/GP29)!"
  echo "     Anda dapat langsung menjalankan perintah esptool.py atau Arduino IDE"
  echo "     dengan memilih port: $SERIAL_PORT"
  exit 0
fi

if [[ "$TARGET_ARG" == *.bin ]]; then
  echo "      Meng-upload binary $TARGET_ARG via esptool..."
  python3 -m esptool --chip esp32c3 --port "$SERIAL_PORT" --baud 115200 write_flash 0x10000 "$TARGET_ARG"
else
  echo "      Mengompilasi & meng-upload sketch $TARGET_ARG via arduino-cli..."
  "$ARDUINO_CLI" compile --fqbn esp32:esp32:nologo_esp32c3_super_mini "$TARGET_ARG"
  "$ARDUINO_CLI" upload -p "$SERIAL_PORT" --fqbn esp32:esp32:nologo_esp32c3_super_mini "$TARGET_ARG"
fi

echo ""
echo "============================================================"
echo "  SUKSES! Firmware ESP32-C3 berhasil di-flash via RP2040!"
echo "  (Cabut-colok USB atau tekan Reset di RP2040 untuk kembali ke FC)"
echo "============================================================"
