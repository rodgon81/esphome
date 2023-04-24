#include "dxs238xw.h"

namespace esphome {
namespace dxs238xw {

static const char *const TAG = "dxs238xw";

#ifdef USE_SENSOR
#define UPDATE_SENSOR(name, value) \
  if (this->name##_sensor_ != nullptr) { \
    if (this->name##_sensor_->get_raw_state() != (value) || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_sensor_->publish_state(value); \
    } \
  }
#else
#define UPDATE_SENSOR(name, value)
#endif

#ifdef USE_SENSOR
#define UPDATE_SENSOR_MEASUREMENTS_(name, value, factor) \
  if (this->name##_sensor_ != nullptr) { \
    float value_float = value; \
\
    if (factor > 0 && value_float >= factor) { \
      value_float = factor - value_float; \
    } \
\
    if (this->name##_sensor_->get_raw_state() != value_float || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_sensor_->publish_state(value_float); \
    } \
  }

#define UPDATE_SENSOR_MEASUREMENTS(name, value) UPDATE_SENSOR_MEASUREMENTS_(name, value, 0)
#define UPDATE_SENSOR_MEASUREMENTS_POWER(name, value) UPDATE_SENSOR_MEASUREMENTS_(name, value, 100)
#define UPDATE_SENSOR_MEASUREMENTS_CURRENT(name, value) UPDATE_SENSOR_MEASUREMENTS_(name, value, 1000)
#else
#define UPDATE_SENSOR_MEASUREMENTS(name, value)
#define UPDATE_SENSOR_MEASUREMENTS_POWER(name, value)
#define UPDATE_SENSOR_MEASUREMENTS_CURRENT(name, value)
#endif

#ifdef USE_TEXT_SENSOR
#define UPDATE_TEXT_SENSOR(name, value) \
  if (this->name##_text_sensor_ != nullptr) { \
    if (this->name##_text_sensor_->get_raw_state() != (value) || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_text_sensor_->publish_state(value); \
    } \
  }
#else
#define UPDATE_TEXT_SENSOR(name, value)
#endif

#ifdef USE_BINARY_SENSOR
#define UPDATE_BINARY_SENSOR(name, value) \
  if (this->name##_binary_sensor_ != nullptr) { \
    if (this->name##_binary_sensor_->state != (value) || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_binary_sensor_->publish_state(value); \
    } \
  }
#else
#define UPDATE_BINARY_SENSOR(name, value)
#endif

#ifdef USE_NUMBER
#define UPDATE_NUMBER(name, value) \
  if (this->name##_number_ != nullptr) { \
    if (this->name##_number_->state != (value) || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_number_->publish_state(value); \
    } \
  }
#else
#define UPDATE_NUMBER(name, value)
#endif

#ifdef USE_SWITCH
#define UPDATE_SWITCH(name, value) \
  if (this->name##_switch_ != nullptr) { \
    if (this->name##_switch_->state != (value) || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_switch_->publish_state(value); \
    } \
  }
#else
#define UPDATE_SWITCH(name, value)
#endif

#define LOAD_PREFERENCE(name, preference_string, default_value, data) \
  this->preference_##name##_ = global_preferences->make_preference<uint32_t>(this->hash_##name##_); \
  if (this->name##_number_ != nullptr) { \
    this->data##_.name = this->read_initial_number_value_(this->preference_##name##_, preference_string, default_value); \
  }

#define LOAD_PREFERENCE_MS(name, preference_string, default_value) LOAD_PREFERENCE(name, preference_string, default_value, ms_data)
#define LOAD_PREFERENCE_LP(name, preference_string, default_value) LOAD_PREFERENCE(name, preference_string, default_value, lp_data)

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

float Dxs238xwComponent::get_setup_priority() const {
  //
  return setup_priority::LATE;
}

void Dxs238xwComponent::setup() {
  if (this->postpone_setup_time_ == 0) {
    this->postpone_setup_time_ = millis() + SM_POSTPONE_SETUP_TIME;
  }

  if (postpone_setup_time_ > millis()) {
    this->component_state_ &= ~COMPONENT_STATE_MASK;
    this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;
  } else {
    if (this->count_error_data_acquisition_ == 0) {
      ESP_LOGI(TAG, "In --- setup");

      ESP_LOGI(TAG, "* Get Initial Values");
      LOAD_PREFERENCE_MS(delay_value_set, SM_STR_DELAY_VALUE_SET, SmLimitValue::MAX_DELAY_SET)
      LOAD_PREFERENCE_MS(starting_kWh, SM_STR_STARTING_KWH, 0.0f)
      LOAD_PREFERENCE_MS(price_kWh, SM_STR_PRICE_KWH, 0.0f)
      LOAD_PREFERENCE_LP(energy_purchase_value, SM_STR_ENERGY_PURCHASE_VALUE, SmLimitValue::MIN_ENERGY_PURCHASE_VALUE)
      LOAD_PREFERENCE_LP(energy_purchase_alarm, SM_STR_ENERGY_PURCHASE_ALARM, SmLimitValue::MIN_ENERGY_PURCHASE_ALARM)

      UPDATE_NUMBER(delay_value_set, this->ms_data_.delay_value_set)
      UPDATE_NUMBER(starting_kWh, this->ms_data_.starting_kWh)
      UPDATE_NUMBER(price_kWh, this->ms_data_.price_kWh)
      UPDATE_NUMBER(energy_purchase_value, this->lp_data_.energy_purchase_value)
      UPDATE_NUMBER(energy_purchase_alarm, this->lp_data_.energy_purchase_alarm)
    }

    if (!this->first_data_acquisition_()) {
      ESP_LOGE(TAG, "* Error in the communication with the meter");

      this->component_state_ &= ~COMPONENT_STATE_MASK;
      this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;

      this->status_set_error();

      this->count_error_data_acquisition_++;
      this->postpone_setup_time_ = 0;

      return;
    } else {
      if (this->count_error_data_acquisition_ > 0) {
        this->status_clear_error();
      }
    }

    ESP_LOGI(TAG, "Out --- setup");
  }
}

void Dxs238xwComponent::loop() {
  this->incoming_messages_();

  this->send_command_(SmCommandSend::GET_POWER_STATE);
  this->send_command_(SmCommandSend::GET_LIMIT_AND_PURCHASE_DATA);
}

void Dxs238xwComponent::update() {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    this->send_command_(SmCommandSend::GET_MEASUREMENT_DATA);
  }
}

void Dxs238xwComponent::dump_config() {
  LOG_UPDATE_INTERVAL(this)
  ESP_LOGCONFIG(TAG, "*** COMPONENT VERSION: %s ***", SM_STR_COMPONENT_VERSION);
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::meter_state_toggle() {
  //
  this->set_meter_state_(!this->ms_data_.meter_state);
}

void Dxs238xwComponent::meter_state_on() {
  //
  this->set_meter_state_(true);
}

void Dxs238xwComponent::meter_state_off() {
  //
  this->set_meter_state_(false);
}

void Dxs238xwComponent::set_meter_state_(bool state) {
  this->ms_data_.warning_off_by_user = !state;

  this->send_command_(SmCommandSend::SET_POWER_STATE, state);
}

void Dxs238xwComponent::hex_message(std::string message, bool check_crc) {
  ESP_LOGD(TAG, "In --- send_hex_message");

  ESP_LOGD(TAG, "* message in = %s", message.c_str());

  this->error_type_ = SmErrorType::NO_ERROR;
  this->error_code_ = SmErrorCode::NO_ERROR;

  uint8_t length_message = message.length();

  if (length_message == 0 || length_message > SM_MAX_HEX_MSG_LENGTH) {
    this->error_type_ = SmErrorType::INPUT_DATA;
    this->error_code_ = SmErrorCode::MESSAGE_LENGTH;
  }

  if (this->error_code_ == SmErrorCode::NO_ERROR) {
    char tmp_message[SM_MAX_HEX_MSG_LENGTH_PARSE];

    uint8_t size_message_without_dots = 0;

    uint8_t character_hex_index = 0;

    for (uint8_t i = 0; i < length_message; i++) {
      if (message[i] == '.') {
        if (character_hex_index == 2) {
          character_hex_index = 0;
        } else {
          this->error_type_ = SmErrorType::INPUT_DATA;
          this->error_code_ = SmErrorCode::WRONG_MSG;

          break;
        }
      } else {
        if (character_hex_index == 2) {
          this->error_type_ = SmErrorType::INPUT_DATA;
          this->error_code_ = SmErrorCode::WRONG_MSG;

          break;
        } else {
          tmp_message[size_message_without_dots] = message[i];
          size_message_without_dots++;

          character_hex_index++;
        }
      }
    }

    if (this->error_code_ == SmErrorCode::NO_ERROR) {
      if ((size_message_without_dots % 2) != 0) {
        this->error_type_ = SmErrorType::INPUT_DATA;
        this->error_code_ = SmErrorCode::WRONG_MSG;
      }

      if (this->error_code_ == SmErrorCode::NO_ERROR) {
        const char *hex_message = tmp_message;

        uint8_t length_array = size_message_without_dots / 2;

        uint8_t send_array[length_array];

        parse_hex(hex_message, size_message_without_dots, send_array, length_array);

        if (check_crc) {
          if (this->calculate_crc_(send_array, length_array) != send_array[length_array - 1]) {
            this->error_type_ = SmErrorType::INPUT_DATA;
            this->error_code_ = SmErrorCode::CRC;
          }
        } else {
          send_array[length_array - 1] = this->calculate_crc_(send_array, length_array);
        }

        if (this->error_code_ == SmErrorCode::NO_ERROR) {
          ESP_LOGD(TAG, "* Message send: %s", format_hex_pretty(send_array, length_array).c_str());

          if (this->transmit_serial_data_(send_array, length_array)) {
            ESP_LOGD(TAG, "* Waiting answer:");

            if (this->receive_serial_data_(this->receive_array_, HEKR_TYPE_RECEIVE)) {
              ESP_LOGD(TAG, "* Successful answer: %s", format_hex_pretty(this->receive_array_, this->receive_array_[1]).c_str());

              this->process_and_update_data_(this->receive_array_);

              ESP_LOGD(TAG, "Out --- send_hex_message");

              return;
            }

            ESP_LOGD(TAG, "* Failed answer");
          }
        }
      }
    }
  }

  this->print_error_();

  ESP_LOGD(TAG, "Out --- send_hex_message");
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::set_switch_value(SmIdEntity entity, bool value) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    SmCommandSend tmp_cmd_send;

    switch (entity) {
      case SmIdEntity::SWITCH_ENERGY_PURCHASE_STATE: {
        tmp_cmd_send = SmCommandSend::SET_PURCHASE_DATA;
        break;
      }
      case SmIdEntity::SWITCH_METER_STATE: {
        this->ms_data_.warning_off_by_user = !value;

        tmp_cmd_send = SmCommandSend::SET_POWER_STATE;
        break;
      }
      case SmIdEntity::SWITCH_DELAY_STATE: {
        tmp_cmd_send = SmCommandSend::SET_DELAY;
        break;
      }
      default: {
        ESP_LOGE(TAG, "ID %hhu is not a SWITCH or is not included in the case list", entity);
        return;
      }
    }

    this->send_command_(tmp_cmd_send, value);
  }
}

void Dxs238xwComponent::set_button_value(SmIdEntity entity) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    switch (entity) {
      case SmIdEntity::BUTTON_RESET_DATA: {
        this->send_command_(SmCommandSend::SET_PURCHASE_DATA, false);
        this->send_command_(SmCommandSend::SET_RESET);
        break;
      }
      default: {
        ESP_LOGE(TAG, "ID %hhu is not a BUTTON or is not included in the case list", entity);
        break;
      }
    }
  }
}

