#pragma once
#include "esphome/components/output/binary_output.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace wall01fan {

class Wall01fanBinaryOutput : public output::BinaryOutput, public Component {
 public:
  void set_pin(InternalGPIOPin *pin);

  void setup() override;

  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  void write_state(bool state) override;

  InternalGPIOPin *pin_;
};

}  // namespace wall01fan
}  // namespace esphome
