#pragma once

#include "main.h"

#define isPinHigh(pinName) isPinHigh_(pinName ## _GPIO_Port, pinName ## _Pin)
#define isPinLow(pinName) isPinLow_(pinName ## _GPIO_Port, pinName ## _Pin)
#define setPinHigh(pinName) setPinHigh_(pinName ## _GPIO_Port, pinName ## _Pin)
#define setPinLow(pinName) setPinLow_(pinName ## _GPIO_Port, pinName ## _Pin)

static inline uint8_t isPinHigh_(GPIO_TypeDef * port, uint16_t pin) {
  return ((port->IDR & pin) == pin);
}

static inline uint8_t isPinLow_(GPIO_TypeDef * port, uint16_t pin) {
  return ((port->IDR & pin) == 0);
}

static inline void setPinHigh_(GPIO_TypeDef * port, uint16_t pin) {
  port->BSRR = pin;
}

static inline void setPinLow_(GPIO_TypeDef * port, uint16_t pin) {
  port->BSRR = (pin << 16U);
}

static inline uint8_t i8008_getState() {
  // 12 13 14
  return (STM_IN_S0_GPIO_Port->IDR & 0b0111000000000000U) >> 12U;
}

static inline void i8008_writeData(uint8_t data) {
  // 0 1 2 3 4 5 6 7
  STM_OUT_D0_GPIO_Port->BSRR = (uint32_t)((uint8_t)(~data) << 16U) | STM_OUT_DBUS_Pin | (uint32_t)(data);
}

static inline void i8008_freeDataBus() {
  STM_OUT_DBUS_GPIO_Port->BSRR = (uint32_t)STM_OUT_DBUS_Pin << 16U;
}

static inline uint8_t i8008_readDataBus() {
  // 0 1 2 3 4 5 6 7
  return STM_IN_D0_GPIO_Port->IDR & 0b11111111U;
}
