/*
 * BNO055_STM32.h
 *
 *  Created on: Mar 29, 2026
 *      Author: pchin
 */
#ifndef INC_BNO055_STM32_H_
#define INC_BNO055_STM32_H_

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef FREERTOS_ENABLED
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#endif

#include "stm32f4xx_hal.h"
#include "BNO055.h"

/* --------------------------------------------------------------------------
 * Bus type tag
 * -------------------------------------------------------------------------- */
typedef enum {
  BNO055_BUS_NONE   = 0,
  BNO055_BUS_I2C    = 1,
  BNO055_BUS_FMPI2C = 2
} bno055_bus_type_t;

/* --------------------------------------------------------------------------
 * Internal state — do not access directly
 * -------------------------------------------------------------------------- */
static bno055_bus_type_t       _bno055_bus_type  = BNO055_BUS_NONE;
static I2C_HandleTypeDef      *_bno055_i2c_port  = NULL;
static FMPI2C_HandleTypeDef   *_bno055_fmpi2c_port = NULL;

/* --------------------------------------------------------------------------
 * Internal setters — called by the _Generic macro below
 * -------------------------------------------------------------------------- */
static inline void _bno055_assignI2C(I2C_HandleTypeDef *hi2c) {
  _bno055_i2c_port    = hi2c;
  _bno055_fmpi2c_port = NULL;
  _bno055_bus_type    = BNO055_BUS_I2C;
}

static inline void _bno055_assignFMPI2C(FMPI2C_HandleTypeDef *hfmpi2c) {
  _bno055_fmpi2c_port = hfmpi2c;
  _bno055_i2c_port    = NULL;
  _bno055_bus_type    = BNO055_BUS_FMPI2C;
}

/* --------------------------------------------------------------------------
 * Public interface — same call site regardless of peripheral type:
 *   bno055_assignI2C(&hi2c1);      // standard I2C
 *   bno055_assignI2C(&hfmpi2c1);   // FMPI2C
 * -------------------------------------------------------------------------- */
#define bno055_assignI2C(handle) _Generic((handle),         \
  I2C_HandleTypeDef    *: _bno055_assignI2C,                \
  FMPI2C_HandleTypeDef *: _bno055_assignFMPI2C              \
)(handle)

/* --------------------------------------------------------------------------
 * Delay
 * -------------------------------------------------------------------------- */
void bno055_delay(int time) {
#ifdef FREERTOS_ENABLED
  osDelay(time);
#else
  HAL_Delay(time);
#endif
}

/* --------------------------------------------------------------------------
 * Write
 * -------------------------------------------------------------------------- */
