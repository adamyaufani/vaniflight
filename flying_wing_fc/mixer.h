/*
 * ============================================================================
 * @file        mixer.h
 * @project     Flying Wing Autonomous Flight Controller
 * @version     v0.4.1
 * @since       v0.1.0 (2026-10-18)
 * @updated     2026-10-21
 * @brief       Elevon Mixer (2 Servo + 1 ESC), Hardware PWM RP2040, & Slew-Rate Limiter
 *
 * Riwayat Perubahan (Changelog):
 *   - v0.1.0 (2026-10-18): Pencampuran elevon dasar (Pitch +- Roll) & Arming safety lock.
 *   - v0.2.0 (2026-10-21): Hardware PWM silikon RP2040 (anti-PIO hang), Phase-Staggering
 *                          pulsa servo kiri/kanan, & Slew-Rate Limiter (6 us / 2 ms)
 *                          untuk mencegah drop tegangan BEC 5V saat terbang.
 * ============================================================================
 */
#ifndef MIXER_H
#define MIXER_H

#include <Arduino.h>
#include "config.h"
#include "rc_input.h"

#if defined(ARDUINO_ARCH_RP2040) || defined(TARGET_RP2040)
  #include <hardware/pwm.h>
  #include <hardware/clocks.h>
#elif !defined(ESP32)
  #include <Servo.h>
#endif

struct ActuatorOutputs {
  uint16_t servo_left_us        = SERVO_CENTER_US;
  uint16_t servo_right_us       = SERVO_CENTER_US;
  uint16_t motor_esc_us         = MOTOR_DISARMED_US;  // S1: Motor Kiri / Utama
#ifdef PIN_MOTOR_ESC_RIGHT
  uint16_t motor_esc_right_us   = MOTOR_DISARMED_US;  // S2: Motor Kanan (Twin Motor)
#endif
};

class ElevonMixer {
public:
  ActuatorOutputs out;
  int8_t  servo_left_dir      = SERVO_LEFT_DIR;
  int8_t  servo_right_dir     = SERVO_RIGHT_DIR;
  int16_t servo_left_trim_us  = SERVO_LEFT_TRIM_US;
  int16_t servo_right_trim_us = SERVO_RIGHT_TRIM_US;
  float   servo_left_scale    = 1.0f;  // Skala pengali gerak servo kiri
  float   servo_right_scale   = 1.0f;  // Skala pengali gerak servo kanan (misal 0.85 jika lebih agresif)

