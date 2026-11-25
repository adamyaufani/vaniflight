# 5. Prosedur Pengecekan Pra-Terbang, Mode Terbang, dan Keamanan

> **Peringatan Keselamatan:** Lepaskan baling-baling dari motor selama melakukan pengujian kendali, kalibrasi sensor, *binding* radio, dan verifikasi *arming* di meja kerja.

---

## Prosedur Verifikasi Tiga Tahap Sebelum Penerbangan Perdana

Urutan pemeriksaan berikut wajib dilakukan secara berurutan (Tahap 1 hingga Tahap 3) untuk mencegah umpan balik positif (*positive feedback*) antara giroskop dan *mixer* elevon.

### Tahap 1: Verifikasi Orientasi 3D pada Badan Pesawat
1. Pasang modul FC di dalam badan pesawat sesuai orientasi standar ([Perangkat Keras dan Pengkabelan](Hardware-and-Wiring.md)), kemudian hubungkan ke Web Configurator.
2. Gerakkan pesawat secara fisik dan amati tampilan 3D:
   - Mengangkat hidung pesawat harus menghasilkan nilai `+Pitch` dan model 3D mendongak ke atas.
   - Mengangkat sayap kiri pesawat harus menghasilkan nilai `+Roll` dan sayap kiri model 3D naik.
3. Letakkan pesawat pada sikap terbang jelajah datar, kemudian jalankan **Calibrate Level** (`[c]`).

### Tahap 2: Verifikasi Arah Koreksi Giroskop pada `MODE_ANGLE`
1. Pindahkan sakelar `SwC` ke posisi tengah (`ANGLE`) tanpa menyentuh stik pemancar.
2. Miringkan badan pesawat dengan tangan dan periksa defleksi elevon:
   - **Hidung diturunkan (menukik):** Kedua bilah elevon harus bergerak **naik** untuk menghasilkan momen hidung naik.
   - **Sayap kanan diturunkan (miring kanan):** Elevon kanan harus bergerak **turun** dan elevon kiri bergerak **naik** untuk mengembalikan sayap ke posisi datar.
3. Apabila arah gerak elevon tidak sesuai, gunakan tombol *Reverse Servo* kiri (`[1]`) atau kanan (`[2]`) pada FC (atau tukar konektor `S3` dan `S4`) hingga arah koreksi giroskop melawan kemiringan pesawat pada kedua sumbu.
4. Setelah sesuai, simpan pengaturan ke memori Flash (`[w]`). Setelah tahap ini, arah *reverse* servo pada FC tidak boleh diubah lagi.

### Tahap 3: Verifikasi Arah Stik pada `MODE_MANUAL`
1. Pindahkan sakelar `SwC` ke posisi atas (`MANUAL`).
2. Gerakkan stik pemancar dan amati respons pada Web Configurator serta bidang kendali:
   - **Menarik stik *Pitch* ke belakang:** Kanal `CH2` harus naik menuju `2000 µs` dan kedua elevon bergerak naik.
   - **Menggeser stik *Roll* ke kanan:** Kanal `CH1` harus naik menuju `2000 µs`, elevon kanan naik, dan elevon kiri turun.
3. Apabila arah stik terbalik pada tahap ini, balikkan kanal terkait melalui menu *Outputs* atau *Mixer* pada pemancar, bukan dengan mengubah *reverse* servo pada FC.

---

## Spesifikasi Mode Terbang

| Mode | Posisi Sakelar | Deskripsi Kerja |
| :--- | :--- | :--- |
| **`MANUAL`** | `SwC` Atas | Masukan stik diteruskan langsung ke *mixer* elevon tanpa koreksi PID/giroskop. Disarankan digunakan pada peluncuran perdana pesawat baru hingga mencapai ketinggian aman (25 hingga 30 m). |
| **`ANGLE`** | `SwC` Tengah | Mode stabilisasi sudut (*auto-level*) dengan batas kemiringan `±60° Roll`, `+50° Pitch` naik, dan `-30° Pitch` turun, dilengkapi kompensasi *pitch* saat berbelok (*coordinated turn assist*). |
| **`ALT_HOLD`** | `SwC` Bawah | Mengunci ketinggian relatif terhadap tanah (*AGL*) berdasarkan bacaan barometer BMP280. Stik *roll* mengendalikan arah belok, sedangkan stik *pitch* menggeser target ketinggian (`±4 m/s`). Jika barometer terputus, sistem beralih ke `ANGLE`. |
| **`RTH`** | `SwA` Aktif (atau *failsafe* di udara) | Mengarahkan pesawat kembali ke koordinat *Home* (terkunci saat GPS mencapai minimal 5 satelit *3D fix*) pada ketinggian target `40 m AGL`, kemudian terbang melingkar (*loiter*) dengan radius `20 m`. |

