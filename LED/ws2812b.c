/**
 * ws2812b.c
 * WS2812B LED Strip Driver — TIM3 CH1 + DMA implementation
 * STM32 Nucleo F446ZE @ 180 MHz
 */

#include "ws2812b.h"

/* ─── Private variables ──────────────────────────────────────────── */

/* Public LED array — access like FastLED's leds[] */
CRGB leds[WS2812B_NUM_LEDS];

/* DMA pulse buffer: each entry is a PWM CCR value (T0H or T1H) */
static uint32_t dmaBuffer[WS2812B_DMA_BUF_SIZE];

/* Internal brightness scaler (0–255) */
static uint8_t  globalBrightness = 255;

/* TIM handle pointer set during Init */
static TIM_HandleTypeDef *phtim = NULL;

/* DMA transfer complete flag */
static volatile uint8_t dmaTransferDone = 1;

/* ─── Private helpers ────────────────────────────────────────────── */

/**
 * @brief Scale a byte by brightness (0-255).
 *        Uses integer math: result = (val * brightness) / 255
 */
static inline uint8_t ScaleBrightness(uint8_t val, uint8_t brightness)
{
    return (uint8_t)(((uint16_t)val * brightness) >> 8);
}

/**
 * @brief Convert CRGB color to 24 PWM pulses in the DMA buffer.
 *        WS2812B expects GRB order, MSB first.
 * @param ledIndex  Index into leds[]
 * @param bufOffset Starting index in dmaBuffer
 */
static void EncodeLED(uint16_t ledIndex, uint16_t bufOffset)
{
    uint8_t r = ScaleBrightness(leds[ledIndex].r, globalBrightness);
    uint8_t g = ScaleBrightness(leds[ledIndex].g, globalBrightness);
    uint8_t b = ScaleBrightness(leds[ledIndex].b, globalBrightness);

    /* GRB order, MSB first — 8 bits each */
    uint32_t color24 = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;

    for (int bit = 23; bit >= 0; bit--)
    {
        dmaBuffer[bufOffset + (23 - bit)] =
            (color24 & (1UL << bit)) ? WS2812B_T1H : WS2812B_T0H;
    }
}

/**
 * @brief Build the full DMA buffer for all LEDs + reset pulse.
 */
static void BuildDMABuffer(void)
{
    for (uint16_t i = 0; i < WS2812B_NUM_LEDS; i++)
    {
        EncodeLED(i, i * 24);
    }

    /* Reset pulse: hold line LOW for >= 50µs (40 cycles of 1.25µs each) */
    uint16_t resetStart = WS2812B_NUM_LEDS * 24;
    for (uint16_t i = 0; i < WS2812B_RESET_CYCLES; i++)
    {
        dmaBuffer[resetStart + i] = 0;
    }
}

/* ─── Public API implementation ──────────────────────────────────── */

void WS2812B_Init(TIM_HandleTypeDef *htim)
{
    phtim = htim;
    memset(leds, 0, sizeof(leds));
    memset(dmaBuffer, 0, sizeof(dmaBuffer));
    dmaTransferDone = 1;
}

void WS2812B_SetLED(uint16_t index, CRGB color)
{
    if (index < WS2812B_NUM_LEDS)
    {
        leds[index] = color;
    }
}

void WS2812B_Fill(CRGB color)
{
    for (uint16_t i = 0; i < WS2812B_NUM_LEDS; i++)
    {
        leds[i] = color;
    }
}

void WS2812B_Clear(void)
{
    memset(leds, 0, sizeof(leds));
}

void WS2812B_SetBrightness(uint8_t brightness)
{
    globalBrightness = brightness;
}

CRGB WS2812B_HSV(uint8_t h, uint8_t s, uint8_t v)
{
    CRGB rgb = {0, 0, 0};

    if (s == 0) {
        rgb.r = rgb.g = rgb.b = v;
        return rgb;
    }

    uint8_t region   = h / 43;
    uint8_t remain   = (h - (region * 43)) * 6;

    uint8_t p = (v * (255 - s)) >> 8;
    uint8_t q = (v * (255 - ((s * remain) >> 8))) >> 8;
    uint8_t t = (v * (255 - ((s * (255 - remain)) >> 8))) >> 8;

    switch (region)
    {
        case 0:  rgb.r = v; rgb.g = t; rgb.b = p; break;
        case 1:  rgb.r = q; rgb.g = v; rgb.b = p; break;
        case 2:  rgb.r = p; rgb.g = v; rgb.b = t; break;
        case 3:  rgb.r = p; rgb.g = q; rgb.b = v; break;
        case 4:  rgb.r = t; rgb.g = p; rgb.b = v; break;
        default: rgb.r = v; rgb.g = p; rgb.b = q; break;
    }

    return rgb;
}

void WS2812B_Show(void)
{
    if (phtim == NULL) return;

    /* Wait for any previous transfer to finish */
    uint32_t timeout = HAL_GetTick() + 10;  /* 10ms timeout */
    while (!dmaTransferDone && HAL_GetTick() < timeout);

    BuildDMABuffer();
    dmaTransferDone = 0;

    /* Start PWM DMA — TIM3 drives PA6 automatically via DMA */
    HAL_TIM_PWM_Start_DMA(phtim,
                           WS2812B_TIM_CHANNEL,
                           dmaBuffer,
                           WS2812B_DMA_BUF_SIZE);

    /* Block until complete (safe for simple polling usage) */
    timeout = HAL_GetTick() + 10;
    while (!dmaTransferDone && HAL_GetTick() < timeout);
}

/* ─── DMA Transfer Complete Callback ────────────────────────────── */
/**
 * @brief Called by HAL when DMA transfer finishes.
 *        Stop PWM to avoid re-triggering and set flag.
 *
 * IMPORTANT: Add this to your stm32f4xx_it.c or main.c:
 *
 *   void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
 *   {
 *       WS2812B_TransferComplete(htim);
 *   }
 */
void WS2812B_TransferComplete(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        HAL_TIM_PWM_Stop_DMA(htim, WS2812B_TIM_CHANNEL);
        dmaTransferDone = 1;
    }
}
