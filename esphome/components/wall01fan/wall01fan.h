#pragma once

#include "esphome/core/application.h"
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif
#ifdef USE_BINARY_OUTPUT
#include "esphome/components/output/binary_output.h"
#endif
#ifdef USE_BINARY_INPUT_OUTPUT
#include "binary_sensor/wall01fan_binary_input_output.h"
#endif

namespace esphome {
namespace wall01fan {

//------------------------------------------------------------------------------
// DEFAULTS
//------------------------------------------------------------------------------

static const uint16_t SM_WAIT_TO_LOOP_AND_UPDATE_STATE = 2000;
static const uint8_t CLICK_PRESS_TIME = 150;

static const uint16_t INFRARED_POWER = 1728;
static const uint16_t INFRARED_OSCILLATION = 1736;
static const uint16_t INFRARED_SPEED = 1729;
static const uint16_t INFRARED_MODE = 1730;
static const uint16_t INFRARED_TIMER = 1732;

const std::string TIMER_REMAINING_NAME = "timer_remaining_name";
const std::string TIMER_MODE_NAME = "timer_mode_name";

//------------------------------------------------------------------------------

enum class SmIdEntity : uint8_t {
    ID_NULL = 0,

    SWITCH_POWER = 21,
    SWITCH_OSCILLATION = 22,
    SWITCH_SPEED_LOW = 23,
    SWITCH_SPEED_MID = 24,
    SWITCH_SPEED_HIGH = 25,
    SWITCH_MODE_NORMAL = 26,
    SWITCH_MODE_NATURAL = 27,
    SWITCH_MODE_SLEEP = 28,
    SWITCH_TIMER_05H = 29,
    SWITCH_TIMER_1H = 30,
    SWITCH_TIMER_2H = 31,
    SWITCH_TIMER_4H = 32,
    SWITCH_MORE_CONFIG = 33,
};

enum class FanClickState : uint8_t {
    STATE_CLEAN,
    STATE_START_PRESS,
    STATE_PRESS,
};

enum class FanButton : uint8_t {
    BUTTON_POWER,
    BUTTON_OSCILLATION,
    BUTTON_SPEED,
    BUTTON_MODE,
    BUTTON_TIMER,
};

enum FanSpeed : uint8_t {
    SPEED_NONE = 0,
    SPEED_LOW = 1,
    SPEED_MID = 2,
    SPEED_HIGH = 3,
};

enum class FanMode : uint8_t {
    MODE_NONE,
    MODE_NORMAL,
    MODE_NATURAL,
    MODE_SLEEP,
};

enum FanTimer : uint16_t {
    TIMER_0H_00M = 0,  // minutes
    TIMER_0H_30M = 30,
    TIMER_1H_00M = 60,
    TIMER_1H_30M = 90,
    TIMER_2H_00M = 120,
    TIMER_2H_30M = 150,
    TIMER_3H_00M = 180,
    TIMER_3H_30M = 210,
    TIMER_4H_00M = 240,
    TIMER_4H_30M = 270,
    TIMER_5H_00M = 300,
    TIMER_5H_30M = 330,
    TIMER_6H_00M = 360,
    TIMER_6H_30M = 390,
    TIMER_7H_00M = 420,
    TIMER_7H_30M = 450,
};

struct FanStateData {
    uint16_t seconds_time_remaining = 0;

    bool is_power_on = false;
    bool is_oscillation_on = false;

    bool is_speed_low = false;
    bool is_speed_mid = false;
    bool is_speed_high = false;

    bool is_mode_normal = false;
    bool is_mode_natural = false;
    bool is_mode_sleep = false;

    bool is_timer_05h = false;
    bool is_timer_1h = false;
    bool is_timer_2h = false;
    bool is_timer_4h = false;

    bool is_natural_or_sleep_mode = false;

    bool is_temperature_mode = false;

    FanSpeed selected_speed = FanSpeed::SPEED_NONE;
    FanMode selected_mode = FanMode::MODE_NONE;
    FanTimer selected_timer = FanTimer::TIMER_0H_00M;

    FanClickState click_sate_row_1 = FanClickState::STATE_CLEAN;
    FanClickState click_sate_row_2 = FanClickState::STATE_CLEAN;
    FanClickState click_sate_row_3 = FanClickState::STATE_CLEAN;
    FanClickState click_sate_row_4 = FanClickState::STATE_CLEAN;
    FanClickState click_sate_row_5 = FanClickState::STATE_CLEAN;

