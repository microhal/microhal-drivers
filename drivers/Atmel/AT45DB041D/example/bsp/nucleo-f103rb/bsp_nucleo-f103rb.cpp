/**
 * @file
 * @license    BSD 3-Clause
 * @copyright  microHAL
 * @version    $Id$
 * @brief      board support package for nucleo-f411re board
 *
 * @authors    Pawel Okas
 * created on: 18-11-2016
 * last modification: <DD-MM-YYYY>
 *
 * @copyright Copyright (c) 2016, Paweł Okas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 	   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 * 	      documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this
 *        software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "microhal.h"

#include "bsp.h"

#include "ports/stmCommon/spi_v1/spi_interrupt_stmCommon.h"
#include "ports/stmCommon/spi_v1/spi_polling_stmCommon.h"

using namespace microhal;
using namespace stm32f1xx;

// microhal::stm32f1xx::SPI_polling &spi1 = microhal::stm32f1xx::SPI_polling::create<1, IOPin::PortA, 6, IOPin::PortA, 7, IOPin::PortA, 5>();
microhal::stm32f1xx::SPI_interrupt &spi1 = microhal::stm32f1xx::SPI_interrupt::create<1, IOPin::PortA, 6, IOPin::PortA, 7, IOPin::PortA, 5>();

namespace bsp {
namespace detail {
stm32f1xx::GPIO ce(con2::a::ss, stm32f1xx::GPIO::Direction::Output);
stm32f1xx::GPIO reset(con2::a::io1, stm32f1xx::GPIO::Direction::Output);
stm32f1xx::GPIO wp(con2::a::io2, stm32f1xx::GPIO::Direction::Output);
}  // namespace detail

namespace at45db {
microhal::SPI &spi = spi1;
microhal::GPIO &ce = detail::ce;
microhal::GPIO &reset = detail::reset;
microhal::GPIO &wp = detail::wp;

}  // namespace at45db
}  // namespace bsp

void hardwareConfig(void) {
    (void)bsp::at45db::spi;
    (void)bsp::debugPort;
    // Core::pll_start(8000000, 168000000);
    // Core::fpu_enable();

    IOManager::routeSerial<2, Txd, stm32f1xx::IOPin::PortA, 2>();
    IOManager::routeSerial<2, Rxd, stm32f1xx::IOPin::PortA, 3>();

    spi1.init(stm32f1xx::SPI::Mode0, stm32f1xx::SPI::Prescaler8);
    spi1.enable();

    // SysTick_Config(84000000 / 1000);
}
uint64_t SysTick_time = 0;

extern "C" void SysTick_Handler(void) {
    SysTick_time++;
}
