#include "tfluna.h"
#include <cstddef>
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace tfluna {

// see https://files.waveshare.com/upload/a/ac/SJ-PM-TF-Luna_A05_Product_Manual.pdf
static const char *const TAG = "tfluna";
static const unsigned char DATA[] = {0x5A,0x05,0x00,0x01,0x60};
static const size_t DATA_LENGTH = 5;
static const size_t RESPONSE_LENGTH = 9;

void TFLuna::dump_config() {
  ESP_LOGCONFIG(TAG, "TF-Luna (i2c):");
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  LOG_SENSOR("  ", "Sensor:", this);
}

void TFLuna::update() {

  auto write_error = this->write(DATA, DATA_LENGTH);
  if (write_error != i2c::ERROR_OK) {
    ESP_LOGD(TAG, "Error writing data: %d", write_error);
    return;
  }

  this->set_timeout("read_distance", 1000, [this]() {
    uint8_t i2c_response[RESPONSE_LENGTH];
    auto read_error = this->read(i2c_response, RESPONSE_LENGTH);
    if read_error != i2c::ERROR_OK) {
      ESP_LOGD(TAG, "Error reading data: %d", read_error);
      this->status_set_warning();
      return;
    }

    uint16_t distance = i2c_response[2] + i2c_response[3] * 256;
    ESP_LOGD(TAG, "Got distance=%d cm", distance);
    this->publish_state(distance);
    this->status_clear_warning();
  });
}

}  // namespace tfluna
}  // namespace esphome
