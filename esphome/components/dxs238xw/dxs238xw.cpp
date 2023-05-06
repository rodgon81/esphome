#include "dxs238xw.h"

#ifdef USE_WIFI
#include "esphome/components/wifi/wifi_component.h"
#endif

#ifdef USE_CAPTIVE_PORTAL
#include "esphome/components/captive_portal/captive_portal.h"
#endif

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

#define UPDATE_SENSOR_FORCE(name, value) \
  if (this->name##_sensor_ != nullptr) { \
    if (this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_sensor_->publish_state(value); \
    } \
  }

#define UPDATE_SENSOR_CASE(name_case, name, value) \
  case DatapointId::name_case: { \
    UPDATE_SENSOR(name, value) \
    break; \
  }

#define UPDATE_SENSOR_CASE_FORCE(name_case, name, value) \
  case DatapointId::name_case: { \
    UPDATE_SENSOR_FORCE(name, value) \
    break; \
  }

#else
#define UPDATE_SENSOR(name, value)
#define UPDATE_SENSOR_FORCE(name, value)
#define UPDATE_SENSOR_CASE(name, value)
#define UPDATE_SENSOR_CASE_FORCE(name, value)
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
    if ((this->name##_number_->state != (value)) || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_number_->publish_state(value); \
    } \
  }

#define UPDATE_NUMBER_FORCE(name, value) \
  if (this->name##_number_ != nullptr) { \
    if (this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_number_->publish_state(value); \
    } \
  }
#else
#define UPDATE_NUMBER(name, value)
#define UPDATE_NUMBER_FORCE(name, value)
#endif

#ifdef USE_SWITCH
#define UPDATE_SWITCH(name, value) \
  if (this->name##_switch_ != nullptr) { \
    if (this->name##_switch_->state != (value) || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_switch_->publish_state(value); \
    } \
  }

#define UPDATE_SWITCH_FORCE(name, value) \
  if (this->name##_switch_ != nullptr) { \
    if (this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_switch_->publish_state(value); \
    } \
  }
#else
#define UPDATE_SWITCH(name, value)
#define UPDATE_SWITCH_FORCE(name, value)
#endif

#define LOAD_PREFERENCE(name, preference_string, default_value) \
  this->preference_##name##_ = global_preferences->make_preference<uint32_t>(this->hash_##name##_); \
  if (this->name##_number_ != nullptr) { \
    this->data_.name = this->read_initial_number_value_(this->preference_##name##_, preference_string, default_value); \
  }

#define LOAD_PREFERENCE_FLOAT(name, preference_string, default_value) \
  this->preference_##name##_ = global_preferences->make_preference<uint32_t>(this->hash_##name##_); \
  if (this->name##_number_ != nullptr) { \
    this->data_.name = this->read_initial_number_value_(this->preference_##name##_, preference_string, default_value) * 0.1; \
  }

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

  if (this->postpone_setup_time_ > millis()) {
    this->component_state_ &= ~COMPONENT_STATE_MASK;
    this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;

    this->load_preferences = true;
  } else {
    if (this->load_preferences) {
      ESP_LOGI(TAG, "In --- setup");

      ESP_LOGI(TAG, "* Get Initial Values");
#ifdef USE_MODEL_DDS238_2
      LOAD_PREFERENCE_FLOAT(starting_kWh, SM_STR_STARTING_KWH, SmLimitValue::MIN_STARTING_KWH)
      LOAD_PREFERENCE_FLOAT(price_kWh, SM_STR_PRICE_KWH, SmLimitValue::MIN_PRICE_KWH)

      UPDATE_NUMBER(starting_kWh, this->data_.starting_kWh)
      UPDATE_NUMBER(price_kWh, this->data_.price_kWh)
      UPDATE_SENSOR(price_kWh, this->data_.price_kWh)
#endif

#ifdef USE_MODEL_DDS238_4
      LOAD_PREFERENCE(delay_value_set, SM_STR_DELAY_VALUE_SET, SmLimitValue::MAX_DELAY_SET)
      LOAD_PREFERENCE_FLOAT(starting_kWh, SM_STR_STARTING_KWH, SmLimitValue::MIN_STARTING_KWH)
      LOAD_PREFERENCE_FLOAT(price_kWh, SM_STR_PRICE_KWH, SmLimitValue::MIN_PRICE_KWH)
      LOAD_PREFERENCE(energy_purchase_value, SM_STR_ENERGY_PURCHASE_VALUE, SmLimitValue::MIN_ENERGY_PURCHASE_VALUE)
      LOAD_PREFERENCE(energy_purchase_alarm, SM_STR_ENERGY_PURCHASE_ALARM, SmLimitValue::MIN_ENERGY_PURCHASE_ALARM)

      UPDATE_NUMBER(delay_value_set, this->data_.delay_value_set)
      UPDATE_NUMBER(starting_kWh, this->data_.starting_kWh)
      UPDATE_NUMBER(price_kWh, this->data_.price_kWh)
      UPDATE_SENSOR(price_kWh, this->data_.price_kWh)
      UPDATE_NUMBER(energy_purchase_value, this->data_.energy_purchase_value)
      UPDATE_NUMBER(energy_purchase_alarm, this->data_.energy_purchase_alarm)
#endif

#ifdef USE_MODEL_DTS238_7
      LOAD_PREFERENCE(delay_value_set, SM_STR_DELAY_VALUE_SET, SmLimitValue::MAX_DELAY_SET)
      LOAD_PREFERENCE_FLOAT(starting_kWh, SM_STR_STARTING_KWH, SmLimitValue::MIN_STARTING_KWH)
      LOAD_PREFERENCE_FLOAT(price_kWh, SM_STR_PRICE_KWH, SmLimitValue::MIN_PRICE_KWH)
      LOAD_PREFERENCE(energy_purchase_value, SM_STR_ENERGY_PURCHASE_VALUE, SmLimitValue::MIN_ENERGY_PURCHASE_VALUE)
      LOAD_PREFERENCE(energy_purchase_alarm, SM_STR_ENERGY_PURCHASE_ALARM, SmLimitValue::MIN_ENERGY_PURCHASE_ALARM)

      UPDATE_NUMBER(delay_value_set, this->data_.delay_value_set)
      UPDATE_NUMBER(starting_kWh, this->data_.starting_kWh)
      UPDATE_NUMBER(price_kWh, this->data_.price_kWh)
      UPDATE_SENSOR(price_kWh, this->data_.price_kWh)
      UPDATE_NUMBER(energy_purchase_value, this->data_.energy_purchase_value)
      UPDATE_NUMBER(energy_purchase_alarm, this->data_.energy_purchase_alarm)
#endif

      this->load_preferences = false;
    }

    this->incoming_messages_();

    this->process_command_queue_();

    if (!this->is_expected_message()) {
      if (!this->first_data_acquisition_()) {
        this->component_state_ &= ~COMPONENT_STATE_MASK;
        this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;

        return;
      } else {
        if (this->count_error_data_acquisition_ > 0) {
          this->status_clear_error();
          this->count_error_data_acquisition_ = 0;
        }
      }
    } else {
      this->component_state_ &= ~COMPONENT_STATE_MASK;
      this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;

      return;
    }

#ifdef USE_PROTOCOL_HEKR
    this->set_interval("get_data", SM_MIN_INTERVAL_TO_GET_DATA, [this] {
      if (this->get_component_state() == COMPONENT_STATE_LOOP) {
        this->send_command_(SmCommandType::GET_METER_STATE);
        this->send_command_(SmCommandType::GET_LIMIT_AND_PURCHASE);
      }
    });
#endif

#ifdef USE_PROTOCOL_TUYA
    this->set_interval("heartbeat", 1000, [this] { this->push_command_(SmCommand{.cmd = CommandType::HEARTBEAT}); });

    if (this->status_pin_.has_value()) {
      this->status_pin_.value()->digital_write(false);
    }
#endif

    this->set_interval("log_command_queue", 2000, [this] { ESP_LOGI(TAG, "Comandos en cola %u", this->command_queue_.size()); });

    ESP_LOGI(TAG, "Out --- setup");
  }
}

void Dxs238xwComponent::loop() {
  this->incoming_messages_();

  this->process_command_queue_();
}

void Dxs238xwComponent::update() {
#ifdef USE_PROTOCOL_HEKR
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    this->send_command_(SmCommandType::GET_MEASUREMENT);
  }
#endif
}

void Dxs238xwComponent::dump_config() {
  LOG_UPDATE_INTERVAL(this)
  ESP_LOGCONFIG(TAG, "*** COMPONENT VERSION: %s ***", SM_STR_COMPONENT_VERSION);
  ESP_LOGCONFIG(TAG, "*** Meter Model: %s ***", SM_STR_METER_MODEL);
  ESP_LOGCONFIG(TAG, "*** Protocol: %s ***", SM_STR_PROTOCOL);
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

bool Dxs238xwComponent::first_data_acquisition_() {
  switch (this->index_first_command) {
#ifdef USE_PROTOCOL_HEKR
    case 0:
      ESP_LOGI(TAG, "* Try to load GET_METER_ID");
      this->send_command_(SmCommandType::GET_METER_ID);

      break;
    case 1:
      ESP_LOGI(TAG, "* Try to load GET_METER_STATE");
      this->send_command_(SmCommandType::GET_METER_STATE);

      break;
    case 2:
      ESP_LOGI(TAG, "* Try to load GET_LIMIT_AND_PURCHASE");
      this->send_command_(SmCommandType::GET_LIMIT_AND_PURCHASE);

      break;
    case 3:
      ESP_LOGI(TAG, "* Try to load GET_MEASUREMENT");
      this->send_command_(SmCommandType::GET_MEASUREMENT);

      break;
#endif
    default:
      return true;

      break;
  }

  this->index_first_command++;

  return false;
}

void Dxs238xwComponent::incoming_messages_() {
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);

    this->rx_message_.push_back(byte);

    if (!this->validate_message_()) {
      this->rx_message_.clear();

      this->last_rx_char_timestamp_ = 0;

      while (this->available()) {
        this->read();

        delay(2);

        yield();
      }

      this->print_error_();
    } else {
      this->last_rx_char_timestamp_ = millis();
    }
  }
}

