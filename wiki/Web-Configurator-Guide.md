# 3. Panduan Web Configurator dan CLI

**VaniFlight Web Configurator** ([`configurator/index.html`](../configurator/index.html)) adalah antarmuka pemantauan dan konfigurasi berbasis web yang beroperasi melalui **Web Serial API** pada peramban **Google Chrome**, **Microsoft Edge**, atau **Brave** tanpa memerlukan instalasi perangkat lunak desktop.

---

## Menjalankan Server Lokal Web Configurator

Jalankan skrip peluncur dari direktori utama repositori:
```bash
./open_configurator.sh
```
Alternatifnya, jalankan server HTTP lokal secara manual:
```bash
cd configurator && python3 -m http.server 8080
```
Buka alamat `http://localhost:8080` pada peramban yang mendukung Web Serial API.

---

## Panel Koneksi Port Serial

Panel koneksi terletak pada bagian kanan atas bilah navigasi dengan komponen berikut:

- **Pemilih Port (`#portSelect`) dan *Baud Rate* (`115200`):**
  Pada koneksi pertama, klik tombol **Connect** dan pilih perangkat serial RP2040-Zero pada dialog izin peramban. Setelah izin diberikan satu kali, perangkat tersebut akan muncul secara otomatis pada daftar *dropdown* (misalnya `usbmodem (RP2040-Zero 2E8A:000A)`) sehingga koneksi berikutnya tidak lagi menampilkan dialog pemilihan port.
- **Opsi *Auto-Connect* dan Deteksi *Hot-Plug*:**
  Apabila kotak centang **Auto-Connect** diaktifkan, antarmuka akan terhubung secara otomatis saat halaman dibuka atau ketika kabel USB perangkat disambungkan kembali.
- **Tombol Koneksi USB:**
  - **Hijau (`Connect`):** Membuka sesi komunikasi serial dengan port terpilih.
  - **Merah (`Disconnect`):** Menutup sesi serial dan melepaskan port agar dapat digunakan oleh utilitas pemrograman *firmware*.

---

## Struktur Lima Tab Antarmuka

### 1. Tab *Setup & 3D*
- **Visualisasi Sikap 3D (*Chase View*):** Model 3D menampilkan hidung pesawat menghadap ke depan (menjauhi layar), sehingga sisi kiri dan kanan sesuai dengan orientasi fisik pesawat. Sudut kamera dapat diputar dengan menyeret tetikus (*drag*) dan dikembalikan ke posisi awal melalui klik ganda.
- **Calibrate Level:** Mengkalibrasi bias giroskop dan menetapkan sikap datar saat ini sebagai referensi `0° Roll` dan `0° Pitch`.
- **Reset Z axis:** Menolkan *offset* tampilan sudut *yaw* (`0.0°`) relatif terhadap arah hadap saat ini.
- **Reset View:** Mengembalikan orientasi kamera 3D serta *offset* tampilan ke posisi awal.
- **Status Sensor dan *Pre-Arm Check*:** Menampilkan status koneksi MPU-6050, BMP280, GPS M10, mode terbang aktif, serta hasil pemeriksaan *pre-arm* secara *real-time*.
- **Perintah Cepat Sistem:** Tombol akses cepat untuk *Bypass GPS/Tilt* (`[a]`), uji *beeper* (`[v]`), mode DFU/BOOTSEL (`[b]`), jembatan *flash* ESP32 (`[e]`), dan pembaruan status (`[h]`).

### 2. Tab *Receiver (RC)*
- Menampilkan nilai pulsa kanal utama (`CH1` hingga `CH3`) serta sakelar tambahan (`SwA` hingga `SwD`) dalam rentang `1000` hingga `2000 µs`.
- Menyediakan indikator status sinyal RC dan tombol penggantian protokol *receiver* (`[i]`).

### 3. Tab *Servos & Motor*
- Menampilkan nilai pulsa keluaran aktual untuk servo elevon kiri (`S3`), servo elevon kanan (`S4`), dan ESC motor (`S1`).
- Menyediakan tombol pembalikan arah (*reverse*) servo kiri (`[1]`) dan servo kanan (`[2]`) secara langsung.

### 4. Tab *GPS & Nav*
- Menampilkan jumlah satelit, status *fix* (`2D` atau `3D LOCKED`), koordinat lintang dan bujur, ketinggian MSL, kecepatan darat (*km/h*), jarak terhadap titik *Home*, serta peta navigasi berbasis Leaflet/OpenStreetMap beserta lintasan gerak pesawat.

### 5. Tab *CLI Terminal*
- Menyediakan konsol teks langsung pada `115200` baud untuk mengirim perintah satu huruf dan mengamati keluaran diagnostik dari mikrokontroler.

---

## Daftar Perintah Antarmuka Baris Perintah (CLI)

Seluruh perintah berikut dikirim melalui USB Serial (`115200` baud) diikuti penekanan tombol **Enter**:

| Perintah | Deskripsi Fungsi | Syarat Status |
| :---: | :--- | :---: |
| **`r`** | Mengaktifkan streaming telemetri IMU (`R/P/Y`), *receiver*, servo, dan *pre-arm* (`10 Hz`) | Bebas |
| **`p`** | Mengaktifkan format keluaran tabulasi untuk Arduino Serial Plotter | Bebas |
| **`s`** | Mengaktifkan format tabel status ringkas | Bebas |
| **` ` (Spasi)** | Menjeda (*pause*) atau melanjutkan (*resume*) streaming telemetri | Bebas |
| **`g`** | Menampilkan diagnostik barometer BMP280 dan penerima GPS M10 | Bebas |
| **`t`** | Menampilkan nilai *servo trim* (`µs`) dan *level trim* (`°`) tersimpan | Bebas |
| **`v`** | Menguji keluaran *beeper* (`GP15`) dan mengganti mode driver (`Active` / `Hybrid` / `Passive`) | Bebas |
| **`h`** | Menampilkan informasi versi *firmware*, status sensor, dan ringkasan menu | Bebas |
| **`c`** | Menjalankan kalibrasi giroskop dan titik datar (*ground level calibration*) | Wajib DISARMED |
| **`z`** | Mengembalikan seluruh nilai *trim* servo dan *level trim* ke `0` | Wajib DISARMED |
| **`1` / `2`** | Membalik arah gerak (*reverse*) servo kiri / servo kanan | Wajib DISARMED |
| **`a`** | Mengubah status *Pre-Arm Bypass* untuk pengecekan GPS 3D dan kemiringan $> 35^\circ$ | Wajib DISARMED |
| **`i`** | Mengganti protokol *receiver* (`SBUS` $\to$ `iBUS` $\to$ `PPM` $\to$ `CRSF`) | Wajib DISARMED |
| **`m`** | Mengganti mode terbang melalui CLI untuk pengujian di meja | Wajib DISARMED |
| **`w`** | Menyimpan seluruh parameter konfigurasi ke memori Flash EEPROM | Wajib DISARMED |
| **`x`** | Memulai ulang mikrokontroler melalui perangkat lunak (*software reboot*) | Wajib DISARMED |
| **`b`** | Memasuki mode *bootloader* BOOTSEL/DFU melalui perangkat lunak | Wajib DISARMED |
| **`e`** | Mengaktifkan mode jembatan *USB-to-UART* (`GP28`/`GP29`) untuk memprogram ESP32-C3 | Wajib DISARMED |