void Dxs238xwComponent::set_number_value(SmIdEntity entity, float value) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    uint32_t tmp_value = std::round(value);

    switch (entity) {
      case SmIdEntity::NUMBER_MAX_CURRENT_LIMIT: {
        this->lp_data_.max_current_limit = tmp_value;

        this->send_command_(SmCommandSend::SET_LIMIT_DATA);

        UPDATE_NUMBER(max_current_limit, this->lp_data_.max_current_limit)
        break;
      }
      case SmIdEntity::NUMBER_MAX_VOLTAGE_LIMIT: {
        if (tmp_value > this->lp_data_.min_voltage_limit) {
          this->lp_data_.max_voltage_limit = tmp_value;

          this->send_command_(SmCommandSend::SET_LIMIT_DATA);
        } else {
          ESP_LOGW(TAG, "max_voltage_limit - Value %u must not be less than min_voltage_limit %u", tmp_value, this->lp_data_.min_voltage_limit);
        }

        UPDATE_NUMBER(max_voltage_limit, this->lp_data_.max_voltage_limit)
        break;
      }
      case SmIdEntity::NUMBER_MIN_VOLTAGE_LIMIT: {
        if (tmp_value < this->lp_data_.max_voltage_limit) {
          this->lp_data_.min_voltage_limit = tmp_value;

          this->send_command_(SmCommandSend::SET_LIMIT_DATA);
        } else {
          ESP_LOGW(TAG, "min_voltage_limit - Value %u must not be greater than max_voltage_limit %u", tmp_value, this->lp_data_.max_voltage_limit);
        }

        UPDATE_NUMBER(min_voltage_limit, this->lp_data_.min_voltage_limit)
        break;
      }
      case SmIdEntity::NUMBER_ENERGY_PURCHASE_VALUE: {
        this->lp_data_.energy_purchase_value = tmp_value;

        this->save_initial_number_value_(this->preference_energy_purchase_value_, this->lp_data_.energy_purchase_value);

        if (this->lp_data_.energy_purchase_state) {
          if (this->send_command_(SmCommandSend::SET_PURCHASE_DATA, false)) {
            this->send_command_(SmCommandSend::SET_PURCHASE_DATA, true);
          }
        }

        UPDATE_NUMBER(energy_purchase_value, this->lp_data_.energy_purchase_value)
        break;
      }
      case SmIdEntity::NUMBER_ENERGY_PURCHASE_ALARM: {
        this->lp_data_.energy_purchase_alarm = tmp_value;

        this->save_initial_number_value_(this->preference_energy_purchase_alarm_, this->lp_data_.energy_purchase_alarm);

        if (this->lp_data_.energy_purchase_state) {
          if (this->send_command_(SmCommandSend::SET_PURCHASE_DATA, false)) {
            this->send_command_(SmCommandSend::SET_PURCHASE_DATA, true);
          }
        }

        UPDATE_NUMBER(energy_purchase_alarm, this->lp_data_.energy_purchase_alarm)
        break;
      }
      case SmIdEntity::NUMBER_DELAY_VALUE_SET: {
        this->ms_data_.delay_value_set = tmp_value;

        this->save_initial_number_value_(this->preference_delay_value_set_, this->ms_data_.delay_value_set);

        if (this->ms_data_.delay_state) {
          this->send_command_(SmCommandSend::SET_DELAY, true);
        }

        UPDATE_NUMBER(delay_value_set, this->ms_data_.delay_value_set)
        break;
      }
      case SmIdEntity::NUMBER_STARTING_KWH: {
        this->ms_data_.starting_kWh = ((float) ((uint32_t) (value * 10))) / 10;

        this->save_initial_number_value_(this->preference_starting_kWh_, this->ms_data_.starting_kWh);

        UPDATE_SENSOR(contract_total_energy, this->ms_data_.starting_kWh + this->ms_data_.total_energy)

        UPDATE_NUMBER(starting_kWh, this->ms_data_.starting_kWh)
        break;
      }
      case SmIdEntity::NUMBER_PRICE_KWH: {
        this->ms_data_.price_kWh = ((float) ((uint32_t) (value * 10))) / 10;

        this->save_initial_number_value_(this->preference_price_kWh_, this->ms_data_.price_kWh);

        UPDATE_SENSOR(total_energy_price, this->ms_data_.price_kWh * this->ms_data_.total_energy)

        UPDATE_NUMBER(price_kWh, this->ms_data_.price_kWh)
        break;
      }
      default: {
        ESP_LOGE(TAG, "ID %hhu is not a NUMBER or is not included in the case list", entity);
        return;
      }
    }
  }
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