bool Dxs238xwComponent::validate_message_() {
  uint32_t at = this->rx_message_.size() - 1;
  auto *data = &this->rx_message_[0];
  uint8_t new_byte = data[at];

#ifdef USE_PROTOCOL_HEKR

  if (at == 0) {
    if (new_byte == HEKR_HEADER) {
      return true;
    } else {
      ESP_LOGW(TAG, "* WRONG_BYTES: HEKR_HEADER / Expected = %02X, Receive = %02X", HEKR_HEADER, new_byte);
      this->error_code_ = SmErrorCode::WRONG_BYTES_HEADER;
      return false;
    }
  }

  uint8_t length = data[1];
  if (at == 1) {
    return true;
  }

  uint8_t command = data[2];
  if (at == 2) {
    return true;
  }

  if (at == 3) {
    // if (new_byte == this->version_msg) {
    return true;
    /*} else {
      ESP_LOGW(TAG, "* WRONG_BYTES: HEKR_VERSION_MESSAGE / Expected = %02X, Receive = %02X", HEKR_TYPE_RECEIVE, new_byte);
      this->error_code_ = SmErrorCode::WRONG_BYTES_TYPE_MESSAGE;
      return false;
    }*/
  }

  // wait until all data is read
  if (at < (length - 1)) {
    return true;
  }

  uint8_t calc_checksum = this->calculate_crc_(this->rx_message_.data(), at);

  if (new_byte != calc_checksum) {
    ESP_LOGW(TAG, "* WRONG_BYTES: CRC / Expected = %02X, Receive = %02X", calc_checksum, new_byte);
    this->error_code_ = SmErrorCode::CRC;
    return false;
  }

  const uint8_t *message_data = data + 4;
  uint8_t length_data = length - 5;

  if (!this->is_expected_message() || this->command_queue_.front().raw) {
    ESP_LOGI(TAG, "Hex Message: %s", format_hex_pretty(this->rx_message_).c_str());

    ESP_LOGI(TAG, "Received incoming message: CMD=0x%02X DATA=[%s] INIT_STATE=%u", command, format_hex_pretty(message_data, length_data).c_str(), static_cast<uint8_t>(this->init_state_));
  }

  this->handle_command_(command, message_data, length_data);

#endif

#ifdef USE_PROTOCOL_TUYA

  if (at == 0) {
    if (new_byte == TUYA_HEADER_1) {
      return true;
    } else {
      ESP_LOGW(TAG, "* WRONG_BYTES: TUYA_HEADER_1 / Expected = %02X, Receive = %02X", TUYA_HEADER_1, new_byte);
      this->error_code_ = SmErrorCode::WRONG_BYTES_HEADER;
      return false;
    }
  }

  // Byte 1: HEADER2 (always 0xAA)
  if (at == 1) {
    if (new_byte == TUYA_HEADER_2) {
      return true;
    } else {
      ESP_LOGW(TAG, "* WRONG_BYTES: TUYA_HEADER_2 / Expected = %02X, Receive = %02X", TUYA_HEADER_2, new_byte);
      this->error_code_ = SmErrorCode::WRONG_BYTES_HEADER;
      return false;
    }
  }

  // Byte 2: VERSION, no validation for the following fields:
  uint8_t version = data[2];
  if (at == 2) {
    return true;
  }

  // Byte 3: COMMAND
  uint8_t command = data[3];
  if (at == 3) {
    return true;
  }

  // Byte 4: LENGTH1
  // Byte 5: LENGTH2
  if (at <= 5) {
    // no validation for these fields
    return true;
  }

  uint16_t length = (uint16_t(data[4]) << 8) | (uint16_t(data[5]));

  // wait until all data is read
  if ((at - 6) < length) {
    return true;
  }

  uint8_t calc_checksum = this->calculate_crc_(this->rx_message_.data(), at);

  if (new_byte != calc_checksum) {
    ESP_LOGW(TAG, "* WRONG_BYTES: CRC / Expected = %02X, Receive = %02X", calc_checksum, new_byte);
    this->error_code_ = SmErrorCode::CRC;
    return false;
  }

  // valid message
  const uint8_t *message_data = data + 6;

  if (!this->is_expected_message() || this->command_queue_.front().raw) {
    ESP_LOGI(TAG, "Hex Message: %s", format_hex_pretty(this->rx_message_).c_str());

    ESP_LOGI(TAG, "Received incoming message: CMD=0x%02X VERSION=%u DATA=[%s] INIT_STATE=%u", command, version, format_hex_pretty(message_data, length).c_str(), static_cast<uint8_t>(this->init_state_));
  }

  this->handle_command_(command, version, message_data, length);

#endif

  // return false to reset rx buffer
  return false;
}

#ifdef USE_PROTOCOL_HEKR
void Dxs238xwComponent::handle_command_(uint8_t command, const uint8_t *buffer, size_t len) {
  CommandType command_type = (CommandType) command;

  if (this->expected_confirmation_.has_value()) {
    if (this->expected_confirmation_ == command_type) {
      if (this->command_queue_.front().raw) {
        ESP_LOGD(TAG, "* Successful Confirmation");
        ESP_LOGD(TAG, "* Waiting response:");
      } else {
        ESP_LOGV(TAG, "* Successful Confirmation");
        ESP_LOGV(TAG, "* Waiting response:");
      }

      this->expected_confirmation_.reset();

      this->expected_response_ = CommandType::RECEIVE;

      return;
    } else {
      if (this->command_queue_.front().raw) {
        ESP_LOGD(TAG, "* Confirmation Failed");
      } else {
        ESP_LOGV(TAG, "* Confirmation Failed");
      }

      ESP_LOGW(TAG, "* WRONG_BYTES: HEKR_COMMAND / Expected = %02X, Receive = %02X", this->expected_confirmation_.value(), command);

      this->error_type_ = SmErrorType::COMMUNICATION_CONFIRMATION;
      this->error_code_ = SmErrorCode::WRONG_BYTES_COMMAND;

      this->expected_confirmation_.reset();
      this->command_queue_.erase(this->command_queue_.begin());

      this->print_error_();

      return;
    }
  }

  if (this->expected_response_.has_value()) {
    if (this->expected_response_ == command_type) {
      if (this->command_queue_.front().raw) {
        ESP_LOGD(TAG, "* Successful response");
      } else {
        ESP_LOGV(TAG, "* Successful response");
      }

      this->expected_response_.reset();

      bool is_null_old_response = this->command_queue_.front().null_old_response;

      // comprobamos si no se declaro el mensaje nulo
      if (is_null_old_response) {
        ESP_LOGI(TAG, "* Anulamos respuesta recibida, ya que llego un comando mas actualizado");

        // procesamos otra vez el mensaje con los datos actualizados.
        this->command_queue_.front().null_old_response = false;

        return;
      } else {
        // Liberamos el mensaje retenido y lo borramos ya que si resivio respuesta
        this->command_queue_.erase(this->command_queue_.begin());
      }
    } else {
      if (this->command_queue_.front().raw) {
        ESP_LOGD(TAG, "* Failed response");
      } else {
        ESP_LOGV(TAG, "* Failed response");
      }

      ESP_LOGW(TAG, "* WRONG_BYTES: HEKR_COMMAND / Expected = %02X, Receive = %02X", this->expected_response_.value(), command);

      this->error_type_ = SmErrorType::COMMUNICATION_RESPONSE;
      this->error_code_ = SmErrorCode::WRONG_BYTES_COMMAND;

      this->expected_response_.reset();
      this->command_queue_.erase(this->command_queue_.begin());

      this->print_error_();
    }
  }

  switch (command_type) {
    case CommandType::RECEIVE: {
      this->process_and_update_data_(buffer, len);

      break;
    }
    case CommandType::MODULE_OPERATION: {
      break;
    }
    case CommandType::ERROR_FRAME: {
      break;
    }
    default:
      ESP_LOGE(TAG, "Invalid command (0x%02X) received", command);
  }
}

