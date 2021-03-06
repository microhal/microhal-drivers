/* =========================================================================================== */ /**
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
#include "microhal.h"

static microhal::SerialPort &debugPort = microhal::stm32f4xx::SerialPort::Serial3;

static microhal::I2C &sensorI2C = microhal::stm32f4xx::I2C::i2c2;

constexpr microhal::IOPin Led3(microhal::stm32f4xx::GPIO::Port::PortD, 13);
constexpr microhal::IOPin Led4(microhal::stm32f4xx::GPIO::Port::PortD, 12);
constexpr microhal::IOPin Led5(microhal::stm32f4xx::GPIO::Port::PortD, 14);
constexpr microhal::IOPin Led6(microhal::stm32f4xx::GPIO::Port::PortD, 15);

constexpr microhal::IOPin Sw1(microhal::stm32f4xx::GPIO::Port::PortA, 0);

constexpr microhal::IOPin GreenLed = Led4;
constexpr microhal::IOPin RedLed = Led3;

#endif /* STM32F4DISCOVERY_H_ */
