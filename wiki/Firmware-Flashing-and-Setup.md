# 2. Kompilasi dan Pemrograman Firmware

Dokumen ini menjelaskan persiapan lingkungan kompilasi, prosedur pengunggahan *firmware* ke **Waveshare RP2040-Zero**, serta mekanisme pemrograman modul **ESP32-C3** melalui jembatan serial RP2040.

---

## Persiapan Lingkungan Pengembangan

1. Pasang **Arduino IDE 2.x** atau **`arduino-cli`**.
2. Tambahkan URL *Board Manager* **Earle Philhower Arduino-Pico**:
   ```text
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
3. Pasang paket papan **`Raspberry Pi Pico/RP2040/RP2350`**.
4. Pasang pustaka **`Adafruit NeoPixel`** untuk pengendalian LED indikator RGB pada pin `GP16`. Modul kendali inti (IMU, filter Madgwick, PID, PWM, GPS, barometer, dan dekoder RC) tidak membutuhkan pustaka pihak ketiga tambahan.

---

## Pengunggahan Firmware ke RP2040-Zero

### Metode 1: Skrip Otomatis melalui Terminal (macOS / Linux)
> **Catatan:** Putuskan koneksi serial (*Disconnect*) pada Web Configurator atau tutup Serial Monitor terlebih dahulu agar port USB tidak terkunci saat proses pengunggahan.

Jalankan perintah berikut dari direktori utama repositori:
```bash
./flying_wing_fc/flash_pico.sh
```
Tahapan yang dijalankan secara otomatis oleh skrip:
1. Mengompilasi berkas `flying_wing_fc.ino` untuk target `rp2040:rp2040:waveshare_rp2040_zero`.
2. Mengirim sinyal *baud rate* `1200` bps ke port serial untuk memicu perpindahan ke mode **BOOTSEL** secara perangkat lunak.
3. Mengunggah dan memverifikasi berkas biner melalui `picotool`, kemudian memulai ulang perangkat.

### Metode 2: Melalui Arduino IDE
1. Buka berkas [`flying_wing_fc/flying_wing_fc.ino`](../flying_wing_fc/flying_wing_fc.ino).
2. Pilih menu **Tools $\rightarrow$ Board $\rightarrow$ Waveshare RP2040 Zero**.
3. Pilih port serial USB yang terdeteksi (atau tahan tombol fisik `BOOT` saat menyambungkan kabel USB apabila perangkat belum memiliki *firmware*).
4. Klik **Upload**.

---

## Pemrograman Modul ESP32-C3 melalui Jembatan Serial RP2040

Modul **ESP32-C3 Super Mini** yang terhubung pada jalur `GP28` (RX) dan `GP29` (TX) dapat diprogram melalui port USB RP2040-Zero menggunakan fitur *SerialPIO passthrough* tanpa memerlukan kabel USB kedua.

### Prosedur Pengunggahan
1. Sambungkan kabel USB-C ke RP2040-Zero dalam kondisi pesawat **DISARMED**.
2. Aktifkan mode jembatan dengan salah satu cara berikut:
   - Memilih tombol **ESP32 Flash Bridge** pada Web Configurator;
   - Mengirim karakter `e` melalui terminal CLI; atau
   - Menjalankan skrip berikut secara langsung:
     ```bash
     ./flying_wing_fc/flash_esp32_via_pico.sh <direktori_sketch_esp32_atau_berkas.bin>
     ```
3. Apabila pin opsional `GP26` (`BOOT`) dan `GP27` (`RST`) tersambung ke `GPIO9` dan `RST/EN` pada ESP32-C3, sekuens masuk ke *bootloader* dijalankan secara otomatis mengikuti sinyal DTR/RTS dari `esptool`. Jika kedua pin tersebut tidak disambungkan, tahan tombol `BOOT` pada ESP32-C3, tekan `RST` sesaat, lalu lepaskan `BOOT` sebelum proses unggah dimulai.
4. Setelah pengunggahan selesai, lepaskan dan pasang kembali kabel USB (atau tekan tombol Reset pada RP2040) untuk kembali ke operasi normal *flight controller*.