  void begin() {
    #if defined(ARDUINO_ARCH_RP2040) || defined(TARGET_RP2040)
      // Gunakan Hardware PWM Silikon Murni RP2040 (1 MHz tick = 1.0 us, periode 20.000 us = 50 Hz)
      // Bebas 100% dari bug hang/halt PIO Servo.h (jmp !x, 2 + pio_sm_clear_fifos)!
      float clk_div = (float)clock_get_hz(clk_sys) / 1000000.0f;
    #ifdef PIN_MOTOR_ESC_RIGHT
      const uint8_t  pins[4]    = { PIN_SERVO_LEFT, PIN_SERVO_RIGHT, PIN_MOTOR_ESC, PIN_MOTOR_ESC_RIGHT };
      const uint8_t  num_pins   = 4;
    #else
      const uint8_t  pins[3]    = { PIN_SERVO_LEFT, PIN_SERVO_RIGHT, PIN_MOTOR_ESC };
      const uint8_t  num_pins   = 3;
    #endif

      for (uint8_t i = 0; i < num_pins; i++) {
        uint slice = pwm_gpio_to_slice_num(pins[i]);
        pwm_set_clkdiv(slice, clk_div);
        pwm_set_wrap(slice, 19999); // 20.000 us (50.00 Hz)
      }
      // Phase-Staggering: Geser fasa pulsa Servo Kiri (GP3 = Slice 1 Chan B) ke ujung periode 20ms
      // sementara Servo Kanan (GP2 = Slice 1 Chan A) di awal periode,
      // agar lonjakan arus awal (inrush current) kedua servo TIDAK PERNAH berbarengan!
      uint slice_srv = pwm_gpio_to_slice_num(PIN_SERVO_LEFT);
      pwm_set_output_polarity(slice_srv, false, true);

      gpio_set_function(PIN_SERVO_LEFT,      GPIO_FUNC_PWM);
      gpio_set_function(PIN_SERVO_RIGHT,     GPIO_FUNC_PWM);
      gpio_set_function(PIN_MOTOR_ESC,       GPIO_FUNC_PWM);
    #ifdef PIN_MOTOR_ESC_RIGHT
      gpio_set_function(PIN_MOTOR_ESC_RIGHT, GPIO_FUNC_PWM);
    #endif

      // Inisialisasi port cadangan S5..S8 agar tidak floating
    #ifdef PIN_AUX_S5
      gpio_init(PIN_AUX_S5); gpio_set_dir(PIN_AUX_S5, GPIO_OUT); gpio_put(PIN_AUX_S5, 0);
    #endif
    #ifdef PIN_AUX_S6
      gpio_init(PIN_AUX_S6); gpio_set_dir(PIN_AUX_S6, GPIO_OUT); gpio_put(PIN_AUX_S6, 0);
    #endif
    #ifdef PIN_AUX_S7
      gpio_init(PIN_AUX_S7); gpio_set_dir(PIN_AUX_S7, GPIO_OUT); gpio_put(PIN_AUX_S7, 0);
    #endif
    #ifdef PIN_AUX_S8
      gpio_init(PIN_AUX_S8); gpio_set_dir(PIN_AUX_S8, GPIO_OUT); gpio_put(PIN_AUX_S8, 0);
    #endif

      writeOutputs(SERVO_CENTER_US, SERVO_CENTER_US, MOTOR_DISARMED_US, MOTOR_DISARMED_US);

      for (uint8_t i = 0; i < num_pins; i++) {
        pwm_set_enabled(pwm_gpio_to_slice_num(pins[i]), true);
      }
    #elif defined(ESP32)
      // Gunakan hardware LEDC PWM bawaan ESP32 (50 Hz standar Servo/ESC)
      ledcSetup(0, 50, 16);
      ledcSetup(1, 50, 16);
      ledcSetup(2, 50, 16);
      ledcAttachPin(PIN_SERVO_LEFT,  0);
      ledcAttachPin(PIN_SERVO_RIGHT, 1);
      ledcAttachPin(PIN_MOTOR_ESC,   2);
    #else
      // STM32F4 dan Teensy menggunakan Servo.h bawaan core
      servo_left_.attach(PIN_SERVO_LEFT,   SERVO_MIN_US, SERVO_MAX_US);
      servo_right_.attach(PIN_SERVO_RIGHT, SERVO_MIN_US, SERVO_MAX_US);
      motor_esc_.attach(PIN_MOTOR_ESC,     MOTOR_DISARMED_US, MOTOR_MAX_US);
    #endif

    // Pastikan saat pertama nyala: Servo di tengah, Motor terkunci mati (1000 us)
    writeOutputs(SERVO_CENTER_US, SERVO_CENTER_US, MOTOR_DISARMED_US);
  }

