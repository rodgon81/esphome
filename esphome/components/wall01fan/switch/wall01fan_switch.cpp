#include "wall01fan_switch.h"

#include "esphome/core/log.h"

namespace esphome {
namespace wall01fan {

static const char *const TAG = "wall01fan.switch";

void Wall01fanSwitch::write_state(bool state) { this->parent_->set_switch_value(this->entity_id_, state); }

}  // namespace wall01fan
}  // namespace esphome
