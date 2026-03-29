#pragma once

#include <Arduino.h>

#include "acuators/Heater.h"
#include "config.h"
#include "core/Component.h"
#include "sensor/ChipTemp.h"

namespace Core {

class StatusLed : public Component {
private:
  static constexpr float CHIP_TEMP_WARN_C = 105.0f;
  static constexpr uint32_t CHIP_TEMP_BLINK_MS = 500;

  Actuators::Heater *_heater;
  Sensor::ChipTemp *_chipTemp;
  uint32_t _lastBlinkMs = 0;
  bool _blinkOn = false;

  void applyColor(uint8_t red, uint8_t green, uint8_t blue) const {
    rgbLedWrite(LED_PIN, red, green, blue);
  }

public:
  explicit StatusLed(Actuators::Heater *heater, Sensor::ChipTemp *chipTemp)
      : Component(COMPONENT_CLASS_GENERIC), _heater(heater), _chipTemp(chipTemp) {}

  void setBootColor() const {
    applyColor(255, 255, 255);
    applyColor(255, 255, 255);
    applyColor(255, 255, 255);
  }

  void init() override {
    timer250();
  }

  void timer250() override {
    const float chipTempC = _chipTemp ? _chipTemp->getTemperatureC() : NAN;
    if (chipTempC >= CHIP_TEMP_WARN_C) {
      const uint32_t now = millis();
      if ((now - _lastBlinkMs) >= CHIP_TEMP_BLINK_MS) {
        _blinkOn = !_blinkOn;
        _lastBlinkMs = now;
      }
      applyColor(_blinkOn ? 255 : 0, 0, 0);
      return;
    }

    _blinkOn = false;
    _lastBlinkMs = millis();
    if (_heater && _heater->isOn()) {
      applyColor(255, 0, 0);
    } else {
      applyColor(0, 0, 255);
    }
  }
};

} // namespace Core