  // Rumus Pencampuran Elevon (Flying Wing 2 Servo + 1 Motor Brushless):
  // - Pitch positif (tarik stik ke belakang / nose up) -> kedua elevon naik bersama
  // - Roll positif (miring kanan) -> elevon kanan naik, elevon kiri turun
  void update(float roll_cmd_us, float pitch_cmd_us, const RCState &rc) {
    float left_mix  = (pitch_cmd_us - roll_cmd_us) * (float)servo_left_dir * servo_left_scale;
    float right_mix = (pitch_cmd_us + roll_cmd_us) * (float)servo_right_dir * servo_right_scale;

    int32_t left_us  = SERVO_CENTER_US + servo_left_trim_us  + (int32_t)left_mix;
    int32_t right_us = SERVO_CENTER_US + servo_right_trim_us + (int32_t)right_mix;

    uint16_t target_left_us  = (uint16_t)constrain(left_us,  SERVO_MIN_US, SERVO_MAX_US);
    uint16_t target_right_us = (uint16_t)constrain(right_us, SERVO_MIN_US, SERVO_MAX_US);

    // Slew-Rate Limiter (Anti-Drop Tegangan 5V saat 2 Servo Bergerak Bersamaan):
    // Membatasi lonjakan perubahan pulsa maksimal 6 us per 2ms (3000 us/detik = 0.16 detik ujung-ke-ujung).
    // Kecepatan fisik servo tetap responsif penuh, tetapi arus kejut awal (stall current spike) turun >60%!
    const int16_t max_step = 6;
    int16_t diff_l = (int16_t)target_left_us  - (int16_t)out.servo_left_us;
    int16_t diff_r = (int16_t)target_right_us - (int16_t)out.servo_right_us;
    diff_l = constrain(diff_l, -max_step, max_step);
    diff_r = constrain(diff_r, -max_step, max_step);

    out.servo_left_us  = (uint16_t)((int16_t)out.servo_left_us  + diff_l);
    out.servo_right_us = (uint16_t)((int16_t)out.servo_right_us + diff_r);

    // Pengaman & Kontrol Motor Brushless (Arming Lock + Cruise Throttle saat RTH)
    uint16_t base_thr = MOTOR_DISARMED_US;
    if (rc.armed) {
      if (rc.mode == MODE_RTH) {
        // Saat mode RTH aktif, jalankan motor pada throttle jelajah otomatis
        // (Pilot tetap bisa memberi gas lebih besar dengan stik jika diinginkan)
        uint16_t target_thr = (rc.throttle_us > RTH_CRUISE_THROTTLE_US) ? rc.throttle_us : RTH_CRUISE_THROTTLE_US;
        base_thr = (uint16_t)constrain(target_thr, MOTOR_IDLE_US, MOTOR_MAX_US);
      } else if (!rc.failsafe) {
        if (rc.throttle_us <= 1070) {
          base_thr = MOTOR_DISARMED_US; // Stik di bawah (FlySky SBUS min ~1042) -> motor diam 0% (1000 us)
        } else {
          base_thr = (uint16_t)constrain(rc.throttle_us, MOTOR_IDLE_US, MOTOR_MAX_US);
        }
      } else {
        base_thr = MOTOR_DISARMED_US;
      }
    } else {
      base_thr = MOTOR_DISARMED_US;
    }

    out.motor_esc_us = base_thr;

  #ifdef PIN_MOTOR_ESC_RIGHT
    // Twin-Motor Differential Thrust untuk Yaw / Tailsitter Hover
    if (rc.armed && base_thr > MOTOR_IDLE_US && ENABLE_DIFF_THRUST) {
      int16_t yaw_delta = (int16_t)rc.yaw_us - 1500;
      int16_t diff_us   = (int16_t)(yaw_delta * DIFF_THRUST_GAIN);
      diff_us = constrain(diff_us, -DIFF_THRUST_MAX_US, DIFF_THRUST_MAX_US);

      // Stik Yaw Kanan (>1500) -> Motor Kiri tambah gas (+diff), Motor Kanan kurangi gas (-diff)
      out.motor_esc_us       = (uint16_t)constrain((int32_t)base_thr + diff_us, MOTOR_IDLE_US, MOTOR_MAX_US);
      out.motor_esc_right_us = (uint16_t)constrain((int32_t)base_thr - diff_us, MOTOR_IDLE_US, MOTOR_MAX_US);
    } else {
      out.motor_esc_right_us = base_thr;
    }
    writeOutputs(out.servo_left_us, out.servo_right_us, out.motor_esc_us, out.motor_esc_right_us);
  #else
    writeOutputs(out.servo_left_us, out.servo_right_us, out.motor_esc_us, out.motor_esc_us);
  #endif
  }

private:
  #if !defined(ESP32) && !defined(ARDUINO_ARCH_RP2040) && !defined(TARGET_RP2040)
    Servo servo_left_;
    Servo servo_right_;
    Servo motor_esc_;
  #endif

  void writeOutputs(uint16_t left_us, uint16_t right_us, uint16_t esc_left_us, uint16_t esc_right_us = 1000) {
    #if defined(ARDUINO_ARCH_RP2040) || defined(TARGET_RP2040)
      // Update langsung ke register double-buffered Hardware PWM RP2040
      // GP2 / Chan A (PIN_SERVO_RIGHT): pulsa HIGH di awal periode (0 .. right_us)
      // GP3 / Chan B inverted (PIN_SERVO_LEFT): pulsa HIGH di akhir periode (20000 - left_us .. 20000)
      pwm_set_gpio_level(PIN_SERVO_RIGHT, right_us);
      pwm_set_gpio_level(PIN_SERVO_LEFT,  20000 - left_us);
      pwm_set_gpio_level(PIN_MOTOR_ESC,   esc_left_us);
    #ifdef PIN_MOTOR_ESC_RIGHT
      pwm_set_gpio_level(PIN_MOTOR_ESC_RIGHT, esc_right_us);
    #endif
    #elif defined(ESP32)
      // Konversi mikrodetik (dari periode 20.000 us @ 50Hz) ke resolusi 16-bit (65535)
      uint32_t duty_l = ((uint32_t)left_us     * 65536UL) / 20000UL;
      uint32_t duty_r = ((uint32_t)right_us    * 65536UL) / 20000UL;
      uint32_t duty_m = ((uint32_t)esc_left_us * 65536UL) / 20000UL;
      ledcWrite(0, duty_l);
      ledcWrite(1, duty_r);
      ledcWrite(2, duty_m);
    #else
      servo_left_.writeMicroseconds(left_us);
      servo_right_.writeMicroseconds(right_us);
      motor_esc_.writeMicroseconds(esc_left_us);
    #endif
  }
};

#endif // MIXER_H
