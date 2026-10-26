/*
 * ============================================================================
 * @file        config.h
 * @project     Flying Wing Autonomous Flight Controller
 * @version     v0.4.3
 * @since       v0.1.0 (2026-10-18)
 * @updated     2026-10-25
 * @brief       Konfigurasi Pinout Multi-Board, Sensor, Filter IMU, PID, & Trim
 *
 * Riwayat Perubahan (Changelog):
 *   - v0.1.0 (2026-10-18): Konfigurasi awal multi-board (Pico, STM32F4, ESP32, Teensy).
 *   - v0.2.0 (2026-10-19): Pinout Waveshare RP2040-Zero (GP6/GP7 Servo, GP14 ESC, GP1 SBUS).
 *   - v0.3.0 (2026-10-24): Tambah makro BOARD_ROTATION untuk orientasi pemasangan IMU.
 *   - v0.4.0 (2026-10-24): Aktifkan HAS_BAROMETER di RP2040 & batas Auto-Level Trim.
 *   - v0.4.1 (2026-10-24): Tambah makro versi firmware FW_VERSION_STR & tanggal rilis.
 *   - v0.4.2 (2026-10-25): Default GPS_BAUD_RATE 230400 baud untuk biner u-blox UBX-NAV-PVT M10.
 *   - v0.4.3 (2026-10-25): Tambah konfigurasi PIN_BEEPER pada GP15 (RP2040-Zero).
 *   - v0.5.0 (2026-10-26): Migrasi pinout resmi "VANIFLIGHT V 1.0" Custom PCB (RP2040-Zero
 *                          + ESP32-C3), dukungan Twin-Motor Dual ESC (S1/S2) & Differential
 *                          Thrust untuk Tailsitter/Twin Motor, elevon pada S3 (GP3) & S4 (GP2),
 *                          dan persiapan link telemetri PIO UART untuk ESP32-C3.
 * ============================================================================
 */
#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// [Since v0.5.0 - 2026-10-26] Identitas Versi Firmware Resmi
#define FW_VERSION_STR        "v0.5.0"
#define FW_RELEASE_DATE       "2026-10-26"

// ============================================================================
// 1. PILIH JENIS RECEIVER RC YANG DIGUNAKAN
// ============================================================================
#define RC_PROTO_IBUS  1  // FlySky iBUS (1 kabel serial ke pin RX, 115200 baud)
#define RC_PROTO_CRSF  2  // ELRS / Crossfire (kabel TX & RX, 420000 baud)
#define RC_PROTO_SBUS  3  // FrSky / FlySky SBUS (100000 baud, 8E2 pada port J7)
#define RC_PROTO_PPM   4  // Receiver PPM / PWM (Pulsa sinyal pada pin J7)
#define RC_PROTO_PWM   4

#if defined(ARDUINO_ARCH_RP2040) || defined(TARGET_RP2040)
  #define SELECTED_RC_PROTOCOL RC_PROTO_SBUS  // FlySky FS-iA6C SBUS pada pin GP1 (Inverter Silicon Aktif)
#else
  #define SELECTED_RC_PROTOCOL RC_PROTO_SBUS  // SBUS pada J7 Omnibus F4
#endif

