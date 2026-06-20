/**
 * ws2812b.h
 * WS2812B LED Strip Driver for STM32 Nucleo F446ZE
 * Method: TIM3 CH1 (PA6) + DMA — FastLED-style API
 *
 * Wiring:
 *   WS2812B DIN  -->  PA6 (TIM3 CH1)
 *   WS2812B VCC  -->  5V
 *   WS2812B GND  -->  GND
 *
 * Requirements (CubeMX / manually):
 *   - TIM3, Channel 1, PWM Generation CH1
 *   - DMA1 Stream 4, Channel 5, Memory-to-Peripheral, Circular OFF
 *   - PA6 set to AF2 (TIM3_CH1)
 *   - System Clock: 180 MHz (F446ZE default)
 */

#ifndef WS2812B_H
#define WS2812B_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <string.h>

/* ─── Configuration ──────────────────────────────────────────────── */
#define WS2812B_NUM_LEDS        24      /* Number of LEDs in strip   */
#define WS2812B_TIMER           htim3   /* HAL timer handle           */
#define WS2812B_TIM_CHANNEL     TIM_CHANNEL_1

/*
 * WS2812B timing @ 180 MHz system clock
 *   PWM Period  = 180MHz / 800kHz  = 225 ticks  → ARR = 224
 *   T0H  (0 bit high) = ~0.4 µs   = ~72 ticks
 *   T1H  (1 bit high) = ~0.8 µs   = ~144 ticks
 *   Reset pulse >= 50 µs           = 40 zero-CCR cycles (each 1.25µs)
 */
#define WS2812B_ARR             104     /* Auto-reload register value */
#define WS2812B_T0H             32      /* Duty for bit '0'           */
#define WS2812B_T1H             67     /* Duty for bit '1'           */
#define WS2812B_RESET_CYCLES    40      /* Low cycles for reset pulse */

/* Total DMA buffer size: 24 bits/LED * NUM_LEDS + reset cycles */
#define WS2812B_DMA_BUF_SIZE    (WS2812B_NUM_LEDS * 24 + WS2812B_RESET_CYCLES)

/* ─── Color struct (FastLED-compatible field order) ──────────────── */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} CRGB;

/* ─── Predefined Colors (mirrors FastLED) ────────────────────────── */
#define CRGB_Red        ((CRGB){255,   0,   0})
#define CRGB_Green      ((CRGB){  0, 255,   0})
#define CRGB_Blue       ((CRGB){  0,   0, 255})
#define CRGB_White      ((CRGB){255, 255, 255})
#define CRGB_Black      ((CRGB){  0,   0,   0})
#define CRGB_Yellow     ((CRGB){255, 255,   0})
#define CRGB_Cyan       ((CRGB){  0, 255, 255})
#define CRGB_Magenta    ((CRGB){255,   0, 255})
#define CRGB_Orange     ((CRGB){255, 165,   0})
#define CRGB_Purple     ((CRGB){128,   0, 128})
#define CRGB_Pink      ((CRGB){255,20,147})
#define CRGB_Gold      ((CRGB){255,215,0})
#define CRGB_Lime      ((CRGB){50,205,50})
#define CRGB_SkyBlue   ((CRGB){135,206,235})
#define CRGB_Violet    ((CRGB){238,130,238})
#define CRGB_Brown     ((CRGB){165,42,42})
#define CRGB_Teal      ((CRGB){0,128,128})
#define CRGB_Navy      ((CRGB){0,0,128})
#define CRGB_Maroon    ((CRGB){128,0,0})

/* ─── Public API (mirrors FastLED usage) ─────────────────────────── */

/**
 * @brief  Initialize the WS2812B driver.
 *         Call once after MX_TIM3_Init() and MX_DMA_Init().
 * @param  htim  Pointer to the TIM3 HAL handle (pass &htim3)
 */
void WS2812B_Init(TIM_HandleTypeDef *htim);

/**
 * @brief  Push the LED array to the strip (like FastLED.show()).
 *         Blocks until DMA transfer is complete.
 */
void WS2812B_Show(void);

/**
 * @brief  Set a single LED color by index.
 * @param  index  LED index (0 to NUM_LEDS-1)
 * @param  color  CRGB color value
 */
void WS2812B_SetLED(uint16_t index, CRGB color);

/**
 * @brief  Set all LEDs to the same color.
 * @param  color  CRGB color value
 */
void WS2812B_Fill(CRGB color);

/**
 * @brief  Set all LEDs to black (off).
 */
void WS2812B_Clear(void);

/**
 * @brief  Scale brightness of all LEDs (0=off, 255=full).
 *         Apply before WS2812B_Show().
 * @param  brightness  0–255
 */
void WS2812B_SetBrightness(uint8_t brightness);

void WS2812B_TransferComplete(TIM_HandleTypeDef *htim);

/**
 * @brief  Create an HSV color (like FastLED's CHSV).
 * @param  h  Hue 0–255
 * @param  s  Saturation 0–255
 * @param  v  Value (brightness) 0–255
 * @return CRGB color
 */
CRGB WS2812B_HSV(uint8_t h, uint8_t s, uint8_t v);

/* ─── LED Array (access directly like FastLED's leds[]) ─────────── */
extern CRGB leds[WS2812B_NUM_LEDS];

#endif /* WS2812B_H */
