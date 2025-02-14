#pragma once

#include "core/Component.h"
#include "debug.h"
#include "sensor/Sensor.h"
#include "MAX31865_NonBlocking.h"
#include <pt100rtd.h>
#include <Arduino.h>
#include <Wire.h>
#include <sensor/BTProbe.h>

namespace Sensor {

class ETProbe : public Sensor {

private:
  MAX31865 &_adc;
  uint32_t _conversionnCount = 0;
  uint8_t _fault;
  float _Tlut;

  MAX31865 &getAdc(uint8_t cs) {
    static MAX31865 __adc(cs);
    return __adc;
  }


public:
  ETProbe()
      : Sensor(Core::COMPONENT_CLASS_SENSOR, "ET"), 
      _adc(getAdc(0)){};

  void start() {
  }

  void read() {
    _fault = _adc.getFault();
    if (_fault) {
      if (_fault & MAX31865::FAULT_HIGHTHRESH_BIT) {
         _LOGW(_name, "RTD High Threshold");
      }
      if (_fault & MAX31865::FAULT_LOWTHRESH_BIT) {
       _LOGW(_name, "RTD Low Threshold");
      }
      if (_fault & MAX31865::FAULT_REFINLOW_BIT) {
        _LOGW(_name, "REFIN- > 0.85 x Bias");
      }
      if (_fault & MAX31865::FAULT_REFINHIGH_BIT) {
        _LOGW(_name, "REFIN- < 0.85 x Bias - FORCE- open");
      }
      if (_fault & MAX31865::FAULT_RTDINLOW_BIT) {
       _LOGW(_name, "RTDIN- < 0.85 x Bias - FORCE- open");
      }
      if (_fault & MAX31865::FAULT_OVUV_BIT) {
        _LOGW(_name, "Under/Over voltage");
      }
      _adc.clearFault();
    } else {
      auto x	= PT100.celsius(_adc.getResistance(RREF));
      if (_conversionnCount == 0) {
        _Tlut = x;
      } else {
        _Tlut += 0.1 * (x - _Tlut);
      }
    }
  }

  virtual void init() override {
    _adc.begin(MAX31865::RTD_3WIRE);
    _adc.setConvMode(MAX31865::CONV_MODE_SINGLE);
    _adc.setFilter(MAX31865::FILTER_50HZ);
    start();
    _LOGI(_name, "MAX31865 initialized and conversion is started.");
  };

  virtual void loop() override {
    if (_adc.isConversionComplete()) {
      read();
      start();
      _conversionnCount++;
      if (_cnt) {
        _cnt--;
      }
    }
  }

  virtual void getStatus(JsonObject &doc) const override {
    auto node = doc["sensor"][_name];
    node["cnt"] = _conversionnCount;
    node["fault"] = _fault;
    node["Tlut"] = _Tlut;
    Sensor::getStatus(doc);
  };

  virtual void setConfig(const JsonObject &doc) override {
    Sensor::setConfig(doc);
    auto node = doc["sensor"][_name];
  };

  virtual void onArtCommand(Core::command_type_t type, const JsonObject &doc, const JsonObject &reply) override {
    if (type == Core::COMMAND_TYPE_GET_DATA) {
      auto node = reply["data"];
      node[_name] = _Tlut;
    }  
  };
};

} // namespace Sensor
