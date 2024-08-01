#pragma once

#include "Actuator.h"
#include "PWMrelay.h"

namespace Actuators {


class Heater : public Actuator {
protected:

  uint8_t _value;

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
    uint8_t val = constrain(pos, _min, _max);
    uint8_t oldVal = _value;
    _pwm->setPWM(val);
    _value = val;
    return val - oldVal;
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

};

} // namespace Actuators