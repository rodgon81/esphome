#pragma once
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace wall01fan {

class Wall01fanBinaryInputOutput : public Component {
   public:
    float get_setup_priority() const override;
    void dump_config() override;
    void setup() override;

    void set_mode_input(gpio::Flags flags);
    void set_mode_output(gpio::Flags flags);

    void set_state(bool state);
    bool get_state();

    bool is_inverted();

    void set_inverted(bool inverted);
    void set_pin(InternalGPIOPin *pin);

   protected:
    bool inverted_ = false;

    InternalGPIOPin *pin_;

    gpio::Flags flags_input_;
    gpio::Flags flags_output_;
};

}  // namespace wall01fan
}  // namespace esphome
