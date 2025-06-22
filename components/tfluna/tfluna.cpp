#include "tfluna.h"
#include <cstddef>
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace tfluna {

// see https://files.waveshare.com/upload/a/ac/SJ-PM-TF-Luna_A05_Product_Manual.pdf
static const uint8_t VERSION_REVISION_REGISTER = 0x0A;
static const uint8_t VERSION_MINOR_REGISTER = 0x0B;
static const uint8_t VERSION_MAJOR_REGISTER = 0x0C;
static const uint8_t DISTANCE_LOW_REGISTER = 0x00;
static const uint8_t DISTANCE_HIGH_REGISTER = 0x01;
static const uint8_t MODE_REGISTER = 0x23;
static const uint8_t MODE_CONTINUOUS = 0x00;
static const uint8_t MODE_TRIGGER = 0x01;
static const uint8_t TRIGGER_ONESHOT_REGISTER = 0x24;
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
  if (! this->read_byte(VERSION_MAJOR_REGISTER, &major)) {
    ESP_LOGE(TAG, "Failed to get major firmware version");
    this->mark_failed();
    return;
  }
  uint8_t minor;
  if (! this->read_byte(VERSION_MINOR_REGISTER, &minor)) {
    ESP_LOGE(TAG, "Failed to get minor firmware version");
    this->mark_failed();
    return;
  }
  uint8_t revision;
  if (! this->read_byte(VERSION_REVISION_REGISTER, &revision)) {
    ESP_LOGE(TAG, "Failed to get revision firmware version");
    this->mark_failed();
    return;
  }
  ESP_LOGI(TAG, "TFLuna Firmware: %d.%d.%d", major, minor, revision);

  if (! this->write_byte(MODE_REGISTER, MODE_TRIGGER)) {
    ESP_LOGE(TAG, "Failed to set mode to trigger mode");
    this->mark_failed();
    return;
  }
}

void TFLuna::update() {
  if (! this->write_byte(TRIGGER_ONESHOT_REGISTER, 0x01)) {
    ESP_LOGE(TAG, "Failed to trigger a oneshot");
    this->status_set_warning();
    return;
  }

  this->set_timeout("read_distance", 50, [this]() {
    uint8_t distance_low;
    if (! this->read_byte(DISTANCE_LOW_REGISTER, &distance_low)) {
      ESP_LOGE(TAG, "Failed to get distance low");
      this->status_set_warning();
      return;
    }
    uint8_t distance_high;
    if (! this->read_byte(DISTANCE_HIGH_REGISTER, &distance_high, 1)) {
      ESP_LOGE(TAG, "Failed to get distance high");
      this->status_set_warning();
      return;
    }
    uint16_t distance = distance_low + distance_high * 256;

    ESP_LOGD(TAG, "Got distance=%d cm", distance);
    this->publish_state(distance);
    this->status_clear_warning();
  });
}

}  // namespace tfluna
}  // namespace esphome
