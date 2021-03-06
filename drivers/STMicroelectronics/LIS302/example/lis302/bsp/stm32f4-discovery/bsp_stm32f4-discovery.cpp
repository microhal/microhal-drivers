/* ========================================================================================================================== */ /**
 @license    BSD 3-Clause
 @copyright  microHAL
 @version    $Id$
 @brief      hardware definitions

 @authors    Pawel
 created on: 11-08-2015
 last modification: <DD-MM-YYYY>

 Copyright (c) 2014, microHAL
 All rights reserved.
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following
 conditions are met:
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
 in the documentation and/or other materials provided with the distribution.
 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived
 from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */ /* ==========================================================================================================================
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           */
#include "SPIDevice/SPIDevice.h"
#include "bsp.h"
#include "microhal.h"

using namespace microhal;
using namespace stm32f4xx;

namespace bsp {
namespace lis302 {
namespace detail {
microhal::stm32f4xx::GPIO cs({microhal::stm32f4xx::GPIO::Port::PortE, 3}, microhal::stm32f4xx::GPIO::Direction::Input);
}
microhal::GPIO &cs = detail::cs;
}  // namespace lis302
}  // namespace bsp

int main(void);

extern "C" ssize_t _write_r(struct _reent *, int, const void *, size_t) {
    return 0;
}

void hardwareConfig(void) {
    Core::pll_start(8000000, 168000000);
    Core::fpu_enable();

    IOManager::routeSerial<3, Txd, stm32f4xx::IOPin::PortD, 8>();
    IOManager::routeSerial<3, Rxd, stm32f4xx::IOPin::PortD, 9>();

    IOManager::routeSPI<1, SCK, stm32f4xx::IOPin::PortA, 5>();
    IOManager::routeSPI<1, MISO, stm32f4xx::IOPin::PortA, 6>();
    IOManager::routeSPI<1, MOSI, stm32f4xx::IOPin::PortA, 7>();

    // configure Serial Port interfaces
    stm32f4xx::SerialPort::Serial3.clear();

    stm32f4xx::SerialPort::Serial3.setDataBits(stm32f4xx::SerialPort::Data8);
    stm32f4xx::SerialPort::Serial3.setStopBits(stm32f4xx::SerialPort::OneStop);
    stm32f4xx::SerialPort::Serial3.setParity(stm32f4xx::SerialPort::NoParity);
    stm32f4xx::SerialPort::Serial3.setBaudRate(stm32f4xx::SerialPort::Baud115200);
    stm32f4xx::SerialPort::Serial3.open(stm32f4xx::SerialPort::ReadWrite);

    stm32f4xx::SPI::spi1.init(stm32f4xx::SPI::Mode1, stm32f4xx::SPI::Prescaler256);
    stm32f4xx::SPI::spi1.enable();
}

uint64_t SysTick_time = 0;
