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

#define UPDATE_SENSOR_FORCE(name, value) \
  if (this->name##_sensor_ != nullptr) { \
    if (this->get_component_state() == COMPONENT_STATE_LOOP) { \
      this->name##_sensor_->publish_state(value); \
    } \
  }

#else
#define UPDATE_SENSOR(name, value)
#define UPDATE_SENSOR_FORCE(name, value)
#endif

#ifdef USE_SENSOR
#define UPDATE_SENSOR_MEASUREMENTS_(name, value, factor) \
  if (this->name##_sensor_ != nullptr) { \
    float value_float = value; \
\
    if (value_float >= factor) { \
      value_float = factor - value_float; \
    } \
\
    if (this->name##_sensor_->get_raw_state() != value_float || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_sensor_->publish_state(value_float); \
    } \
  }

#define UPDATE_SENSOR_MEASUREMENTS(name, value) \
  if (this->name##_sensor_ != nullptr) { \
    float value_float = value; \
\
    if (this->name##_sensor_->get_raw_state() != value_float || this->get_component_state() == COMPONENT_STATE_SETUP) { \
      this->name##_sensor_->publish_state(value_float); \
    } \
  }

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
    if (this->get_component_state() == COMPONENT_STATE_LOOP) { \
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
    if (this->get_component_state() == COMPONENT_STATE_LOOP) { \
      this->name##_switch_->publish_state(value); \
    } \
  }
#else
#define UPDATE_SWITCH(name, value)
#define UPDATE_SWITCH_FORCE(name, value)
#endif

#ifdef USE_NUMBER
#define LOAD_PREFERENCE_(name, preference_string, default_value, factor) \
  if (this->name##_number_ != nullptr) { \
    this->preference_##name##_ = global_preferences->make_preference<uint32_t>(this->hash_##name##_); \
    this->data_.name = this->read_initial_number_value_(this->preference_##name##_, preference_string, default_value) * factor; \
  }

#define LOAD_PREFERENCE_UINT32(name, preference_string, default_value) LOAD_PREFERENCE_(name, preference_string, default_value, 1)
#define LOAD_PREFERENCE_FLOAT(name, preference_string, default_value) LOAD_PREFERENCE_(name, preference_string, default_value, 0.1)
#else
#define LOAD_PREFERENCE_UINT32(name, preference_string, default_value)
#define LOAD_PREFERENCE_FLOAT(name, preference_string, default_value)
#endif
//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

float Dxs238xwComponent::get_setup_priority() const {
  //
  return setup_priority::PROCESSOR;
}

void Dxs238xwComponent::setup() {
  this->incoming_messages_();

  this->process_command_queue_();

  switch (this->init_state_) {
    case SmInitState::INIT_PRE_SETUP:
      if (remote_is_connected()) {
        this->init_state_ = SmInitState::INIT_CONNECTED;

        this->time_init_connected = millis();
      }

      break;
    case SmInitState::INIT_CONNECTED:
      if ((millis() - this->time_init_connected) > SM_POSTPONE_SETUP_TIME) {
        this->init_state_ = SmInitState::INIT_GET_PREFERENCES;
      }

      break;
    case SmInitState::INIT_GET_PREFERENCES:
      ESP_LOGI(TAG, "In --- setup");

      ESP_LOGI(TAG, "* Get Initial Values");
      LOAD_PREFERENCE_UINT32(delay_value_set, SM_STR_DELAY_VALUE_SET, SmLimitValue::MAX_DELAY_SET)
      LOAD_PREFERENCE_FLOAT(starting_kWh, SM_STR_STARTING_KWH, SmLimitValue::MIN_STARTING_KWH)
      LOAD_PREFERENCE_FLOAT(price_kWh, SM_STR_PRICE_KWH, SmLimitValue::MIN_PRICE_KWH)
      LOAD_PREFERENCE_UINT32(energy_purchase_value, SM_STR_ENERGY_PURCHASE_VALUE, SmLimitValue::MIN_ENERGY_PURCHASE_VALUE)
      LOAD_PREFERENCE_UINT32(energy_purchase_alarm, SM_STR_ENERGY_PURCHASE_ALARM, SmLimitValue::MIN_ENERGY_PURCHASE_ALARM)

      UPDATE_NUMBER(delay_value_set, this->data_.delay_value_set)
      UPDATE_NUMBER(starting_kWh, this->data_.starting_kWh)
      UPDATE_NUMBER(price_kWh, this->data_.price_kWh)
      UPDATE_SENSOR(price_kWh, this->data_.price_kWh)
      UPDATE_NUMBER(energy_purchase_value, this->data_.energy_purchase_value)
      UPDATE_NUMBER(energy_purchase_alarm, this->data_.energy_purchase_alarm)

#ifdef USE_PROTOCOL_HEKR
      this->init_state_ = SmInitState::INIT_GET_METER_ID;
#endif
#ifdef USE_PROTOCOL_TUYA
      this->init_state_ = SmInitState::INIT_HEARTBEAT;
#endif
      break;

#ifdef USE_PROTOCOL_HEKR
    case SmInitState::INIT_GET_METER_ID:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to load GET_METER_ID");
        this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_METER_ID}});
      }

      break;
    case SmInitState::INIT_GET_METER_STATE:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to load GET_METER_STATE");
        this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_METER_STATE}});
      }

      break;
    case SmInitState::INIT_GET_MEASUREMENT:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to load GET_MEASUREMENT");
        this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_MEASUREMENT}});
      }

      break;
    case SmInitState::INIT_GET_LIMIT_AND_PURCHASE:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to load GET_LIMIT_AND_PURCHASE");
        this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_LIMIT_AND_ENERGY_PURCHASE}});
      }

      break;
#endif

#ifdef USE_PROTOCOL_TUYA
    case SmInitState::INIT_HEARTBEAT:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to get HEARTBEAT");
        this->push_command_(SmCommand{.cmd = CommandType::HEARTBEAT});
      }

      break;
    case SmInitState::INIT_PRODUCT_QUERY:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to get PRODUCT_QUERY");
        this->push_command_(SmCommand{.cmd = CommandType::PRODUCT_QUERY});
      }

      break;
    case SmInitState::INIT_WORKING_MODE_QUERY:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to get WORKING_MODE_QUERY");
        this->push_command_(SmCommand{.cmd = CommandType::WORKING_MODE_QUERY});
      }

      break;
    case SmInitState::INIT_WIFI_STATE:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to set WIFI_STATE");

        uint8_t wifi_state = (this->protocol_version_ >= 0x03 ? 0x04 : 0x03);

        this->push_command_(SmCommand{.cmd = CommandType::WIFI_STATE, .payload = std::vector<uint8_t>{wifi_state}});
      }

      break;
    case SmInitState::INIT_DATAPOINT_QUERY:
      if (!this->is_expected_message()) {
        ESP_LOGI(TAG, "* Try to get DATAPOINT_QUERY");
        this->push_command_(SmCommand{.cmd = CommandType::DATAPOINT_QUERY});
      }

      break;
#endif

    case SmInitState::INIT_ERROR:
      ESP_LOGI(TAG, "* Are you sure the selected protocol (%s) is correct?", SM_STR_PROTOCOL);

      this->mark_failed();

      return;
    case SmInitState::INIT_DONE:
#ifdef USE_PROTOCOL_HEKR
      this->set_interval("get_data", SM_MIN_INTERVAL_TO_GET_DATA, [this] {
        if (this->get_component_state() == COMPONENT_STATE_LOOP) {
          this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_METER_STATE}});
          this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_LIMIT_AND_ENERGY_PURCHASE}});
        }
      });
#endif