// ============================================================================
// 2. PINOUT OTOMATIS BERDASARKAN BOARD (PICO / STM32F411 / ESP32 / TEENSY)
// ============================================================================
#if defined(ARDUINO_ARCH_RP2040) || defined(TARGET_RP2040)
  #ifdef BOARD_NAME
    #undef BOARD_NAME
  #endif
  #define BOARD_NAME "RP2040-Zero (Waveshare)"
  // ============================================================================
  // PINOUT RESMI "VANIFLIGHT V 1.0" CUSTOM PCB (RP2040-Zero + ESP32-C3)
  // ============================================================================
  // Port Bus I2C0 untuk Sensor IMU (MPU6050) & Barometer (BMP280)
  #define PIN_I2C_SDA         4   // GP4 (I2C0 SDA)
  #define PIN_I2C_SCL         5   // GP5 (I2C0 SCL)

  // Port Output 8-Channel Header S1 s/d S8 (50 Hz Hardware PWM)
  #define PIN_MOTOR_ESC       14  // GP14 -> Port S1 (Motor Kiri / Main ESC)
  #define PIN_MOTOR_ESC_RIGHT 13  // GP13 -> Port S2 (Motor Kanan / Twin-Motor Tailsitter)
  #define PIN_SERVO_LEFT      3   // GP3  -> Port S3 (Servo Sayap Kiri / Elevon L)
  #define PIN_SERVO_RIGHT     2   // GP2  -> Port S4 (Servo Sayap Kanan / Elevon R)
  #define PIN_AUX_S5          6   // GP6  -> Port S5 (Aux 1 / Motor 3 / Flap / Pan)
  #define PIN_AUX_S6          7   // GP7  -> Port S6 (Aux 2 / Motor 4 / Rudder / Tilt)
  #define PIN_AUX_S7          11  // GP11 -> Port S7 (Aux 3 / Gimbal Pan)
  #define PIN_AUX_S8          10  // GP10 -> Port S8 (Aux 4 / Gimbal Tilt)
  #define PIN_AUX_PAD         12  // GP12 -> Pad Solder Cadangan AUX

  // Beeper / Buzzer Aktif Onboard (GP15)
  #define PIN_BEEPER          15  // GP15 -> Kaki (+) Buzzer Aktif Onboard

  // Port Receiver SBUS Eksternal (Port RX di PCB)
  #define PIN_RC_UART_TX      0   // GP0 (UART0 TX)
  #define PIN_RC_UART_RX      1   // GP1 (UART0 RX -> Port RX SBUS di PCB)
  #define RC_SERIAL           Serial1

  // Port Link Serial Telemetri ESP32-C3 Super Mini Onboard (ESP-NOW)
  // CATATAN ARSITEKTUR RP2040:
  // - UART0 hardware dipakai oleh SBUS Receiver (GP0/GP1).
  // - UART1 hardware dipakai oleh GPS RushFPV M10 (GP8/GP9).
  // Komunikasi telemetri dengan ESP32-C3 pada GP28/GP29 diimplementasikan
  // via SerialPIO (PIO-based UART hardware buffer), bebas dari limitasi port UART0.
  #define PIN_ESP32_UART_RX   28  // GP28 (Menerima sinyal dari TX ESP32-C3 via SerialPIO)
  #define PIN_ESP32_UART_TX   29  // GP29 (Mengirim telemetri ke RX ESP32-C3 via SerialPIO)

  // Port GPS RushFPV M10 (Serial2 / UART1)
  #define PIN_GPS_UART_TX     8   // GP8 (UART1 TX -> Sambung ke RX GPS)
  #define PIN_GPS_UART_RX     9   // GP9 (UART1 RX -> Sambung ke TX GPS)
  #define GPS_SERIAL          Serial2
  #define HAS_BAROMETER       1   // Auto-detect BMP280 pada bus I2C0 (GP4/GP5)
  // LED Indikator (GP25 di Pico standar, GP16 RGB di RP2040-Zero)
  #ifdef PIN_LED
    #define PIN_STATUS_LED    PIN_LED
  #else
    #define PIN_STATUS_LED    25
  #endif
  #define LED_ON_STATE        HIGH

