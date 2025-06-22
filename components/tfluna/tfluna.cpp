#include "tfluna.h"
#include <cstddef>
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace tfluna {

// see https://files.waveshare.com/upload/a/ac/SJ-PM-TF-Luna_A05_Product_Manual.pdf
static const uint8_t VERSION_REVISION = 0x0A;
static const uint8_t VERSION_MINOR = 0x0B;
static const uint8_t VERSION_MAJOR = 0x0C;
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

void TFLuna::setup() {
  ESP_LOGI(TAG, "Setting up TFLuna...");
  uint8_t major;
  if (this->read_register(VERSION_MAJOR, &major, 1) !=
      i2c::ERROR_OK) {
    ESP_LOGE(TAG, "TFLuna Setup Failed");
    this->mark_failed();
    return;
  }
  uint8_t minor;
  if (this->read_register(VERSION_MINOR, &minor, 1) !=
      i2c::ERROR_OK) {
    ESP_LOGE(TAG, "TFLuna Setup Failed");
    this->mark_failed();
    return;
  }
  uint8_t revision;
  if (this->read_register(VERSION_REVISION, &revision, 1) !=
      i2c::ERROR_OK) {
    ESP_LOGE(TAG, "TFLuna Setup Failed");
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "MiniEncoderC Firmware: %d.%d.%d", major, minor, revision);
}

void TFLuna::update() {
  this->setup();

  auto write_error = this->write(DATA, DATA_LENGTH);
  if (write_error != i2c::ERROR_OK) {
    ESP_LOGD(TAG, "Error writing data: %d", write_error);
    return;
  }

  this->set_timeout("read_distance", 1000, [this]() {
    uint8_t i2c_response[RESPONSE_LENGTH];
    auto read_error = this->read(i2c_response, RESPONSE_LENGTH);
    if (read_error != i2c::ERROR_OK) {
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