bool Dxs238xwComponent::first_data_acquisition_() {
  ESP_LOGI(TAG, "* Try to load GET_METER_ID");
  if (!this->send_command_(SmCommandSend::GET_METER_ID)) {
    return false;
  }

  ESP_LOGI(TAG, "* Try to load GET_POWER_STATE");
  if (!this->send_command_(SmCommandSend::GET_POWER_STATE)) {
    return false;
  }

  ESP_LOGI(TAG, "* Try to load GET_LIMIT_AND_PURCHASE_DATA");
  if (!this->send_command_(SmCommandSend::GET_LIMIT_AND_PURCHASE_DATA)) {
    return false;
  }

  ESP_LOGI(TAG, "* Try to load GET_MEASUREMENT_DATA");
  if (!this->send_command_(SmCommandSend::GET_MEASUREMENT_DATA)) {
    return false;
  }

  return true;
}

bool Dxs238xwComponent::transmit_serial_data_(uint8_t *array, uint8_t size) {
  while (available() > 0) {
    read();

    delay(2);
  }

  write_array(array, size);

  flush();

  ESP_LOGV(TAG, "* Waiting confirmation:");

  if (this->receive_serial_data_(array, HEKR_TYPE_SEND, array[4], size)) {
    ESP_LOGV(TAG, "* Successful Confirmation");

    return true;
  }

  ESP_LOGV(TAG, "* Confirmation Failed");

  return false;
}