#elif defined(STM32F4xx) || defined(ARDUINO_ARCH_STM32)
  // Pilihan Board STM32:
  // 1 = STM32F411 BlackPill (Dev board mandiri via I2C PB6/PB7)
  // 2 = OMNIBUS F4 V5 (Flight Controller STM32F405 dengan onboard SPI MPU6000)
  #define STM32_BOARD_VARIANT 2 // Default: OMNIBUS F4 V5

  #if STM32_BOARD_VARIANT == 2
    #define BOARD_NAME "Omnibus F4 V5 (STM32F405)"
    #define IMU_USE_SPI         1   // Pakai sensor Gyro/Accel MPU6000 bawaan board via SPI1
    #define PIN_SPI_CS          PA4 // CS MPU6000 internal
    #define PIN_SPI_SCK         PA5
    #define PIN_SPI_MISO        PA6
    #define PIN_SPI_MOSI        PA7

    // Pad Solder Motor & Servo di Board Omnibus F4:
    #define PIN_MOTOR_ESC       PB0   // Pad M1 -> Sinyal ESC Motor Brushless
    #define PIN_SERVO_LEFT      PB1   // Pad M2 -> Sinyal Servo Sayap Kiri
    #define PIN_SERVO_RIGHT     PA3   // Pad M3 -> Sinyal Servo Sayap Kanan

    // Pad Serial UART1 untuk Receiver RC (ELRS / iBUS / SBUS):
    #define PIN_RC_UART_TX      PA9   // Pad TX1
    #define PIN_RC_UART_RX      PA10  // Pad RX1
    #define RC_SERIAL           Serial1

    // Pad Serial UART6 untuk GPS RushFPV M10:
    #define PIN_GPS_UART_TX     PC6   // Pad TX6 (sambung ke RX GPS)
    #define PIN_GPS_UART_RX     PC7   // Pad RX6 (sambung ke TX GPS)
    #define GPS_SERIAL          Serial6

    // LED Indikator Biru bawaan Omnibus F4
    #define PIN_STATUS_LED      PB5
    #define LED_ON_STATE        LOW

    // Barometer internal/eksternal (0 = tanpa baro saat tes meja)
    #define HAS_BAROMETER       0
  #else
    #define BOARD_NAME "STM32F411 BlackPill"
    #define IMU_USE_SPI         0
    #define PIN_I2C_SDA         PB7
    #define PIN_I2C_SCL         PB6
    #define PIN_SERVO_LEFT      PA0
    #define PIN_SERVO_RIGHT     PA1
    #define PIN_MOTOR_ESC       PA2
    #define PIN_PWM_CH1_ROLL    PB0
    #define PIN_PWM_CH2_PITCH   PB1
    #define PIN_PWM_CH3_THR     PB10
    #define PIN_PWM_CH4_ARM     PB12
    #define PIN_PWM_CH5_MODE    PB13
    #define PIN_RC_UART_TX      PA9
    #define PIN_RC_UART_RX      PA10
    #define RC_SERIAL           Serial1
    #define PIN_STATUS_LED      PC13
    #define LED_ON_STATE        LOW
  #endif

#elif defined(ESP32)
  #define BOARD_NAME "ESP32 / ESP32-S3"
  #define PIN_I2C_SDA         21
  #define PIN_I2C_SCL         22
  #define PIN_SERVO_LEFT      18
  #define PIN_SERVO_RIGHT     19
  #define PIN_MOTOR_ESC       5
  #define PIN_PWM_CH1_ROLL    25
  #define PIN_PWM_CH2_PITCH   26
  #define PIN_PWM_CH3_THR     27
  #define PIN_PWM_CH4_ARM     32
  #define PIN_PWM_CH5_MODE    33
  #define PIN_RC_UART_TX      17
  #define PIN_RC_UART_RX      16
  #define RC_SERIAL           Serial1
  #define PIN_STATUS_LED      2
  #define LED_ON_STATE        HIGH

#else
  #define BOARD_NAME "Teensy 4.0 / 4.1"
  #define PIN_I2C_SDA         18
  #define PIN_I2C_SCL         19
  #define PIN_SERVO_LEFT      2
  #define PIN_SERVO_RIGHT     3
  #define PIN_MOTOR_ESC       4
  #define PIN_PWM_CH1_ROLL    5
  #define PIN_PWM_CH2_PITCH   6
  #define PIN_PWM_CH3_THR     7
  #define PIN_PWM_CH4_ARM     8
  #define PIN_PWM_CH5_MODE    9
  #define PIN_RC_UART_TX      1
  #define PIN_RC_UART_RX      0
  #define RC_SERIAL           Serial1
  #define PIN_STATUS_LED      13
  #define LED_ON_STATE        HIGH
