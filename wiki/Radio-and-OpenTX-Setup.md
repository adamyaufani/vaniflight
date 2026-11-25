# 4. Pengaturan Radio, Receiver, dan OpenTX / OpenI6X

VaniFlight mendukung masukan *receiver* serial (`iBUS`, `SBUS`, `CRSF`) dan `PPM` pada port **`P1 (RECEIVER)`** yang terhubung ke pin **`GP1`**. Pada saat sinyal RC belum terkunci dalam kondisi *disarmed*, *firmware* secara otomatis memindai protokol `SBUS` dan `iBUS` secara bergantian.

---

## Penyambungan Receiver FlySky

### A. FlySky FS-iA6B
Pada modul **FS-iA6B**, keluaran serial digital `i-BUS` terletak pada **header horizontal sebelah kanan atas berlabel `SERVO`**, bukan pada deretan pin vertikal `CH1` hingga `CH6` dan bukan pada header kiri `SENS`.

```text
   +-----------------------------------------+
   |  [ SENS ]         [ SERVO (i-BUS) ] <--+|--- Hubungkan ke P1 RECEIVER (GP1)
   |   (Kiri)              (Kanan)           |
   +-----------------------------------------+
   |  CH1/PPM  CH2  CH3  CH4  CH5  CH6  B/VCC|
   +-----------------------------------------+
```

Urutan pin pada header horizontal `SERVO` dari sisi bawah (dalam) ke atas (tepi casing) adalah: **`S` (Signal)**, **`+5V`**, dan **`GND`**.

### B. FlySky FS-iA6C
Urutan pin pada konektor 4-pin **FS-iA6C**:
1. `PPM` (tidak digunakan apabila memakai `iBUS` atau `SBUS`)
2. `GND` dihubungkan ke `GND` port `P1`
3. `VCC (+5V)` dihubungkan ke `+5V` port `P1`
4. `i-BUS / S.BUS` dihubungkan ke pin `Signal` (`GP1`) port `P1`

---

## Pemetaan Kanal Standar (`AETR` dan Empat Sakelar)

Definisi pemetaan kanal pada kode sumber terdapat pada [`flying_wing_fc/rc_input.h`](../flying_wing_fc/rc_input.h):

| Kanal | Masukan Pemancar | Fungsi pada VaniFlight | Rentang Nilai Normal (`µs`) |
| :---: | :--- | :--- | :--- |
| **`CH1`** | Stik *Roll* (`Ail`) | Kendali *Roll* (*Aileron*) | Kiri `1000`, tengah `1500`, kanan `2000` |
| **`CH2`** | Stik *Pitch* (`Ele`) | Kendali *Pitch* (*Elevator*) | Dorong maju `1000`, tengah `1500`, tarik belakang `2000` |
| **`CH3`** | Stik *Throttle* (`Thr`) | Daya Motor | Minimum (bawah) `1000`, maksimum (atas) `2000` |
| **`CH4`** | Stik *Yaw* (`Rud`) | Kendali *Yaw* / *Differential Thrust* | Kiri `1000`, tengah `1500`, kanan `2000` |
| **`CH7`** | Sakelar **`SwA`** (2 posisi) | *Return-to-Home* (`MODE_RTH`) | `< 1700` nonaktif, `> 1700` aktif |
| **`CH8`** | Sakelar **`SwB`** (2 posisi) | *Arm / Disarm* Motor | `< 1500` *disarmed*, `> 1600` *armed* |
| **`CH9`** | Sakelar **`SwC`** (3 posisi) | Pemilihan Mode Terbang | `< 1300` `MANUAL`, `1300` hingga `1700` `ANGLE`, `> 1700` `ALT_HOLD` |
| **`CH10`** | Sakelar **`SwD`** (2/3 posisi) | *AutoTrim* dan *Auto-Level* | `> 1650` aktif merekam kompensasi *trim* |

---

## Konfigurasi Khusus Pemancar OpenI6X (FlySky FS-i6X / OpenTX)

Pada profil model baru di OpenI6X atau OpenTX, secara baku hanya `CH1` hingga `CH4` yang dipetakan pada menu *Mixer*. Selain itu, urutan kanal bawaan dapat berbeda dari `AETR` dan arah `CH2` (*Pitch*) dapat terbalik.

### Navigasi Tombol pada OpenI6X
- **Tahan tombol `OK` (~1 detik)** pada layar utama untuk membuka menu model (`1/11 MODELSEL`).
- **Tekan tombol `BIND`** (kiri bawah) secara singkat untuk berpindah ke halaman berikutnya.
- **Tahan tombol `BIND`** untuk kembali ke halaman sebelumnya.
- **Tekan tombol `CANCEL`** untuk keluar dari sub-menu.

### Langkah Pengaturan pada OpenI6X
1. **Halaman `2/11 SETUP` (*Internal RF*):**
   - Atur **Mode** ke `AFHDS 2A`.
   - Atur **SubType** ke `PWM, iBUS` (atau `PWM, SBUS`).
   - Pastikan **Channel Range** diatur ke **`CH1-10`** atau **`CH1-14`** agar `CH9` dan `CH10` turut ditransmisikan.
2. **Halaman `5/11 MIXER`:**
   - Pastikan urutan empat kanal pertama adalah `CH1 = Ail`, `CH2 = Ele`, `CH3 = Thr`, dan `CH4 = Rud`.
   - Arahkan kursor ke **`CH7`**, tahan `OK`, pilih **`Source`**, tekan `OK`, lalu gerakkan sakelar fisik **`SwA`** hingga terisi `SA`.
   - Ulangi prosedur yang sama untuk **`CH8 = SB`**, **`CH9 = SC`**, dan **`CH10 = SD`**.
3. **Penyesuaian Arah Kanal Stik:**
   Apabila arah pergerakan `CH1` atau `CH2` di Web Configurator terbalik terhadap tabel di atas, lakukan pembalikan pada halaman **`OUTPUTS` (`SERVOS`)** atau ubah nilai `Weight` menjadi `-100` pada halaman **`MIXER`** di pemancar. Pembalikan arah servo pada *flight controller* hanya dilakukan untuk menyelaraskan arah koreksi giroskop.
