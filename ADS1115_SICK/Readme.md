Here’s a clean and professional README file you can use for your GitHub repository:

---

# 📟 ADS1115 Distance Measurement (STM32)

This project demonstrates how to interface the **ADS1115 ADC** with an STM32 microcontroller using I2C, read analog voltage from a sensor, and convert it into distance measurements.

---

## 🚀 Features

* Initialize ADS1115 with configurable data rate and gain
* Read analog input (Single-ended AIN0)
* Convert ADC readings (mV → Voltage)
* Map voltage to distance (in cm)
* Output data via UART

---

## 🛠️ Hardware Requirements

* STM32 microcontroller (e.g., STM32F1/F4 series)
* ADS1115 ADC module
* Distance sensor (analog output)
* UART-to-USB converter (for serial monitoring)
* Connecting wires

---

## 🔌 Connections

| ADS1115 Pin | STM32 Pin     |
| ----------- | ------------- |
| VDD         | 3.3V/ 5V      |
| GND         | GND           |
| SCL         | I2C SCL       |
| SDA         | I2C SDA       |
| AIN0        | Sensor Output |

---

## ⚙️ Software Requirements

* STM32CubeIDE / Keil / IAR
* HAL drivers enabled:

  * I2C
  * UART
* ADS1115 driver (`ads1115.c / ads1115.h`)

---

## 📂 Project Structure

```
/Core
  /Src
    main.c
    ads1115.c
  /Inc
    ads1115.h
```

---

## 🧠 How It Works

1. **Initialization**

   * ADS1115 is initialized with:

     * Data Rate: 128 SPS
     * Gain: ±6.144V

2. **Reading ADC**

   * Single-ended reading from channel AIN0

3. **Voltage Conversion**

   ```
   voltage = mV / 1000
   ```

4. **Distance Calculation**

   ```
   distance = ((voltage - 0.19) / 4.97) * total_distance
   ```

   * This formula is calibrated for a specific analog distance sensor (e.g., SICK sensor)

5. **UART Output**

   ```
   D0:XXX.X CM
   ```

---

## 📊 Example Output

```
D0:245.3 CM
D0:246.1 CM
D0:244.8 CM
```

---

## ⚠️ Notes

* Calibration constants (`0.19`, `4.97`) depend on your sensor
* `total_dis` defines maximum measurable distance (currently 1000 cm)
* Ensure correct I2C address for ADS1115
* Add proper error handling if initialization fails

---