void bno055_writeData(uint8_t reg, uint8_t data) {
  uint8_t txdata[2] = {reg, data};
  HAL_StatusTypeDef status;

  if (_bno055_bus_type == BNO055_BUS_FMPI2C) {
    status = HAL_FMPI2C_Master_Transmit(_bno055_fmpi2c_port,
                                        BNO055_I2C_ADDR << 1,
                                        txdata, sizeof(txdata), 10);
  } else {
    status = HAL_I2C_Master_Transmit(_bno055_i2c_port,
                                     BNO055_I2C_ADDR << 1,
                                     txdata, sizeof(txdata), 10);
  }

  if (status == HAL_OK) {
    return;
  }
  if (status == HAL_ERROR) {
    printf("HAL_I2C_Master_Transmit HAL_ERROR\r\n");
  } else if (status == HAL_TIMEOUT) {
    printf("HAL_I2C_Master_Transmit HAL_TIMEOUT\r\n");
  } else if (status == HAL_BUSY) {
    printf("HAL_I2C_Master_Transmit HAL_BUSY\r\n");
  } else {
    printf("Unknown status data %d", status);
  }

  if (_bno055_bus_type == BNO055_BUS_FMPI2C) {
    uint32_t error = HAL_FMPI2C_GetError(_bno055_fmpi2c_port);
    if (error == HAL_FMPI2C_ERROR_NONE) {
      return;
    } else if (error == HAL_FMPI2C_ERROR_BERR) {
      printf("HAL_FMPI2C_ERROR_BERR\r\n");
    } else if (error == HAL_FMPI2C_ERROR_ARLO) {
      printf("HAL_FMPI2C_ERROR_ARLO\r\n");
    } else if (error == HAL_FMPI2C_ERROR_AF) {
      printf("HAL_FMPI2C_ERROR_AF\r\n");
    } else if (error == HAL_FMPI2C_ERROR_OVR) {
      printf("HAL_FMPI2C_ERROR_OVR\r\n");
    } else if (error == HAL_FMPI2C_ERROR_DMA) {
      printf("HAL_FMPI2C_ERROR_DMA\r\n");
    } else if (error == HAL_FMPI2C_ERROR_TIMEOUT) {
      printf("HAL_FMPI2C_ERROR_TIMEOUT\r\n");
    }
    HAL_FMPI2C_StateTypeDef state = HAL_FMPI2C_GetState(_bno055_fmpi2c_port);
    if (state == HAL_FMPI2C_STATE_RESET) {
      printf("HAL_FMPI2C_STATE_RESET\r\n");
    } else if (state == HAL_FMPI2C_STATE_READY) {
      printf("HAL_FMPI2C_STATE_READY\r\n");
    } else if (state == HAL_FMPI2C_STATE_BUSY) {
      printf("HAL_FMPI2C_STATE_BUSY\r\n");
    } else if (state == HAL_FMPI2C_STATE_BUSY_TX) {
      printf("HAL_FMPI2C_STATE_BUSY_TX\r\n");
    } else if (state == HAL_FMPI2C_STATE_BUSY_RX) {
      printf("HAL_FMPI2C_STATE_BUSY_RX\r\n");
    } else if (state == HAL_FMPI2C_STATE_ABORT) {
      printf("HAL_FMPI2C_STATE_ABORT\r\n");
    } else if (state == HAL_FMPI2C_STATE_TIMEOUT) {
      printf("HAL_FMPI2C_STATE_TIMEOUT\r\n");
    } else if (state == HAL_FMPI2C_STATE_ERROR) {
      printf("HAL_FMPI2C_STATE_ERROR\r\n");
    }
  } else {
    uint32_t error = HAL_I2C_GetError(_bno055_i2c_port);
    if (error == HAL_I2C_ERROR_NONE) {
      return;
    } else if (error == HAL_I2C_ERROR_BERR) {
      printf("HAL_I2C_ERROR_BERR\r\n");
    } else if (error == HAL_I2C_ERROR_ARLO) {
      printf("HAL_I2C_ERROR_ARLO\r\n");
    } else if (error == HAL_I2C_ERROR_AF) {
      printf("HAL_I2C_ERROR_AF\r\n");
    } else if (error == HAL_I2C_ERROR_OVR) {
      printf("HAL_I2C_ERROR_OVR\r\n");
    } else if (error == HAL_I2C_ERROR_DMA) {
      printf("HAL_I2C_ERROR_DMA\r\n");
    } else if (error == HAL_I2C_ERROR_TIMEOUT) {
      printf("HAL_I2C_ERROR_TIMEOUT\r\n");
    }
    HAL_I2C_StateTypeDef state = HAL_I2C_GetState(_bno055_i2c_port);
    if (state == HAL_I2C_STATE_RESET) {
      printf("HAL_I2C_STATE_RESET\r\n");
    } else if (state == HAL_I2C_STATE_READY) {
      printf("HAL_I2C_STATE_RESET\r\n");
    } else if (state == HAL_I2C_STATE_BUSY) {
      printf("HAL_I2C_STATE_BUSY\r\n");
    } else if (state == HAL_I2C_STATE_BUSY_TX) {
      printf("HAL_I2C_STATE_BUSY_TX\r\n");
    } else if (state == HAL_I2C_STATE_BUSY_RX) {
      printf("HAL_I2C_STATE_BUSY_RX\r\n");
    } else if (state == HAL_I2C_STATE_LISTEN) {
      printf("HAL_I2C_STATE_LISTEN\r\n");
    } else if (state == HAL_I2C_STATE_BUSY_TX_LISTEN) {
      printf("HAL_I2C_STATE_BUSY_TX_LISTEN\r\n");
    } else if (state == HAL_I2C_STATE_BUSY_RX_LISTEN) {
      printf("HAL_I2C_STATE_BUSY_RX_LISTEN\r\n");
    } else if (state == HAL_I2C_STATE_ABORT) {
      printf("HAL_I2C_STATE_ABORT\r\n");
    } else if (state == HAL_I2C_STATE_TIMEOUT) {
      printf("HAL_I2C_STATE_TIMEOUT\r\n");
    } else if (state == HAL_I2C_STATE_ERROR) {
      printf("HAL_I2C_STATE_ERROR\r\n");
    }
  }
}

/* --------------------------------------------------------------------------
 * Read
 * -------------------------------------------------------------------------- */
void bno055_readData(uint8_t reg, uint8_t *data, uint8_t len) {
  if (_bno055_bus_type == BNO055_BUS_FMPI2C) {
    HAL_FMPI2C_Master_Transmit(_bno055_fmpi2c_port, BNO055_I2C_ADDR << 1,
                               &reg, 1, 100);
    HAL_FMPI2C_Master_Receive(_bno055_fmpi2c_port, BNO055_I2C_ADDR << 1,
                              data, len, 100);
  } else {
    HAL_I2C_Master_Transmit(_bno055_i2c_port, BNO055_I2C_ADDR << 1,
                            &reg, 1, 100);
    HAL_I2C_Master_Receive(_bno055_i2c_port, BNO055_I2C_ADDR << 1,
                           data, len, 100);
  }
}

#ifdef __cplusplus
  }
#endif

#endif /* INC_BNO055_STM32_H_ */