#ifdef USE_PROTOCOL_TUYA
      this->set_interval(heartbeat_interval_str_, 15000, [this] { this->push_command_(SmCommand{.cmd = CommandType::HEARTBEAT}); });

      this->show_info_datapoints_();
#endif

      this->set_interval(log_command_queue_str_, 2000, [this] {
        if (this->command_queue_.size() >= 5) {
          ESP_LOGW(TAG, "Queued commands increased abnormally %u", this->command_queue_.size());
        }
      });

      ESP_LOGI(TAG, "Out --- setup");

      break;
    default:
      break;
  }

  if (this->init_state_ != SmInitState::INIT_DONE) {
    this->component_state_ &= ~COMPONENT_STATE_MASK;
    this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;
  }
}

void Dxs238xwComponent::loop() {
  this->incoming_messages_();

  this->process_command_queue_();
}

void Dxs238xwComponent::update() {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
#ifdef USE_PROTOCOL_HEKR
    this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::GET_MEASUREMENT}});
#endif
#ifdef USE_PROTOCOL_TUYA
    //
#endif
  }
}

void Dxs238xwComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "dxs238xw:");
  ESP_LOGCONFIG(TAG, "  Component Version: %s", SM_STR_COMPONENT_VERSION);
  ESP_LOGCONFIG(TAG, "  Meter Model: %s", SM_STR_METER_MODEL);
  ESP_LOGCONFIG(TAG, "  Protocol: %s", SM_STR_PROTOCOL);
  LOG_UPDATE_INTERVAL(this)
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

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
    if (this->expected_confirmation_.has_value()) {
      if (new_byte == this->version_msg) {
        return true;
      } else {
        ESP_LOGW(TAG, "* WRONG_BYTES: HEKR_VERSION_MESSAGE / Expected = %02X, Receive = %02X", this->version_msg, new_byte);
        this->error_code_ = SmErrorCode::WRONG_BYTES_VERSION_MESSAGE;
        return false;
      }
    } else {
      return true;
    }
  }

  if (at < (length - 1)) {
    return true;
  }

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

  if (at == 1) {
    if (new_byte == TUYA_HEADER_2) {
      return true;
    } else {
      ESP_LOGW(TAG, "* WRONG_BYTES: TUYA_HEADER_2 / Expected = %02X, Receive = %02X", TUYA_HEADER_2, new_byte);
      this->error_code_ = SmErrorCode::WRONG_BYTES_HEADER;
      return false;
    }
  }

  uint8_t version = data[2];
  if (at == 2) {
    return true;
  }

  uint8_t command = data[3];
  if (at == 3) {
    return true;
  }

  if (at <= 5) {
    return true;
  }

  uint16_t length = (uint16_t(data[4]) << 8) | (uint16_t(data[5]));

  if ((at - 6) < length) {
    return true;
  }

#endif

  uint8_t calc_checksum = this->calculate_crc_(this->rx_message_.data(), at);

  if (new_byte != calc_checksum) {
    ESP_LOGW(TAG, "* WRONG_BYTES: CRC / Expected = %02X, Receive = %02X", calc_checksum, new_byte);
    this->error_code_ = SmErrorCode::CRC;
    return false;
  }

  if (!this->is_expected_message() || this->command_queue_.front().raw || this->get_component_state() == COMPONENT_STATE_SETUP) {
    ESP_LOGI(TAG, "* Incoming message: %s", format_hex_pretty(this->rx_message_).c_str());
  }

#ifdef USE_PROTOCOL_HEKR
  const uint8_t *message_data = data + 4;
  this->handle_command_(command, message_data, length - 5);
#endif

#ifdef USE_PROTOCOL_TUYA
  const uint8_t *message_data = data + 6;
  this->handle_command_(command, version, message_data, length);
#endif

  return false;
}

