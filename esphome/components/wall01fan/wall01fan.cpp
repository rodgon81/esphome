#include "wall01fan.h"

#include "esphome/core/log.h"

namespace esphome {
namespace wall01fan {

static const char *const TAG = "wall01fan";

#ifdef USE_TEXT_SENSOR
#define UPDATE_TEXT_SENSOR(name, value) \
  if (this->name##_text_sensor_ != nullptr) { \
    this->name##_text_sensor_->publish_state(value); \
  }
#else
#define UPDATE_TEXT_SENSOR(name, value)
#endif

#ifdef USE_SWITCH
#define UPDATE_SWITCH(name, value) \
  if (this->name##_switch_ != nullptr) { \
    this->name##_switch_->publish_state(value); \
  }
#else
#define UPDATE_SWITCH(name, value)
#endif

#ifdef USE_BINARY_OUTPUT
#define UPDATE_BINARY_OUTPUT(name, value) \
  if (this->name##_binary_output_ != nullptr) { \
    this->name##_binary_output_->set_state(value); \
  }

#else
#define UPDATE_BINARY_OUTPUT(name, value)
#endif

#ifdef USE_BINARY_INPUT_OUTPUT
#define UPDATE_BINARY_INPUT_OUTPUT(row, column_1, column_2, action, state_led_1, state_led_2) \
  if (this->row##_binary_input_output_ != nullptr && this->column_1##_binary_output_ != nullptr && this->column_2##_binary_output_ != nullptr) { \
    this->column_1##_binary_output_->set_state(true); \
    this->column_2##_binary_output_->set_state(true); \
    this->process_state_click_(!this->row##_binary_input_output_->get_state(), &this->fan_data_.click_sate_##row, &this->fan_data_.start_click_press_##row, action); \
\
    this->row##_binary_input_output_->set_state(true); \
    this->column_1##_binary_output_->set_state(!state_led_1); \
    this->column_2##_binary_output_->set_state(!state_led_2); \
  }
#else
#define UPDATE_BINARY_INPUT_OUTPUT(row, column_1, column_2, action, state_led_1, state_led_2)
#endif

#ifdef USE_STATUS_LED
#define STATUS_LED_CHECK(cond, value1, value2) (cond ? value1 : value2)
#else
#define STATUS_LED_CHECK(cond, value1, value2) value2
#endif

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

float Wall01fanComponent::get_setup_priority() const { return setup_priority::AFTER_CONNECTION; }
float Wall01fanComponent::get_loop_priority() const { return 50000.0f; }

void Wall01fanComponent::setup() {
  ESP_LOGI(TAG, "Load Default");

  this->power_turn_off();
  this->oscillation_turn_off();
  this->speed_none();
  this->mode_none();
  this->timer_0h_();
}

void Wall01fanComponent::loop() {
#ifdef USE_STATUS_LED
  bool status_led = false;
  bool status_warning_error = false;

  if ((App.get_app_state() & STATUS_LED_ERROR) != 0u) {
    status_warning_error = true;
    status_led = (millis() % 250u < 150u);
  } else if ((App.get_app_state() & STATUS_LED_WARNING) != 0u) {
    status_warning_error = true;
    status_led = (millis() % 1500u < 250u);
  }
#endif

  UPDATE_BINARY_INPUT_OUTPUT(row_1, column_a, column_b, FanButton::BUTTON_POWER, this->fan_data_.is_speed_high, this->fan_data_.is_speed_mid)
  UPDATE_BINARY_INPUT_OUTPUT(row_2, column_c, column_d, FanButton::BUTTON_OSCILLATION, this->fan_data_.is_timer_4h, this->fan_data_.is_timer_1h)
  UPDATE_BINARY_INPUT_OUTPUT(row_3, column_e, column_f, FanButton::BUTTON_SPEED, this->fan_data_.is_timer_2h, this->fan_data_.is_timer_05h)
  UPDATE_BINARY_INPUT_OUTPUT(row_4, column_g, column_h, FanButton::BUTTON_MODE, this->fan_data_.is_mode_sleep, this->fan_data_.is_mode_natural)
  UPDATE_BINARY_INPUT_OUTPUT(row_5, column_i, column_j, FanButton::BUTTON_TIMER, STATUS_LED_CHECK(status_warning_error, status_led, this->fan_data_.is_power_on), this->fan_data_.is_speed_low)
}

void Wall01fanComponent::dump_config() {
  //
  ESP_LOGCONFIG(TAG, "*** COMPONENT VERSION: %s ***", STR_COMPONENT_VERSION);
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Wall01fanComponent::remote_receiver(uint8_t protocol, uint64_t code) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    if (protocol == 1) {
      switch (code) {
        case INFRARED_POWER:
          this->button_click_(FanButton::BUTTON_POWER);
          break;
        case INFRARED_OSCILLATION:
          this->button_click_(FanButton::BUTTON_OSCILLATION);
          break;
        case INFRARED_SPEED:
          this->button_click_(FanButton::BUTTON_SPEED);
          break;
        case INFRARED_MODE:
          this->button_click_(FanButton::BUTTON_MODE);
          break;
        case INFRARED_TIMER:
          this->button_click_(FanButton::BUTTON_TIMER);
          break;
      }
    }
  }
}

void Wall01fanComponent::set_switch_value(SmIdEntity entity, bool state) {
  this->fan_data_.is_temperature_mode = false;

  switch (entity) {
    case SmIdEntity::SWITCH_POWER: {
      this->set_power_(state);

      this->beep_(true);

      break;
    }
    case SmIdEntity::SWITCH_OSCILLATION: {
      this->cancel_natural_or_sleep_mode_();

      this->set_oscillation_(state);

      break;
    }
    case SmIdEntity::SWITCH_SPEED_LOW: {
      this->cancel_natural_or_sleep_mode_();

      this->speed_low();

      break;
    }
    case SmIdEntity::SWITCH_SPEED_MID: {
      this->cancel_natural_or_sleep_mode_();

      this->speed_mid();

      break;
    }
    case SmIdEntity::SWITCH_SPEED_HIGH: {
      this->cancel_natural_or_sleep_mode_();

      this->speed_high();

      break;
    }
    case SmIdEntity::SWITCH_MODE_NORMAL: {
      this->mode_normal();

      break;
    }
    case SmIdEntity::SWITCH_MODE_NATURAL: {
      this->mode_natural();

      break;
    }
    case SmIdEntity::SWITCH_MODE_SLEEP: {
      this->mode_sleep();

      break;
    }
    case SmIdEntity::SWITCH_TIMER_05H: {
      this->timer_30m_(state);

      break;
    }
    case SmIdEntity::SWITCH_TIMER_1H: {
      this->timer_1h_(state);

      break;
    }
    case SmIdEntity::SWITCH_TIMER_2H: {
      this->timer_2h_(state);

      break;
    }
    case SmIdEntity::SWITCH_TIMER_4H: {
      this->timer_4h_(state);

      break;
    }
    case SmIdEntity::SWITCH_MORE_CONFIG: {
      UPDATE_SWITCH(more_config, state)

      break;
    }
    default: {
      ESP_LOGE(TAG, "ID %hhu is not a SWITCH or is not included in the case list", entity);
      return;
    }
  }
}

void Wall01fanComponent::button_click_(FanButton button) {
  this->fan_data_.is_temperature_mode = false;

  switch (button) {
    case FanButton::BUTTON_POWER: {
      this->power_toggle();
      this->beep_(true);
      break;
    }
    case FanButton::BUTTON_OSCILLATION: {
      this->cancel_natural_or_sleep_mode_();

      this->oscillation_toggle();
      this->beep_();
      break;
    }
    case FanButton::BUTTON_SPEED: {
      this->cancel_natural_or_sleep_mode_();

      this->speed_toggle();
      this->beep_();
      break;
    }
    case FanButton::BUTTON_MODE: {
      this->mode_toggle();
      this->beep_();
      break;
    }
    case FanButton::BUTTON_TIMER: {
      this->timer_toggle_();
      this->beep_();
      break;
    }
  }
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Wall01fanComponent::power_turn_on() {
  //
  this->set_power_(true);
}

void Wall01fanComponent::power_turn_off() {
  //
  this->set_power_(false);
}

void Wall01fanComponent::power_toggle() {
  //
  this->set_power_(!this->fan_data_.is_power_on);
}

void Wall01fanComponent::set_power_(bool state) {
  this->fan_data_.is_power_on = state;

  ESP_LOGD(TAG, "Set power = %s", TRUEFALSE(this->fan_data_.is_power_on));

  UPDATE_SWITCH(power, this->fan_data_.is_power_on)

  if (this->fan_data_.is_power_on) {
    // this->high_freq_.start();

    this->oscillation_turn_on();
    this->speed_low();
    this->mode_normal();
    this->timer_0h_();
  } else {
    // this->high_freq_.stop();

    this->fan_data_.is_temperature_mode = true;

    this->oscillation_turn_off();
    this->speed_none();
    this->mode_none();
    this->timer_0h_();
  }
}

//*************************************************************

void Wall01fanComponent::oscillation_turn_on() {
  //
  this->set_oscillation_(true);
}

void Wall01fanComponent::oscillation_turn_off() {
  //
  this->set_oscillation_(false);
}

void Wall01fanComponent::oscillation_toggle() {
  //
  this->set_oscillation_(!this->fan_data_.is_oscillation_on);
}

void Wall01fanComponent::set_oscillation_(bool state) {
  this->fan_data_.is_oscillation_on = state;

  if (!this->fan_data_.is_power_on) {
    ESP_LOGD(TAG, "Fan is off");

    this->fan_data_.is_oscillation_on = false;
  }

  ESP_LOGD(TAG, "Set oscillation = %s", TRUEFALSE(this->fan_data_.is_oscillation_on));

  UPDATE_SWITCH(oscillation, this->fan_data_.is_oscillation_on)
  UPDATE_BINARY_OUTPUT(oscillation, this->fan_data_.is_oscillation_on)
}

//*************************************************************

void Wall01fanComponent::speed_none() {
  //
  this->set_speed_(FanSpeed::SPEED_NONE);
}

void Wall01fanComponent::speed_low() {
  //
  this->set_speed_(FanSpeed::SPEED_LOW);
}

void Wall01fanComponent::speed_mid() {
  //
  this->set_speed_(FanSpeed::SPEED_MID);
}

void Wall01fanComponent::speed_high() {
  //
  this->set_speed_(FanSpeed::SPEED_HIGH);
}

void Wall01fanComponent::speed_toggle() {
  FanSpeed new_speed = FanSpeed::SPEED_NONE;

  switch (this->fan_data_.selected_speed) {
    case FanSpeed::SPEED_NONE:
      new_speed = FanSpeed::SPEED_LOW;
      break;
    case FanSpeed::SPEED_LOW:
      new_speed = FanSpeed::SPEED_MID;
      break;
    case FanSpeed::SPEED_MID:
      new_speed = FanSpeed::SPEED_HIGH;
      break;
    case FanSpeed::SPEED_HIGH:
      new_speed = FanSpeed::SPEED_LOW;
      break;
  }

  this->set_speed_(new_speed);
}

void Wall01fanComponent::set_speed_(FanSpeed speed) {
  this->fan_data_.selected_speed = speed;

  if (!this->fan_data_.is_power_on) {
    ESP_LOGD(TAG, "Fan is off");

    this->fan_data_.selected_speed = FanSpeed::SPEED_NONE;
  }

  switch (this->fan_data_.selected_speed) {
    case FanSpeed::SPEED_NONE: {
      ESP_LOGD(TAG, "Set SPEED_NONE");

      UPDATE_BINARY_OUTPUT(speed_low, false)
      UPDATE_BINARY_OUTPUT(speed_mid, false)
      UPDATE_BINARY_OUTPUT(speed_high, false)

      UPDATE_SWITCH(speed_low, false)
      UPDATE_SWITCH(speed_mid, false)
      UPDATE_SWITCH(speed_high, false)

      this->fan_data_.is_speed_low = false;
      this->fan_data_.is_speed_mid = false;
      this->fan_data_.is_speed_high = false;

      break;
    }
    case FanSpeed::SPEED_LOW: {
      ESP_LOGD(TAG, "Set SPEED_LOW");

      UPDATE_BINARY_OUTPUT(speed_low, true)
      UPDATE_BINARY_OUTPUT(speed_mid, false)
      UPDATE_BINARY_OUTPUT(speed_high, false)

      UPDATE_SWITCH(speed_low, true)
      UPDATE_SWITCH(speed_mid, false)
      UPDATE_SWITCH(speed_high, false)

      this->fan_data_.is_speed_low = true;
      this->fan_data_.is_speed_mid = false;
      this->fan_data_.is_speed_high = false;

      break;
    }
    case FanSpeed::SPEED_MID: {
      ESP_LOGD(TAG, "Set SPEED_MID");

      UPDATE_BINARY_OUTPUT(speed_low, false)
      UPDATE_BINARY_OUTPUT(speed_mid, true)
      UPDATE_BINARY_OUTPUT(speed_high, false)

      UPDATE_SWITCH(speed_low, false)
      UPDATE_SWITCH(speed_mid, true)
      UPDATE_SWITCH(speed_high, false)

      this->fan_data_.is_speed_low = false;
      this->fan_data_.is_speed_mid = true;
      this->fan_data_.is_speed_high = false;

      break;
    }
    case FanSpeed::SPEED_HIGH: {
      ESP_LOGD(TAG, "Set SPEED_HIGH");

      UPDATE_BINARY_OUTPUT(speed_low, false)
      UPDATE_BINARY_OUTPUT(speed_mid, false)
      UPDATE_BINARY_OUTPUT(speed_high, true)

      UPDATE_SWITCH(speed_low, false)
      UPDATE_SWITCH(speed_mid, false)
      UPDATE_SWITCH(speed_high, true)

      this->fan_data_.is_speed_low = false;
      this->fan_data_.is_speed_mid = false;
      this->fan_data_.is_speed_high = true;

      break;
    }
  }
}

//*************************************************************

void Wall01fanComponent::mode_none() {
  //
  this->set_mode_(FanMode::MODE_NONE);
}

void Wall01fanComponent::mode_normal() {
  //
  this->set_mode_(FanMode::MODE_NORMAL);
}

void Wall01fanComponent::mode_natural() {
  //
  this->set_mode_(FanMode::MODE_NATURAL);
}

void Wall01fanComponent::mode_sleep() {
  //
  this->set_mode_(FanMode::MODE_SLEEP);
}

void Wall01fanComponent::mode_toggle() {
  FanMode new_mode = FanMode::MODE_NONE;

  switch (this->fan_data_.selected_mode) {
    case FanMode::MODE_NONE:
      new_mode = FanMode::MODE_NORMAL;
      break;

    case FanMode::MODE_NORMAL:
      new_mode = FanMode::MODE_NATURAL;
      break;

    case FanMode::MODE_NATURAL:
      new_mode = FanMode::MODE_SLEEP;
      break;

    case FanMode::MODE_SLEEP:
      new_mode = FanMode::MODE_NORMAL;
      break;
  }

  this->set_mode_(new_mode);
}

void Wall01fanComponent::set_mode_(FanMode mode) {
  this->fan_data_.selected_mode = mode;

  this->cancel_timeout(TIMER_MODE_NAME);

  if (!this->fan_data_.is_power_on) {
    ESP_LOGD(TAG, "Fan is off");

    this->fan_data_.selected_mode = FanMode::MODE_NONE;
  }

  switch (this->fan_data_.selected_mode) {
    case FanMode::MODE_NONE: {
      ESP_LOGD(TAG, "Set MODE_NONE");

      UPDATE_SWITCH(mode_normal, false)
      UPDATE_SWITCH(mode_natural, false)
      UPDATE_SWITCH(mode_sleep, false)

      this->fan_data_.is_mode_normal = false;
      this->fan_data_.is_mode_natural = false;
      this->fan_data_.is_mode_sleep = false;

      this->fan_data_.is_natural_or_sleep_mode = false;

      break;
    }
    case FanMode::MODE_NORMAL: {
      ESP_LOGD(TAG, "Set MODE_NORMAL");

      UPDATE_SWITCH(mode_normal, true)
      UPDATE_SWITCH(mode_natural, false)
      UPDATE_SWITCH(mode_sleep, false)

      this->fan_data_.is_mode_normal = true;
      this->fan_data_.is_mode_natural = false;
      this->fan_data_.is_mode_sleep = false;

      this->fan_data_.is_natural_or_sleep_mode = false;

      break;
    }
    case FanMode::MODE_NATURAL: {
      ESP_LOGD(TAG, "Set MODE_NATURAL");

      UPDATE_SWITCH(mode_normal, false)
      UPDATE_SWITCH(mode_natural, true)
      UPDATE_SWITCH(mode_sleep, false)

      this->fan_data_.is_mode_normal = false;
      this->fan_data_.is_mode_natural = true;
      this->fan_data_.is_mode_sleep = false;

      this->fan_data_.is_natural_or_sleep_mode = true;

      break;
    }
    case FanMode::MODE_SLEEP: {
      ESP_LOGD(TAG, "Set MODE_SLEEP");

      UPDATE_SWITCH(mode_normal, false)
      UPDATE_SWITCH(mode_natural, false)
      UPDATE_SWITCH(mode_sleep, true)

      this->fan_data_.is_mode_normal = false;
      this->fan_data_.is_mode_natural = false;
      this->fan_data_.is_mode_sleep = true;

      this->fan_data_.is_natural_or_sleep_mode = true;

      break;
    }
  }

  this->process_mode_();
}

void Wall01fanComponent::process_mode_() {
  if (this->fan_data_.is_natural_or_sleep_mode) {
    FanSpeed speed = FanSpeed::SPEED_NONE;
    bool oscillation = false;
    uint32_t timeout = 0;

    if (this->fan_data_.selected_mode == FanMode::MODE_NATURAL) {
      speed = FanSpeed(static_cast<FanSpeed>((random_uint32() % 4) + 1));
      oscillation = bool(static_cast<bool>(random_uint32() % 2));
      timeout = (random_uint32() % 5000) + 5000;
    }

    if (this->fan_data_.selected_mode == FanMode::MODE_SLEEP) {
      speed = FanSpeed(static_cast<FanSpeed>(random_uint32() % 3));
      oscillation = bool(static_cast<bool>(random_uint32() % 2));
      timeout = (random_uint32() % 10000) + 10000;
    }

    this->set_speed_(speed);
    this->set_oscillation_(oscillation);
    this->set_timeout(TIMER_MODE_NAME, timeout, [this]() { this->process_mode_(); });
  }
}

void Wall01fanComponent::cancel_natural_or_sleep_mode_() {
  if (this->fan_data_.is_natural_or_sleep_mode) {
    this->set_mode_(FanMode::MODE_NORMAL);
  }
}

//*************************************************************

void Wall01fanComponent::timer_0h_() {
  //
  this->set_timer_(FanTimer::TIMER_0H_00M);
}

void Wall01fanComponent::timer_30m_(bool state) {
  //
  this->timer_add_remove_(state, FanTimer::TIMER_0H_30M);
}

void Wall01fanComponent::timer_1h_(bool state) {
  //
  this->timer_add_remove_(state, FanTimer::TIMER_1H_00M);
}

void Wall01fanComponent::timer_2h_(bool state) {
  //
  this->timer_add_remove_(state, FanTimer::TIMER_2H_00M);
}

void Wall01fanComponent::timer_4h_(bool state) {
  //
  this->timer_add_remove_(state, FanTimer::TIMER_4H_00M);
}

void Wall01fanComponent::timer_toggle_() {
  FanTimer new_timer = FanTimer::TIMER_0H_00M;

  switch (this->fan_data_.selected_timer) {
    case FanTimer::TIMER_0H_00M:
      new_timer = FanTimer::TIMER_0H_30M;
      break;
    case FanTimer::TIMER_0H_30M:
      new_timer = FanTimer::TIMER_1H_00M;
      break;
    case FanTimer::TIMER_1H_00M:
      new_timer = FanTimer::TIMER_1H_30M;
      break;
    case FanTimer::TIMER_1H_30M:
      new_timer = FanTimer::TIMER_2H_00M;
      break;
    case FanTimer::TIMER_2H_00M:
      new_timer = FanTimer::TIMER_2H_30M;
      break;
    case FanTimer::TIMER_2H_30M:
      new_timer = FanTimer::TIMER_3H_00M;
      break;
    case FanTimer::TIMER_3H_00M:
      new_timer = FanTimer::TIMER_3H_30M;
      break;
    case FanTimer::TIMER_3H_30M:
      new_timer = FanTimer::TIMER_4H_00M;
      break;
    case FanTimer::TIMER_4H_00M:
      new_timer = FanTimer::TIMER_4H_30M;
      break;
    case FanTimer::TIMER_4H_30M:
      new_timer = FanTimer::TIMER_5H_00M;
      break;
    case FanTimer::TIMER_5H_00M:
      new_timer = FanTimer::TIMER_5H_30M;
      break;
    case FanTimer::TIMER_5H_30M:
      new_timer = FanTimer::TIMER_6H_00M;
      break;
    case FanTimer::TIMER_6H_00M:
      new_timer = FanTimer::TIMER_6H_30M;
      break;
    case FanTimer::TIMER_6H_30M:
      new_timer = FanTimer::TIMER_7H_00M;
      break;
    case FanTimer::TIMER_7H_00M:
      new_timer = FanTimer::TIMER_7H_30M;
      break;
    case FanTimer::TIMER_7H_30M:
      new_timer = FanTimer::TIMER_0H_00M;
      break;
  }

  this->set_timer_(new_timer);
}

void Wall01fanComponent::timer_add_remove_(bool state, FanTimer value) {
  FanTimer new_timer = FanTimer::TIMER_0H_00M;

  if (state) {
    ESP_LOGD(TAG, "Add %u minutes", value);

    new_timer = FanTimer(static_cast<FanTimer>(uint16_t(static_cast<uint16_t>(this->fan_data_.selected_timer)) + uint16_t(static_cast<uint16_t>(value))));
  } else {
    ESP_LOGD(TAG, "Remove %u minutes", value);

    new_timer = FanTimer(static_cast<FanTimer>(uint16_t(static_cast<uint16_t>(this->fan_data_.selected_timer)) - uint16_t(static_cast<uint16_t>(value))));
  }

  this->set_timer_(new_timer);
}

void Wall01fanComponent::set_timer_(FanTimer timer) {
  this->fan_data_.selected_timer = timer;

  this->cancel_interval(TIMER_REMAINING_NAME);

  if (!this->fan_data_.is_power_on) {
    ESP_LOGD(TAG, "Fan is off");

    this->fan_data_.selected_timer = FanTimer::TIMER_0H_00M;
  }

  switch (this->fan_data_.selected_timer) {
    case FanTimer::TIMER_0H_00M: {
      ESP_LOGD(TAG, "Set TIMER_0H_00M");

      UPDATE_SWITCH(timer_05h, false)
      UPDATE_SWITCH(timer_1h, false)
      UPDATE_SWITCH(timer_2h, false)
      UPDATE_SWITCH(timer_4h, false)

      this->fan_data_.is_timer_05h = false;
      this->fan_data_.is_timer_1h = false;
      this->fan_data_.is_timer_2h = false;
      this->fan_data_.is_timer_4h = false;

      break;
    }
    case FanTimer::TIMER_0H_30M: {
      ESP_LOGD(TAG, "Set TIMER_0H_30M");

      UPDATE_SWITCH(timer_05h, true)
      UPDATE_SWITCH(timer_1h, false)
      UPDATE_SWITCH(timer_2h, false)
      UPDATE_SWITCH(timer_4h, false)

      this->fan_data_.is_timer_05h = true;
      this->fan_data_.is_timer_1h = false;
      this->fan_data_.is_timer_2h = false;
      this->fan_data_.is_timer_4h = false;

      break;
    }
    case FanTimer::TIMER_1H_00M: {
      ESP_LOGD(TAG, "Set TIMER_1H_00M");

      UPDATE_SWITCH(timer_05h, false)
      UPDATE_SWITCH(timer_1h, true)
      UPDATE_SWITCH(timer_2h, false)
      UPDATE_SWITCH(timer_4h, false)

      this->fan_data_.is_timer_05h = false;
      this->fan_data_.is_timer_1h = true;
      this->fan_data_.is_timer_2h = false;
      this->fan_data_.is_timer_4h = false;

      break;
    }
    case FanTimer::TIMER_1H_30M: {
      ESP_LOGD(TAG, "Set TIMER_1H_30M");

      UPDATE_SWITCH(timer_05h, true)
      UPDATE_SWITCH(timer_1h, true)
      UPDATE_SWITCH(timer_2h, false)
      UPDATE_SWITCH(timer_4h, false)

      this->fan_data_.is_timer_05h = true;
      this->fan_data_.is_timer_1h = true;
      this->fan_data_.is_timer_2h = false;
      this->fan_data_.is_timer_4h = false;

      break;
    }
    case FanTimer::TIMER_2H_00M: {
      ESP_LOGD(TAG, "Set TIMER_2H_00M");

      UPDATE_SWITCH(timer_05h, false)
      UPDATE_SWITCH(timer_1h, false)
      UPDATE_SWITCH(timer_2h, true)
      UPDATE_SWITCH(timer_4h, false)

      this->fan_data_.is_timer_05h = false;
      this->fan_data_.is_timer_1h = false;
      this->fan_data_.is_timer_2h = true;
      this->fan_data_.is_timer_4h = false;

      break;
    }
    case FanTimer::TIMER_2H_30M: {
      ESP_LOGD(TAG, "Set TIMER_2H_30M");

      UPDATE_SWITCH(timer_05h, true)
      UPDATE_SWITCH(timer_1h, false)
      UPDATE_SWITCH(timer_2h, true)
      UPDATE_SWITCH(timer_4h, false)

      this->fan_data_.is_timer_05h = true;
      this->fan_data_.is_timer_1h = false;
      this->fan_data_.is_timer_2h = true;
      this->fan_data_.is_timer_4h = false;

      break;
    }
    case FanTimer::TIMER_3H_00M: {
      ESP_LOGD(TAG, "Set TIMER_3H_00M");

      UPDATE_SWITCH(timer_05h, false)
      UPDATE_SWITCH(timer_1h, true)
      UPDATE_SWITCH(timer_2h, true)
      UPDATE_SWITCH(timer_4h, false)

      this->fan_data_.is_timer_05h = false;
      this->fan_data_.is_timer_1h = true;
      this->fan_data_.is_timer_2h = true;
      this->fan_data_.is_timer_4h = false;

      break;
    }
    case FanTimer::TIMER_3H_30M: {
      ESP_LOGD(TAG, "Set TIMER_3H_30M");

      UPDATE_SWITCH(timer_05h, true)
      UPDATE_SWITCH(timer_1h, true)
      UPDATE_SWITCH(timer_2h, true)
      UPDATE_SWITCH(timer_4h, false)

      this->fan_data_.is_timer_05h = true;
      this->fan_data_.is_timer_1h = true;
      this->fan_data_.is_timer_2h = true;
      this->fan_data_.is_timer_4h = false;

      break;
    }
    case FanTimer::TIMER_4H_00M: {
      ESP_LOGD(TAG, "Set TIMER_4H_00M");

      UPDATE_SWITCH(timer_05h, false)
      UPDATE_SWITCH(timer_1h, false)
      UPDATE_SWITCH(timer_2h, false)
      UPDATE_SWITCH(timer_4h, true)

      this->fan_data_.is_timer_05h = false;
      this->fan_data_.is_timer_1h = false;
      this->fan_data_.is_timer_2h = false;
      this->fan_data_.is_timer_4h = true;

      break;
    }
    case FanTimer::TIMER_4H_30M: {
      ESP_LOGD(TAG, "Set TIMER_4H_30M");

      UPDATE_SWITCH(timer_05h, true)
      UPDATE_SWITCH(timer_1h, false)
      UPDATE_SWITCH(timer_2h, false)
      UPDATE_SWITCH(timer_4h, true)

      this->fan_data_.is_timer_05h = true;
      this->fan_data_.is_timer_1h = false;
      this->fan_data_.is_timer_2h = false;
      this->fan_data_.is_timer_4h = true;

      break;
    }
    case FanTimer::TIMER_5H_00M: {
      ESP_LOGD(TAG, "Set TIMER_5H_00M");

      UPDATE_SWITCH(timer_05h, false)
      UPDATE_SWITCH(timer_1h, true)
      UPDATE_SWITCH(timer_2h, false)
      UPDATE_SWITCH(timer_4h, true)

      this->fan_data_.is_timer_05h = false;
      this->fan_data_.is_timer_1h = true;
      this->fan_data_.is_timer_2h = false;
      this->fan_data_.is_timer_4h = true;

      break;
    }
    case FanTimer::TIMER_5H_30M: {
      ESP_LOGD(TAG, "Set TIMER_5H_30M");

      UPDATE_SWITCH(timer_05h, true)
      UPDATE_SWITCH(timer_1h, true)
      UPDATE_SWITCH(timer_2h, false)
      UPDATE_SWITCH(timer_4h, true)

      this->fan_data_.is_timer_05h = true;
      this->fan_data_.is_timer_1h = true;
      this->fan_data_.is_timer_2h = false;
      this->fan_data_.is_timer_4h = true;

      break;
    }
    case FanTimer::TIMER_6H_00M: {
      ESP_LOGD(TAG, "Set TIMER_6H_00M");

      UPDATE_SWITCH(timer_05h, false)
      UPDATE_SWITCH(timer_1h, false)
      UPDATE_SWITCH(timer_2h, true)
      UPDATE_SWITCH(timer_4h, true)

      this->fan_data_.is_timer_05h = false;
      this->fan_data_.is_timer_1h = false;
      this->fan_data_.is_timer_2h = true;
      this->fan_data_.is_timer_4h = true;

      break;
    }
    case FanTimer::TIMER_6H_30M: {
      ESP_LOGD(TAG, "Set TIMER_6H_30M");

      UPDATE_SWITCH(timer_05h, true)
      UPDATE_SWITCH(timer_1h, false)
      UPDATE_SWITCH(timer_2h, true)
      UPDATE_SWITCH(timer_4h, true)

      this->fan_data_.is_timer_05h = true;
      this->fan_data_.is_timer_1h = false;
      this->fan_data_.is_timer_2h = true;
      this->fan_data_.is_timer_4h = true;

      break;
    }
    case FanTimer::TIMER_7H_00M: {
      ESP_LOGD(TAG, "Set TIMER_7H_00M");

      UPDATE_SWITCH(timer_05h, false)
      UPDATE_SWITCH(timer_1h, true)
      UPDATE_SWITCH(timer_2h, true)
      UPDATE_SWITCH(timer_4h, true)

      this->fan_data_.is_timer_05h = false;
      this->fan_data_.is_timer_1h = true;
      this->fan_data_.is_timer_2h = true;
      this->fan_data_.is_timer_4h = true;

      break;
    }
    case FanTimer::TIMER_7H_30M: {
      ESP_LOGD(TAG, "Set TIMER_7H_30M");

      UPDATE_SWITCH(timer_05h, true)
      UPDATE_SWITCH(timer_1h, true)
      UPDATE_SWITCH(timer_2h, true)
      UPDATE_SWITCH(timer_4h, true)

      this->fan_data_.is_timer_05h = true;
      this->fan_data_.is_timer_1h = true;
      this->fan_data_.is_timer_2h = true;
      this->fan_data_.is_timer_4h = true;

      break;
    }
  }

  this->fan_data_.seconds_time_remaining = 0;

  if (this->fan_data_.is_timer_05h)
    this->fan_data_.seconds_time_remaining = this->fan_data_.seconds_time_remaining + (0.5 * 60 * 60);

  if (this->fan_data_.is_timer_1h)
    this->fan_data_.seconds_time_remaining = this->fan_data_.seconds_time_remaining + (1 * 60 * 60);

  if (this->fan_data_.is_timer_2h)
    this->fan_data_.seconds_time_remaining = this->fan_data_.seconds_time_remaining + (2 * 60 * 60);

  if (this->fan_data_.is_timer_4h)
    this->fan_data_.seconds_time_remaining = this->fan_data_.seconds_time_remaining + (4 * 60 * 60);

  if (this->fan_data_.seconds_time_remaining > 0) {
    this->set_interval(TIMER_REMAINING_NAME, 60000, [this]() { this->interval_delay_value_remaining_(); });
  } else {
    this->publish_delay_value_remaining_string_();
  }
}

void Wall01fanComponent::publish_delay_value_remaining_string_() {
  uint16_t seconds = this->fan_data_.seconds_time_remaining;

  uint8_t hours = seconds / 3600;
  seconds = seconds % 3600;
  uint8_t minutes = seconds / 60;

  std::string delay_value_remaining_string = ((hours ? to_string(hours) + "h " : "") + (to_string(minutes) + "m"));

  UPDATE_TEXT_SENSOR(timer_remaining, delay_value_remaining_string)
}

void Wall01fanComponent::interval_delay_value_remaining_() {
  this->publish_delay_value_remaining_string_();

  if (this->fan_data_.seconds_time_remaining == 0) {
    this->cancel_interval(TIMER_REMAINING_NAME);

    this->set_power_(false);
  }

  this->fan_data_.seconds_time_remaining = this->fan_data_.seconds_time_remaining - 60;
}

//*************************************************************

void Wall01fanComponent::beep_(bool force) {
  //
  this->beep_callback_.call(this->fan_data_.is_power_on || force);
}

void Wall01fanComponent::add_on_beep_callback(std::function<void(bool)> &&callback) {
  //
  this->beep_callback_.add(std::move(callback));
}

//*************************************************************

void Wall01fanComponent::process_state_click_(bool state_pin_row, FanClickState *click_sate_row, uint32_t *start_click_press_row, FanButton action) {
  if (state_pin_row && (*click_sate_row == FanClickState::STATE_CLEAN)) {
    ESP_LOGD(TAG, "Button state = STATE_START_PRESS");

    *click_sate_row = FanClickState::STATE_START_PRESS;

    *start_click_press_row = millis();
  }

  if (*click_sate_row == FanClickState::STATE_START_PRESS) {
    if ((*start_click_press_row + CLICK_PRESS_TIME) > millis()) {
      ESP_LOGD(TAG, "Button state = STATE_PRESS");

      *click_sate_row = FanClickState::STATE_PRESS;

      this->button_click_(action);
    }
  }

  if (!state_pin_row && (*click_sate_row == FanClickState::STATE_START_PRESS || *click_sate_row == FanClickState::STATE_PRESS)) {
    ESP_LOGD(TAG, "Button state = STATE_CLEAN");

    *click_sate_row = FanClickState::STATE_CLEAN;

    *start_click_press_row = 0;
  }
}

}  // namespace wall01fan
}  // namespace esphome
