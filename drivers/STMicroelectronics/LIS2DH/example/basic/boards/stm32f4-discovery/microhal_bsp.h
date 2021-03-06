/* ===========================================================================================
 */
/**
 @file        STM32F4Discovery.h
 @authors     Michal Karwatowski
 @version     $Id$
 @package
 @brief       hardware definitions
 @hardware    STM32F4Discovery
 @copyright   $Copyright$
 @details

 */ /* ============================================================================================
       */
/* Created on: 20 sty 2014 */

#ifndef STM32F4DISCOVERY_H_
#define STM32F4DISCOVERY_H_

#include "i2c.h"
// clang-format off
static microhal::SerialPort &debugPort = microhal::stm32f4xx::SerialPort::Serial3;

static microhal::I2C &sensorI2C = microhal::stm32f4xx::I2C::i2c1;

constexpr microhal::GPIO::IOPin lisCS(microhal::stm32f4xx::GPIO::Port::PortE, 3);
constexpr microhal::GPIO::IOPin lisINT1(microhal::stm32f4xx::GPIO::Port::PortC, 13);
//constexpr microhal::GPIO::IOPin lisINT2(microhal::stm32f4xx::GPIO::Port::PortE, 1);
//constexpr microhal::GPIO::IOPin lisSEL(microhal::stm32f4xx::GPIO::Port::PortA, 6);
constexpr microhal::GPIO::IOPin lisSA0(microhal::stm32f4xx::GPIO::Port::PortC, 14);

constexpr microhal::GPIO::IOPin Led3(microhal::stm32f4xx::GPIO::Port::PortD, 13);
constexpr microhal::GPIO::IOPin Led4(microhal::stm32f4xx::GPIO::Port::PortD, 12);
constexpr microhal::GPIO::IOPin Led5(microhal::stm32f4xx::GPIO::Port::PortD, 14);
constexpr microhal::GPIO::IOPin Led6(microhal::stm32f4xx::GPIO::Port::PortD, 15);

constexpr microhal::GPIO::IOPin Sw1(microhal::stm32f4xx::GPIO::Port::PortA, 0);

constexpr microhal::GPIO::IOPin GreenLed = Led4;
constexpr microhal::GPIO::IOPin RedLed = Led3;
// clang-format on
#endif /* STM32F4DISCOVERY_H_ */
