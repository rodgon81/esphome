#pragma once

#include "../wall01fan.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace wall01fan {

class Wall01fanSwitch : public switch_::Switch {
 public:
  void set_entity_id(SmIdEntity entity_id) { this->entity_id_ = entity_id; }
  void set_wall01fan_parent(Wall01fanComponent *parent) { this->parent_ = parent; }

 protected:
  void write_state(bool state) override;

  Wall01fanComponent *parent_;
  SmIdEntity entity_id_ = SmIdEntity::ID_NULL;
};

}  // namespace wall01fan
}  // namespace esphome
