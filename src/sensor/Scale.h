#pragma once

#include "core/Component.h"
#include "debug.h"
#include "sensor/Sensor.h"
#include <Arduino.h>
#include <HX711_ADC.h>

namespace Sensor {

constexpr float SCALE_GRAMS_PER_ADC = 0.0031091391f;
constexpr float SCALE_ADC_LPF_ALPHA = 0.25f;
constexpr uint8_t SCALE_ADC_MEDIAN_WINDOW = 3;

class Scale : public Sensor {

  private:
  HX711_ADC &_adc;
  uint32_t _conversionnCount = 0;
  bool _tareComplete = false;
  float _W;
  float _rawHistory[SCALE_ADC_MEDIAN_WINDOW] = {0.0f, 0.0f, 0.0f};
  uint8_t _rawHistoryCount = 0;
  uint8_t _rawHistoryIndex = 0;

  HX711_ADC &getAdc(uint8_t dout, uint8_t sck) {
    static HX711_ADC __adc(dout, sck);
    return __adc;
  }

  float median3(float a, float b, float c) const {
    if (a > b) {
      float tmp = a;
      a = b;
      b = tmp;
    }
    if (b > c) {
      float tmp = b;
      b = c;
      c = tmp;
    }
    if (a > b) {
      float tmp = a;
      a = b;
      b = tmp;
    }
    return b;
  }

  float filteredRawSample(float raw) {
    _rawHistory[_rawHistoryIndex] = raw;
    _rawHistoryIndex = (_rawHistoryIndex + 1) % SCALE_ADC_MEDIAN_WINDOW;
    if (_rawHistoryCount < SCALE_ADC_MEDIAN_WINDOW) {
      _rawHistoryCount++;
    }
    if (_rawHistoryCount < SCALE_ADC_MEDIAN_WINDOW) {
      return raw;
    }
    return median3(_rawHistory[0], _rawHistory[1], _rawHistory[2]);
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
      _W = 0.0f;
      _rawHistoryCount = 0;
      _rawHistoryIndex = 0;
    }
    if (_adc.dataWaitingAsync()) {
      if (_adc.updateAsync() /*&& _tareComplete*/) {
        const float raw = filteredRawSample(_adc.getData());
        if (_conversionnCount == 0) {
          _W = raw;
        } else {
          _W += SCALE_ADC_LPF_ALPHA * (raw - _W);
        }
        _conversionnCount++;
      }
    }
  };
  
  virtual void getStatus(JsonObject &doc) const override {
    auto node = doc["sensor"][_name];
    node["cnt"] = _conversionnCount;
    node["adc"] = _W;
    node["W"] = SCALE_GRAMS_PER_ADC * _W;
    Sensor::getStatus(doc);
  };

  virtual void onArtCommand(Core::command_type_t type, const JsonObject &doc, const JsonObject &reply) override {
    if (type == Core::COMMAND_TYPE_TARE) {
      _conversionnCount = 0;
      _W = 0.0;
      _rawHistoryCount = 0;
      _rawHistoryIndex = 0;
      _adc.tareNoDelay();
      _LOGI(_name, "HX711 Tare requested.");
    } 
    if (type == Core::COMMAND_TYPE_GET_DATA) {
      auto node = reply["data"];
      node[_name] = SCALE_GRAMS_PER_ADC * _W;
    }  
  };

};    

} // namespace Sensor