void Dxs238xwComponent::process_and_update_data_(const uint8_t *buffer, size_t len) {
  ResponseType command_type = (ResponseType) buffer[0];

  switch (command_type) {
    case ResponseType::HEKR_CMD_RECEIVE_METER_STATE: {
      ESP_LOGV(TAG, "* process_and_update METER_STATE");

      this->data_.phase_count = buffer[5 - 4];
      this->data_.meter_state = buffer[6 - 4];
      this->data_.delay_state = buffer[18 - 4];
      this->data_.delay_value_remaining = (buffer[16 - 4] << 8) | buffer[17 - 4];

      this->data_.total_energy = ((buffer[7 - 4] << 24) | (buffer[8 - 4] << 16) | (buffer[9 - 4] << 8) | buffer[10 - 4]) * 0.01;

      if (this->data_.meter_state) {
        this->data_.warning_off_by_over_voltage = false;
        this->data_.warning_off_by_under_voltage = false;
        this->data_.warning_off_by_over_current = false;
        this->data_.warning_off_by_end_purchase = false;
        this->data_.warning_off_by_end_delay = false;
        this->data_.warning_off_by_user = false;
      }

      if (!this->data_.warning_off_by_over_voltage) {
        this->data_.warning_off_by_over_voltage = (buffer[11 - 4] == 1);
      }

      if (!this->data_.warning_off_by_under_voltage) {
        this->data_.warning_off_by_under_voltage = (buffer[11 - 4] == 2);
      }

      if (!this->data_.warning_off_by_over_current) {
        this->data_.warning_off_by_over_current = buffer[15 - 4];
      }

      if (!this->data_.warning_off_by_end_purchase) {
        this->data_.warning_off_by_end_purchase = (len == 16 ? buffer[19 - 4] : false);
      }

      if (!this->data_.warning_off_by_end_delay && this->get_component_state() == COMPONENT_STATE_LOOP) {
        this->data_.warning_off_by_end_delay = (this->data_.delay_value_remaining == 0 && this->data_.delay_state);
      }

      if (this->data_.warning_off_by_end_delay && this->data_.meter_state) {
        ESP_LOGD(TAG, "* End Delay, send command to set Meter State to off");

        this->send_command_(SmCommandType::SET_METER_STATE, false);
      }

      if ((this->data_.warning_off_by_end_delay && this->data_.delay_state) || (this->get_component_state() == COMPONENT_STATE_SETUP && this->data_.delay_state)) {
        ESP_LOGD(TAG, "* End Delay, send command to set Delay State to off");

        this->send_command_(SmCommandType::SET_DELAY, false);
      }

      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      UPDATE_TEXT_SENSOR(delay_value_remaining, this->get_delay_value_remaining_string_(this->data_.delay_value_remaining))

      UPDATE_BINARY_SENSOR(warning_off_by_over_voltage, this->data_.warning_off_by_over_voltage)
      UPDATE_BINARY_SENSOR(warning_off_by_under_voltage, this->data_.warning_off_by_under_voltage)
      UPDATE_BINARY_SENSOR(warning_off_by_over_current, this->data_.warning_off_by_over_current)
      UPDATE_BINARY_SENSOR(warning_off_by_end_purchase, this->data_.warning_off_by_end_purchase)
      UPDATE_BINARY_SENSOR(warning_off_by_end_delay, this->data_.warning_off_by_end_delay)
      UPDATE_BINARY_SENSOR(warning_off_by_user, this->data_.warning_off_by_user)

      UPDATE_BINARY_SENSOR(meter_state, this->data_.meter_state)

      UPDATE_SENSOR(phase_count, this->data_.phase_count)

      UPDATE_SENSOR(contract_total_energy, this->data_.starting_kWh + this->data_.total_energy)
      UPDATE_SENSOR(total_energy_price, this->data_.price_kWh * this->data_.total_energy)

      UPDATE_SWITCH(meter_state, this->data_.meter_state)
      UPDATE_SWITCH(delay_state, this->data_.delay_state)

      this->update_meter_state_detail_();

      break;
    }
    case ResponseType::HEKR_CMD_RECEIVE_MEASUREMENT: {
      ESP_LOGV(TAG, "* process_and_update MEASUREMENT");

#ifdef USE_MODEL_DDS238_2
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current, ((buffer[5] << 16) | (buffer[6] << 8) | buffer[7]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(voltage, ((buffer[14] << 8) | buffer[15]) * 0.1)

      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power, ((buffer[23] << 16) | (buffer[24] << 8) | buffer[25]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power, ((buffer[35] << 16) | (buffer[36] << 8) | buffer[37]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS(power_factor, ((buffer[46] << 8) | buffer[47]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(total_energy, ((buffer[54] << 24) | (buffer[55] << 16) | (buffer[56] << 8) | buffer[57]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(import_active_energy, ((buffer[58] << 24) | (buffer[59] << 16) | (buffer[60] << 8) | buffer[61]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(export_active_energy, ((buffer[62] << 24) | (buffer[63] << 16) | (buffer[64] << 8) | buffer[65]) * -0.01)

      UPDATE_SENSOR_MEASUREMENTS(frequency, ((buffer[52] << 8) | buffer[53]) * 0.01)
#endif

#ifdef USE_MODEL_DDS238_4
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current, ((buffer[5 - 4] << 16) | (buffer[6 - 4] << 8) | buffer[7 - 4]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(voltage, ((buffer[14 - 4] << 8) | buffer[15 - 4]) * 0.1)

      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power, ((buffer[23 - 4] << 16) | (buffer[24 - 4] << 8) | buffer[25 - 4]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power, ((buffer[35 - 4] << 16) | (buffer[36 - 4] << 8) | buffer[37 - 4]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS(power_factor, ((buffer[46 - 4] << 8) | buffer[47 - 4]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(total_energy, ((buffer[54 - 4] << 24) | (buffer[55 - 4] << 16) | (buffer[56 - 4] << 8) | buffer[57 - 4]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(import_active_energy, ((buffer[58 - 4] << 24) | (buffer[59 - 4] << 16) | (buffer[60 - 4] << 8) | buffer[61 - 4]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(export_active_energy, ((buffer[62 - 4] << 24) | (buffer[63 - 4] << 16) | (buffer[64 - 4] << 8) | buffer[65 - 4]) * -0.01)

      UPDATE_SENSOR_MEASUREMENTS(frequency, ((buffer[52] << 8) | buffer[53]) * 0.01)
#endif

#ifdef USE_MODEL_DTS238_7
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_1, ((buffer[5] << 16) | (buffer[6] << 8) | buffer[7]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_2, ((buffer[8] << 16) | (buffer[9] << 8) | buffer[10]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_3, ((buffer[11] << 16) | (buffer[12] << 8) | buffer[13]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_1, ((buffer[14] << 8) | buffer[15]) * 0.1)
      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_2, ((buffer[16] << 8) | buffer[17]) * 0.1)
      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_3, ((buffer[18] << 8) | buffer[19]) * 0.1)

      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_total, ((buffer[20] << 16) | (buffer[21] << 8) | buffer[22]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_1, ((buffer[23] << 16) | (buffer[24] << 8) | buffer[25]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_2, ((buffer[26] << 16) | (buffer[27] << 8) | buffer[28]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_3, ((buffer[29] << 16) | (buffer[30] << 8) | buffer[31]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_total, ((buffer[32] << 16) | (buffer[33] << 8) | buffer[34]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_1, ((buffer[35] << 16) | (buffer[36] << 8) | buffer[37]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_2, ((buffer[38] << 16) | (buffer[39] << 8) | buffer[40]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_3, ((buffer[41] << 16) | (buffer[42] << 8) | buffer[43]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS(power_factor_total, ((buffer[44] << 8) | buffer[45]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_1, ((buffer[46] << 8) | buffer[47]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_2, ((buffer[48] << 8) | buffer[49]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_3, ((buffer[50] << 8) | buffer[51]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(total_energy, ((buffer[54] << 24) | (buffer[55] << 16) | (buffer[56] << 8) | buffer[57]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(import_active_energy, ((buffer[58] << 24) | (buffer[59] << 16) | (buffer[60] << 8) | buffer[61]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(export_active_energy, ((buffer[62] << 24) | (buffer[63] << 16) | (buffer[64] << 8) | buffer[65]) * -0.01)

      UPDATE_SENSOR_MEASUREMENTS(frequency, ((buffer[52] << 8) | buffer[53]) * 0.01)
#endif
      break;
    }
    case ResponseType::HEKR_CMD_RECEIVE_LIMIT_AND_PURCHASE: {
      ESP_LOGV(TAG, "* process_and_update LIMIT_AND_PURCHASE");

      this->data_.max_voltage_limit = (buffer[5 - 4] << 8) | buffer[6 - 4];
      this->data_.min_voltage_limit = (buffer[7 - 4] << 8) | buffer[8 - 4];
      this->data_.max_current_limit = ((buffer[9 - 4] << 8) | buffer[10 - 4]) * 0.01;

      UPDATE_NUMBER(max_voltage_limit, this->data_.max_voltage_limit)
      UPDATE_NUMBER(min_voltage_limit, this->data_.min_voltage_limit)
      UPDATE_NUMBER(max_current_limit, this->data_.max_current_limit)

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if (buffer[1] == 25) {
        this->data_.energy_purchase_state = buffer[23];

        // this->data_.energy_purchase_value = (((buffer[11] << 24) | (buffer[12] << 16) | (buffer[13] << 8) | buffer[14]) * 0.01);
        // this->data_.energy_purchase_alarm = (((buffer[19] << 24) | (buffer[20] << 16) | (buffer[21] << 8) | buffer[22]) * 0.01);
        this->data_.energy_purchase_balance = (((buffer[15 - 4] << 24) | (buffer[16 - 4] << 16) | (buffer[17 - 4] << 8) | buffer[18 - 4]) * 0.01);

        this->data_.warning_purchase_alarm = ((this->data_.energy_purchase_balance <= this->data_.energy_purchase_alarm) && this->data_.energy_purchase_state);
      }

      UPDATE_SENSOR(energy_purchase_balance, this->data_.energy_purchase_balance)
      UPDATE_SENSOR(energy_purchase_price, this->data_.energy_purchase_balance * this->data_.price_kWh)

      UPDATE_SWITCH(energy_purchase_state, this->data_.energy_purchase_state)

      UPDATE_BINARY_SENSOR(warning_purchase_alarm, this->data_.warning_purchase_alarm)

      break;
    }
    case ResponseType::HEKR_CMD_RECEIVE_METER_ID: {
      ESP_LOGV(TAG, "* process_and_update METER_ID");

      char serial_number[20];

      sprintf(serial_number, "%02u%02u%02u %02u%02u%02u", buffer[5 - 4], buffer[6 - 4], buffer[7 - 4], buffer[8 - 4], buffer[9 - 4], buffer[10 - 4]);
      std::string string_serial_number(serial_number);

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      UPDATE_TEXT_SENSOR(meter_id, string_serial_number)

      break;
    }
    default:
      ESP_LOGE(TAG, "* Invalid command (0x%02X) received", command_type);
  }
}
#endif

#ifdef USE_PROTOCOL_TUYA
void Dxs238xwComponent::handle_command_(uint8_t command, uint8_t version, const uint8_t *buffer, size_t len) {
  CommandType command_type = (CommandType) command;

  if (this->expected_response_.has_value() && this->expected_response_ == command_type) {
    this->expected_response_.reset();

    // Liberamos el mensaje retenido y lo borramos ya que si resivio respuesta
    this->command_queue_.erase(command_queue_.begin());

    // this->init_retries_ = 0;
  }

  switch (command_type) {
    case CommandType::HEARTBEAT: {
      ESP_LOGV(TAG, "MCU Heartbeat (0x%02X)", buffer[0]);

      this->protocol_version_ = version;

      if (buffer[0] == 0x00) {
        ESP_LOGI(TAG, "MCU restarted");

        this->init_state_ = TuyaInitState::INIT_HEARTBEAT;
      }

      if (this->init_state_ == TuyaInitState::INIT_HEARTBEAT) {
        this->init_state_ = TuyaInitState::INIT_PRODUCT;

        this->push_command_(SmCommand{.cmd = CommandType::PRODUCT_QUERY});
      }

      break;
    }
    case CommandType::PRODUCT_QUERY: {
      // check it is a valid string made up of printable characters
      bool valid = true;

      for (size_t i = 0; i < len; i++) {
        if (!std::isprint(buffer[i])) {
          valid = false;
          break;
        }
      }

      if (valid) {
        this->product_ = std::string(reinterpret_cast<const char *>(buffer), len);
      } else {
        this->product_ = R"({"p":"INVALID"})";
      }

      if (this->init_state_ == TuyaInitState::INIT_PRODUCT) {
        this->init_state_ = TuyaInitState::INIT_CONF;

        this->push_command_(SmCommand{.cmd = CommandType::CONF_QUERY});
      }

      break;
    }
    case CommandType::CONF_QUERY: {
      if (len >= 2) {
        this->status_pin_reported_ = buffer[0];
        this->reset_pin_reported_ = buffer[1];
      }

      if (this->init_state_ == TuyaInitState::INIT_CONF) {
        // If mcu returned status gpio, then we can omit sending wifi state
        if (this->status_pin_reported_ != -1) {
          this->init_state_ = TuyaInitState::INIT_DATAPOINT;
          this->push_command_(SmCommand{.cmd = CommandType::DATAPOINT_QUERY});

          bool is_pin_equals = this->status_pin_.has_value() && (this->status_pin_.value()->get_pin() == this->status_pin_reported_);

          // Configure status pin toggling (if reported and configured) or WIFI_STATE periodic send
          if (is_pin_equals) {
            ESP_LOGV(TAG, "Configured status pin %i", this->status_pin_reported_);

            this->set_interval("wifi", 1000, [this] { this->set_status_pin_(); });
          } else {
            ESP_LOGW(TAG, "Supplied status_pin does not equals the reported pin %i. TuyaMcu will work in limited mode.", this->status_pin_reported_);
          }
        } else {
          this->init_state_ = TuyaInitState::INIT_WIFI;

          ESP_LOGV(TAG, "Configured WIFI_STATE periodic send");

          this->set_interval("wifi", 1000, [this] { this->send_wifi_status_(); });
        }
      }

      break;
    }
    case CommandType::WIFI_STATE: {
      if (this->init_state_ == TuyaInitState::INIT_WIFI) {
        this->init_state_ = TuyaInitState::INIT_DATAPOINT;

        this->push_command_(SmCommand{.cmd = CommandType::DATAPOINT_QUERY});
      }

      break;
    }
    case CommandType::WIFI_RESET: {
      ESP_LOGE(TAG, "WIFI_RESET is not handled");

      break;
    }
    case CommandType::WIFI_SELECT: {
      ESP_LOGE(TAG, "WIFI_SELECT is not handled");

      break;
    }
    case CommandType::DATAPOINT_DELIVER: {
      break;
    }
    case CommandType::DATAPOINT_REPORT: {
      if (this->init_state_ == TuyaInitState::INIT_DATAPOINT) {
        this->init_state_ = TuyaInitState::INIT_DONE;

        this->set_timeout("datapoint_dump", 1000, [this] { this->show_info_datapoints_(); });
      }

      this->process_and_update_data_(buffer, len);

      break;
    }
    case CommandType::DATAPOINT_QUERY: {
      break;
    }
    case CommandType::WIFI_TEST: {
      this->push_command_(SmCommand{.cmd = CommandType::WIFI_TEST, .payload = std::vector<uint8_t>{0x00, 0x00}});

      break;
    }
    case CommandType::WIFI_RSSI: {
      this->push_command_(SmCommand{.cmd = CommandType::WIFI_RSSI, .payload = std::vector<uint8_t>{get_wifi_rssi_()}});

      break;
    }
    case CommandType::LOCAL_TIME_QUERY: {
#ifdef USE_TIME
      if (this->time_id_.has_value()) {
        this->send_local_time_();

        auto *time_id = *this->time_id_;

        time_id->add_on_time_sync_callback([this] { this->send_local_time_(); });
      } else {
        ESP_LOGW(TAG, "LOCAL_TIME_QUERY is not handled because time is not configured");
      }
#else
      ESP_LOGE(TAG, "LOCAL_TIME_QUERY is not handled");
#endif
      break;
    }
    case CommandType::GET_NETWORK_STATUS: {
      uint8_t wifi_status = this->get_wifi_status_code_();

      this->push_command_(SmCommand{.cmd = CommandType::GET_NETWORK_STATUS, .payload = std::vector<uint8_t>{wifi_status}});

      ESP_LOGV(TAG, "Network status requested, reported as %i", wifi_status);

      break;
    }
    default:
      ESP_LOGE(TAG, "Invalid command (0x%02X) received", command);
  }
}

void Dxs238xwComponent::process_and_update_data_(const uint8_t *buffer, size_t len) {
  while (len >= 4) {
    TuyaDatapoint datapoint{};
    datapoint.id = (DatapointId) buffer[0];
    datapoint.type = (TuyaDatapointType) buffer[1];
    datapoint.value_uint = 0;

    size_t data_size = (buffer[2] << 8) + buffer[3];
    const uint8_t *data = buffer + 4;
    size_t data_len = len - 4;

    if (data_size > data_len) {
      ESP_LOGW(TAG, "Datapoint %u is truncated and cannot be parsed (%zu > %zu)", datapoint.id, data_size, data_len);
      return;
    }

    datapoint.len = data_size;

    switch (datapoint.type) {
      case TuyaDatapointType::RAW:
        datapoint.value_raw = std::vector<uint8_t>(data, data + data_size);
        ESP_LOGD(TAG, "Datapoint %u update to %s", datapoint.id, format_hex_pretty(datapoint.value_raw).c_str());

        break;
      case TuyaDatapointType::BOOLEAN:
        if (data_size != 1) {
          ESP_LOGW(TAG, "Datapoint %u has bad boolean len %zu", datapoint.id, data_size);
          return;
        }

        datapoint.value_bool = data[0];
        ESP_LOGD(TAG, "Datapoint %u update to %s", datapoint.id, ONOFF(datapoint.value_bool));

        break;
      case TuyaDatapointType::INTEGER:
        if (data_size != 4) {
          ESP_LOGW(TAG, "Datapoint %u has bad integer len %zu", datapoint.id, data_size);
          return;
        }

        datapoint.value_uint = encode_uint32(data[0], data[1], data[2], data[3]);

        ESP_LOGD(TAG, "Datapoint %u update to %d", datapoint.id, datapoint.value_int);

        break;
      case TuyaDatapointType::STRING:
        datapoint.value_string = std::string(reinterpret_cast<const char *>(data), data_size);
        ESP_LOGD(TAG, "Datapoint %u update to %s", datapoint.id, datapoint.value_string.c_str());

        break;
      case TuyaDatapointType::ENUM:
        if (data_size != 1) {
          ESP_LOGW(TAG, "Datapoint %u has bad enum len %zu", datapoint.id, data_size);
          return;
        }
        datapoint.value_enum = data[0];

        ESP_LOGD(TAG, "Datapoint %u update to %d", datapoint.id, datapoint.value_enum);

        break;
      case TuyaDatapointType::BITMASK:
        switch (data_size) {
          case 1:
            datapoint.value_bitmask = encode_uint32(0, 0, 0, data[0]);
            break;
          case 2:
            datapoint.value_bitmask = encode_uint32(0, 0, data[0], data[1]);
            break;
          case 4:
            datapoint.value_bitmask = encode_uint32(data[0], data[1], data[2], data[3]);
            break;
          default:
            ESP_LOGW(TAG, "Datapoint %u has bad bitmask len %zu", datapoint.id, data_size);
            return;
        }

        ESP_LOGD(TAG, "Datapoint %u update to %#08X", datapoint.id, datapoint.value_bitmask);

        break;
      default:
        ESP_LOGW(TAG, "Datapoint %u has unknown type %#02hhX", datapoint.id, static_cast<uint8_t>(datapoint.type));

        return;
    }

    len -= data_size + 4;
    buffer = data + data_size;

    // Update internal datapoints
    bool found = false;

    for (auto &other : this->datapoints_) {
      if (other.id == datapoint.id) {
        other = datapoint;
        found = true;
      }
    }

    if (!found) {
      this->datapoints_.push_back(datapoint);
    }

    DatapointId datapoint_id = (DatapointId) datapoint.id;
    switch (datapoint_id) {
      UPDATE_SENSOR_CASE(FREQUENCY, frequency, datapoint.value_uint)

#ifdef USE_MODEL_DDS238_2
      UPDATE_SENSOR_CASE(CURRENT, current, datapoint.value_uint)
      UPDATE_SENSOR_CASE(VOLTAGE, voltage, datapoint.value_uint)

      UPDATE_SENSOR_CASE(REACTIVE_POWER, reactive_power, datapoint.value_uint)
      UPDATE_SENSOR_CASE(ACTIVE_POWER, active_power, datapoint.value_uint)

      UPDATE_SENSOR_CASE(POWER_FACTOR, power_factor, datapoint.value_uint)

      UPDATE_SENSOR_CASE(IMPORT_ACTIVE_ENERGY, import_active_energy, datapoint.value_uint)
      UPDATE_SENSOR_CASE(EXPORT_ACTIVE_ENERGY, export_active_energy, datapoint.value_uint)

      UPDATE_SENSOR_CASE(PHASE_COUNT, phase_count, datapoint.value_uint)

      UPDATE_SENSOR_CASE(ENERGY_PURCHASE_BALANCE, frequency, datapoint.value_uint)
#endif
#ifdef USE_MODEL_DDS238_4
      UPDATE_SENSOR_CASE(CURRENT, current, datapoint.value_uint)
      UPDATE_SENSOR_CASE(VOLTAGE, voltage, datapoint.value_uint)

      UPDATE_SENSOR_CASE(REACTIVE_POWER, reactive_power, datapoint.value_uint)
      UPDATE_SENSOR_CASE(ACTIVE_POWER, active_power, datapoint.value_uint)

      UPDATE_SENSOR_CASE(POWER_FACTOR, power_factor, datapoint.value_uint)

      UPDATE_SENSOR_CASE(IMPORT_ACTIVE_ENERGY, import_active_energy, datapoint.value_uint)
      UPDATE_SENSOR_CASE(EXPORT_ACTIVE_ENERGY, export_active_energy, datapoint.value_uint)

      UPDATE_SENSOR_CASE(PHASE_COUNT, phase_count, datapoint.value_uint)

      UPDATE_SENSOR_CASE(ENERGY_PURCHASE_BALANCE, frequency, datapoint.value_uint)
#endif
#ifdef USE_MODEL_DTS238_7
      UPDATE_SENSOR_CASE(CURRENT_PHASE_1, current_phase_1, datapoint.value_uint)
      UPDATE_SENSOR_CASE(CURRENT_PHASE_2, current_phase_2, datapoint.value_uint)
      UPDATE_SENSOR_CASE(CURRENT_PHASE_3, current_phase_3, datapoint.value_uint)

      UPDATE_SENSOR_CASE(VOLTAGE_PHASE_1, voltage_phase_1, datapoint.value_uint)
      UPDATE_SENSOR_CASE(VOLTAGE_PHASE_2, voltage_phase_2, datapoint.value_uint)
      UPDATE_SENSOR_CASE(VOLTAGE_PHASE_3, voltage_phase_3, datapoint.value_uint)

      UPDATE_SENSOR_CASE(REACTIVE_POWER_PHASE_1, reactive_power_phase_1, datapoint.value_uint)
      UPDATE_SENSOR_CASE(REACTIVE_POWER_PHASE_2, reactive_power_phase_2, datapoint.value_uint)
      UPDATE_SENSOR_CASE(REACTIVE_POWER_PHASE_3, reactive_power_phase_3, datapoint.value_uint)
      UPDATE_SENSOR_CASE(REACTIVE_POWER_TOTAL, reactive_power_total, datapoint.value_uint)

      UPDATE_SENSOR_CASE(ACTIVE_POWER_PHASE_1, active_power_phase_1, datapoint.value_uint)
      UPDATE_SENSOR_CASE(ACTIVE_POWER_PHASE_2, active_power_phase_2, datapoint.value_uint)
      UPDATE_SENSOR_CASE(ACTIVE_POWER_PHASE_3, active_power_phase_3, datapoint.value_uint)
      UPDATE_SENSOR_CASE(ACTIVE_POWER_TOTAL, active_power_total, datapoint.value_uint)

      UPDATE_SENSOR_CASE(POWER_FACTOR_PHASE_1, power_factor_phase_1, datapoint.value_uint)
      UPDATE_SENSOR_CASE(POWER_FACTOR_PHASE_2, power_factor_phase_2, datapoint.value_uint)
      UPDATE_SENSOR_CASE(POWER_FACTOR_PHASE_3, power_factor_phase_3, datapoint.value_uint)
      UPDATE_SENSOR_CASE(POWER_FACTOR_TOTAL, power_factor_total, datapoint.value_uint)

      UPDATE_SENSOR_CASE(IMPORT_ACTIVE_ENERGY, import_active_energy, datapoint.value_uint)
      UPDATE_SENSOR_CASE(EXPORT_ACTIVE_ENERGY, export_active_energy, datapoint.value_uint)

      UPDATE_SENSOR_CASE(PHASE_COUNT, phase_count, datapoint.value_uint)

      UPDATE_SENSOR_CASE(ENERGY_PURCHASE_BALANCE, frequency, datapoint.value_uint)
#endif

      default:
        return;
    }
  }
}

//----------------------

void Dxs238xwComponent::set_boolean_datapoint_value_(DatapointId datapoint_id, bool value, bool forced) {
  //
  this->numeric_datapoint_value_(datapoint_id, TuyaDatapointType::BOOLEAN, value, 1, forced);
}

void Dxs238xwComponent::set_integer_datapoint_value_(DatapointId datapoint_id, uint32_t value, bool forced) {
  //
  this->numeric_datapoint_value_(datapoint_id, TuyaDatapointType::INTEGER, value, 4, forced);
}

void Dxs238xwComponent::set_enum_datapoint_value_(DatapointId datapoint_id, uint8_t value, bool forced) {
  //
  this->numeric_datapoint_value_(datapoint_id, TuyaDatapointType::ENUM, value, 1, forced);
}

void Dxs238xwComponent::set_bitmask_datapoint_value_(DatapointId datapoint_id, uint32_t value, uint8_t length, bool forced) {
  //
  this->numeric_datapoint_value_(datapoint_id, TuyaDatapointType::BITMASK, value, length, forced);
}

void Dxs238xwComponent::set_raw_datapoint_value_(DatapointId datapoint_id, const std::vector<uint8_t> &value, bool forced) {
  ESP_LOGD(TAG, "Setting datapoint %u to %s", datapoint_id, format_hex_pretty(value).c_str());

  optional<TuyaDatapoint> datapoint = this->get_datapoint_(datapoint_id);

  if (!datapoint.has_value()) {
    ESP_LOGW(TAG, "Setting unknown datapoint %u", datapoint_id);
  } else if (datapoint->type != TuyaDatapointType::RAW) {
    ESP_LOGE(TAG, "Attempt to set datapoint %u with incorrect type", datapoint_id);
    return;
  } else if (!forced && datapoint->value_raw == value) {
    ESP_LOGV(TAG, "Not sending unchanged value");
    return;
  }

  this->send_datapoint_command_(datapoint_id, TuyaDatapointType::RAW, value);
}

void Dxs238xwComponent::set_string_datapoint_value_(DatapointId datapoint_id, const std::string &value, bool forced) {
  ESP_LOGD(TAG, "Setting datapoint %u to %s", datapoint_id, value.c_str());

  optional<TuyaDatapoint> datapoint = this->get_datapoint_(datapoint_id);

  if (!datapoint.has_value()) {
    ESP_LOGW(TAG, "Setting unknown datapoint %u", datapoint_id);
  } else if (datapoint->type != TuyaDatapointType::STRING) {
    ESP_LOGE(TAG, "Attempt to set datapoint %u with incorrect type", datapoint_id);
    return;
  } else if (!forced && datapoint->value_string == value) {
    ESP_LOGV(TAG, "Not sending unchanged value");
    return;
  }

  std::vector<uint8_t> data;

  for (char const &c : value) {
    data.push_back(c);
  }

  this->send_datapoint_command_(datapoint_id, TuyaDatapointType::STRING, data);
}

//----------------------

void Dxs238xwComponent::numeric_datapoint_value_(DatapointId datapoint_id, TuyaDatapointType datapoint_type, const uint32_t value, uint8_t length, bool forced) {
  ESP_LOGD(TAG, "Setting datapoint %u to %u", datapoint_id, value);

  optional<TuyaDatapoint> datapoint = this->get_datapoint_(datapoint_id);

  if (!datapoint.has_value()) {
    ESP_LOGW(TAG, "Setting unknown datapoint %u", datapoint_id);
  } else if (datapoint->type != datapoint_type) {
    ESP_LOGE(TAG, "Attempt to set datapoint %u with incorrect type", datapoint_id);

    return;
  } else if (!forced && datapoint->value_uint == value) {
    ESP_LOGV(TAG, "Not sending unchanged value");
    return;
  }

  std::vector<uint8_t> data;

  switch (length) {
    case 4:
      data.push_back(value >> 24);
      data.push_back(value >> 16);
    case 2:
      data.push_back(value >> 8);
    case 1:
      data.push_back(value >> 0);
      break;
    default:
      ESP_LOGE(TAG, "Unexpected datapoint length %u", length);

      return;
  }

  this->send_datapoint_command_(datapoint_id, datapoint_type, data);
}

void Dxs238xwComponent::send_datapoint_command_(DatapointId datapoint_id, TuyaDatapointType datapoint_type, std::vector<uint8_t> data) {
  std::vector<uint8_t> buffer;

  buffer.push_back((uint8_t) datapoint_id);
  buffer.push_back(static_cast<uint8_t>(datapoint_type));
  buffer.push_back(data.size() >> 8);
  buffer.push_back(data.size() >> 0);

  buffer.insert(buffer.end(), data.begin(), data.end());

  this->push_command_(SmCommand{.cmd = CommandType::DATAPOINT_DELIVER, .payload = buffer});
}

optional<TuyaDatapoint> Dxs238xwComponent::get_datapoint_(DatapointId datapoint_id) {
  for (auto &datapoint : this->datapoints_) {
    if (datapoint.id == datapoint_id)
      return datapoint;
  }

  return {};
}

void Dxs238xwComponent::show_info_datapoints_() {
  ESP_LOGCONFIG(TAG, "Datapoints:");

  for (auto &info : this->datapoints_) {
    if (info.type == TuyaDatapointType::RAW) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: raw (value: %s)", info.id, format_hex_pretty(info.value_raw).c_str());
    } else if (info.type == TuyaDatapointType::BOOLEAN) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: switch (value: %s)", info.id, ONOFF(info.value_bool));
    } else if (info.type == TuyaDatapointType::INTEGER) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: int value (value: %d)", info.id, info.value_int);
    } else if (info.type == TuyaDatapointType::STRING) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: string value (value: %s)", info.id, info.value_string.c_str());
    } else if (info.type == TuyaDatapointType::ENUM) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: enum (value: %d)", info.id, info.value_enum);
    } else if (info.type == TuyaDatapointType::BITMASK) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: bitmask (value: %x)", info.id, info.value_bitmask);
    } else {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: unknown", info.id);
    }
  }

  if ((this->status_pin_reported_ != -1) || (this->reset_pin_reported_ != -1)) {
    ESP_LOGCONFIG(TAG, "* GPIO Configuration: status: pin %d, reset: pin %d", this->status_pin_reported_, this->reset_pin_reported_);
  }

  if (this->status_pin_.has_value()) {
    LOG_PIN("  Status Pin: ", this->status_pin_.value());
  }

  ESP_LOGCONFIG(TAG, "* Product: '%s'", this->product_.c_str());
}

void Dxs238xwComponent::set_status_pin_() {
  bool is_network_ready = network::is_connected() && remote_is_connected();

  this->status_pin_.value()->digital_write(is_network_ready);
}
#endif

void Dxs238xwComponent::send_wifi_status_() {
  uint8_t status = this->get_wifi_status_code_();

  if (status == this->wifi_status_) {
    return;
  }

  ESP_LOGD(TAG, "Sending WiFi Status");

  this->wifi_status_ = status;

  // this->push_command_(SmCommand{.cmd = CommandType::WIFI_STATE, .payload = std::vector<uint8_t>{status}});
}

uint8_t Dxs238xwComponent::get_wifi_status_code_() {
  uint8_t status = 0x02;

  if (network::is_connected()) {
    status = 0x03;

    // Protocol version 3 also supports specifying when connected to "the cloud"
    // if (this->protocol_version_ >= 0x03 && remote_is_connected()) {
    //  status = 0x04;
    //}
  } else {
#ifdef USE_CAPTIVE_PORTAL
    if (captive_portal::global_captive_portal != nullptr && captive_portal::global_captive_portal->is_active()) {
      status = 0x01;
    }
#endif
  }

  return status;
}

uint8_t Dxs238xwComponent::get_wifi_rssi_() {
#ifdef USE_WIFI
  if (wifi::global_wifi_component != nullptr) {
    return wifi::global_wifi_component->wifi_rssi();
  }
#endif

  return 0;
}

#ifdef USE_TIME
void Dxs238xwComponent::send_local_time_() {
  std::vector<uint8_t> payload;

  auto *time_id = *this->time_id_;

  time::ESPTime now = time_id->now();

  if (now.is_valid()) {
    uint8_t year = now.year - 2000;
    uint8_t month = now.month;
    uint8_t day_of_month = now.day_of_month;
    uint8_t hour = now.hour;
    uint8_t minute = now.minute;
    uint8_t second = now.second;
    // Tuya days starts from Monday, esphome uses Sunday as day 1
    uint8_t day_of_week = now.day_of_week - 1;

    if (day_of_week == 0) {
      day_of_week = 7;
    }

    ESP_LOGD(TAG, "Sending local time");

    payload = std::vector<uint8_t>{0x01, year, month, day_of_month, hour, minute, second, day_of_week};
  } else {
    // By spec we need to notify MCU that the time was not obtained if this is a response to a query
    ESP_LOGW(TAG, "Sending missing local time");

    payload = std::vector<uint8_t>{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  }

  // this->push_command_(SmCommand{.cmd = CommandType::LOCAL_TIME_QUERY, .payload = payload});
}
#endif

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::process_command_queue_() {
  uint32_t now = millis();

  // Limpiamos el buffer de rx si ya paso mucho tiempo y no llego un mensaje valido
  if ((this->last_rx_char_timestamp_ != 0) && (now - this->last_rx_char_timestamp_) > SM_MAX_MILLIS_TO_RX_BYTE) {
    ESP_LOGW(TAG, "* Se agoto tiempo de espera para un nuevo caracter");

    this->rx_message_.clear();

    this->error_type_ = SmErrorType::COMMUNICATION;
    this->error_code_ = SmErrorCode::NOT_ENOUGHT_BYTES;

    this->print_error_();
  }

  // verificamos si no hemos superado el tiempo de espera para resivir una respuesta
  // si superamos el tiempo podemos seguir procesando mensajes en cola
  if (this->is_message_timeout_()) {
    ESP_LOGW(TAG, "* Se agoto el tiempo de espera para confirmacion o respuesta");

    /*
        if (init_state_ != TuyaInitState::INIT_DONE) {
          if (++this->init_retries_ >= MAX_RETRIES) {
            this->init_failed_ = true;
            ESP_LOGE(TAG, "Initialization failed at init_state %u", static_cast<uint8_t>(this->init_state_));
            this->command_queue_.erase(command_queue_.begin());
            this->init_retries_ = 0;
          }
        } else {
          this->command_queue_.erase(command_queue_.begin());
        }
        */

    if (this->get_component_state() == COMPONENT_STATE_SETUP) {
      ESP_LOGD(TAG, "* No llego respuesta del MCU");

      this->status_set_error();

      this->count_error_data_acquisition_++;
      this->postpone_setup_time_ = 0;
    } else {
      // Borramos el mensaje retenido esperando respuesta ya que nunca resivio respuesta
      this->command_queue_.erase(this->command_queue_.begin());
    }

    this->error_type_ = SmErrorType::COMMUNICATION;
    this->error_code_ = SmErrorCode::TIMEOUT;

    this->print_error_();
  }

  if (!this->command_queue_.empty() && this->rx_message_.empty() && !this->is_expected_message()) {
    this->send_raw_command_(this->command_queue_.front());

    // El comando se queda retenido si es necesario esperar una respuesta, si no se elimina del listado
    if (!this->is_expected_message()) {
      this->command_queue_.erase(this->command_queue_.begin());
    }
  }
}

void Dxs238xwComponent::send_raw_command_(SmCommand command) {
  this->error_type_ = SmErrorType::NO_ERROR;
  this->error_code_ = SmErrorCode::NO_ERROR;

  ESP_LOGV(TAG, "* Procesando comando en cola");

  this->last_command_timestamp_ = millis();

  std::vector<uint8_t> buffer;

#ifdef USE_PROTOCOL_HEKR
  switch (command.cmd) {
    case CommandType::SEND:
      this->expected_confirmation_ = command.cmd;
      break;
    default:
      break;
  }

  buffer.push_back(HEKR_HEADER);
  buffer.push_back(4 + command.payload.size() + 1);
  buffer.push_back((uint8_t) command.cmd);
  buffer.push_back(this->version_msg++);
#endif

#ifdef USE_PROTOCOL_TUYA
  switch (command.cmd) {
    case CommandType::HEARTBEAT:
      this->expected_response_ = CommandType::HEARTBEAT;
      break;
    case CommandType::PRODUCT_QUERY:
      this->expected_response_ = CommandType::PRODUCT_QUERY;
      break;
    case CommandType::CONF_QUERY:
      this->expected_response_ = CommandType::CONF_QUERY;
      break;
    case CommandType::DATAPOINT_DELIVER:
    case CommandType::DATAPOINT_QUERY:
      this->expected_response_ = CommandType::DATAPOINT_REPORT;
      break;
    default:
      break;
  }

  buffer.push_back(TUYA_HEADER_1);
  buffer.push_back(TUYA_HEADER_2);
  buffer.push_back(0x00);
  buffer.push_back((uint8_t) command.cmd);
  buffer.push_back((uint8_t) (command.payload.size() >> 8));
  buffer.push_back((uint8_t) (command.payload.size() & 0xFF));
#endif

  if (!command.payload.empty()) {
    buffer.insert(buffer.end(), command.payload.begin(), command.payload.end());
  }

  buffer.push_back(this->calculate_crc_(buffer.data(), buffer.size()));

  if (command.raw) {
    ESP_LOGD(TAG, "* Message send: %s", format_hex_pretty(buffer).c_str());
  } else {
    ESP_LOGV(TAG, "* Message send: %s", format_hex_pretty(buffer).c_str());
  }

  this->write_array(buffer);

#ifdef USE_PROTOCOL_HEKR

  if (command.raw) {
    ESP_LOGD(TAG, "* Waiting to confirmation:");
  } else {
    ESP_LOGV(TAG, "* Waiting to confirmation:");
  }

#endif

#ifdef USE_PROTOCOL_TUYA

  if (!this->expected_response_.has_value()) {
    if (command.raw) {
      ESP_LOGD(TAG, "* Waiting to response:");
    } else {
      ESP_LOGV(TAG, "* Waiting to response:");
    }
  }

#endif
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::push_command_(const SmCommand &command, bool push_back) {
  uint8_t index = 0;

  // Buscamos si es que ya existe el mensaje
  for (auto &cmd : this->command_queue_) {
    if (cmd.cmd == command.cmd && cmd.payload.front() == command.payload.front()) {
      bool replaced = false;

      // comprobamos si reemplazamos el comando existente o descartamos el comando que llego
      if (index == 0) {
        if (this->is_expected_message()) {
          // Si el mensaje existente no tiene payload descartamos el mensaje nuevo
          if (!command.payload.empty()) {
            // Si tiene payload comparamos si es distinto el dato guardado al nuevo comando
            if (cmd.payload != command.payload) {
              replaced = true;

              cmd.null_old_response = true;
            }
          }
        } else {
          replaced = true;
        }
      } else {
        replaced = true;
      }

      if (replaced) {
        cmd = command;

        if (command.raw) {
          ESP_LOGD(TAG, "* Command exist in queue, is replaced with new command");

          if (cmd.null_old_response) {
            ESP_LOGD(TAG, "* Se anula la respuesta en espera");
          }

        } else {
          ESP_LOGV(TAG, "* Command exist in queue, is replaced with new command");

          if (cmd.null_old_response) {
            ESP_LOGV(TAG, "* Se anula la respuesta en espera");
          }
        }
      } else {
        if (command.raw) {
          ESP_LOGD(TAG, "* Command exist in queue, se descarta nuevo mensaje");
        } else {
          ESP_LOGV(TAG, "* Command exist in queue, se descarta nuevo mensaje");
        }
      }

      return;
    }

    index++;
  }

  if (push_back) {
    command_queue_.push_back(command);
  } else {
    if (this->is_expected_message()) {
      this->command_queue_.insert(this->command_queue_.begin() + 1, command);
    } else {
      this->command_queue_.insert(this->command_queue_.begin(), command);
    }
  }
}

void Dxs238xwComponent::send_command_(SmCommandType cmd, bool state) {
  switch (cmd) {
    case SmCommandType::GET_METER_STATE: {
      ESP_LOGV(TAG, "Push Command - GET_METER_STATE");

#ifdef USE_PROTOCOL_HEKR
      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_METER_STATE}});
#endif

#ifdef USE_PROTOCOL_TUYA
      this->set_boolean_datapoint_value_(DatapointId::TOTAL_ENERGY, false);
#endif

      break;
    }
    case SmCommandType::GET_MEASUREMENT: {
      ESP_LOGV(TAG, "Push Command - GET_MEASUREMENT");

#ifdef USE_PROTOCOL_HEKR
      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_MEASUREMENT}});
#endif
      break;
    }
    case SmCommandType::GET_LIMIT_AND_PURCHASE: {
      ESP_LOGV(TAG, "Push Command - GET_LIMIT_AND_PURCHASE");

#ifdef USE_PROTOCOL_HEKR
      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_LIMIT_AND_PURCHASE}});
#endif
      break;
    }
    case SmCommandType::GET_METER_ID: {
      ESP_LOGD(TAG, "Push Command - GET_METER_ID");

#ifdef USE_PROTOCOL_HEKR
      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_METER_ID}});
#endif
      break;
    }
    case SmCommandType::SET_LIMIT: {
      ESP_LOGD(TAG, "Push Command - SET_LIMIT");

      uint16_t tmp_current_limit = this->data_.max_current_limit * 100;

#ifdef USE_PROTOCOL_HEKR
      std::vector<uint8_t> buffer;

      buffer.push_back((uint8_t) CommandData::SET_LIMIT);

      buffer.push_back(tmp_current_limit >> 8);
      buffer.push_back(tmp_current_limit & SM_GET_ONE_BYTE);

      buffer.push_back(this->data_.max_voltage_limit >> 8);
      buffer.push_back(this->data_.max_voltage_limit & SM_GET_ONE_BYTE);

      buffer.push_back(this->data_.min_voltage_limit >> 8);
      buffer.push_back(this->data_.min_voltage_limit & SM_GET_ONE_BYTE);

      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = buffer}, false);
#endif
      ESP_LOGD(TAG, "* Input Data: max_current_limit = %u, max_voltage_limit = %u, min_voltage_limit = %u", this->data_.max_current_limit, this->data_.max_voltage_limit, this->data_.min_voltage_limit);

      break;
    }
    case SmCommandType::SET_PURCHASE: {
      ESP_LOGD(TAG, "Push Command - SET_PURCHASE");

      uint32_t purchase_value = 0;
      uint32_t purchase_alarm = 0;

      if (state) {
        purchase_value = this->data_.energy_purchase_value * 100;
        purchase_alarm = this->data_.energy_purchase_alarm * 100;
      }

#ifdef USE_PROTOCOL_HEKR
      std::vector<uint8_t> buffer;

      buffer.push_back((uint8_t) CommandData::SET_PURCHASE);

      buffer.push_back(purchase_value >> 24);
      buffer.push_back(purchase_value >> 16);
      buffer.push_back(purchase_value >> 8);
      buffer.push_back(purchase_value & SM_GET_ONE_BYTE);

      buffer.push_back(purchase_alarm >> 24);
      buffer.push_back(purchase_alarm >> 16);
      buffer.push_back(purchase_alarm >> 8);
      buffer.push_back(purchase_alarm & SM_GET_ONE_BYTE);

      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = buffer}, false);
#endif
      ESP_LOGD(TAG, "* Input Data: purchase_value = %u, purchase_alarm = %u, state = %s", (state ? this->data_.energy_purchase_value : 0), (state ? this->data_.energy_purchase_alarm : 0), ONOFF(state));

      break;
    }
    case SmCommandType::SET_METER_STATE: {
      ESP_LOGD(TAG, "Push Command - SET_METER_STATE");

#ifdef USE_PROTOCOL_HEKR
      std::vector<uint8_t> buffer;

      buffer.push_back((uint8_t) CommandData::SET_METER_STATE);
      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = buffer}, false);
#endif
      ESP_LOGD(TAG, "* Input Data: state = %s", ONOFF(state));

      break;
    }
    case SmCommandType::SET_DELAY: {
      ESP_LOGD(TAG, "Push Command - SET_DELAY");

      uint16_t delay_value_set = 0;

      if (state) {
        delay_value_set = this->data_.delay_value_set;
      }

#ifdef USE_PROTOCOL_HEKR
      std::vector<uint8_t> buffer;

      buffer.push_back((uint8_t) CommandData::SET_DELAY);

      buffer.push_back(delay_value_set >> 8);
      buffer.push_back(delay_value_set & SM_GET_ONE_BYTE);

      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = buffer}, false);
#endif
      ESP_LOGD(TAG, "* Input Data: delay_value_set = %u, state = %s", delay_value_set, ONOFF(state));

      break;
    }
    case SmCommandType::SET_RESET: {
      ESP_LOGD(TAG, "Push Command - SET_RESET");

#ifdef USE_PROTOCOL_HEKR
      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::SET_RESET}}, false);
#endif
      break;
    }
    default:
      break;
  }
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::meter_state_toggle() {
  //
  this->set_meter_state_(!this->data_.meter_state);
}

void Dxs238xwComponent::meter_state_on() {
  //
  this->set_meter_state_(true);
}

void Dxs238xwComponent::meter_state_off() {
  //
  this->set_meter_state_(false);
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
    std::vector<uint8_t> message_without_dots;

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
          message_without_dots.push_back(message[i]);

          character_hex_index++;
        }
      }
    }

    if (this->error_code_ == SmErrorCode::NO_ERROR) {
      uint8_t size_message_without_dots = message_without_dots.size();

      if ((size_message_without_dots % 2) != 0) {
        this->error_type_ = SmErrorType::INPUT_DATA;
        this->error_code_ = SmErrorCode::WRONG_MSG;
      }

      const uint8_t *ptr_message_without_dots = &message_without_dots[0];

      if (this->error_code_ == SmErrorCode::NO_ERROR) {
        std::string str_message_without_dots = std::string(reinterpret_cast<const char *>(ptr_message_without_dots), size_message_without_dots);

        std::vector<uint8_t> message_hex;
        uint8_t message_hex_size = size_message_without_dots / 2;

        if (!parse_hex(str_message_without_dots, message_hex, message_hex_size) || message_hex_size < SM_MIN_COMMAND_LENGHT) {
          this->error_type_ = SmErrorType::INPUT_DATA;
          this->error_code_ = SmErrorCode::WRONG_MSG;
        }

        if (this->error_code_ == SmErrorCode::NO_ERROR) {
          if (check_crc) {
            if (this->calculate_crc_(message_hex.data(), message_hex_size - 1) != message_hex[message_hex_size - 1]) {
              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::CRC;
            }
          }

          if (this->error_code_ == SmErrorCode::NO_ERROR) {
#ifdef USE_PROTOCOL_HEKR
            CommandType command = (CommandType) message_hex[2];

            if (message_hex[0] != HEKR_HEADER) {
              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::WRONG_BYTES_HEADER;
            } else if (message_hex[1] != message_hex_size) {
              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::WRONG_BYTES_LENGTH;
            } else if (command != CommandType::SEND && command != CommandType::MODULE_OPERATION && command != CommandType::ERROR_FRAME) {
              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::WRONG_BYTES_TYPE_MESSAGE;
            } else {
              message_hex.erase(message_hex.begin(), message_hex.begin() + 4);
              message_hex.pop_back();

              ESP_LOGD(TAG, "* message data size = %u bytes", message_hex.size());

              this->push_command_(SmCommand{.cmd = command, .payload = message_hex, .raw = true}, false);
            }
#endif

#ifdef USE_PROTOCOL_TUYA
            uint16_t length = (uint16_t(message_hex[4]) << 8) | (uint16_t(message_hex[5]));

            if (message_hex[0] != TUYA_HEADER_1 || message_hex[1] != TUYA_HEADER_2) {
              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::WRONG_BYTES_HEADER;
            } else if (message_hex[2] != 0x00) {
              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::WRONG_BYTES_VERSION_PROTOCOL;
            } else if (length != message_hex_size) {
              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::WRONG_BYTES_LENGTH;
            } else {
              CommandType command = (CommandType) message_hex[3];

              if (message_hex_size == SM_MIN_COMMAND_LENGHT) {
                ESP_LOGD(TAG, "* message data size = 0 bytes");

                this->push_command_(SmCommand{.cmd = command, .raw = true}, false);
              } else {
                message_hex.erase(message_hex.begin(), message_hex.begin() + 6);
                message_hex.pop_back();

                ESP_LOGD(TAG, "* message data size = %u bytes", message_hex.size());

                this->push_command_(SmCommand{.cmd = command, .payload = message_hex, .raw = true}, false);
              }
            }
#endif
            return;
          }
        }
      }
    }
  }

  this->print_error_();
}

void Dxs238xwComponent::process_switch_state(SmIdEntity entity, bool state) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    switch (entity) {
      case SmIdEntity::SWITCH_ENERGY_PURCHASE_STATE: {
        this->send_command_(SmCommandType::SET_PURCHASE, state);

        UPDATE_SWITCH_FORCE(energy_purchase_state, state)
        break;
      }
      case SmIdEntity::SWITCH_METER_STATE: {
        this->data_.warning_off_by_user = !state;

        this->send_command_(SmCommandType::SET_METER_STATE, state);

        UPDATE_SWITCH_FORCE(meter_state, state)
        break;
      }
      case SmIdEntity::SWITCH_DELAY_STATE: {
        this->send_command_(SmCommandType::SET_DELAY, state);

        UPDATE_SWITCH_FORCE(delay_state, state)
        break;
      }
      default: {
        ESP_LOGE(TAG, "ID %hhu is not a SWITCH or is not included in the case list", entity);
        return;
      }
    }
  }
}

void Dxs238xwComponent::process_button_action(SmIdEntity entity) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    switch (entity) {
      case SmIdEntity::BUTTON_RESET_DATA: {
        this->send_command_(SmCommandType::SET_PURCHASE, false);
        this->send_command_(SmCommandType::SET_RESET);

        break;
      }
      default: {
        ESP_LOGE(TAG, "ID %hhu is not a BUTTON or is not included in the case list", entity);

        break;
      }
    }
  }
}

void Dxs238xwComponent::process_number_value(SmIdEntity entity, float value) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    switch (entity) {
      case SmIdEntity::NUMBER_MAX_CURRENT_LIMIT: {
        uint32_t tmp_value = std::round(value);

        this->data_.max_current_limit = tmp_value;

        this->send_command_(SmCommandType::SET_LIMIT);

        UPDATE_NUMBER_FORCE(max_current_limit, this->data_.max_current_limit)
        break;
      }
      case SmIdEntity::NUMBER_MAX_VOLTAGE_LIMIT: {
        uint32_t tmp_value = std::round(value);

        if (tmp_value > this->data_.min_voltage_limit) {
          this->data_.max_voltage_limit = tmp_value;

          this->send_command_(SmCommandType::SET_LIMIT);
        } else {
          ESP_LOGW(TAG, "max_voltage_limit - Value %u must not be less than min_voltage_limit %u", tmp_value, this->data_.min_voltage_limit);
        }

        UPDATE_NUMBER_FORCE(max_voltage_limit, this->data_.max_voltage_limit)
        break;
      }
      case SmIdEntity::NUMBER_MIN_VOLTAGE_LIMIT: {
        uint32_t tmp_value = std::round(value);

        if (tmp_value < this->data_.max_voltage_limit) {
          this->data_.min_voltage_limit = tmp_value;

          this->send_command_(SmCommandType::SET_LIMIT);
        } else {
          ESP_LOGW(TAG, "min_voltage_limit - Value %u must not be greater than max_voltage_limit %u", tmp_value, this->data_.max_voltage_limit);
        }

        UPDATE_NUMBER_FORCE(min_voltage_limit, this->data_.min_voltage_limit)
        break;
      }
      case SmIdEntity::NUMBER_ENERGY_PURCHASE_VALUE: {
        uint32_t tmp_value = std::round(value);

        this->data_.energy_purchase_value = tmp_value;

        this->save_initial_number_value_(this->preference_energy_purchase_value_, this->data_.energy_purchase_value);

        // if (this->data_.energy_purchase_state) {
        //  if (this->send_command_(SmCommandType::SET_PURCHASE, false)) {
        // this->send_command_(SmCommandType::SET_PURCHASE, true);
        // }
        //}

        UPDATE_NUMBER_FORCE(energy_purchase_value, this->data_.energy_purchase_value)
        break;
      }
      case SmIdEntity::NUMBER_ENERGY_PURCHASE_ALARM: {
        uint32_t tmp_value = std::round(value);

        this->data_.energy_purchase_alarm = tmp_value;

        this->save_initial_number_value_(this->preference_energy_purchase_alarm_, this->data_.energy_purchase_alarm);

        // if (this->data_.energy_purchase_state) {
        //  if (this->send_command_(SmCommandType::SET_PURCHASE, false)) {
        // this->send_command_(SmCommandType::SET_PURCHASE, true);
        // }
        //}

        UPDATE_NUMBER_FORCE(energy_purchase_alarm, this->data_.energy_purchase_alarm)
        break;
      }
      case SmIdEntity::NUMBER_DELAY_VALUE_SET: {
        uint32_t tmp_value = std::round(value);

        this->data_.delay_value_set = tmp_value;

        this->save_initial_number_value_(this->preference_delay_value_set_, this->data_.delay_value_set);

        if (this->data_.delay_state) {
          this->send_command_(SmCommandType::SET_DELAY, true);
        }

        UPDATE_NUMBER_FORCE(delay_value_set, this->data_.delay_value_set)
        break;
      }
      case SmIdEntity::NUMBER_STARTING_KWH: {
        uint32_t starting_kWh = std::round(value * 10);

        this->data_.starting_kWh = starting_kWh * 0.1;

        this->save_initial_number_value_(this->preference_starting_kWh_, starting_kWh);

        UPDATE_SENSOR(contract_total_energy, this->data_.starting_kWh + this->data_.total_energy)
        UPDATE_NUMBER_FORCE(starting_kWh, this->data_.starting_kWh)
        break;
      }
      case SmIdEntity::NUMBER_PRICE_KWH: {
        uint32_t price_kWh = std::round(value * 10);

        this->data_.price_kWh = price_kWh * 0.1;

        this->save_initial_number_value_(this->preference_price_kWh_, price_kWh);

        UPDATE_SENSOR(total_energy_price, this->data_.price_kWh * this->data_.total_energy)

        UPDATE_NUMBER_FORCE(price_kWh, this->data_.price_kWh)
        UPDATE_SENSOR(price_kWh, this->data_.price_kWh)
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

bool Dxs238xwComponent::is_expected_message() {
#ifdef USE_PROTOCOL_HEKR
  if (this->expected_confirmation_.has_value() || this->expected_response_.has_value()) {
    return true;
  }
#endif

#ifdef USE_PROTOCOL_TUYA
  if (this->expected_response_.has_value()) {
    return true;
  }
#endif

  return false;
}

bool Dxs238xwComponent::is_message_timeout_() {
  uint32_t delay = millis() - this->last_command_timestamp_;

#ifdef USE_PROTOCOL_HEKR
  if ((this->expected_confirmation_.has_value() && (delay > SM_MAX_MILLIS_TO_CONFIRM)) || (this->expected_response_.has_value() && (delay > SM_MAX_MILLIS_TO_RESPONSE))) {
    this->expected_confirmation_.reset();
    this->expected_response_.reset();

    return true;
  }
#endif

#ifdef USE_PROTOCOL_TUYA
  if (this->expected_response_.has_value() && (delay > SM_MAX_MILLIS_TO_RESPONSE)) {
    this->expected_response_.reset();

    return true;
  }
#endif

  return false;
}

void Dxs238xwComponent::set_meter_state_(bool state) {
  this->data_.warning_off_by_user = !state;

  this->send_command_(SmCommandType::SET_METER_STATE, state);
}

void Dxs238xwComponent::update_meter_state_detail_() {
  SmErrorMeterStateType tmp_meter_state_detail = SmErrorMeterStateType::UNKNOWN;

  if (this->data_.meter_state) {
    tmp_meter_state_detail = SmErrorMeterStateType::POWER_OK;
  } else {
    if (this->data_.warning_off_by_over_current) {
      tmp_meter_state_detail = SmErrorMeterStateType::OVER_CURRENT;
    } else if (this->data_.warning_off_by_over_voltage) {
      tmp_meter_state_detail = SmErrorMeterStateType::OVER_VOLTAGE;
    } else if (this->data_.warning_off_by_under_voltage) {
      tmp_meter_state_detail = SmErrorMeterStateType::UNDER_VOLTAGE;
    } else if (this->data_.warning_off_by_end_delay) {
      tmp_meter_state_detail = SmErrorMeterStateType::END_DELAY;
    } else if (this->data_.warning_off_by_end_purchase) {
      tmp_meter_state_detail = SmErrorMeterStateType::END_PURCHASE;
    } else if (this->data_.warning_off_by_user) {
      tmp_meter_state_detail = SmErrorMeterStateType::END_BY_USER;
    }
  }

  if (tmp_meter_state_detail != this->data_.meter_state_detail) {
    this->data_.meter_state_detail = tmp_meter_state_detail;

    std::string meter_state_detail_string = this->get_meter_state_string_(tmp_meter_state_detail);
    UPDATE_TEXT_SENSOR(meter_state_detail, meter_state_detail_string)
  }
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

  for (uint8_t n = 0; n < size; n++) {
    tmp_crc = tmp_crc + array[n];
  }

  return tmp_crc & SM_GET_ONE_BYTE;
}

void Dxs238xwComponent::print_error_() {
  std::string string_type;
  std::string string_code;

  switch (this->error_type_) {
    case SmErrorType::NO_ERROR:
      return;
    case SmErrorType::COMMUNICATION:
      string_type = SM_STR_TYPE_COMMUNICATION;
      break;
    case SmErrorType::COMMUNICATION_CONFIRMATION:
      string_type = SM_STR_TYPE_COMMUNICATION_CONFIRMATION;
      break;
    case SmErrorType::COMMUNICATION_RESPONSE:
      string_type = SM_STR_TYPE_COMMUNICATION_RESPONSE;
      break;
    case SmErrorType::INPUT_DATA:
      string_type = SM_STR_TYPE_INPUT_DATA;
      break;
  }

  switch (this->error_code_) {
    case SmErrorCode::NO_ERROR:
      return;
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
    case SmErrorCode::WRONG_BYTES_VERSION_PROTOCOL:
      string_code = SM_STR_CODE_WRONG_BYTES_VERSION_PROTOCOL;
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

uint32_t Dxs238xwComponent::read_initial_number_value_(ESPPreferenceObject &preference, const std::string &preference_string, uint32_t default_value) {
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

}  // namespace dxs238xw
}  // namespace esphome
