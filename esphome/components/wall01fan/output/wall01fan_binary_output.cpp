#include "wall01fan_binary_output.h"

#include "esphome/core/log.h"

namespace esphome {
namespace wall01fan {

static const char *const TAG = "wall01fan.binary_output";

void Wall01fanBinaryOutput::set_pin(InternalGPIOPin *pin) { pin_ = pin; }

void Wall01fanBinaryOutput::setup() {
    this->pin_->setup();
    this->turn_off();
}

void Wall01fanBinaryOutput::dump_config() {
    ESP_LOGCONFIG(TAG, "Binary Output:");
    LOG_PIN("  Pin: ", this->pin_);
    LOG_BINARY_OUTPUT(this);
}

void Wall01fanBinaryOutput::write_state(bool state) { this->pin_->digital_write(state); }

float Wall01fanBinaryOutput::get_setup_priority() const { return setup_priority::HARDWARE; }

}  // namespace wall01fan
}  // namespace esphome