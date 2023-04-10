#pragma once

#include "esphome/core/automation.h"
#include "wall01fan.h"

namespace esphome {
namespace wall01fan {

template <typename... Ts>
class RemoteReceiverAction : public Action<Ts...> {
   public:
    explicit RemoteReceiverAction(Wall01fanComponent *parent) : parent_(parent) {}

    TEMPLATABLE_VALUE(uint8_t, protocol)
    TEMPLATABLE_VALUE(uint64_t, code)

    void play(Ts... x) override { this->parent_->remote_receiver(this->protocol_.value(x...), this->code_.value(x...)); }

   protected:
    Wall01fanComponent *parent_;
};

//-----------------------------------------------------------------------------------

template <typename... Ts>
class PowerTurnOnAction : public Action<Ts...> {
   public:
    explicit PowerTurnOnAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->power_turn_on(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class PowerTurnOffAction : public Action<Ts...> {
   public:
    explicit PowerTurnOffAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->power_turn_off(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class PowerToggleAction : public Action<Ts...> {
   public:
    explicit PowerToggleAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->power_toggle(); }

   protected:
    Wall01fanComponent *parent_;
};

//-----------------------------------------------------------------------------------

template <typename... Ts>
class OscillationTurnOnAction : public Action<Ts...> {
   public:
    explicit OscillationTurnOnAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->oscillation_turn_on(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class OscillationTurnOffAction : public Action<Ts...> {
   public:
    explicit OscillationTurnOffAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->oscillation_turn_off(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class OscillationToggleAction : public Action<Ts...> {
   public:
    explicit OscillationToggleAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->oscillation_toggle(); }

   protected:
    Wall01fanComponent *parent_;
};

//-----------------------------------------------------------------------------------

template <typename... Ts>
class SpeedNoneAction : public Action<Ts...> {
   public:
    explicit SpeedNoneAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->speed_none(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class SpeedLowAction : public Action<Ts...> {
   public:
    explicit SpeedLowAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->speed_low(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class SpeedMidAction : public Action<Ts...> {
   public:
    explicit SpeedMidAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->speed_mid(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class SpeedHighAction : public Action<Ts...> {
   public:
    explicit SpeedHighAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->speed_high(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class SpeedToggleAction : public Action<Ts...> {
   public:
    explicit SpeedToggleAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->speed_toggle(); }

   protected:
    Wall01fanComponent *parent_;
};

//-----------------------------------------------------------------------------------

template <typename... Ts>
class ModeNoneAction : public Action<Ts...> {
   public:
    explicit ModeNoneAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->mode_none(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class ModeNormalAction : public Action<Ts...> {
   public:
    explicit ModeNormalAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->mode_normal(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class ModeNaturalAction : public Action<Ts...> {
   public:
    explicit ModeNaturalAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->mode_natural(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class ModeSleepAction : public Action<Ts...> {
   public:
    explicit ModeSleepAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->mode_sleep(); }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class ModeToggleAction : public Action<Ts...> {
   public:
    explicit ModeToggleAction(Wall01fanComponent *parent) : parent_(parent) {}

    void play(Ts... x) override { this->parent_->mode_toggle(); }

   protected:
    Wall01fanComponent *parent_;
};

//-----------------------------------------------------------------------------------

template <typename... Ts>
class IsPowerOnCondition : public Condition<Ts...> {
   public:
    IsPowerOnCondition(Wall01fanComponent *parent) : parent_(parent) {}
    bool check(Ts... x) override { return this->parent_->fan_data_.is_power_on; }

   protected:
    Wall01fanComponent *parent_;
};

template <typename... Ts>
class IsTemperatureModeCondition : public Condition<Ts...> {
   public:
    IsTemperatureModeCondition(Wall01fanComponent *parent) : parent_(parent) {}
    bool check(Ts... x) override { return this->parent_->fan_data_.is_temperature_mode; }

   protected:
    Wall01fanComponent *parent_;
};

//-----------------------------------------------------------------------------------

class BeepTrigger : public Trigger<> {
   public:
    BeepTrigger(Wall01fanComponent *parent) {
        parent->add_on_beep_callback([this](bool state) {
            if (state) {
                this->trigger();
            }
        });
    }
};

}  // namespace wall01fan
}  // namespace esphome