bool Dxs238xwComponent::pre_transmit_serial_data_(uint8_t cmd, const uint8_t *array_data, uint8_t array_size) {
  static uint8_t version = 0;

  uint8_t send_array_size = (6 + array_size);
  uint8_t send_array[send_array_size];

  send_array[0] = HEKR_HEADER;
  send_array[1] = send_array_size;
  send_array[2] = HEKR_TYPE_SEND;
  send_array[3] = version++;
  send_array[4] = cmd;

  if (array_data != nullptr) {
    uint8_t send_array_index = 5;

    for (uint8_t i = 0; i < array_size; i++) {
      send_array[send_array_index] = array_data[i];
      send_array_index++;
    }
  }

  send_array[send_array_size - 1] = this->calculate_crc_(send_array, send_array_size);

  ESP_LOGV(TAG, "* Message send: %s", format_hex_pretty(send_array, send_array_size).c_str());

  return this->transmit_serial_data_(send_array, send_array_size);
}

bool Dxs238xwComponent::receive_serial_data_(uint8_t *array, uint8_t type_message, uint8_t cmd, uint8_t size_expected) {
  uint32_t response_time;
  uint8_t index_size = 0;

  SmErrorCode read_error = SmErrorCode::NO_ERROR;

  response_time = millis() + SM_MAX_MILLIS_TO_RESPONSE;

  while (true) {
    if (response_time < millis()) {
      if (index_size > 0) {
        read_error = SmErrorCode::NOT_ENOUGHT_BYTES;
      } else {
        read_error = SmErrorCode::TIMEOUT;
      }

      break;
    } else {
      if (available() > 0) {
        array[index_size] = read();

        if (index_size == 0 && array[0] != HEKR_HEADER) {
          ESP_LOGV(TAG, "* WRONG_BYTES: HEKR_HEADER / Expected = %u, Receive = %u", HEKR_HEADER, array[0]);
          read_error = SmErrorCode::WRONG_BYTES_HEADER;
          break;
        } else if (index_size == 1 && size_expected > 0 && array[1] != size_expected) {
          ESP_LOGV(TAG, "* WRONG_BYTES: HEKR_LENGTH / Expected = %u, Receive = %u", size_expected, array[1]);
          read_error = SmErrorCode::WRONG_BYTES_LENGTH;
          break;
        } else if (index_size == 2 && array[2] != type_message) {
          ESP_LOGV(TAG, "* WRONG_BYTES: HEKR_TYPE_MESSAGE / Expected = %u, Receive = %u", type_message, array[2]);
          read_error = SmErrorCode::WRONG_BYTES_TYPE_MESSAGE;
          break;
        } else if (index_size == 4 && cmd > 0 && array[4] != cmd) {
          ESP_LOGV(TAG, "* WRONG_BYTES: HEKR_COMMAND / Expected = %u, Receive = %u", cmd, array[4]);
          read_error = SmErrorCode::WRONG_BYTES_COMMAND;
          break;
        } else if (index_size > 4) {
          if (index_size == array[1] - 1) {
            ESP_LOGV(TAG, "* Message received: %s", format_hex_pretty(array, array[1]).c_str());

            if (this->calculate_crc_(array, array[1]) != array[index_size]) {
              read_error = SmErrorCode::CRC;
            }

            break;
          }
        }

        index_size++;
      }
    }

    yield();
  }

  delay(2);

  while (available() > 0) {
    read();

    delay(2);
  }

  if (read_error != SmErrorCode::NO_ERROR) {
    if (read_error == SmErrorCode::WRONG_BYTES_HEADER || read_error == SmErrorCode::WRONG_BYTES_LENGTH || read_error == SmErrorCode::WRONG_BYTES_TYPE_MESSAGE || read_error == SmErrorCode::WRONG_BYTES_COMMAND || read_error == SmErrorCode::CRC) {
      ESP_LOGD(TAG, "* Message with error received: %s", format_hex_pretty(array, array[1]).c_str());
    }

    if (type_message == HEKR_TYPE_SEND) {
      this->error_type_ = SmErrorType::COMMUNICATION_CONFIRMATION;
    } else {
      this->error_type_ = SmErrorType::COMMUNICATION_ANSWER;
    }

    this->error_code_ = read_error;
  }

  return (this->error_code_ == SmErrorCode::NO_ERROR);
}

bool Dxs238xwComponent::pre_receive_serial_data_(uint8_t cmd) {
  ESP_LOGV(TAG, "* Waiting answer:");

  if (this->receive_serial_data_(this->receive_array_, HEKR_TYPE_RECEIVE, cmd)) {
    ESP_LOGV(TAG, "* Successful answer");

    return true;
  }

  ESP_LOGV(TAG, "* Failed answer");

  return false;
}

