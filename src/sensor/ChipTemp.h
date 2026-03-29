#pragma once

#include <Arduino.h>

#include "debug.h"
#include "sensor/Sensor.h"

#if CONFIG_IDF_TARGET_ESP32C3
#include "driver/temperature_sensor.h"
#endif

namespace Sensor {

class ChipTemp : public Sensor {
private:
  float _temperatureC = NAN;
  uint32_t _sampleCount = 0;

#if CONFIG_IDF_TARGET_ESP32C3
  temperature_sensor_handle_t _handle = nullptr;
#endif

public:
  ChipTemp() : Sensor(Core::COMPONENT_CLASS_SENSOR, "chipTemp") {}

  float getTemperatureC() const {
    return _temperatureC;
  }

  void sample() {
#if CONFIG_IDF_TARGET_ESP32C3
    if (_handle == nullptr) {
      return;
    }
    float result = NAN;
    const esp_err_t err = temperature_sensor_get_celsius(_handle, &result);
    if (err != ESP_OK) {
      _LOGE(_name, "Chip temperature read failed: %d", err);
      return;
    }
    _temperatureC = result;
#else
    _temperatureC = temperatureRead();
#endif
    _sampleCount++;
  }

  void init() override {
#if CONFIG_IDF_TARGET_ESP32C3
    const temperature_sensor_config_t tsens =
        TEMPERATURE_SENSOR_CONFIG_DEFAULT(50, 125);
    esp_err_t err = temperature_sensor_install(&tsens, &_handle);
    if (err != ESP_OK) {
      _LOGE(_name, "Chip temperature install failed: %d", err);
      _handle = nullptr;
      return;
    }
    err = temperature_sensor_enable(_handle);
    if (err != ESP_OK) {
      _LOGE(_name, "Chip temperature enable failed: %d", err);
      _handle = nullptr;
      return;
    }
    _LOGI(_name, "Chip temperature sensor initialized for 50C..125C range.");
#else
    _LOGI(_name, "Chip temperature sensor initialized.");
#endif
    sample();
  }

  void timer1000() override {
    sample();
  }

  void getStatus(JsonObject &doc) const override {
    auto node = doc["sensor"][_name];
    node["cnt"] = _sampleCount;
    node["T"] = _temperatureC;
    Sensor::getStatus(doc);
  }
};

} // namespace Sensor
