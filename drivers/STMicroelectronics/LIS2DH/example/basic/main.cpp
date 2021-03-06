#include "diagnostic/diagnostic.h"
#include "lis2dh.h"
#include "microhal.h"
#include "microhal_bsp.h"

using namespace microhal;
using namespace diagnostic;
using namespace std::chrono_literals;

LIS2DH lis2dh(sensorI2C, LIS2DH::I2C_ADDRESS_0);

void lisInterrupt1() {
  LIS2DH::interruptSource source;
  lis2dh.checkInterrupt1Source(source);
  diagChannel << Informational << "Interrupt source:" << source.value;
  if (source.sources.IA) {
    diagChannel << Informational << "Interrupt" << endl;
  } else {
    diagChannel << Error
                << "Interrupt detected but not generated by Accelerometer"
                << endl;
  }
}

void lisInterrupt2() {}
void test0ReadTemp() {
  lis2dh.resetRegisters();
  diagChannel << lock << Informational << "======Test0======" << endl << unlock;
  diagChannel << lock << Informational << "Simple temperature reading" << endl
              << unlock;
  bool flag = true;

  do {
    flag = lis2dh.temperatureSensorEnable();
    if (false == flag) break;
    // temperature reading works only in discontinuous mode
    flag = lis2dh.ContinuousUpdate(false);
    if (false == flag) break;

  } while (0);

  for (int i = 0; (i < 50) && (flag == true); i++) {
    if (lis2dh.isTemperatureAvailable()) {
      auto temp = lis2dh.getTemperatue();
      if (temp) {
        diagChannel << lock << Informational
                    << "Temperature:" << temp->getCelsius() << endl
                    << unlock;
      }
    }
  }
  if (false == flag) {
    diagChannel << lock << MICROHAL_ERROR << "Test0 error" << endl << unlock;
  }
}
void test1PollRawRead() {
  lis2dh.resetRegisters();
  diagChannel << lock << Informational << "======Test1======" << endl << unlock;
  diagChannel << lock << Informational << "Simple  accelerate read test" << endl
              << unlock;
  bool flag = true;

  do {
    // it works without it,because defaults this is enabled
    // flag = lis2dh.enableAxis(true, true, true);
    if (false == flag) break;
    flag = lis2dh.setDataRate(LIS2DH::DataRate::Mode1Hz);
    if (false == flag) break;

    break;
  } while (1);

  int16_t x, y, z;
  for (int i = 0; (i < 10) && (flag == true); i++) {
    flag = lis2dh.getLastData(x, y, z);
    diagChannel << lock << Informational << "Raw X:" << x << " Raw Y:" << y
                << " Raw Z:" << z << endl
                << unlock;
    if (false == flag) {
      break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
  }
  if (false == flag) {
    diagChannel << lock << MICROHAL_ERROR << "Test1 error" << endl << unlock;
  }
}
void test2AccelerationRead() {
  lis2dh.resetRegisters();
  diagChannel << lock << Informational << "======Test2======" << endl << unlock;
  diagChannel << lock << Informational << "Accelerate reading test " << endl
              << unlock;
  bool flag = true;

  do {
    flag = lis2dh.enableAxis(true, true, true);
    if (false == flag) break;
    flag = lis2dh.setDataRate(LIS2DH::DataRate::Mode1Hz);
    if (false == flag) break;

    break;
  } while (1);

  int16_t x, y, z;
  for (int i = 0; (i < 10) && (flag == true); i++) {
    flag = lis2dh.getLastData(x, y, z);
    diagChannel << lock << Informational << "Raw X:" << x << " Raw Y:" << y
                << " Raw Z:" << z << endl
                << unlock;
    flag = lis2dh.getLastRawX(x);
    diagChannel << lock << Informational << "Only raw X:" << x << endl
                << unlock;
    flag = lis2dh.getLastRawY(y);
    diagChannel << lock << Informational << "Only raw Y:" << y << endl
                << unlock;
    flag = lis2dh.getLastRawZ(z);
    diagChannel << lock << Informational << "Only raw Z:" << z << endl
                << unlock;
    LIS2DH::AccelerationVector v;
    flag = lis2dh.getLastData(v);
    diagChannel << lock << Informational << "X:" << v[0] << " Y:" << v[1]
                << " Z:" << v[2] << endl
                << unlock;
    std::this_thread::sleep_for(std::chrono::milliseconds{100});
  }
  diagChannel << lock << MICROHAL_ERROR << "Test2 error" << endl << unlock;
}

void lisInterrupt1Clicktest() {
  LIS2DH::interruptSource source;
  lis2dh.checkInterrupt1Source(source);
  diagChannel << Informational << endl << "Interrupt source:" << source.value;
  LIS2DH::clickinterruptSource clickSource;
  lis2dh.checkClickSource(clickSource);
  diagChannel << Informational << " Click source:" << clickSource.value;
  if (clickSource.sources.IA) {
    diagChannel << Informational << " Click Interrupt";
    if (clickSource.sources.X) {
      diagChannel << Informational << " X";
    }
    if (clickSource.sources.Y) {
      diagChannel << Informational << " Y";
    }
    if (clickSource.sources.Z) {
      diagChannel << Informational << " Z";
    }
    if (clickSource.sources.Sign) {
      diagChannel << Informational << " Positive";
    } else {
      diagChannel << Informational << " Negative";
    }
    if (clickSource.sources.Sign) {
      diagChannel << Informational << " Single";
    }
    if (clickSource.sources.DClick) {
      diagChannel << Informational << " Double";
    }

  } else {
    diagChannel << Error
                << "Interrupt detected but not generated by Accelerometer"
                << endl;
  }
}
//  click interrupt test
void test3SingleClick() {
  diagChannel << lock << Informational << endl
              << "======Test3======" << endl
              << unlock;
  diagChannel << lock << Informational << "Single click detection " << endl
              << unlock;
  // rising edge is genereated on event, fallign edge occur after INT1_SRC read
  ExternalInterrupt::connect(lisInterrupt1Clicktest,
                             ExternalInterrupt::Trigger::OnRisingEdge, lisINT1);
  ExternalInterrupt::enable(lisINT1);

  lis2dh.setDataRate(LIS2DH::DataRate::Mode400Hz);
  lis2dh.interrupt1EnableSource(LIS2DH::Interrupt1Sources::Click);  // CTRL_REG3
  lis2dh.setScale(LIS2DH::Scale::Scale4G);                          // REG4
  lis2dh.interrupt1LatchRequest(true);                              // REG5

  Acceleration threshold(3.125f);  // 3.125G
  std::chrono::microseconds timeLimit = 127ms;
  std::chrono::microseconds timeLatency = std::chrono::microseconds::zero();
  std::chrono::microseconds timeWindow = std::chrono::microseconds::zero();
  lis2dh.setClick(threshold, timeLimit, timeLatency, timeWindow, true, true,
                  true, true, false);

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  ExternalInterrupt::disable(lisINT1);
  ExternalInterrupt::disconnect(lisInterrupt1Clicktest, lisINT1);
}

void lisInterrupt1test4() {
  LIS2DH::interruptSource source;
  lis2dh.checkInterrupt1Source(source);
  diagChannel << Informational << "Interrupt source:" << source.value << endl;

  LIS2DH::AccelerationVector v;
  lis2dh.getLastData(v);
  diagChannel << Informational << "X:" << v[0] << " Y:" << v[1] << " Z:" << v[2]
              << endl;
}
// Test4 - Data ready interrupt test(with interrupt latch)
void test4IntRead() {
  diagChannel << lock << Informational << endl
              << "======Test4======" << endl
              << unlock;
  diagChannel << lock << Informational
              << "Read last data using data ready interrupt " << endl
              << unlock;
  // rising edge is genereated on event, fallign edge occur after INT1_SRC read
  ExternalInterrupt::connect(lisInterrupt1test4,
                             ExternalInterrupt::Trigger::OnRisingEdge, lisINT1);
  ExternalInterrupt::enable(lisINT1);
  lis2dh.resetRegisters();
  lis2dh.setDataRate(LIS2DH::DataRate::Mode10Hz);
  lis2dh.enableAxis(true, true, true);
  lis2dh.interrupt1EnableSource(LIS2DH::Interrupt1Sources::DRDY1);  // CTRL_REG3
  lis2dh.setScale(LIS2DH::Scale::Scale4G);                          // CTRL_REG4
  lis2dh.interrupt1LatchRequest(true);
  lis2dh.interruptSetLevel(LIS2DH::interruptLevel::HIGH);  // CTRL_REG6

  // dummy read because interrupt is generated only on new set of data
  LIS2DH::AccelerationVector v;
  lis2dh.getLastData(v);
  // wait 5 seconds and go to next test
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  ExternalInterrupt::disable(lisINT1);
  ExternalInterrupt::disconnect(lisInterrupt1test4, lisINT1);
}

void lisInterrupt1FreeFall() {
  LIS2DH::interruptSource source;
  lis2dh.checkInterrupt1Source(source);
  diagChannel << Informational << "Interrupt source:" << source.value;
  if (source.sources.IA) {
    diagChannel << Informational << "Freefall" << endl;
  } else {
    diagChannel << Error
                << "Interrupt detected but not generated by Accelerometer"
                << endl;
  }
}
// freefall detection test
void test5FreeFall() {
  diagChannel << lock << Informational << endl
              << "======Test6======" << endl
              << unlock;
  diagChannel << lock << Informational << "Freefall detection " << endl
              << unlock;
  // rising edge is genereated on event, fallign edge occur after INT1_SRC read
  ExternalInterrupt::connect(lisInterrupt1FreeFall,
                             ExternalInterrupt::Trigger::OnRisingEdge, lisINT1);
  ExternalInterrupt::enable(lisINT1);
  lis2dh.resetRegisters();

  lis2dh.setDataRate(LIS2DH::DataRate::Mode100Hz);
  lis2dh.enableAxis(true, true, true);

  lis2dh.disableHighPassFilters();

  lis2dh.interrupt1EnableSource(LIS2DH::Interrupt1Sources::AOI1);  //
  lis2dh.setScale(LIS2DH::Scale::Scale2G);
  lis2dh.interrupt1LatchRequest(true);
  lis2dh.configureFreefall(0x16, 0x03);
  lis2dh.interruptSetLevel(LIS2DH::interruptLevel::HIGH);  // CTRL_REG6
  lis2dh.configureInterrupt1source(LIS2DH::INT1Source::AndCombination);
  lis2dh.configureInterrupt1source(LIS2DH::INT1Source::XLowEvent);
  lis2dh.configureInterrupt1source(LIS2DH::INT1Source::YLowEvent);
  lis2dh.configureInterrupt1source(LIS2DH::INT1Source::ZLowEvent);

  // wait 5 seconds and go to next test
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  ExternalInterrupt::disable(lisINT1);
  ExternalInterrupt::disconnect(lisInterrupt1FreeFall, lisINT1);
}
void test6DoubleClick() {
  diagChannel << lock << Informational << endl
              << "======Test6======" << endl
              << unlock;
  diagChannel << lock << Informational << "Double click detection " << endl
              << unlock;
  // rising edge is genereated on event, fallign edge occur after INT1_SRC read
  ExternalInterrupt::connect(lisInterrupt1Clicktest,
                             ExternalInterrupt::Trigger::OnRisingEdge, lisINT1);
  ExternalInterrupt::enable(lisINT1);
  lis2dh.resetRegisters();
  lis2dh.setDataRate(LIS2DH::DataRate::Mode400Hz);
  lis2dh.interrupt1EnableSource(LIS2DH::Interrupt1Sources::Click);  // CTRL_REG3
  lis2dh.setScale(LIS2DH::Scale::Scale4G);                          // REG4
  lis2dh.interrupt1LatchRequest(true);                              // REG5

  Acceleration threshold(3.125f);  // 3.125G
  std::chrono::microseconds timeLimit = 127ms;
  std::chrono::microseconds timeLatency = 52ms;  // 0x15
  std::chrono::microseconds timeWindow = 565ms;
  lis2dh.setClick(threshold, timeLimit, timeLatency, timeWindow, true, true,
                  true, false, true);

  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  // ExternalInterrupt::disable(lisINT1);
  // ExternalInterrupt::disconnect(lisInterrupt1Clicktest, lisINT1);
}

int main(void) {
  ExternalInterrupt::init();
  debugPort.write("\n\r----------------- LIS2DH Demo -----------------\n\r");
  diagChannel.setOutputDevice(debugPort);

  GPIO CS(lisCS, GPIO::Direction::Output);
  CS.set();
  GPIO SA0(lisSA0, GPIO::Direction::Output);
  SA0.reset();

  if (false == lis2dh.init()) {
    diagChannel << lock << MICROHAL_ERROR << "Cannot initialize LIS2DH" << endl
                << unlock;
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  // test0ReadTemp();  // this test not work
  // test1PollRawRead();
  // test2AccelerationRead();
  // test3SingleClick();
  // test4IntRead();
  // test5FreeFall();  // freefall detection
  test6DoubleClick();
  while (1) {
  }

  return 0;
}