#endif

#ifndef PIN_GPS_UART_TX
  #define PIN_GPS_UART_TX     8
#endif
#ifndef PIN_GPS_UART_RX
  #define PIN_GPS_UART_RX     9
#endif
#ifndef GPS_SERIAL
  #define GPS_SERIAL          Serial2
#endif
#ifndef PIN_BEEPER
  #define PIN_BEEPER          15
#endif

// ============================================================================
// 3. KONFIGURASI LOOP & FILTER IMU
// ============================================================================
#define LOOP_RATE_HZ          500     // Frekuensi kendali utama: 500 Hz (2000 us)
#define LOOP_INTERVAL_US      (1000000UL / LOOP_RATE_HZ)

// Pilihan Orientasi Pemasangan Board / Sensor IMU MPU6050:
#define ROTATION_DEFAULT      0  // Normal: Chip menghadap atas, sumbu X menghadap ke DEPAN (Hidung)
#define ROTATION_CW90         1  // Diputar 90° searah jarum jam (Kanan)
#define ROTATION_CW180        2  // Diputar 180° (Sumbu X menghadap ke BELAKANG / Ekor)
#define ROTATION_CW270        3  // Diputar 270° / 90° berlawanan jarum jam (Kiri)
#define ROTATION_FLIP         4  // Terbalik (Menghadap bawah)
#define ROTATION_FLIP_CW90    5  // Terbalik + putar 90°

// PILIH ROTASI BOARD ANDA DI SINI:
#ifndef BOARD_ROTATION
  #define BOARD_ROTATION      ROTATION_DEFAULT
#endif

#define IMU_I2C_ADDR          0x68    // Alamat I2C standar MPU6050 / MPU6500
#define MADGWICK_BETA         0.04f   // Bobot koreksi Accelerometer pada filter AHRS
#define GYRO_LPF_ALPHA        0.35f   // Low-pass filter gyro (0.1 = sangat halus, 1.0 = tanpa filter)

// Baudrate GPS:
// - Gunakan 230400 (default protokol biner UBX-NAV-PVT RushFPV M10) atau 115200
// - Gunakan 9600 untuk GPS biru GY-NEO6MV2
#define GPS_BAUD_RATE         230400

// ============================================================================
// 4. PARAMETER KENDALI & BATAS SUDUT TERBANG (FLYING WING)
// ============================================================================
// Batas sudut kemiringan maksimal saat Mode ANGLE (Auto-Level / Stabilize)
#define MAX_ROLL_ANGLE_DEG    60.0f   // Maksimal miring kiri/kanan (derajat, 60° = belok lincah & tajam)
#define MAX_PITCH_UP_DEG      50.0f   // Maksimal mendaki/hidung naik (derajat, nanjak leluasa & bertenaga)
#define MAX_PITCH_DOWN_DEG    30.0f   // Maksimal menukik/hidung turun (derajat, aman dari nyungsep)
#define MAX_PITCH_ANGLE_DEG   MAX_PITCH_UP_DEG
#define MAX_LEVEL_TRIM_DEG    15.0f   // Batas maksimal offset Auto-Level Trim (Roll & Pitch, derajat)
#define MAX_LEVEL_STEP_DEG    5.0f    // Batas maksimal perubahan Auto-Level Trim per sesi (derajat)

// Batas kecepatan putar maksimal saat Mode ACRO (Rate Mode)
#define MAX_ROLL_RATE_DPS     240.0f  // Derajat per detik (responsif & gesit)
#define MAX_PITCH_RATE_DPS    220.0f  // Derajat per detik (tarikan elevator lebih gesit)

