# 1. Perangkat Keras dan Pengkabelan

Dokumen ini merinci spesifikasi komponen perangkat keras, pemetaan pin PCB **VANIFLIGHT V1.0**, konvensi pemasangan sensor MPU-6050, serta konfigurasi jalur catu daya untuk ESC dan servo.

---

## Daftar Komponen Utama (*Bill of Materials*)

| Komponen | Spesifikasi / Modul | Keterangan |
| :--- | :--- | :--- |
| **Mikrokontroler** | Waveshare RP2040-Zero (atau RP2350-Zero) | Dual-Core, 2 MB Flash, USB-C, LED indikator RGB WS2812 pada `GP16` |
| **Sensor IMU** | GY-521 (MPU-6050) | Bus I2C0 (`GP4` SDA, `GP5` SCL), alamat `0x68` |
| **Barometer** | GY-BMP280 (opsional, diperlukan untuk `ALT_HOLD`) | Bus I2C0 paralel (`GP4`/`GP5`), alamat `0x76` atau `0x77` |
| **Modul GPS** | RushFPV M10 / u-blox M10 (atau NEO-6M/M8N) | UART1 (`GP8` TX, `GP9` RX), deteksi *baud rate* otomatis hingga 230400 |
| **Beeper** | Buzzer 5V (`GMD9505YB`, tipe aktif atau pasif) | Pin `GP15` (`+`) dan `GND` (`-`) |
| **Modul Telemetri** | ESP32-C3 Super Mini (opsional) | Antarmuka `SerialPIO` (`GP28` RX, `GP29` TX) |
| **Kapasitor Filter** | 2x Elco `470 µF` (16V hingga 25V, *Low-ESR*) | Peredam fluktuasi arus servo pada rel daya 5V |
| **Dioda Isolasi** | 1x Dioda Schottky (`SS14` atau `1N5819`) | Mencegah arus balik dari port USB menuju rel daya servo |

---

## Pemetaan Pin PCB "VANIFLIGHT V1.0"

Definisi pin dalam kode sumber tersimpan pada [`flying_wing_fc/config.h`](../flying_wing_fc/config.h).

| Header PCB | Pin RP2040-Zero | Fungsi | Protokol / Keterangan |
| :--- | :---: | :--- | :--- |
| **`P1` (RECEIVER)** | **`GP1`** (RX), `GP0` (TX) | Masukan *receiver* RC (iBUS, SBUS, CRSF, PPM) | UART0 dengan dukungan inversi sinyal internal RP2040 |
| **`P2` / `S1`** | **`GP14`** | ESC Motor Kiri / Motor Utama | Hardware PWM 50 Hz (Slice 7A) |
| **`P3` / `S2`** | **`GP13`** | ESC Motor Kanan (*twin-motor* / *tailsitter*) | Hardware PWM 50 Hz (Slice 6B) |
| **`P4` / `S3`** | **`GP3`** | Servo Elevon Kiri | Hardware PWM 50 Hz *phase-staggered* (Slice 1B) |
| **`P5` / `S4`** | **`GP2`** | Servo Elevon Kanan | Hardware PWM 50 Hz (Slice 1A) |
| **`S5` hingga `S8`** | `GP6`, `GP7`, `GP11`, `GP10` | Kanal tambahan (motor 3/4, *flap*, *rudder*, *gimbal*) | Keluaran PWM / GPIO ekspansi |
| **`AUX` Pad** | `GP12` | Bantalan solder cadangan | GPIO / PWM serbaguna |
| **`IMU` dan `BARO`** | **`GP4`** (SDA), **`GP5`** (SCL) | Bus I2C0 (MPU-6050, BMP280, dan sensor I2C lain) | Catu daya `3.3V` teregulasi |
| **`CN1` (GPS)** | **`GP8`** (TX), **`GP9`** (RX) | Modul GPS (`TX FC -> RX GPS`, `RX FC <- TX GPS`) | UART1, *ring buffer* 256 byte, catu daya `+5V` |
| **`U5` (ESP32)** | **`GP28`** (RX), **`GP29`** (TX) | Tautan serial ESP32-C3 (`SerialPIO`) | `GP26` (opsional `BOOT`), `GP27` (opsional `RST`) |
| **`BUZZER1`** | **`GP15`** | Indikator suara *non-blocking* | SIO GPIO dengan *drive strength* 12 mA |