#ifdef USE_PROTOCOL_HEKR
void Dxs238xwComponent::handle_command_(uint8_t command, const uint8_t *buffer, size_t len) {
  CommandType command_type = (CommandType) command;

  if (this->expected_confirmation_.has_value()) {
    if (this->expected_confirmation_ == command_type) {
      if (this->command_queue_.front().raw) {
        ESP_LOGD(TAG, "* Successful confirmation");
        ESP_LOGD(TAG, "* Waiting response");
      } else {
        ESP_LOGV(TAG, "* Successful confirmation");
        ESP_LOGV(TAG, "* Waiting response");
      }

      this->expected_confirmation_.reset();

      this->expected_response_ = CommandType::RECEIVE;

      return;
    } else {
      if (this->command_queue_.front().raw) {
        ESP_LOGD(TAG, "* Confirmation failed");
      } else {
        ESP_LOGV(TAG, "* Confirmation failed");
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

      if (this->command_queue_.front().null_old_response) {
        ESP_LOGI(TAG, "* The response received was cancelled, since a more updated command arrived 0x%02X-0x%02X", command_type, buffer[0]);

        this->command_queue_.front().null_old_response = false;

        return;
      } else {
        this->command_queue_.erase(this->command_queue_.begin());
      }

      this->init_retries_ = 0;
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
      ModuleOperation command_Operation = (ModuleOperation) buffer[0];

      switch (command_Operation) {
        case ModuleOperation::STATUS: {
          this->push_command_(SmCommand{.cmd = CommandType::MODULE_OPERATION, .payload = std::vector<uint8_t>{(uint8_t) ModuleOperation::STATUS, 0x01, 0x01, 0x01, 0x05, 0x00}, .priority_cmd = true});

          break;
        }
        case ModuleOperation::SOFT_REBOOT: {
          ESP_LOGI(TAG, "* Restarting device...");

          delay(100);

          App.safe_reboot();

          break;
        }
        case ModuleOperation::FACTORY_RESET: {
          ESP_LOGE(TAG, "* FACTORY_RESET is not handled");

          break;
        }
        case ModuleOperation::CONFIG: {
          ESP_LOGE(TAG, "* CONFIG is not handled");

          break;
        }
        case ModuleOperation::SET_SLEEP: {
          ESP_LOGE(TAG, "* SET_SLEEP is not handled");

          break;
        }
        case ModuleOperation::WEAKUP: {
          ESP_LOGE(TAG, "* WEAKUP is not handled");

          break;
        }
        case ModuleOperation::FIRMWARE_VERSION: {
          this->push_command_(SmCommand{.cmd = CommandType::MODULE_OPERATION, .payload = std::vector<uint8_t>{(uint8_t) ModuleOperation::FIRMWARE_VERSION, 0x04, 0x01, 0x0F, 0x01}, .priority_cmd = true});

          break;
        }
        case ModuleOperation::PRODKEY_GET: {
          this->push_command_(SmCommand{.cmd = CommandType::MODULE_OPERATION, .payload = std::vector<uint8_t>{(uint8_t) ModuleOperation::PRODKEY_GET, 0x01}, .priority_cmd = true});

          break;
        }
        case ModuleOperation::FACTORY_TEST: {
          ESP_LOGE(TAG, "* FACTORY_TEST is not handled");

          break;
        }
        case ModuleOperation::SET_PRODKEY: {
          std::vector<uint8_t> prod_key;

          prod_key.push_back((uint8_t) ModuleOperation::SET_PRODKEY);

          for (uint8_t i = 1; i <= 16; i++) {
            prod_key.push_back(buffer[i]);
          }

          this->push_command_(SmCommand{.cmd = CommandType::MODULE_OPERATION, .payload = prod_key, .priority_cmd = true});

          break;
        }
        default:
          ESP_LOGE(TAG, "* Invalid command operation (0x%02X) received", command_Operation);
          return;
      }

      break;
    }
    case CommandType::ERROR_FRAME: {
      ErrorValue command_error = (ErrorValue) buffer[0];

      switch (command_error) {
        case ErrorValue::ERROR_OPERATION:
        case ErrorValue::ERROR_SUM_CHECK:
        case ErrorValue::ERROR_DATA_RANGE:
        case ErrorValue::ERROR_CMD_NO_SUPPORTED:
        case ErrorValue::ERROR_DATA_IS_INCONSISTENT:
          ESP_LOGE(TAG, "Meter send error: 0x%02X", command_error);

          break;
        default:
          ESP_LOGE(TAG, "Invalid command error (0x%02X) received", command_error);
          return;
      }

      break;
    }
    default:
      if (len == 0) {
        ESP_LOGE(TAG, "Command (0x%02X) is not handled", command);
      } else {
        ESP_LOGE(TAG, "Command (0x%02X) is not handled - Data: %s", command, format_hex_pretty(buffer, len).c_str());
      }
  }
}

void Dxs238xwComponent::process_and_update_data_(const uint8_t *buffer, size_t len) {
  ResponseType command_type = (ResponseType) buffer[0];

  switch (command_type) {
    case ResponseType::RECEIVE_METER_ID: {
      ESP_LOGV(TAG, "* process_and_update METER_ID");

      char serial_number[20];

      sprintf(serial_number, "%02u%02u%02u %02u%02u%02u", buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6]);

      std::string string_serial_number(serial_number);

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      UPDATE_TEXT_SENSOR(meter_id, string_serial_number)

      if (this->init_state_ == SmInitState::INIT_GET_METER_ID) {
#ifdef USE_TEXT_SENSOR
        if (this->meter_id_text_sensor_ == nullptr) {
          ESP_LOGD(TAG, "'meter_id': %s", string_serial_number.c_str());
        }
#else
        ESP_LOGD(TAG, "'meter_id': %s", string_serial_number.c_str());
#endif
        this->init_state_ = SmInitState::INIT_GET_METER_STATE;
      }

      break;
    }
    case ResponseType::RECEIVE_METER_STATE: {
      ESP_LOGV(TAG, "* process_and_update METER_STATE");

      this->data_.phase_count = buffer[1];
      this->data_.meter_state = buffer[2];
      this->data_.delay_state = buffer[14];
      this->data_.delay_value_remaining = (buffer[12] << 8) | buffer[13];

      this->data_.total_energy = ((buffer[3] << 24) | (buffer[4] << 16) | (buffer[5] << 8) | buffer[6]) * 0.01;
      this->data_.active_power_total = ((buffer[8] << 16) | (buffer[9] << 8) | buffer[10]) * 0.0001;

      if (this->data_.meter_state) {
        this->data_.warning_off_by_over_voltage = false;
        this->data_.warning_off_by_under_voltage = false;
        this->data_.warning_off_by_over_current = false;
        this->data_.warning_off_by_end_purchase = false;
        this->data_.warning_off_by_end_delay = false;
        this->data_.warning_off_by_user = false;
      }

      if (!this->data_.warning_off_by_over_voltage) {
        this->data_.warning_off_by_over_voltage = (buffer[7] == 1);
      }

      if (!this->data_.warning_off_by_under_voltage) {
        this->data_.warning_off_by_under_voltage = (buffer[7] == 2);
      }

      if (!this->data_.warning_off_by_over_current) {
        this->data_.warning_off_by_over_current = buffer[11];
      }

      if (!this->data_.warning_off_by_end_purchase) {
        this->data_.warning_off_by_end_purchase = (len == 16 ? buffer[15] : false);
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

      UPDATE_SWITCH(meter_state, this->data_.meter_state)
      UPDATE_SWITCH(delay_state, this->data_.delay_state)

      this->update_meter_state_detail_();

      if (this->init_state_ == SmInitState::INIT_GET_METER_STATE) {
        this->init_state_ = SmInitState::INIT_GET_MEASUREMENT;
      }

      break;
    }
    case ResponseType::RECEIVE_MEASUREMENT: {
      ESP_LOGV(TAG, "* process_and_update MEASUREMENT");

      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current, encode_uint24(buffer[1], buffer[2], buffer[3]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_1, ((buffer[1] << 16) | (buffer[2] << 8) | buffer[3]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_2, ((buffer[4] << 16) | (buffer[5] << 8) | buffer[6]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS_CURRENT(current_phase_3, ((buffer[7] << 16) | (buffer[8] << 8) | buffer[9]) * 0.001)

      UPDATE_SENSOR_MEASUREMENTS(voltage, ((buffer[10] << 8) | buffer[11]) * 0.1)
      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_1, ((buffer[10] << 8) | buffer[11]) * 0.1)
      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_2, ((buffer[12] << 8) | buffer[13]) * 0.1)
      UPDATE_SENSOR_MEASUREMENTS(voltage_phase_3, ((buffer[14] << 8) | buffer[15]) * 0.1)

      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power, ((buffer[19] << 16) | (buffer[20] << 8) | buffer[21]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_1, ((buffer[19] << 16) | (buffer[20] << 8) | buffer[21]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_2, ((buffer[22] << 16) | (buffer[23] << 8) | buffer[24]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_phase_3, ((buffer[25] << 16) | (buffer[26] << 8) | buffer[27]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(reactive_power_total, ((buffer[16] << 16) | (buffer[17] << 8) | buffer[18]) * 0.0001)

      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power, ((buffer[31] << 16) | (buffer[32] << 8) | buffer[33]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_1, ((buffer[31] << 16) | (buffer[32] << 8) | buffer[33]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_2, ((buffer[34] << 16) | (buffer[35] << 8) | buffer[36]) * 0.0001)
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_phase_3, ((buffer[37] << 16) | (buffer[38] << 8) | buffer[39]) * 0.0001)
      this->data_.active_power_total = ((buffer[28] << 16) | (buffer[29] << 8) | buffer[30]) * 0.0001;
      UPDATE_SENSOR_MEASUREMENTS_POWER(active_power_total, this->data_.active_power_total)

      UPDATE_SENSOR_MEASUREMENTS(power_factor, ((buffer[42] << 8) | buffer[43]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_1, ((buffer[42] << 8) | buffer[43]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_2, ((buffer[44] << 8) | buffer[45]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_phase_3, ((buffer[46] << 8) | buffer[47]) * 0.001)
      UPDATE_SENSOR_MEASUREMENTS(power_factor_total, ((buffer[40] << 8) | buffer[41]) * 0.001)

      this->data_.total_energy = ((buffer[50] << 24) | (buffer[51] << 16) | (buffer[52] << 8) | buffer[53]) * 0.01;
      UPDATE_SENSOR_MEASUREMENTS(total_energy, this->data_.total_energy)
      UPDATE_SENSOR_MEASUREMENTS(contract_total_energy, this->data_.starting_kWh + this->data_.total_energy)
      UPDATE_SENSOR_MEASUREMENTS(total_energy_price, this->data_.price_kWh * this->data_.total_energy)

      UPDATE_SENSOR_MEASUREMENTS(import_active_energy, ((buffer[54] << 24) | (buffer[55] << 16) | (buffer[56] << 8) | buffer[57]) * 0.01)
      UPDATE_SENSOR_MEASUREMENTS(export_active_energy, ((buffer[58] << 24) | (buffer[59] << 16) | (buffer[60] << 8) | buffer[61]) * -0.01)

      UPDATE_SENSOR_MEASUREMENTS(frequency, ((buffer[48] << 8) | buffer[49]) * 0.01)

      if (this->init_state_ == SmInitState::INIT_GET_MEASUREMENT) {
        this->init_state_ = SmInitState::INIT_GET_LIMIT_AND_PURCHASE;
      }

      break;
    }
    case ResponseType::RECEIVE_LIMIT_AND_ENERGY_PURCHASE: {
      ESP_LOGV(TAG, "* process_and_update LIMIT_AND_PURCHASE");

      this->data_.max_voltage_limit = (buffer[1] << 8) | buffer[2];
      this->data_.min_voltage_limit = (buffer[3] << 8) | buffer[4];
      this->data_.max_current_limit = ((buffer[5] << 8) | buffer[6]) * 0.01;

      UPDATE_NUMBER(max_voltage_limit, this->data_.max_voltage_limit)
      UPDATE_NUMBER(min_voltage_limit, this->data_.min_voltage_limit)
      UPDATE_NUMBER(max_current_limit, this->data_.max_current_limit)

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if (len == 20) {
        this->data_.energy_purchase_state = buffer[19];

        // this->data_.energy_purchase_value = (((buffer[7] << 24) | (buffer[8] << 16) | (buffer[9] << 8) | buffer[10]) * 0.01);
        // this->data_.energy_purchase_alarm = (((buffer[15] << 24) | (buffer[16] << 16) | (buffer[17] << 8) | buffer[18]) * 0.01);
        this->data_.energy_purchase_balance = (((buffer[11] << 24) | (buffer[12] << 16) | (buffer[13] << 8) | buffer[14]) * 0.01);

        this->data_.warning_purchase_alarm = ((this->data_.energy_purchase_balance <= this->data_.energy_purchase_alarm) && this->data_.energy_purchase_state);
      }

      UPDATE_SENSOR(energy_purchase_balance, this->data_.energy_purchase_balance)
      UPDATE_SENSOR(energy_purchase_price, this->data_.energy_purchase_balance * this->data_.price_kWh)

      UPDATE_SWITCH(energy_purchase_state, this->data_.energy_purchase_state)

      UPDATE_BINARY_SENSOR(warning_purchase_alarm, this->data_.warning_purchase_alarm)

      if (this->init_state_ == SmInitState::INIT_GET_LIMIT_AND_PURCHASE) {
        if (this->status_has_warning()) {
          this->init_state_ = SmInitState::INIT_ERROR;
        } else {
          this->init_state_ = SmInitState::INIT_DONE;
        }
      }

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

  if (this->expected_response_.has_value()) {
    if (this->expected_response_ == command_type) {
      if (this->command_queue_.front().raw) {
        ESP_LOGD(TAG, "* Successful response");
      } else {
        ESP_LOGV(TAG, "* Successful response");
      }

      this->expected_response_.reset();

      if (this->command_queue_.front().null_old_response) {
        ESP_LOGI(TAG, "* The response received was cancelled, since a more updated command arrived 0x%02X-0x%02X", command_type, buffer[0]);

        this->command_queue_.front().null_old_response = false;

        return;
      } else {
        this->command_queue_.erase(this->command_queue_.begin());
      }

      this->init_retries_ = 0;
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
    case CommandType::HEARTBEAT: {
      ESP_LOGV(TAG, "MCU Heartbeat (0x%02X)", buffer[0]);

      if (buffer[0] == 0x00) {
        ESP_LOGI(TAG, "MCU restarted");

        this->init_state_ = SmInitState::INIT_HEARTBEAT;

        this->cancel_interval(heartbeat_interval_str_);
        this->cancel_interval(log_command_queue_str_);

        this->component_state_ &= ~COMPONENT_STATE_MASK;
        this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;
      }

      if (this->init_state_ == SmInitState::INIT_HEARTBEAT) {
        this->protocol_version_ = version;

        this->init_state_ = SmInitState::INIT_PRODUCT_QUERY;
      }

      break;
    }
    case CommandType::PRODUCT_QUERY: {
      ESP_LOGD(TAG, "Process Command PRODUCT_QUERY");

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

      if (this->init_state_ == SmInitState::INIT_PRODUCT_QUERY) {
        this->init_state_ = SmInitState::INIT_WORKING_MODE_QUERY;
      }

      break;
    }
    case CommandType::WORKING_MODE_QUERY: {
      ESP_LOGD(TAG, "Process Command WORKING_MODE_QUERY");

      if (len >= 2) {
        this->status_pin_reported_ = buffer[0];
        this->reset_pin_reported_ = buffer[1];
      }

      if (this->init_state_ == SmInitState::INIT_WORKING_MODE_QUERY) {
        if (len >= 2) {
          this->init_state_ = SmInitState::INIT_DATAPOINT_QUERY;
        } else {
          this->init_state_ = SmInitState::INIT_WIFI_STATE;
        }
      }

      break;
    }
    case CommandType::WIFI_STATE: {
      ESP_LOGD(TAG, "Process Command WIFI_STATE");

      if (this->init_state_ == SmInitState::INIT_WIFI_STATE) {
        this->init_state_ = SmInitState::INIT_DATAPOINT_QUERY;
      }

      break;
    }
    case CommandType::DATAPOINT_REPORT: {
      this->process_and_update_data_(buffer, len);

      break;
    }
    case CommandType::WIFI_TEST: {
      ESP_LOGD(TAG, "Process Command WIFI_TEST");

      this->push_command_(SmCommand{.cmd = CommandType::WIFI_TEST, .payload = std::vector<uint8_t>{0x00, 0x00}});

      break;
    }
    case CommandType::WIFI_RSSI: {
      ESP_LOGD(TAG, "Process Command WIFI_RSSI");

      this->push_command_(SmCommand{.cmd = CommandType::WIFI_RSSI, .payload = std::vector<uint8_t>{0xEC}});

      break;
    }
    case CommandType::LOCAL_TIME_QUERY: {
      ESP_LOGD(TAG, "Process Command LOCAL_TIME_QUERY");

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
      ESP_LOGD(TAG, "Process Command GET_NETWORK_STATUS");

      this->push_command_(SmCommand{.cmd = CommandType::GET_NETWORK_STATUS, .payload = std::vector<uint8_t>{0x04}});

      break;
    }
    default:
      if (len == 0) {
        ESP_LOGE(TAG, "Command (0x%02X) is not handled", command);
      } else {
        ESP_LOGE(TAG, "Command (0x%02X) is not handled - Data: %s", command, format_hex_pretty(buffer, len).c_str());
      }
  }
}

void Dxs238xwComponent::process_and_update_data_(const uint8_t *buffer, size_t len) {
  while (len >= 4) {
    TuyaDatapoint datapoint{};
    datapoint.id = (DatapointId) buffer[0];
    datapoint.type = (TuyaDatapointType) buffer[1];
    datapoint.value_uint32 = 0;

    size_t data_size = (buffer[2] << 8) + buffer[3];
    const uint8_t *data = buffer + 4;
    size_t data_len = len - 4;

    if (data_size > data_len) {
      ESP_LOGW(TAG, "Datapoint %u is truncated and cannot be parsed (%zu > %zu)", datapoint.id, data_size, data_len);
      return;
    }

    datapoint.len = data_size;

    len -= data_size + 4;
    buffer = data + data_size;

    switch (datapoint.type) {
      case TuyaDatapointType::RAW:
        datapoint.value_raw = std::vector<uint8_t>(data, data + data_size);

        ESP_LOGD(TAG, "Datapoint %u update to %s", datapoint.id, format_hex_pretty(datapoint.value_raw).c_str());

        break;
      case TuyaDatapointType::BOOL:
        if (data_size != 1) {
          ESP_LOGW(TAG, "Datapoint %u has bad bool len %zu", datapoint.id, data_size);

          continue;
        }

        datapoint.value_bool = data[0];

        ESP_LOGD(TAG, "Datapoint %u update to %s", datapoint.id, TRUEFALSE(datapoint.value_bool));

        break;
      case TuyaDatapointType::UINT8:
        if (data_size != 1) {
          ESP_LOGW(TAG, "Datapoint %u has bad uint8 len %zu", datapoint.id, data_size);

          continue;
        }

        datapoint.value_uint8 = data[0];

        ESP_LOGD(TAG, "Datapoint %u update to %d", datapoint.id, datapoint.value_uint8);

        break;
      case TuyaDatapointType::UINT32:
        if (data_size != 4) {
          ESP_LOGW(TAG, "Datapoint %u has bad uint32 len %zu", datapoint.id, data_size);

          continue;
        }

        datapoint.value_uint32 = encode_uint32(data[0], data[1], data[2], data[3]);

        ESP_LOGD(TAG, "Datapoint %u update to %d", datapoint.id, datapoint.value_uint32);

        break;
      case TuyaDatapointType::STRING:
        datapoint.value_string = std::string(reinterpret_cast<const char *>(data), data_size);

        ESP_LOGD(TAG, "Datapoint %u update to %s", datapoint.id, datapoint.value_string.c_str());

        break;
      case TuyaDatapointType::BITMAP:
        switch (data_size) {
          case 1:
            datapoint.value_bitmap = encode_uint32(0, 0, 0, data[0]);
            break;
          case 2:
            datapoint.value_bitmap = encode_uint32(0, 0, data[0], data[1]);
            break;
          case 4:
            datapoint.value_bitmap = encode_uint32(data[0], data[1], data[2], data[3]);
            break;
          default:
            ESP_LOGW(TAG, "Datapoint %u has bad bitmask len %zu", datapoint.id, data_size);
            continue;
        }

        ESP_LOGD(TAG, "Datapoint %u update to %#08X", datapoint.id, datapoint.value_bitmap);

        break;
      default:
        ESP_LOGW(TAG, "Datapoint %u has unknown type %#02hhX", datapoint.id, static_cast<uint8_t>(datapoint.type));

        continue;
    }

    bool found = false;

    for (auto &other : this->datapoints_) {
      if (other.id == datapoint.id) {
        other = datapoint;
        found = true;
        break;
      }
    }

    if (!found) {
      this->datapoints_.push_back(datapoint);
    }

    DatapointId datapoint_id = (DatapointId) datapoint.id;

    switch (datapoint_id) {
#ifdef USE_MODEL_DDS238_2
      case DatapointId::CURRENT: {
        UPDATE_SENSOR(current, datapoint.value_uint32 * 0.001)

        break;
      }
      case DatapointId::VOLTAGE: {
        UPDATE_SENSOR(voltage, datapoint.value_uint32 * 0.1)

        break;
      }
      case DatapointId::ACTIVE_POWER: {
        UPDATE_SENSOR(active_power, datapoint.value_uint32 * 0.1)

        break;
      }
      case DatapointId::IMPORT_ACTIVE_ENERGY: {
        UPDATE_SENSOR(import_active_energy, datapoint.value_uint32 * 0.01)

        break;
      }
      case DatapointId::EXPORT_ACTIVE_ENERGY: {
        UPDATE_SENSOR(export_active_energy, datapoint.value_uint32 * -0.01)

        break;
      }
      case DatapointId::METER_STATE: {
        this->data_.meter_state = datapoint.value_bool;

        if (this->data_.meter_state) {
          this->data_.warning_off_by_over_voltage = false;
          this->data_.warning_off_by_under_voltage = false;
          this->data_.warning_off_by_over_current = false;
          this->data_.warning_off_by_end_purchase = false;
          this->data_.warning_off_by_end_delay = false;
          this->data_.warning_off_by_user = false;
        }

        UPDATE_BINARY_SENSOR(warning_off_by_over_voltage, this->data_.warning_off_by_over_voltage)
        UPDATE_BINARY_SENSOR(warning_off_by_under_voltage, this->data_.warning_off_by_under_voltage)
        UPDATE_BINARY_SENSOR(warning_off_by_over_current, this->data_.warning_off_by_over_current)
        UPDATE_BINARY_SENSOR(warning_off_by_end_purchase, this->data_.warning_off_by_end_purchase)
        UPDATE_BINARY_SENSOR(warning_off_by_end_delay, this->data_.warning_off_by_end_delay)
        UPDATE_BINARY_SENSOR(warning_off_by_user, this->data_.warning_off_by_user)

        UPDATE_BINARY_SENSOR(meter_state, this->data_.meter_state)

        UPDATE_SWITCH(meter_state, this->data_.meter_state)

        this->update_meter_state_detail_();

        break;
      }
#endif
#ifdef USE_MODEL_DDS238_4
#endif
#ifdef USE_MODEL_DTS238_7
#endif
      default:
        ESP_LOGW(TAG, "Datapoint %u has not processed", datapoint.id);
    }
  }

  if (this->init_state_ == SmInitState::INIT_DATAPOINT_QUERY) {
    if (this->status_has_warning()) {
      this->init_state_ = SmInitState::INIT_ERROR;
    } else {
      this->init_state_ = SmInitState::INIT_DONE;
    }
  }
}

//----------------------

void Dxs238xwComponent::set_bool_datapoint_value_(DatapointId datapoint_id, bool value, bool priority_cmd) {
  //
  this->numeric_datapoint_value_(datapoint_id, TuyaDatapointType::BOOL, value, 1, priority_cmd);
}

void Dxs238xwComponent::set_uint8_datapoint_value_(DatapointId datapoint_id, uint8_t value, bool priority_cmd) {
  //
  this->numeric_datapoint_value_(datapoint_id, TuyaDatapointType::UINT8, value, 1, priority_cmd);
}

void Dxs238xwComponent::set_uint32_datapoint_value_(DatapointId datapoint_id, uint32_t value, bool priority_cmd) {
  //
  this->numeric_datapoint_value_(datapoint_id, TuyaDatapointType::UINT32, value, 4, priority_cmd);
}

void Dxs238xwComponent::set_bitmap_datapoint_value_(DatapointId datapoint_id, uint32_t value, uint8_t length, bool priority_cmd) {
  //
  this->numeric_datapoint_value_(datapoint_id, TuyaDatapointType::BITMAP, value, length, priority_cmd);
}

void Dxs238xwComponent::set_raw_datapoint_value_(DatapointId datapoint_id, const std::vector<uint8_t> &value, bool priority_cmd) {
  ESP_LOGD(TAG, "Setting datapoint %u to %s", datapoint_id, format_hex_pretty(value).c_str());

  optional<TuyaDatapoint> datapoint = this->get_datapoint_(datapoint_id);

  if (!datapoint.has_value()) {
    ESP_LOGW(TAG, "Setting unknown datapoint %u", datapoint_id);
  } else if (datapoint->type != TuyaDatapointType::RAW) {
    ESP_LOGE(TAG, "Attempt to set datapoint %u with incorrect type", datapoint_id);
    return;
  } else if (datapoint->value_raw == value) {
    ESP_LOGV(TAG, "Not sending unchanged value");
    return;
  }

  this->send_datapoint_command_(datapoint_id, TuyaDatapointType::RAW, value, priority_cmd);
}

void Dxs238xwComponent::set_string_datapoint_value_(DatapointId datapoint_id, const std::string &value, bool priority_cmd) {
  ESP_LOGD(TAG, "Setting datapoint %u to %s", datapoint_id, value.c_str());

  optional<TuyaDatapoint> datapoint = this->get_datapoint_(datapoint_id);

  if (!datapoint.has_value()) {
    ESP_LOGW(TAG, "Setting unknown datapoint %u", datapoint_id);
  } else if (datapoint->type != TuyaDatapointType::STRING) {
    ESP_LOGE(TAG, "Attempt to set datapoint %u with incorrect type", datapoint_id);
    return;
  } else if (datapoint->value_string == value) {
    ESP_LOGV(TAG, "Not sending unchanged value");
    return;
  }

  std::vector<uint8_t> data;

  for (char const &c : value) {
    data.push_back(c);
  }

  this->send_datapoint_command_(datapoint_id, TuyaDatapointType::STRING, data, priority_cmd);
}

//----------------------

void Dxs238xwComponent::numeric_datapoint_value_(DatapointId datapoint_id, TuyaDatapointType datapoint_type, const uint32_t value, uint8_t length, bool priority_cmd) {
  ESP_LOGD(TAG, "Setting datapoint %u to %u", datapoint_id, value);

  optional<TuyaDatapoint> datapoint = this->get_datapoint_(datapoint_id);

  if (!datapoint.has_value()) {
    ESP_LOGW(TAG, "Setting unknown datapoint %u", datapoint_id);
  } else if (datapoint->type != datapoint_type) {
    ESP_LOGE(TAG, "Attempt to set datapoint %u with incorrect type", datapoint_id);

    return;
  } else if (datapoint->value_uint32 == value) {
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

  this->send_datapoint_command_(datapoint_id, datapoint_type, data, priority_cmd);
}

void Dxs238xwComponent::send_datapoint_command_(DatapointId datapoint_id, TuyaDatapointType datapoint_type, std::vector<uint8_t> data, bool priority_cmd) {
  std::vector<uint8_t> buffer;

  buffer.push_back((uint8_t) datapoint_id);
  buffer.push_back(static_cast<uint8_t>(datapoint_type));
  buffer.push_back(data.size() >> 8);
  buffer.push_back(data.size() >> 0);

  buffer.insert(buffer.end(), data.begin(), data.end());

  this->push_command_(SmCommand{.cmd = CommandType::DATAPOINT_DELIVER, .payload = buffer, .priority_cmd = priority_cmd});
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
    } else if (info.type == TuyaDatapointType::BOOL) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: bool (value: %s)", info.id, TRUEFALSE(info.value_bool));
    } else if (info.type == TuyaDatapointType::UINT8) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: uint8 (value: %d)", info.id, info.value_uint8);
    } else if (info.type == TuyaDatapointType::UINT32) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: uint32 value (value: %d)", info.id, info.value_uint32);
    } else if (info.type == TuyaDatapointType::STRING) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: string value (value: %s)", info.id, info.value_string.c_str());
    } else if (info.type == TuyaDatapointType::BITMAP) {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: bitmap (value: %x)", info.id, info.value_bitmap);
    } else {
      ESP_LOGCONFIG(TAG, "* Datapoint %u: unknown", info.id);
    }
  }

  if ((this->status_pin_reported_ != -1) || (this->reset_pin_reported_ != -1)) {
    ESP_LOGCONFIG(TAG, "* GPIO Configuration: status: pin %d, reset: pin %d", this->status_pin_reported_, this->reset_pin_reported_);
  }

  ESP_LOGCONFIG(TAG, "* Product: '%s'", this->product_.c_str());
}

#endif

#ifdef USE_TIME
void Dxs238xwComponent::send_local_time_() {  // XXXXXXXXXXXXXXXXXXXXXXXXXXX
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

  // this->push_command_(SmCommand{.cmd = CommandType::LOCAL_TIME_QUERY, .payload = payload}, PUSH_BACK);
}
#endif

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::process_command_queue_() {
  uint32_t now = millis();

  if ((this->last_rx_char_timestamp_ != 0) && (now - this->last_rx_char_timestamp_) > SM_MAX_MILLIS_TO_RX_BYTE) {
    ESP_LOGW(TAG, "* Time ran out waiting for a new char");

    this->rx_message_.clear();

    this->last_rx_char_timestamp_ = 0;

    this->error_type_ = SmErrorType::COMMUNICATION;
    this->error_code_ = SmErrorCode::NOT_ENOUGHT_BYTES;

    this->print_error_();
  }

  if (this->is_message_timeout_()) {
    if (this->init_state_ != SmInitState::INIT_DONE) {
      if (++this->init_retries_ >= SM_MAX_RETRIES) {
        ESP_LOGE(TAG, "Initialization failed at init_state %u", static_cast<uint8_t>(this->init_state_));

        this->status_set_warning();

        this->init_state_ = (SmInitState) (static_cast<uint8_t>(this->init_state_) + 1);

        this->command_queue_.erase(command_queue_.begin());

        this->init_retries_ = 0;
      }
    } else {
      if (this->command_queue_.front().payload.empty()) {
        ESP_LOGW(TAG, "* Confirmation or Response command timed out = %02X", this->command_queue_.front().cmd);
      } else {
        ESP_LOGW(TAG, "* Confirmation or Response command timed out = %02X/%02X", this->command_queue_.front().cmd, this->command_queue_.front().payload[0]);
      }

      this->command_queue_.erase(this->command_queue_.begin());

      this->error_type_ = SmErrorType::COMMUNICATION;
      this->error_code_ = SmErrorCode::TIMEOUT;

      this->print_error_();
    }
  }

  if ((now - this->last_command_timestamp_) > SM_COMMAND_DELAY && !this->command_queue_.empty() && this->rx_message_.empty() && !this->is_expected_message()) {
    this->send_raw_command_(this->command_queue_.front());

    if (!this->is_expected_message()) {
      this->command_queue_.erase(this->command_queue_.begin());
    }
  }
}

void Dxs238xwComponent::send_raw_command_(SmCommand command) {
  this->error_type_ = SmErrorType::NO_ERROR;
  this->error_code_ = SmErrorCode::NO_ERROR;

  ESP_LOGV(TAG, "* Queue command processing");

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
  buffer.push_back(++this->version_msg);
#endif

#ifdef USE_PROTOCOL_TUYA
  switch (command.cmd) {
    case CommandType::HEARTBEAT:
      this->expected_response_ = CommandType::HEARTBEAT;
      break;
    case CommandType::PRODUCT_QUERY:
      this->expected_response_ = CommandType::PRODUCT_QUERY;
      break;
    case CommandType::WORKING_MODE_QUERY:
      this->expected_response_ = CommandType::WORKING_MODE_QUERY;
      break;
    case CommandType::WIFI_STATE:
      this->expected_response_ = CommandType::WIFI_STATE;
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
  if (this->expected_confirmation_.has_value()) {
    if (command.raw) {
      ESP_LOGD(TAG, "* Waiting to confirmation");
    } else {
      ESP_LOGV(TAG, "* Waiting to confirmation");
    }
  }
#endif

#ifdef USE_PROTOCOL_TUYA
  if (this->expected_response_.has_value()) {
    if (command.raw) {
      ESP_LOGD(TAG, "* Waiting to response");
    } else {
      ESP_LOGV(TAG, "* Waiting to response");
    }
  }
#endif
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::push_command_(const SmCommand &command) {
  uint8_t index = 0;

  for (auto &cmd : this->command_queue_) {
    if (cmd.cmd == command.cmd) {
      if (cmd.payload.empty() && command.payload.empty()) {
        ESP_LOGV(TAG, "* We discard the command because it is repeated %02X", command.cmd);

        return;
      } else {
        bool replaced = false;
        bool null_response = false;

        if (!cmd.payload.empty() && !command.payload.empty() && cmd.payload.front() == command.payload.front()) {
          if (index == 0) {
            if (this->is_expected_message()) {
              if (cmd.payload == command.payload) {
                ESP_LOGV(TAG, "* We discard the command because it is repeated %02X-%02X", command.cmd, command.payload.front());

                return;
              } else {
                replaced = true;

                null_response = true;
              }
            } else {
              replaced = true;
            }
          } else {
            replaced = true;
          }

          if (replaced) {
            cmd = command;

            ESP_LOGV(TAG, "* Command exist in queue, is replaced with new command %02X-%02X", command.cmd, command.payload.front());

            if (null_response) {
              cmd.null_old_response = true;

              ESP_LOGV(TAG, "* Waiting response is discarded %02X-%02X", command.cmd, command.payload.front());
            }

          } else {
            ESP_LOGV(TAG, "* Command exist in queue, new message is discarded %02X-%02X", command.cmd, command.payload.front());
          }

          return;
        }
      }
    }

    index++;
  }

  if (command.priority_cmd) {
    if (this->command_queue_.empty()) {
      ESP_LOGV(TAG, "* Priority command added to empty list: 1° place - 0x%02X-0x%02X", command.cmd, command.payload.front());

      command_queue_.push_back(command);
    } else {
      index = 0;

      for (auto &cmd_ex : this->command_queue_) {
        if (cmd_ex.priority_cmd) {
          if (index == (this->command_queue_.size() - 1)) {
            ESP_LOGV(TAG, "* Priority command added after other priorities: %u° place - 0x%02X-0x%02X", index + 1, command.cmd, command.payload.front());

            command_queue_.push_back(command);

            return;
          }
        } else {
          if (index == 0) {
            if (this->is_expected_message()) {
              if (this->command_queue_.size() == 1) {
                ESP_LOGV(TAG, "* Priority command added after message in process: 2° place - 0x%02X-0x%02X", command.cmd, command.payload.front());

                command_queue_.push_back(command);

                return;
              }
            } else {
              ESP_LOGV(TAG, "* Added priority command: 1° place - 0x%02X-0x%02X", command.cmd, command.payload.front());

              this->command_queue_.insert(this->command_queue_.begin(), command);

              return;
            }
          } else {
            ESP_LOGV(TAG, "* Priority command added after other priorities: %u° place - 0x%02X-0x%02X", index + 1, command.cmd, command.payload.front());

            this->command_queue_.insert(this->command_queue_.begin() + index, command);

            return;
          }
        }

        index++;
      }
    }
  } else {
    ESP_LOGV(TAG, "* Normal command added to the end of the list: %u° lugar - 0x%02X-0x%02X", this->command_queue_.size() + 1, command.cmd, command.payload.front());

    command_queue_.push_back(command);
  }
}

void Dxs238xwComponent::send_command_(SmCommandType cmd, bool state) {
  switch (cmd) {
    case SmCommandType::SET_LIMIT: {
      ESP_LOGD(TAG, "Push Command - SET_LIMIT");

#ifdef USE_PROTOCOL_HEKR
      uint16_t tmp_current_limit = this->data_.max_current_limit * 100;

      std::vector<uint8_t> buffer;

      buffer.push_back((uint8_t) CommandData::SET_LIMIT);

      buffer.push_back(tmp_current_limit >> 8);
      buffer.push_back(tmp_current_limit & SM_GET_ONE_BYTE);

      buffer.push_back(this->data_.max_voltage_limit >> 8);
      buffer.push_back(this->data_.max_voltage_limit & SM_GET_ONE_BYTE);

      buffer.push_back(this->data_.min_voltage_limit >> 8);
      buffer.push_back(this->data_.min_voltage_limit & SM_GET_ONE_BYTE);

      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = buffer, .priority_cmd = true});

      ESP_LOGD(TAG, "* Input Data: max_current_limit = %u, max_voltage_limit = %u, min_voltage_limit = %u", this->data_.max_current_limit, this->data_.max_voltage_limit, this->data_.min_voltage_limit);
#endif

#ifdef USE_PROTOCOL_TUYA
      ESP_LOGD(TAG, "* Command not available");
#endif

      break;
    }
    case SmCommandType::SET_ENERGY_PURCHASE: {
      ESP_LOGD(TAG, "Push Command - SET_PURCHASE");

#ifdef USE_PROTOCOL_HEKR
      uint32_t purchase_value = 0;
      uint32_t purchase_alarm = 0;

      if (state) {
        purchase_value = this->data_.energy_purchase_value * 100;
        purchase_alarm = this->data_.energy_purchase_alarm * 100;
      }

      std::vector<uint8_t> buffer;

      buffer.push_back((uint8_t) CommandData::SET_ENERGY_PURCHASE);

      buffer.push_back(purchase_value >> 24);
      buffer.push_back(purchase_value >> 16);
      buffer.push_back(purchase_value >> 8);
      buffer.push_back(purchase_value & SM_GET_ONE_BYTE);

      buffer.push_back(purchase_alarm >> 24);
      buffer.push_back(purchase_alarm >> 16);
      buffer.push_back(purchase_alarm >> 8);
      buffer.push_back(purchase_alarm & SM_GET_ONE_BYTE);

      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = buffer, .priority_cmd = true});

      ESP_LOGD(TAG, "* Input Data: purchase_value = %u, purchase_alarm = %u, state = %s", (state ? this->data_.energy_purchase_value : 0), (state ? this->data_.energy_purchase_alarm : 0), ONOFF(state));
#endif

#ifdef USE_PROTOCOL_TUYA
      ESP_LOGD(TAG, "* Command not available");
#endif

      break;
    }
    case SmCommandType::SET_METER_STATE: {
      ESP_LOGD(TAG, "Push Command - SET_METER_STATE");

#ifdef USE_PROTOCOL_HEKR
      std::vector<uint8_t> buffer;

      buffer.push_back((uint8_t) CommandData::SET_METER_STATE);
      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = buffer, .priority_cmd = true});
#endif

#ifdef USE_PROTOCOL_TUYA
      this->set_bool_datapoint_value_(DatapointId::METER_STATE, state, true);
#endif

      ESP_LOGD(TAG, "* Input Data: state = %s", ONOFF(state));

      break;
    }
    case SmCommandType::SET_DELAY: {
      ESP_LOGD(TAG, "Push Command - SET_DELAY");

#ifdef USE_PROTOCOL_HEKR
      uint16_t delay_value_set = 0;

      if (state) {
        delay_value_set = this->data_.delay_value_set;
      }

      std::vector<uint8_t> buffer;

      buffer.push_back((uint8_t) CommandData::SET_DELAY);

      buffer.push_back(delay_value_set >> 8);
      buffer.push_back(delay_value_set & SM_GET_ONE_BYTE);

      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = buffer, .priority_cmd = true});

      ESP_LOGD(TAG, "* Input Data: delay_value_set = %u, state = %s", delay_value_set, ONOFF(state));
#endif

#ifdef USE_PROTOCOL_TUYA
      ESP_LOGD(TAG, "* Command not available");
#endif

      break;
    }
    case SmCommandType::SET_RESET: {
      ESP_LOGD(TAG, "Push Command - SET_RESET");

#ifdef USE_PROTOCOL_HEKR
      this->push_command_(SmCommand{.cmd = CommandType::SEND, .payload = std::vector<uint8_t>{(uint8_t) CommandData::SET_RESET}, .priority_cmd = true});
#endif

#ifdef USE_PROTOCOL_TUYA
      ESP_LOGD(TAG, "* Command not available");
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
  ESP_LOGD(TAG, "In --- hex_message");

  ESP_LOGD(TAG, "* Message in: %s", message.c_str());

  this->error_type_ = SmErrorType::NO_ERROR;
  this->error_code_ = SmErrorCode::NO_ERROR;

  uint32_t length_message = message.length();

  if (length_message == 0 || length_message > SM_MAX_HEX_MSG_LENGTH) {
    ESP_LOGV(TAG, "* Very long message");

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
          ESP_LOGV(TAG, "* The number of hex characters must be 2 characters per byte");

          this->error_type_ = SmErrorType::INPUT_DATA;
          this->error_code_ = SmErrorCode::WRONG_MSG;

          break;
        }
      } else {
        if (character_hex_index == 2) {
          ESP_LOGV(TAG, "* Each hexadecimal value must have two characters and a period that separates them");

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
        ESP_LOGV(TAG, "* The number of hex characters must be 2 characters per byte");

        this->error_type_ = SmErrorType::INPUT_DATA;
        this->error_code_ = SmErrorCode::WRONG_MSG;
      }

      const uint8_t *ptr_message_without_dots = &message_without_dots[0];

      if (this->error_code_ == SmErrorCode::NO_ERROR) {
        std::string str_message_without_dots = std::string(reinterpret_cast<const char *>(ptr_message_without_dots), size_message_without_dots);

        ESP_LOGV(TAG, "* Message without points: %s", str_message_without_dots.c_str());

        std::vector<uint8_t> message_hex;
        uint8_t message_hex_size = size_message_without_dots / 2;

        if (!parse_hex(str_message_without_dots, message_hex, message_hex_size)) {
          ESP_LOGV(TAG, "* There are characters that are not hexadecimal");

          this->error_type_ = SmErrorType::INPUT_DATA;
          this->error_code_ = SmErrorCode::WRONG_MSG;
        } else if (message_hex_size < SM_MIN_COMMAND_LENGHT) {
          ESP_LOGV(TAG, "* The length of the message does not meet the minimum number of bytes expected");

          this->error_type_ = SmErrorType::INPUT_DATA;
          this->error_code_ = SmErrorCode::WRONG_MSG;
        }

        if (this->error_code_ == SmErrorCode::NO_ERROR) {
          ESP_LOGV(TAG, "* Mensaje hex without crc: %s", format_hex_pretty(message_hex).c_str());

          if (check_crc) {
            if (this->calculate_crc_(message_hex.data(), message_hex_size - 1) != message_hex[message_hex_size - 1]) {
              ESP_LOGV(TAG, "* Crc is incorrect");

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

              this->push_command_(SmCommand{.cmd = command, .payload = message_hex, .raw = true, .priority_cmd = true});

              return;
            }
#endif

#ifdef USE_PROTOCOL_TUYA
            uint16_t length = (uint16_t(message_hex[4]) << 8) | (uint16_t(message_hex[5]));

            ESP_LOGV(TAG, "* Message length: %u", length);

            if (message_hex[0] != TUYA_HEADER_1 || message_hex[1] != TUYA_HEADER_2) {
              ESP_LOGV(TAG, "* Header error");

              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::WRONG_BYTES_HEADER;
            } else if (message_hex[2] != 0x00) {
              ESP_LOGV(TAG, "* Message version error");

              this->error_type_ = SmErrorType::INPUT_DATA;
              this->error_code_ = SmErrorCode::WRONG_BYTES_VERSION_PROTOCOL;
            } else {
              CommandType command = (CommandType) message_hex[3];
              ESP_LOGV(TAG, "* Command to send %u", command);

              if (message_hex_size == SM_MIN_COMMAND_LENGHT) {
                ESP_LOGD(TAG, "* Command without payload");

                this->push_command_(SmCommand{.cmd = command, .raw = true, .priority_cmd = true});
              } else {
                message_hex.erase(message_hex.begin(), message_hex.begin() + 6);
                message_hex.pop_back();

                ESP_LOGD(TAG, "* Command with payload: %s", format_hex_pretty(message_hex).c_str());

                this->push_command_(SmCommand{.cmd = command, .payload = message_hex, .raw = true, .priority_cmd = true});
              }

              return;
            }
#endif
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
        this->send_command_(SmCommandType::SET_ENERGY_PURCHASE, state);

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
        this->send_command_(SmCommandType::SET_ENERGY_PURCHASE, false);
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

        UPDATE_SENSOR_FORCE(contract_total_energy, this->data_.starting_kWh + this->data_.total_energy)
        UPDATE_NUMBER_FORCE(starting_kWh, this->data_.starting_kWh)
        break;
      }
      case SmIdEntity::NUMBER_PRICE_KWH: {
        uint32_t price_kWh = std::round(value * 10);

        this->data_.price_kWh = price_kWh * 0.1;

        this->save_initial_number_value_(this->preference_price_kWh_, price_kWh);

        UPDATE_SENSOR_FORCE(total_energy_price, this->data_.total_energy * this->data_.price_kWh)
        UPDATE_SENSOR_FORCE(energy_purchase_price, this->data_.energy_purchase_balance * this->data_.price_kWh)

        UPDATE_NUMBER_FORCE(price_kWh, this->data_.price_kWh)
        UPDATE_SENSOR_FORCE(price_kWh, this->data_.price_kWh)
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
    case SmErrorCode::WRONG_BYTES_VERSION_MESSAGE:
      string_code = SM_STR_CODE_WRONG_BYTES_VERSION_MESSAGE;
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

std::string Dxs238xwComponent::base64_encode_(const uint8_t *buf, size_t bufLen) {
  std::string ret;
  int i = 0;
  int j = 0;
  uint8_t char_array_3[3];
  uint8_t char_array_4[4];

  while (bufLen--) {
    char_array_3[i++] = *(buf++);

    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xFC) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xF0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0F) << 2) + ((char_array_3[2] & 0xC0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3F;

      for (i = 0; (i < 4); i++) {
        ret += base64_chars[char_array_4[i]];
      }

      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 3; j++) {
      char_array_3[j] = '\0';
    }

    char_array_4[0] = (char_array_3[0] & 0xFC) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xF0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0F) << 2) + ((char_array_3[2] & 0xC0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3F;

    for (j = 0; (j < i + 1); j++) {
      ret += base64_chars[char_array_4[j]];
    }

    while ((i++ < 3)) {
      ret += '=';
    }
  }

  return ret;
}

std::vector<uint8_t> Dxs238xwComponent::base64_decode_(const std::string &encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  uint8_t char_array_4[4], char_array_3[3];
  std::vector<uint8_t> ret;

  while (in_len-- && (encoded_string[in_] != '=') && (isalnum(encoded_string[in_]) || (encoded_string[in_] == '+') || (encoded_string[in_] == '/'))) {
    char_array_4[i++] = encoded_string[in_];
    in_++;

    if (i == 4) {
      for (i = 0; i < 4; i++) {
        char_array_4[i] = base64_chars.find(char_array_4[i]);
      }

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xF) << 4) + ((char_array_4[2] & 0x3C) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++) {
        ret.push_back(char_array_3[i]);
      }

      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++) {
      char_array_4[j] = 0;
    }

    for (j = 0; j < 4; j++) {
      char_array_4[j] = base64_chars.find(char_array_4[j]);
    }

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xF) << 4) + ((char_array_4[2] & 0x3C) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++) {
      ret.push_back(char_array_3[j]);
    }
  }

  return ret;
}

}  // namespace dxs238xw
}  // namespace esphome

   /*
       std::vector<uint8_t> myData;
       myData.push_back(0x25);
       myData.push_back(0x35);
       myData.push_back(0x45);
       myData.push_back(0x55);
   
       std::string encodedData = this->base64_encode_(&myData[0], myData.size());
       std::vector<uint8_t> decodedData = this->base64_decode_(encodedData);
   */