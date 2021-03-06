/**
 * @license    BSD 3-Clause
 * @copyright  microHAL
 * @version    $Id$
 * @brief      FLIR Lepton IR camera example main file
 *
 * @authors    Pawel Okas
 * created on: 2014
 * last modification: <DD-MM-YYYY>
 *
 * @copyright Copyright (c) 2016, Pawel Okas
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 *     1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
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

#include <cstring>

#include "bsp.h"
#include "diagnostic/diagnostic.h"
#include "hostComm.h"
#include "microhal.h"

#include "lepton.h"
#include "picturePacket.h"

using namespace microhal;
using namespace microhal::diagnostic;
using namespace std::literals::chrono_literals;

HostComm hostComm(cameraPort, debugPort);

void proceedPacket(HostCommPacket &packet) {}

int main(void) {
    debugPort.write("\n\r------------------- FLIR Lepton example -------------------------\n\r");

    diagChannel.setOutputDevice(debugPort);
    // lets check if diagChannal is working
    diagChannel << lock << MICROHAL_EMERGENCY << "Information from diagnostic channel. Core frequency: " << hardware::Device::coreFrequency()
                << unlock;

    static Lepton lepton(leptonSPI, leptonI2C, leptonCS, leptonPower, leptonReset);

    // connect function that will be called when new packet will be received
    hostComm.incommingPacket.connect(proceedPacket);

    // create and run hostComm proc task
    std::thread hostCommThread([]() {
        while (1) {
            std::this_thread::sleep_for(1ms);
            hostComm.timeProc();
        }
    });

    lepton.startup();
    static PicturePacket picturePacket;
    Picture &picture = picturePacket.payload();

    uint32_t pictureNumber = 0;
    uint32_t pictureFailureCounter = 0;
    while (1) {
        auto beginTime = std::chrono::system_clock::now();
        while (1) {
            if (lepton.isNewPictureAvailable()) {
                picture.pictureNumber = pictureNumber++;
                diagChannel << lock << MICROHAL_INFORMATIONAL << "Sending picture " << unlock;
                memcpy(picture.data, lepton.getPicture(), lepton.getPictureSize());
                hostComm.send(picturePacket);
                break;
            }
            if (beginTime + 150ms < std::chrono::system_clock::now()) {
                pictureFailureCounter++;
                diagChannel << lock << MICROHAL_INFORMATIONAL << "Timeout while receiving Lepton picture. Failures: " << pictureFailureCounter
                            << unlock;
                break;
            }
            lepton.timeProc();
            std::this_thread::sleep_for(1ms);
        }
        std::this_thread::sleep_until(beginTime + 300ms);
    }

    return 0;
}