// Gain Outer Loop (Angle Error -> Target Rate DPS) untuk Mode ANGLE (Anti-Hunting/Oleng)
#define ANGLE_KP_ROLL         2.2f    // Diturunkan dari 4.5 agar sayap anteng tanpa osilasi "kapal kena ombak"
#define ANGLE_KP_PITCH        2.4f

// Gain Inner Loop PID (Rate DPS -> Koreksi Servo dalam mikrodetik)
#define PID_ROLL_KP           1.0f    // Diturunkan dari 1.8 agar elevon tidak over-correcting di udara
#define PID_ROLL_KI           0.25f   // Diturunkan dari 0.6 agar integral tidak menimbulkan lag fase
#define PID_ROLL_KD           0.04f
#define PID_ROLL_MAX_I        120.0f  // Batas anti-windup Integral (us)

#define PID_PITCH_KP          1.2f    // Diturunkan dari 2.0
#define PID_PITCH_KI          0.30f   // Diturunkan dari 0.8
#define PID_PITCH_KD          0.05f
#define PID_PITCH_MAX_I       120.0f

// Parameter Navigasi Otonom (Altitude Hold & Return-to-Home)
#define RTH_CRUISE_THROTTLE_US 1520   // Throttle jelajah otomatis saat pulang (~52%)
#define RTH_TARGET_ALTITUDE_M  40.0f  // Ketinggian jelajah pulang otomatis (40 meter di atas tanah)
#define ALT_HOLD_KP            2.0f   // Gain koreksi Pitch terhadap beda tinggi (derajat per meter)
#define MAX_NAV_PITCH_DEG      15.0f  // Sudut Pitch maksimal saat mendaki/menukik otomatis
#define MAX_NAV_ROLL_DEG       30.0f  // Sudut Bank maksimal saat berbelok pulang otomatis
#define RTH_HOME_RADIUS_M      20.0f  // Radius putar melingkar (Loiter) di atas titik Home (meter)

// ============================================================================
// 5. KONFIGURASI ELEVON MIXER (2 SERVO + 1 MOTOR BRUSHLESS)
// ============================================================================
// Ubah antara 1 atau -1 jika arah gerak servo terbalik saat dipasang di sayap
#define SERVO_LEFT_DIR        1
#define SERVO_RIGHT_DIR       -1

// Trim titik tengah servo (mikrodetik, normal = 0, ubah misal +20 atau -20 jika kurang rata)
#define SERVO_LEFT_TRIM_US    0
#define SERVO_RIGHT_TRIM_US   0

// Batas gerak sinyal servo aman (1250..1750 us) agar gir tidak mentok stopper
#define SERVO_MIN_US          1250
#define SERVO_CENTER_US       1500
#define SERVO_MAX_US          1750

// Sinyal ESC Motor Brushless
#define MOTOR_DISARMED_US     1000    // Motor mati terkunci
#define MOTOR_IDLE_US         1000    // Flying Wing: motor tetap diam (0%) saat Armed & throttle di bawah
#define MOTOR_MAX_US          2000    // Gas penuh
#define ARM_SAFETY_MAX_THR_US 1080    // Throttle harus di bawah nilai ini agar bisa di-Arm

// ============================================================================
// 6. KONFIGURASI TWIN-MOTOR DIFFERENTIAL THRUST (TAILSITTER / TWIN MOTOR)
// ============================================================================
#define ENABLE_DIFF_THRUST    1       // 1 = Aktifkan beda gas motor kiri (S1) & kanan (S2)
#define DIFF_THRUST_GAIN      0.25f   // Skala respons yaw terhadap perbedaan kecepatan motor (0.1..0.5)
#define DIFF_THRUST_MAX_US    150     // Perbedaan maksimum sinyal antar kedua motor (+-150 us)

#endif // CONFIG_H