void Dxs238xwComponent::process_and_update_data_(const uint8_t *receive_array) {
  switch (receive_array[4]) {
    case HEKR_CMD_RECEIVE_METER_STATE: {
      this->ms_data_.time = millis();

      this->ms_data_.phase_count = receive_array[5];
      this->ms_data_.meter_state = receive_array[6];
      this->ms_data_.delay_state = receive_array[18];
      this->ms_data_.delay_value_remaining = (receive_array[16] << 8) | receive_array[17];

      this->ms_data_.total_energy = ((receive_array[7] << 24) | (receive_array[8] << 16) | (receive_array[9] << 8) | receive_array[10]) * 0.01;

      if (this->ms_data_.meter_state) {
        this->ms_data_.warning_off_by_over_voltage = false;
        this->ms_data_.warning_off_by_under_voltage = false;
        this->ms_data_.warning_off_by_over_current = false;
        this->ms_data_.warning_off_by_end_purchase = false;
        this->ms_data_.warning_off_by_end_delay = false;
        this->ms_data_.warning_off_by_user = false;
      }

      if (!this->ms_data_.warning_off_by_over_voltage) {
        this->ms_data_.warning_off_by_over_voltage = (receive_array[11] == 1);
      }

      if (!this->ms_data_.warning_off_by_under_voltage) {
        this->ms_data_.warning_off_by_under_voltage = (receive_array[11] == 2);
      }

      if (!this->ms_data_.warning_off_by_over_current) {
        this->ms_data_.warning_off_by_over_current = receive_array[15];
      }

      if (!this->ms_data_.warning_off_by_end_purchase) {
        this->ms_data_.warning_off_by_end_purchase = (receive_array[1] == 21 ? receive_array[19] : false);
      }

      if (!this->ms_data_.warning_off_by_end_delay && this->get_component_state() == COMPONENT_STATE_LOOP) {
        this->ms_data_.warning_off_by_end_delay = (this->ms_data_.delay_value_remaining == 0 && this->ms_data_.delay_state);
      }

      if (this->ms_data_.warning_off_by_end_delay && this->ms_data_.meter_state) {
        ESP_LOGD(TAG, "* End Delay, trying to set Power State to off");

        if (this->send_command_(SmCommandSend::SET_POWER_STATE, false, false)) {
          this->ms_data_.meter_state = receive_array[6];
        }

        ESP_LOGD(TAG, "* New Power State = %s", ONOFF(this->ms_data_.meter_state));
      }

      if ((this->ms_data_.warning_off_by_end_delay && this->ms_data_.delay_state) || (this->get_component_state() == COMPONENT_STATE_SETUP && this->ms_data_.delay_state)) {
        ESP_LOGD(TAG, "* End Delay, trying to set Delay State to off");

        if (this->send_command_(SmCommandSend::SET_DELAY, false, false)) {
          this->ms_data_.delay_state = receive_array[18];
        }

        ESP_LOGD(TAG, "* New Delay State = %s", ONOFF(this->ms_data_.delay_state));
      }

      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      UPDATE_TEXT_SENSOR(delay_value_remaining, this->get_delay_value_remaining_string_(this->ms_data_.delay_value_remaining))

      UPDATE_BINARY_SENSOR(warning_off_by_over_voltage, this->ms_data_.warning_off_by_over_voltage)
      UPDATE_BINARY_SENSOR(warning_off_by_under_voltage, this->ms_data_.warning_off_by_under_voltage)
      UPDATE_BINARY_SENSOR(warning_off_by_over_current, this->ms_data_.warning_off_by_over_current)
      UPDATE_BINARY_SENSOR(warning_off_by_end_purchase, this->ms_data_.warning_off_by_end_purchase)
      UPDATE_BINARY_SENSOR(warning_off_by_end_delay, this->ms_data_.warning_off_by_end_delay)
      UPDATE_BINARY_SENSOR(warning_off_by_user, this->ms_data_.warning_off_by_user)

      UPDATE_BINARY_SENSOR(meter_state, this->ms_data_.meter_state)

      UPDATE_SENSOR(phase_count, this->ms_data_.phase_count)

      UPDATE_SENSOR(contract_total_energy, this->ms_data_.starting_kWh + this->ms_data_.total_energy)
      UPDATE_SENSOR(total_energy_price, this->ms_data_.price_kWh * this->ms_data_.total_energy)

      UPDATE_SWITCH(meter_state, this->ms_data_.meter_state)
      UPDATE_SWITCH(delay_state, this->ms_data_.delay_state)

      this->update_meter_state_detail_();

      break;
    }
    case HEKR_CMD_RECEIVE_MEASUREMENT: {
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_1, ((receive_array[5] << 16) | (receive_array[6] << 8) | receive_array[7]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_2, ((receive_array[8] << 16) | (receive_array[9] << 8) | receive_array[10]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_3, ((receive_array[11] << 16) | (receive_array[12] << 8) | receive_array[13]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_1, ((receive_array[14] << 8) | receive_array[15]) * 0.1)
      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_2, ((receive_array[16] << 8) | receive_array[17]) * 0.1)
      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_3, ((receive_array[18] << 8) | receive_array[19]) * 0.1)

      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_total, ((receive_array[20] << 16) | (receive_array[21] << 8) | receive_array[22]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_1, ((receive_array[23] << 16) | (receive_array[24] << 8) | receive_array[25]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_2, ((receive_array[26] << 16) | (receive_array[27] << 8) | receive_array[28]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_3, ((receive_array[29] << 16) | (receive_array[30] << 8) | receive_array[31]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_total, ((receive_array[32] << 16) | (receive_array[33] << 8) | receive_array[34]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_1, ((receive_array[35] << 16) | (receive_array[36] << 8) | receive_array[37]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_2, ((receive_array[38] << 16) | (receive_array[39] << 8) | receive_array[40]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_3, ((receive_array[41] << 16) | (receive_array[42] << 8) | receive_array[43]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS(power_factor_total, ((receive_array[44] << 8) | receive_array[45]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_1, ((receive_array[46] << 8) | receive_array[47]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_2, ((receive_array[48] << 8) | receive_array[49]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_3, ((receive_array[50] << 8) | receive_array[51]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(total_energy, ((receive_array[54] << 24) | (receive_array[55] << 16) | (receive_array[56] << 8) | receive_array[57]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(import_active_energy, ((receive_array[58] << 24) | (receive_array[59] << 16) | (receive_array[60] << 8) | receive_array[61]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(export_active_energy, ((receive_array[62] << 24) | (receive_array[63] << 16) | (receive_array[64] << 8) | receive_array[65]) * -0.01)

      UPDATE_SENSOR_MEASUREMENTS(frequency, ((receive_array[52] << 8) | receive_array[53]) * 0.01)

      break;
    }
    case HEKR_CMD_RECEIVE_LIMIT_AND_PURCHASE: {
      this->lp_data_.time = millis();

      this->lp_data_.max_voltage_limit = (receive_array[5] << 8) | receive_array[6];
      this->lp_data_.min_voltage_limit = (receive_array[7] << 8) | receive_array[8];
      this->lp_data_.max_current_limit = ((receive_array[9] << 8) | receive_array[10]) * 0.01;

      UPDATE_NUMBER(max_voltage_limit, this->lp_data_.max_voltage_limit)
      UPDATE_NUMBER(min_voltage_limit, this->lp_data_.min_voltage_limit)
      UPDATE_NUMBER(max_current_limit, this->lp_data_.max_current_limit)

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if (receive_array[1] == 25) {
        this->lp_data_.energy_purchase_state = receive_array[23];

        // this->lp_data_.energy_purchase_value = (((receive_array[11] << 24) | (receive_array[12] << 16) | (receive_array[13] << 8) | receive_array[14]) * 0.01);
        // this->lp_data_.energy_purchase_alarm = (((receive_array[19] << 24) | (receive_array[20] << 16) | (receive_array[21] << 8) | receive_array[22]) * 0.01);
        this->lp_data_.energy_purchase_balance = (((receive_array[15] << 24) | (receive_array[16] << 16) | (receive_array[17] << 8) | receive_array[18]) * 0.01);

        this->ms_data_.warning_purchase_alarm = ((this->lp_data_.energy_purchase_balance <= this->lp_data_.energy_purchase_alarm) && this->lp_data_.energy_purchase_state);
      }

      UPDATE_SENSOR(energy_purchase_balance, this->lp_data_.energy_purchase_balance)
      UPDATE_SENSOR(energy_purchase_price, this->lp_data_.energy_purchase_balance * this->ms_data_.price_kWh)

      UPDATE_SWITCH(energy_purchase_state, this->lp_data_.energy_purchase_state)

      UPDATE_BINARY_SENSOR(warning_purchase_alarm, this->ms_data_.warning_purchase_alarm)

      break;
    }
    case HEKR_CMD_RECEIVE_METER_ID: {
      char serial_number[20];

      sprintf(serial_number, "%02u%02u%02u %02u%02u%02u", receive_array[5], receive_array[6], receive_array[7], receive_array[8], receive_array[9], receive_array[10]);
      std::string string_serial_number(serial_number);

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      UPDATE_TEXT_SENSOR(meter_id, string_serial_number)

      break;
    }
  }
}

bool Dxs238xwComponent::send_command_(SmCommandSend cmd, bool state, bool process_data) {
  bool is_good_communication = false;

  switch (cmd) {
    case SmCommandSend::GET_POWER_STATE: {
      if ((millis() - this->ms_data_.time) >= SM_MIN_INTERVAL_TO_GET_DATA) {
        ESP_LOGV(TAG, "In --- send_command - GET_POWER_STATE");

        is_good_communication = this->put_command_data_(HEKR_CMD_SEND_GET_METER_STATE, HEKR_CMD_RECEIVE_METER_STATE);

        ESP_LOGV(TAG, "Out --- send_command - GET_POWER_STATE - Communication Result = %s", TRUEFALSE(is_good_communication));
      }

      break;
    }
    case SmCommandSend::GET_MEASUREMENT_DATA: {
      ESP_LOGV(TAG, "In --- send_command - GET_MEASUREMENT_DATA");

      is_good_communication = this->put_command_data_(HEKR_CMD_SEND_GET_MEASUREMENT, HEKR_CMD_RECEIVE_MEASUREMENT);

      ESP_LOGV(TAG, "Out --- send_command - GET_MEASUREMENT_DATA - Communication Result = %s", TRUEFALSE(is_good_communication));

      break;
    }
    case SmCommandSend::GET_LIMIT_AND_PURCHASE_DATA: {
      if ((millis() - this->lp_data_.time) >= SM_MIN_INTERVAL_TO_GET_DATA) {
        ESP_LOGV(TAG, "In --- send_command - GET_LIMIT_AND_PURCHASE_DATA");

        is_good_communication = this->put_command_data_(HEKR_CMD_SEND_GET_LIMIT_AND_PURCHASE, HEKR_CMD_RECEIVE_LIMIT_AND_PURCHASE);

        ESP_LOGV(TAG, "Out --- send_command - GET_LIMIT_AND_PURCHASE_DATA - Communication Result = %s", TRUEFALSE(is_good_communication));
      }

      break;
    }
    case SmCommandSend::GET_METER_ID: {
      ESP_LOGV(TAG, "In --- send_command - GET_METER_ID");

      is_good_communication = this->put_command_data_(HEKR_CMD_SEND_GET_METER_ID, HEKR_CMD_RECEIVE_METER_ID);

      ESP_LOGV(TAG, "Out --- send_command - GET_METER_ID - Communication Result = %s", TRUEFALSE(is_good_communication));

      break;
    }
    case SmCommandSend::SET_LIMIT_DATA: {
      ESP_LOGD(TAG, "In --- send_command - SET_LIMIT_DATA");

      uint16_t tmp_current_limit = this->lp_data_.max_current_limit * 100;

      uint8_t array_size = 6;
      uint8_t array_data[array_size];

      array_data[0] = (tmp_current_limit >> 8);
      array_data[1] = (tmp_current_limit & SM_GET_ONE_BYTE);

      array_data[2] = (this->lp_data_.max_voltage_limit >> 8);
      array_data[3] = (this->lp_data_.max_voltage_limit & SM_GET_ONE_BYTE);

      array_data[4] = (this->lp_data_.min_voltage_limit >> 8);
      array_data[5] = (this->lp_data_.min_voltage_limit & SM_GET_ONE_BYTE);

      is_good_communication = this->put_command_data_(HEKR_CMD_SEND_SET_LIMIT, HEKR_CMD_RECEIVE_LIMIT_AND_PURCHASE, array_data, array_size, process_data);

      ESP_LOGD(TAG, "* Input Data: max_current_limit = %u, max_voltage_limit = %u, min_voltage_limit = %u", this->lp_data_.max_current_limit, this->lp_data_.max_voltage_limit, this->lp_data_.min_voltage_limit);
      ESP_LOGD(TAG, "Out --- send_command - SET_LIMIT_DATA - Communication Result = %s", TRUEFALSE(is_good_communication));

      break;
    }
    case SmCommandSend::SET_PURCHASE_DATA: {
      ESP_LOGD(TAG, "In --- send_command - SET_PURCHASE_DATA");

      uint32_t purchase_value = 0;
      uint32_t purchase_alarm = 0;

      if (state) {
        purchase_value = this->lp_data_.energy_purchase_value * 100;
        purchase_alarm = this->lp_data_.energy_purchase_alarm * 100;
      }

      uint8_t array_size = 9;
      uint8_t array_data[array_size];

      array_data[0] = (purchase_value >> 24);
      array_data[1] = (purchase_value >> 16);
      array_data[2] = (purchase_value >> 8);
      array_data[3] = (purchase_value & SM_GET_ONE_BYTE);

      array_data[4] = (purchase_alarm >> 24);
      array_data[5] = (purchase_alarm >> 16);
      array_data[6] = (purchase_alarm >> 8);
      array_data[7] = (purchase_alarm & SM_GET_ONE_BYTE);

      array_data[8] = state;

      is_good_communication = this->put_command_data_(HEKR_CMD_SEND_SET_PURCHASE, HEKR_CMD_RECEIVE_LIMIT_AND_PURCHASE, array_data, array_size, process_data);

      ESP_LOGD(TAG, "* Input Data: purchase_value = %u, purchase_alarm = %u, state = %s", (state ? this->lp_data_.energy_purchase_value : 0), (state ? this->lp_data_.energy_purchase_alarm : 0), ONOFF(state));
      ESP_LOGD(TAG, "Out --- send_command - SET_PURCHASE_DATA - Communication Result = %s", TRUEFALSE(is_good_communication));

      break;
    }
    case SmCommandSend::SET_POWER_STATE: {
      ESP_LOGD(TAG, "In --- send_command - SET_POWER_STATE");

      uint8_t array_size = 1;
      uint8_t array_data[array_size];

      array_data[0] = state;

      is_good_communication = this->put_command_data_(HEKR_CMD_SEND_SET_METER_STATE, HEKR_CMD_RECEIVE_METER_STATE, array_data, array_size, process_data);

      ESP_LOGD(TAG, "* Input Data: state = %s", ONOFF(state));
      ESP_LOGD(TAG, "Out --- send_command - SET_POWER_STATE - Communication Result = %s", TRUEFALSE(is_good_communication));

      break;
    }
    case SmCommandSend::SET_DELAY: {
      ESP_LOGD(TAG, "In --- send_command - SET_DELAY");

      uint16_t delay_value_set = 0;

      if (state) {
        delay_value_set = this->ms_data_.delay_value_set;
      }

      uint8_t array_size = 3;
      uint8_t array_data[array_size];

      array_data[0] = (delay_value_set >> 8);
      array_data[1] = (delay_value_set & SM_GET_ONE_BYTE);

      array_data[2] = state;

      is_good_communication = this->put_command_data_(HEKR_CMD_SEND_SET_DELAY, HEKR_CMD_RECEIVE_METER_STATE, array_data, array_size, process_data);

      ESP_LOGD(TAG, "* Input Data: delay_value_set = %u, state = %s", delay_value_set, ONOFF(state));
      ESP_LOGD(TAG, "Out --- send_command - SET_DELAY - Communication Result = %s", TRUEFALSE(is_good_communication));

      break;
    }
    case SmCommandSend::SET_RESET: {
      ESP_LOGD(TAG, "In --- send_command - SET_RESET");

      is_good_communication = this->put_command_data_(HEKR_CMD_SEND_SET_RESET, HEKR_CMD_RECEIVE_MEASUREMENT);

      ESP_LOGD(TAG, "Out --- send_command - SET_RESET - Communication Result = %s", TRUEFALSE(is_good_communication));

      break;
    }
  }

  return is_good_communication;
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::update_meter_state_detail_() {
  SmErrorMeterStateType tmp_meter_state_detail = SmErrorMeterStateType::UNKNOWN;

  if (this->ms_data_.meter_state) {
    tmp_meter_state_detail = SmErrorMeterStateType::POWER_OK;
  } else {
    if (this->ms_data_.warning_off_by_over_current) {
      tmp_meter_state_detail = SmErrorMeterStateType::OVER_CURRENT;
    } else if (this->ms_data_.warning_off_by_over_voltage) {
      tmp_meter_state_detail = SmErrorMeterStateType::OVER_VOLTAGE;
    } else if (this->ms_data_.warning_off_by_under_voltage) {
      tmp_meter_state_detail = SmErrorMeterStateType::UNDER_VOLTAGE;
    } else if (this->ms_data_.warning_off_by_end_delay) {
      tmp_meter_state_detail = SmErrorMeterStateType::END_DELAY;
    } else if (this->ms_data_.warning_off_by_end_purchase) {
      tmp_meter_state_detail = SmErrorMeterStateType::END_PURCHASE;
    } else if (this->ms_data_.warning_off_by_user) {
      tmp_meter_state_detail = SmErrorMeterStateType::END_BY_USER;
    }
  }

  if (tmp_meter_state_detail != this->ms_data_.meter_state_detail) {
    this->ms_data_.meter_state_detail = tmp_meter_state_detail;

    std::string meter_state_detail_string = this->get_meter_state_string_(tmp_meter_state_detail);
    UPDATE_TEXT_SENSOR(meter_state_detail, meter_state_detail_string)
  }
}

bool Dxs238xwComponent::put_command_data_(uint8_t cmd_send, uint8_t cmd_receive, const uint8_t *array_data, uint8_t array_size, bool process_data) {
  this->error_type_ = SmErrorType::NO_ERROR;
  this->error_code_ = SmErrorCode::NO_ERROR;

  if (this->pre_transmit_serial_data_(cmd_send, array_data, array_size)) {
    if (this->pre_receive_serial_data_(cmd_receive)) {
      if (process_data) {
        this->process_and_update_data_(this->receive_array_);
      }

      return true;
    }
  }

  this->print_error_();

  return false;
}

void Dxs238xwComponent::incoming_messages_() {
  if (available() == 0) {
    return;
  }

  ESP_LOGI(TAG, "Incoming message");

  if (this->receive_serial_data_(this->receive_array_, HEKR_TYPE_RECEIVE)) {
    ESP_LOGI(TAG, "* Successful message arrived: %s", format_hex_pretty(this->receive_array_, this->receive_array_[1]).c_str());

    this->process_and_update_data_(this->receive_array_);

    return;
  }

  ESP_LOGI(TAG, "* Failed message");

  this->print_error_();
}

std::string Dxs238xwComponent::get_delay_value_remaining_string_(uint16_t value) {
  uint32_t seconds = value * 60;
  uint16_t days = seconds / (24 * 3600);
  seconds = seconds % (24 * 3600);
  uint8_t hours = seconds / 3600;
  seconds = seconds % 3600;
  uint8_t minutes = seconds / 60;

  return ((days ? to_string(days) + "d " : "") + (hours ? to_string(hours) + "h " : "") + (to_string(minutes) + "m"));
}

std::string Dxs238xwComponent::get_meter_state_string_(SmErrorMeterStateType error) {
  switch (error) {
    case SmErrorMeterStateType::POWER_OK:
      return SM_STR_POWER_STATE_DETAILS_POWER_OK;
      break;
    case SmErrorMeterStateType::OVER_VOLTAGE:
      return SM_STR_POWER_STATE_DETAILS_OVER_VOLTAGE;
      break;
    case SmErrorMeterStateType::UNDER_VOLTAGE:
      return SM_STR_POWER_STATE_DETAILS_UNDER_VOLTAGE;
      break;
    case SmErrorMeterStateType::OVER_CURRENT:
      return SM_STR_POWER_STATE_DETAILS_OVER_CURRENT;
      break;
    case SmErrorMeterStateType::END_PURCHASE:
      return SM_STR_POWER_STATE_DETAILS_END_PURCHASE;
      break;
    case SmErrorMeterStateType::END_DELAY:
      return SM_STR_POWER_STATE_DETAILS_END_DELAY;
      break;
    case SmErrorMeterStateType::END_BY_USER:
      return SM_STR_POWER_STATE_DETAILS_END_BY_USER;
      break;
    case SmErrorMeterStateType::UNKNOWN:
      return SM_STR_POWER_STATE_DETAILS_UNKNOWN;
      break;
  }

  return SM_STR_POWER_STATE_DETAILS_UNKNOWN;
}

uint8_t Dxs238xwComponent::calculate_crc_(const uint8_t *array, uint8_t size) {
  uint16_t tmp_crc = 0;
  uint8_t tmp_size = size - 1;

  for (uint8_t n = 0; n < tmp_size; n++) {
    tmp_crc = tmp_crc + array[n];
  }

  return tmp_crc & SM_GET_ONE_BYTE;
}

void Dxs238xwComponent::print_error_() {
  std::string string_type;
  std::string string_code;

  switch (this->error_type_) {
    case SmErrorType::NO_ERROR:
      string_type = SM_STR_TYPE_NO_ERROR;
      break;
    case SmErrorType::COMMUNICATION_CONFIRMATION:
      string_type = SM_STR_TYPE_COMMUNICATION_CONFIRMATION;
      break;
    case SmErrorType::COMMUNICATION_ANSWER:
      string_type = SM_STR_TYPE_COMMUNICATION_ANSWER;
      break;
    case SmErrorType::INPUT_DATA:
      string_type = SM_STR_TYPE_INPUT_DATA;
      break;
  }

  switch (this->error_code_) {
    case SmErrorCode::NO_ERROR:
      string_code = SM_STR_CODE_NO_ERROR;
      break;
    case SmErrorCode::CRC:
      string_code = SM_STR_CODE_CRC;
      break;
    case SmErrorCode::WRONG_BYTES_HEADER:
      string_code = SM_STR_CODE_WRONG_BYTES_HEADER;
      break;
    case SmErrorCode::WRONG_BYTES_LENGTH:
      string_code = SM_STR_CODE_WRONG_BYTES_LENGTH;
      break;
    case SmErrorCode::WRONG_BYTES_TYPE_MESSAGE:
      string_code = SM_STR_CODE_WRONG_BYTES_TYPE_MESSAGE;
      break;
    case SmErrorCode::WRONG_BYTES_COMMAND:
      string_code = SM_STR_CODE_WRONG_BYTES_COMMAND;
      break;
    case SmErrorCode::NOT_ENOUGHT_BYTES:
      string_code = SM_STR_CODE_NOT_ENOUGH_BYTES;
      break;
    case SmErrorCode::TIMEOUT:
      string_code = SM_STR_CODE_TIMED_OUT;
      break;
    case SmErrorCode::WRONG_MSG:
      string_code = SM_STR_CODE_WRONG_MSG;
      break;
    case SmErrorCode::MESSAGE_LENGTH:
      string_code = SM_STR_CODE_MESSAGE_LENGTH;
      break;
  }

  ESP_LOGE(TAG, "* Error, Type: %s, Description: %s", string_type.c_str(), string_code.c_str());

  this->error_type_ = SmErrorType::NO_ERROR;
  this->error_code_ = SmErrorCode::NO_ERROR;
}

uint32_t Dxs238xwComponent::read_initial_number_value_(ESPPreferenceObject &preference, const std::string preference_string, uint32_t default_value) {
  uint32_t initial_value = 0;

  if (!preference.load(&initial_value)) {
    ESP_LOGE(TAG, "* Error load initial value %s, return default value = %u", preference_string.c_str(), default_value);

    this->save_initial_number_value_(preference, default_value);

    return default_value;
  }

  ESP_LOGD(TAG, "* Load initial value %s, return value = %u", preference_string.c_str(), initial_value);

  return initial_value;
}

void Dxs238xwComponent::save_initial_number_value_(ESPPreferenceObject &preference, uint32_t value) {
  if (preference.save(&value)) {
    ESP_LOGD(TAG, "* Save new initial value = %u", value);
  } else {
    ESP_LOGE(TAG, "* Error save new initial value = %u", value);
  }
}

float Dxs238xwComponent::read_initial_number_value_(ESPPreferenceObject &preference, const std::string preference_string, float default_value) {
  float initial_value = 0;

  if (!preference.load(&initial_value)) {
    ESP_LOGE(TAG, "* Error load initial value %s, return default value = %f", preference_string.c_str(), default_value);

    this->save_initial_number_value_(preference, default_value);

    return default_value;
  }

  ESP_LOGD(TAG, "* Load initial value %s, return value = %f", preference_string.c_str(), initial_value);

  return ((float) ((uint32_t) (initial_value * 10))) / 10;
}

void Dxs238xwComponent::save_initial_number_value_(ESPPreferenceObject &preference, float value) {
  if (preference.save(&value)) {
    ESP_LOGD(TAG, "* Save new initial value = %f", value);
  } else {
    ESP_LOGE(TAG, "* Error save new initial value = %f", value);
  }
}

}  // namespace dxs238xw
}  // namespace esphome
