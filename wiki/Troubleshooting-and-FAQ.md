# 6. Pemecahan Masalah dan Pertanyaan Umum

Halaman ini merangkum penyebab teknis dan langkah perbaikan untuk kendala yang paling sering dijumpai selama perakitan dan pengujian **VaniFlight**.

---

### 1. Pesawat terbang normal pada `MANUAL`, tetapi berguling atau berbalik tak terkendali pada mode `ANGLE` atau `ACRO`
- **Penyebab:** Arah koreksi giroskop terbalik terhadap *mixer* elevon (*positive feedback*), orientasi fisik pemasangan sensor MPU-6050 tidak sesuai dengan konfigurasi *firmware*, atau nilai *gain* PID (`PID_ROLL_KP` dan `PID_PITCH_KP`) terlalu tinggi untuk pesawat berukuran kecil dengan inersia rendah.
- **Langkah Perbaikan:**
  1. Lakukan ulang **Prosedur Verifikasi Tiga Tahap** pada halaman [Pengecekan Pra-Terbang dan Keamanan](Pre-Flight-and-Safety-Checks.md). Pastikan arah koreksi giroskop pada `MODE_ANGLE` telah melawan kemiringan pesawat sebelum menyesuaikan arah stik di pemancar.
  2. Untuk pesawat dengan bentang sayap kecil (`< 70 cm`), turunkan nilai `PID_ROLL_KP` dan `PID_PITCH_KP` pada [`flying_wing_fc/config.h`](../flying_wing_fc/config.h) ke kisaran `0.5` hingga `0.6` untuk pengujian awal.
  3. Lakukan peluncuran perdana pada `MODE_MANUAL` hingga ketinggian aman (25 hingga 30 m) sebelum menguji mode stabilisasi.

---

### 2. Status telah `ARMED`, tetapi motor tidak berputar
- **Pemeriksaan 1 (Urutan Kanal Pemancar):** Amati tab *Receiver (RC)* di Web Configurator saat stik gas didorong naik. Pastikan kanal yang meningkat adalah `CH3` (bukan `CH1` atau `CH2`). Apabila berbeda, sesuaikan urutan `AETR` pada menu *Mixer* pemancar.
- **Pemeriksaan 2 (Daya Baterai dan Inisialisasi ESC):** Catu daya USB komputer tidak dapat menggerakkan motor *brushless*. Pastikan baterai LiPo terpasang dan FC berada dalam kondisi `DISARMED` (`1000 µs`) selama 2 hingga 3 detik saat penyalaan hingga ESC menyelesaikan nada inisialisasi.
- **Pemeriksaan 3 (Pengkabelan ESC OPTO):** Pastikan kabel `GND` pada konektor sinyal ESC OPTO terhubung ke `GND` FC dan konektor sinyal terpasang pada port `S1` (`GP14`).

---

### 3. Permintaan *Arming* ditolak dan *beeper* berbunyi tiga kali pendek
- **Penyebab:** Salah satu kondisi pada *Pre-Arm Safety Check* belum terpenuhi.
- **Langkah Perbaikan:** Periksa indikator **Pre-Arm Safety Check** pada tab *Setup* Web Configurator atau keluaran `PREARM[...]` pada CLI:
  - `THR_HIGH`: Turunkan stik *throttle* ke posisi paling bawah (`< 1080 µs`).
  - `BOOT_SW`: Turunkan sakelar `SwB` ke posisi *disarm* satu kali sebelum mengaktifkannya kembali.
  - `RTH_SW`: Matikan sakelar `SwA` (`RTH`).
  - `NO_GPS` atau `TILT`: Tunggu penguncian GPS 3D di luar ruangan, atau aktifkan *bypass* melalui tombol **Bypass GPS/Tilt** (CLI `a`), atau lakukan perpindahan sakelar `SwB` (`ON` $\to$ `OFF` $\to$ `ON`) dua kali dalam 2,5 detik.

---

### 4. LED *receiver* menyala tetap, tetapi Configurator menampilkan `NO RC SIGNAL`
- **Pada FlySky FS-iA6B:** Pastikan kabel sinyal terpasang pada **header horizontal kanan atas berlabel `SERVO` (`i-BUS`)**, bukan pada header `SENS` dan bukan pada deretan pin `CH1` hingga `CH6`.
- **Pada FlySky FS-iA6C:** Pastikan kabel menuju pin `GP1` tersambung pada pin ke-4 (`i-BUS / S.BUS`), bukan pin ke-1 (`PPM`).
- Setelah sinyal terkunci oleh mekanisme *auto-detect* `SBUS` $\leftrightarrow$ `iBUS`, jalankan perintah `w` (atau klik **SAVE**) agar protokol tersimpan di memori Flash.

---

### 5. Skrip `flash_pico.sh` gagal masuk mode BOOTSEL
- **Penyebab:** Port serial `/dev/cu.usbmodem*` masih digunakan oleh Web Configurator di peramban atau oleh Serial Monitor.
- **Langkah Perbaikan:** Klik tombol **Disconnect** pada Web Configurator dan tutup seluruh jendela Serial Monitor sebelum menjalankan `./flying_wing_fc/flash_pico.sh`.

---

### 6. Sudut *Yaw* pada tampilan 3D bergeser perlahan saat pesawat diam di meja
- **Penyebab:** Sensor MPU-6050 merupakan IMU 6-DOF tanpa magnetometer, sehingga sudut *yaw* diperoleh melalui integrasi laju sudut giroskop yang secara alami memiliki *drift* kecil terhadap waktu.
- **Langkah Perbaikan:** Gunakan tombol **Reset Z axis** atau **Reset View** pada Web Configurator untuk menolkan arah hadap *yaw* visual ke `0.0°`.
