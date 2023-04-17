#include "wall01fan_binary_input_output.h"

#include "esphome/core/log.h"

namespace esphome {
namespace wall01fan {

static const char *const TAG = "wall01fan.binary_input_output";

float Wall01fanBinaryInputOutput::get_setup_priority() const { return setup_priority::HARDWARE; }

void Wall01fanBinaryInputOutput::dump_config() {
  ESP_LOGCONFIG(TAG, "wall01fan Binary Input Output:");
  LOG_PIN("  Pin: ", this->pin_);
  if (this->inverted_) {
    ESP_LOGCONFIG(TAG, "  Inverted: YES");
  }
}

void Wall01fanBinaryInputOutput::setup() { this->pin_->setup(); }

void Wall01fanBinaryInputOutput::set_mode_input(gpio::Flags flags) { this->flags_input_ = flags; }
void Wall01fanBinaryInputOutput::set_mode_output(gpio::Flags flags) { this->flags_output_ = flags; }

void Wall01fanBinaryInputOutput::set_state(bool state) {
  if (state) {
    this->pin_->pin_mode(this->flags_output_);
    this->pin_->digital_write(!this->inverted_);
  } else {
    this->pin_->pin_mode(this->flags_output_);
    this->pin_->digital_write(this->inverted_);
  }
}

bool Wall01fanBinaryInputOutput::get_state() {
  this->pin_->pin_mode(this->flags_input_);
  return this->pin_->digital_read();
}

bool Wall01fanBinaryInputOutput::is_inverted() { return this->inverted_; }

void Wall01fanBinaryInputOutput::set_inverted(bool inverted) { this->inverted_ = inverted; }
void Wall01fanBinaryInputOutput::set_pin(InternalGPIOPin *pin) { this->pin_ = pin; }

}  // namespace wall01fan
}  // namespace esphome
