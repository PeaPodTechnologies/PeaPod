#ifndef PEAPOD_AIR_H_
#error __FILE__ should only be included AFTER <air.h>
#endif

#ifdef PEAPOD_AIR_H_

#ifndef PEAPOD_AIR_T_
#define PEAPOD_AIR_T_

#include <DebugJson.h>

template <unsigned char M> void PeaPod::PeaPodModuleAir::callback_sht45_mean(bool _, const FSM::fsm_timestamp_t& __) {
  if(M < 0 || M > I2CIP_MUX_COUNT || I2CIP::modules[M] == nullptr) return;

  // If the module is not in an errored state,
  if(I2CIP::errlev[M] == I2CIP_ERR_NONE) {
    // Get SHT45 device group
    I2CIP::DeviceGroup* dg_sht45 = I2CIP::modules[M]->operator[]("SHT45");

    // If we have devices,
    if(dg_sht45 != nullptr && dg_sht45->getNumDevices() > 0) {

      // Read from all devices and take a mean
      state_sht45_t th = {0.0f, 0.0f}; uint8_t c = 0;
      for(uint8_t i = 0; i < dg_sht45->getNumDevices(); i++) {
        SHT45* d = (SHT45*)(dg_sht45->getDevice(i));
        if(d == nullptr) continue;

        i2cip_errorlevel_t errlev_sht45 = I2CIP::modules[M]->operator()<SHT45>(d->getFQA(), true, _i2cip_args_io_default, DebugJsonBreakpoints);
        if(errlev_sht45 != I2CIP_ERR_NONE) continue;

        th.temperature += d->getCache().temperature;
        th.humidity += d->getCache().humidity;
        c++;
      }

      // If readings were taken,
      if(c > 0) {
        // Set variables according to averages
        PeaPod::air_temperature.set(th.temperature / c);
        PeaPod::air_humidity.set(th.humidity / c);
      }
    }
  }
}

template <unsigned char M> void PeaPod::PeaPodModuleAir::callback_k30_mean(bool _, const FSM::fsm_timestamp_t& __) {
  if(M < 0 || M > I2CIP_MUX_COUNT || I2CIP::modules[M] == nullptr) return;

  // If the module is not in an errored state,
  if(I2CIP::errlev[M] == I2CIP_ERR_NONE) {
    // Get K30 device group
    I2CIP::DeviceGroup* dg_k30 = I2CIP::modules[M]->operator[]("K30");

    // If we have devices,
    if(dg_k30 != nullptr && dg_k30->getNumDevices() > 0) {

      // Read from all devices and take a mean
      uint16_t co2 = 0; uint8_t c = 0;
      for(uint8_t i = 0; i < dg_k30->getNumDevices(); i++) {
        K30* d = (K30*)(dg_k30->getDevice(i));
        if(d == nullptr) continue;

        i2cip_errorlevel_t errlev_k30 = I2CIP::modules[M]->operator()<K30>(d->getFQA(), true, _i2cip_args_io_default, DebugJsonBreakpoints);
        if(errlev_k30 != I2CIP_ERR_NONE) continue;

        co2 += d->getCache();
        c++;
      }

      // If readings were taken,
      if(c > 0) {
        // Set variable according to average
        PeaPod::air_co2.set(co2 / c);
      }
    }
  }
}

#endif

#endif