    uint32_t start_click_press_row_1 = 0;
    uint32_t start_click_press_row_2 = 0;
    uint32_t start_click_press_row_3 = 0;
    uint32_t start_click_press_row_4 = 0;
    uint32_t start_click_press_row_5 = 0;
};

#ifdef USE_TEXT_SENSOR
#define WALL01FAN_TEXT_SENSOR(name)               \
   protected:                                     \
    text_sensor::TextSensor *name##_text_sensor_; \
                                                  \
   public:                                        \
    void set_##name##_text_sensor(text_sensor::TextSensor *name##_text_sensor) { this->name##_text_sensor_ = name##_text_sensor; }
#else
#define WALL01FAN_TEXT_SENSOR(name)
#endif

#ifdef USE_SWITCH
#define WALL01FAN_SWITCH(name)       \
   protected:                        \
    switch_::Switch *name##_switch_; \
                                     \
   public:                           \
    void set_##name##_switch(switch_::Switch *name##_switch) { this->name##_switch_ = name##_switch; }
#else
#define WALL01FAN_SWITCH(name)
#endif

#ifdef USE_BINARY_OUTPUT
#define WALL01FAN_BINARY_OUTPUT(name)            \
   protected:                                    \
    output::BinaryOutput *name##_binary_output_; \
                                                 \
   public:                                       \
    void set_##name##_binary_output(output::BinaryOutput *name##_binary_output) { this->name##_binary_output_ = name##_binary_output; }
#else
#define WALL01FAN_BINARY_OUTPUT(name)
#endif

#ifdef USE_BINARY_INPUT_OUTPUT
#define WALL01FAN_BINARY_INPUT_OUTPUT(name)                             \
   protected:                                                           \
    wall01fan::Wall01fanBinaryInputOutput *name##_binary_input_output_; \
                                                                        \
   public:                                                              \
    void set_##name##_binary_input_output(wall01fan::Wall01fanBinaryInputOutput *name##_binary_input_output) { this->name##_binary_input_output_ = name##_binary_input_output; }
#else
#define WALL01FAN_BINARY_INPUT_OUTPUT(name)
#endif

class Wall01fanComponent : public Component {
    WALL01FAN_TEXT_SENSOR(timer_remaining)

    WALL01FAN_SWITCH(power)
    WALL01FAN_SWITCH(oscillation)
    WALL01FAN_SWITCH(speed_low)
    WALL01FAN_SWITCH(speed_mid)
    WALL01FAN_SWITCH(speed_high)
    WALL01FAN_SWITCH(mode_normal)
    WALL01FAN_SWITCH(mode_natural)
    WALL01FAN_SWITCH(mode_sleep)
    WALL01FAN_SWITCH(timer_05h)
    WALL01FAN_SWITCH(timer_1h)
    WALL01FAN_SWITCH(timer_2h)
    WALL01FAN_SWITCH(timer_4h)
    WALL01FAN_SWITCH(more_config)

    WALL01FAN_BINARY_INPUT_OUTPUT(row_1)
    WALL01FAN_BINARY_INPUT_OUTPUT(row_2)
    WALL01FAN_BINARY_INPUT_OUTPUT(row_3)
    WALL01FAN_BINARY_INPUT_OUTPUT(row_4)
    WALL01FAN_BINARY_INPUT_OUTPUT(row_5)

    WALL01FAN_BINARY_OUTPUT(oscillation)
    WALL01FAN_BINARY_OUTPUT(speed_low)
    WALL01FAN_BINARY_OUTPUT(speed_mid)
    WALL01FAN_BINARY_OUTPUT(speed_high)
    WALL01FAN_BINARY_OUTPUT(column_a)
    WALL01FAN_BINARY_OUTPUT(column_b)
    WALL01FAN_BINARY_OUTPUT(column_c)
    WALL01FAN_BINARY_OUTPUT(column_d)
    WALL01FAN_BINARY_OUTPUT(column_e)
    WALL01FAN_BINARY_OUTPUT(column_f)
    WALL01FAN_BINARY_OUTPUT(column_g)
    WALL01FAN_BINARY_OUTPUT(column_h)
    WALL01FAN_BINARY_OUTPUT(column_i)
    WALL01FAN_BINARY_OUTPUT(column_j)

   public:
    Wall01fanComponent() = default;

    float get_setup_priority() const override;
    float get_loop_priority() const override;

    void setup() override;
    void loop() override;
    void dump_config() override;

    void remote_receiver(uint8_t protocol, uint64_t code);
    void set_switch_value(SmIdEntity entity, bool value);

    void power_turn_on();
    void power_turn_off();
    void power_toggle();

    void oscillation_turn_on();
    void oscillation_turn_off();
    void oscillation_toggle();

    void speed_none();
    void speed_low();
    void speed_mid();
    void speed_high();
    void speed_toggle();

    void mode_none();
    void mode_normal();
    void mode_natural();
    void mode_sleep();
    void mode_toggle();

    void add_on_beep_callback(std::function<void(bool)> &&callback);

    FanStateData fan_data_;

   protected:
    CallbackManager<void(bool)> beep_callback_{};

    HighFrequencyLoopRequester high_freq_;

    void button_click_(FanButton button);

    void set_power_(bool state);
    void set_oscillation_(bool state);
    void set_speed_(FanSpeed speed);

    void set_mode_(FanMode mode);
    void process_mode_();
    void cancel_natural_or_sleep_mode_();

    void timer_0h_();
    void timer_30m_(bool state);
    void timer_1h_(bool state);
    void timer_2h_(bool state);
    void timer_3h_(bool state);
    void timer_4h_(bool state);
    void timer_toggle_();
    void timer_add_remove_(bool state, FanTimer value);
    void set_timer_(FanTimer timer);
    void publish_delay_value_remaining_string_();
    void interval_delay_value_remaining_();

    void beep_(bool force = false);

    void process_state_click_(bool state_pin_row, FanClickState *click_sate_row, uint32_t *start_click_press_row, FanButton action);
};

}  // namespace wall01fan
}  // namespace esphome
