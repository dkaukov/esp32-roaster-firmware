#pragma once

#include "core/Component.h"
#include "debug.h"
#include "sensor/Sensor.h"
#include <Arduino.h>
#include <HX711_ADC.h>

namespace Sensor {

class Scale : public Sensor {

  private:
  HX711_ADC &_adc;
  uint32_t _conversionnCount = 0;
  bool _tareComplete = false;
  float _W;

  HX711_ADC &getAdc(uint8_t dout, uint8_t sck) {
    static HX711_ADC __adc(dout, sck);
    return __adc;
  }

public:
  Scale(uint8_t dout, uint8_t sck)
      : Sensor(Core::COMPONENT_CLASS_SENSOR, "scale"), 
      _adc(getAdc(dout, sck)){};

  void start() {
  }  

  virtual void init() override {
    _adc.begin();
    _adc.start(2000, false);
    _adc.tareNoDelay();
    _tareComplete = false;
    _LOGI(_name, "HX711 initialized and conversion is started.");
  };

  virtual void loop() override {
    if (_adc.getTareStatus()) {
      _LOGI(_name, "HX711 Tare complete.");
      _tareComplete = true;
    }
    if (_adc.dataWaitingAsync()) {
      if (_adc.updateAsync() && _tareComplete) 
        if (_conversionnCount == 0) {
          _W = _adc.getData();
        } else {
          //_W += 0.1 * (_adc.getData() - _W);
          _W = _adc.getData();
        }
        _conversionnCount++;
      }
  };
  
  virtual void getStatus(JsonObject &doc) const override {
    auto node = doc["sensor"][_name];
    node["cnt"] = _conversionnCount;
    node["adc"] = _W;
    node["W"] = 0.0366*_W + 3.315;
    Sensor::getStatus(doc);
  };

  virtual void onArtCommand(Core::command_type_t type, const JsonObject &doc, const JsonObject &reply) override {
    if (type == Core::COMMAND_TYPE_TARE) {
      _conversionnCount = 0;
      _W = 0.0;
      _adc.tareNoDelay();
      _LOGI(_name, "HX711 Tare requested.");
    } 
    if (type == Core::COMMAND_TYPE_GET_DATA) {
      auto node = reply["data"];
      node[_name] = 0.0366*_W + 3.315;
    }  
  };

};    

} // namespace Sensor