---

## Konvensi Orientasi Pemasangan MPU-6050 (`ROTATION_DEFAULT`)

Pada konfigurasi baku *firmware* ([`flying_wing_fc/imu.h`](../flying_wing_fc/imu.h)), modul GY-521 MPU-6050 dipasang sejajar dengan sumbu badan pesawat sebagai berikut:

```text
               DEPAN / HIDUNG PESAWAT
                (Sisi Tanpa LED / VCC)
                  +---------------+
                  | o VCC   [REG] |
  SAYAP KIRI      | o GND         |      SAYAP KANAN
 (Sisi Bantalan   | o SCL  [MPU]  |   (Sisi Tanpa Pin /
  Solder VCC..INT)| o SDA  [6050] |    Label ITG/MPU)
                  | o XDA         |
                  | o XCL         |
                  | o AD0         |
                  | o INT   [LED] |
                  +---------------+
                BELAKANG / EKOR PESAWAT
                 (Sisi Dengan LED / INT)
```

Verifikasi tanda sudut:
- Ketika sisi bantalan solder (`VCC..INT`) terangkat naik, nilai *roll* bernilai positif (`+Roll`, sayap kiri naik).
- Ketika sisi LED (`INT`) turun dan sisi `VCC` naik, nilai *pitch* bernilai positif (`+Pitch`, hidung naik).
- Ketika sisi LED (`INT`) naik dan sisi `VCC` turun, nilai *pitch* bernilai negatif (`-Pitch`, hidung turun).

Untuk integrasi IC MPU-6050 (QFN-24) secara langsung pada papan PCB tanpa modul GY-521, tempatkan penanda Pin 1 pada sudut kiri-depan agar sesuai dengan orientasi baku di atas.

---

## Jalur Catu Daya (BEC dan ESC OPTO)

1. **ESC dengan BEC 5V Internal:**
   Hubungkan konektor 3-pin ESC (`Signal`, `+5V`, `GND`) ke header **`S1`**. Rel `+5V` akan memasok daya ke servo sekaligus ke RP2040-Zero, *receiver*, dan GPS melalui dioda isolasi `D1`.
2. **ESC Tipe OPTO (Tanpa BEC 5V):**
   - Gunakan modul UBEC 5V eksternal (kapasitas arus minimal 3A) atau papan distribusi daya (PDB) ber-BEC 5V untuk memasok rel `+5V` dan `GND` pada FC dan servo.
   - Kabel `GND` pada konektor sinyal ESC OPTO tetap wajib dihubungkan ke `GND` FC sebagai referensi tegangan sinyal PWM.
   - Apabila ESC OPTO tidak memiliki kapasitor elektrolit bawaan dan panjang kabel daya baterai melebihi 20 cm, pasang satu kapasitor *Low-ESR* (`470 µF` hingga `1000 µF`, 35V) pada terminal masukan baterai ESC atau konektor XT60.

---

## Jalur Ekspansi Sensor Tambahan

- **Sensor Kecepatan Udara (*Pitot Tube* I2C seperti MS4525DO/SDP3x) dan Magnetometer (QMC5883L):** Dihubungkan secara paralel pada bus I2C0 (`GP4` SDA, `GP5` SCL, `3.3V`, `GND`) selama alamat I2C masing-masing sensor tidak bertabrakan.
- **Sensor Analog (*Pitot Tube* Analog MPXV7002DP atau Pembagi Tegangan Baterai VBAT):** Menggunakan pin ADC 12-bit yang tersedia pada **`GP26 (ADC0)`** dan **`GP27 (ADC1)`**. Tegangan masukan pada pin ADC tidak boleh melebihi `3.3V`.
