#pragma once

#include "Actuator.h"
#include "PWMrelay.h"

namespace Actuators {


class Heater : public Actuator {
protected:

  uint8_t _value;
  bool _on = false;

  PWMrelay *_pwm;


public:
  Heater(const char *name,
              const float min,
              const float max,
              PWMrelay * pwm)
      : Actuator(Core::COMPONENT_CLASS_ACTUATOR, name),
      _pwm(pwm) {

    _min = 0;
    _max = 255;

    _a = (max - min) / (_max - _min);
    _b = min - _min * _a;

    __a = 1.0 / _a;
    __b = _b / _a;

    _inverted = min > max;
  };

  virtual int32_t setValue(int32_t pos) override {
    if (_on) {
      uint8_t val = constrain(pos, _min, _max);
      uint8_t oldVal = _value;
      _pwm->setPWM(val);
      _value = val;
      return val - oldVal;
    } else {
      return 0;
    }
  };

  virtual int32_t getValue() const override { return _value; };

  virtual bool isReady() const override { return true; }

  virtual void init() override {
    
  };

  virtual void onCommand(Core::command_type_t type, const JsonObject &doc) override {
    Actuator::onCommand(type, doc);
    if (type == Core::COMMAND_TYPE_ACTUATE) {
      auto node = doc["actuator"][_name];
    }
  }

  virtual void setConfig(const JsonObject &doc) override {
    Actuator::setConfig(doc);
    auto node = doc["actuator"][_name];
  };

  virtual void getInfo(JsonObject &doc) const override {
    Actuator::getInfo(doc);
    doc["actuator"][_name]["type"] = "heater";
  }

  virtual void loop() override {
    _pwm->tick();
  };

  virtual void onArtCommand(Core::command_type_t type, const JsonObject &doc, const JsonObject &reply) override {
    if (type == Core::COMMAND_TYPE_GET_DATA) {
      auto node = reply["data"];
      node[_name] = _value;
    }
    if (type == Core::COMMAND_TYPE_ON) {
      _on = true;
      setValue(1);
      auto node = reply["data"];
      node[_name] = _value;
    }  
    if (type == Core::COMMAND_TYPE_OFF) {
      setValue(0);
      _on = false;
      auto node = reply["data"];
      node[_name] = _value;
    }
    if (type == Core::COMMAND_SET_CONTROL_PARAMS) {
       if (!doc["data"][_name].isNull()) {
        setValue(doc["data"][_name]);
        auto node = reply["data"];
        node[_name] = _value;
       }
    }    
  };

};

} // namespace Actuators