### Mekanisme *AutoTrim* dan *Auto-Level* di Udara (`SwD` / `CH10`)
- **Pada `MODE_ANGLE` dengan `SwD` aktif:** Ketika pesawat diterbangkan lurus dan stabil selama sekitar 2 detik, kompensasi integral PID dipindahkan ke titik tengah servo secara bertahap (*bumpless transfer*).
- **Pada `MODE_MANUAL` dengan `SwD` aktif:** Ketika pesawat terbang datar pada ketinggian konstan (divalidasi barometer), sudut *pitch* jelajah direkam sebagai referensi *level trim* baru.
- Hasil *trim* langsung aktif di RAM dan otomatis disimpan ke EEPROM Flash ketika pesawat mendarat dan masuk status **DISARMED**.

---

## Prosedur Penyalaan di Lapangan dan Indikator Status

1. **Inisialisasi di Lapangan:**
   Permukaan tanah tempat menyambungkan baterai tidak harus rata karena *level trim* tersimpan di EEPROM Flash dan akselerometer membaca vektor gravitasi secara absolut. Pastikan badan pesawat tetap diam selama 2 detik pertama setelah baterai dihubungkan hingga nada *beeper* **Boot Ready** selesai agar kalibrasi bias giroskop berjalan akurat.
2. **Indikator LED RGB (`GP16` pada RP2040-Zero):**
   - **Biru/Cyan berkedip:** Siaga, belum menerima sinyal *receiver* (*failsafe*).
   - **Hijau berkedip:** Sinyal *receiver* aktif dalam status *disarmed*.
   - **Merah menyala tetap:** Status **ARMED** (keluaran motor aktif).
   - **Kuning berkedip cepat:** Proses perekaman *AutoTrim* sedang berlangsung.

---

## Pemeriksaan Keamanan Sebelum Arming (*Pre-Arm Safety Check*)

Sebelum mengizinkan transisi ke status **ARMED**, *firmware* mengevaluasi tujuh kondisi keamanan ([`flying_wing_fc/rc_input.h`](../flying_wing_fc/rc_input.h)). Status hasil pemeriksaan ditampilkan pada Web Configurator (`PREARM[...]`) dan ditandai dengan tiga bunyi *beep* pendek apabila permintaan *arming* ditolak.

| Kode Flag | Kondisi Penolakan *Arming* | Opsi *Bypass* |
| :--- | :--- | :---: |
| **`NO_RC`** | Sinyal *receiver* tidak terdeteksi (*failsafe*) | Tidak dapat di-*bypass* |
| **`THR_HIGH`** | Posisi stik *throttle* belum berada pada batas bawah ($\ge 1080\text{ µs}$) | Tidak dapat di-*bypass* |
| **`BOOT_SW`** | Sakelar *arm* (`SwB`) sudah aktif sejak daya dinyalakan (wajib diturunkan sekali ke posisi *disarm*) | Tidak dapat di-*bypass* |
| **`RTH_SW`** | Sakelar `SwA` (`RTH`) dalam keadaan aktif di darat | Tidak dapat di-*bypass* |
| **`NO_IMU`** | Sensor IMU MPU-6050 tidak terdeteksi | Diizinkan otomatis pada `MODE_MANUAL` |
| **`TILT`** | Kemiringan *roll* atau *pitch* melebihi $35^\circ$ pada mode stabilisasi | Dapat di-*bypass* |
| **`NO_GPS`** | Penerima GPS belum memperoleh *3D fix* atau titik *Home* belum terkunci | Dapat di-*bypass* |

### Prosedur *Bypass* Pemeriksaan `NO_GPS` dan `TILT`
Apabila pengujian dilakukan di dalam ruangan atau penerbangan hendak dilakukan tanpa menunggu kunci GPS:
1. **Melalui Web Configurator atau CLI:** Tekan tombol **Bypass GPS/Tilt** pada tab *Setup* (atau kirim perintah `a` pada CLI), lalu simpan ke Flash dengan perintah `w` apabila ingin bersifat permanen.
2. **Melalui Sakelar Pemancar di Lapangan:** Jika penolakan *arming* hanya disebabkan oleh `NO_GPS` atau `TILT` (sementara seluruh syarat wajib `NO_RC`, `THR_HIGH`, `BOOT_SW`, dan `RTH_SW` telah terpenuhi), gerakkan sakelar *arm* `SwB` (`ON` $\to$ `OFF` $\to$ `ON`) sebanyak dua kali dalam rentang waktu 2,5 detik untuk mengaktifkan *bypass* sementara pada sesi penerbangan tersebut.

Apabila sensor IMU mengalami kegagalan komunikasi saat pesawat sudah mengudara, sistem kendali otomatis beralih ke mode `MANUAL` agar pilot tetap memegang kendali langsung atas bidang elevon.
