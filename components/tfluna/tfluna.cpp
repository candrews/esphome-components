#include "tfluna.h"
#include <cstddef>
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace tfluna {

static const char *const TAG = "tfluna";
static const unsigned char DATA[] = {0x5A,0x05,0x00,0x01,0x60};
static const size_t DATA_LENGTH = 9;

void TFLuna::dump_config() {
  ESP_LOGCONFIG(TAG, "TF-Luna (i2c):");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Sensor:", this);
}

void TFLuna::update() {

  if (this->write(DATA, DATA_LENGTH) != i2c::ERROR_OK) {
    return;
  }

  this->set_timeout("read_distance", 1000, [this]() {
    uint8_t i2c_response[DATA_LENGTH];
    if (this->read(i2c_response, DATA_LENGTH) != i2c::ERROR_OK) {
      this->status_set_warning();
      return;
    }

    uint16_t distance = i2c_response[2] + i2c_response[3] * 256;
    ESP_LOGD(TAG, "Got distance=%lldl cm", distance);
    this->publish_state(distance);
    this.status_clear_warning();
  });
}

}  // namespace tfluna
}  // namespace esphome
