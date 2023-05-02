#include "dxs238xw.h"
#include "esphome/components/network/util.h"
#include "esphome/core/util.h"

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
#else
#define UPDATE_SENSOR(name, value)
#define UPDATE_SENSOR_FORCE(name, value)
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

  if (this->postpone_setup_time_ > millis()) {
    this->component_state_ &= ~COMPONENT_STATE_MASK;
    this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;

    this->load_preferences = true;
  } else {
    if (this->load_preferences) {
      ESP_LOGI(TAG, "In --- setup");

      ESP_LOGI(TAG, "* Get Initial Values");
      LOAD_PREFERENCE_MS(delay_value_set, SM_STR_DELAY_VALUE_SET, SmLimitValue::MAX_DELAY_SET)
      LOAD_PREFERENCE_MS(starting_kWh, SM_STR_STARTING_KWH, SmLimitValue::MIN_STARTING_KWH)
      LOAD_PREFERENCE_MS(price_kWh, SM_STR_PRICE_KWH, SmLimitValue::MIN_PRICE_KWH)
      LOAD_PREFERENCE_LP(energy_purchase_value, SM_STR_ENERGY_PURCHASE_VALUE, SmLimitValue::MIN_ENERGY_PURCHASE_VALUE)
      LOAD_PREFERENCE_LP(energy_purchase_alarm, SM_STR_ENERGY_PURCHASE_ALARM, SmLimitValue::MIN_ENERGY_PURCHASE_ALARM)

      UPDATE_NUMBER(delay_value_set, this->ms_data_.delay_value_set)
      UPDATE_NUMBER(starting_kWh, this->ms_data_.starting_kWh * 0.1)
      UPDATE_NUMBER(price_kWh, this->ms_data_.price_kWh * 0.1)
      UPDATE_SENSOR(price_kWh, this->ms_data_.price_kWh * 0.1)
      UPDATE_NUMBER(energy_purchase_value, this->lp_data_.energy_purchase_value)
      UPDATE_NUMBER(energy_purchase_alarm, this->lp_data_.energy_purchase_alarm)

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
        this->send_command_(SmCommandSend::GET_POWER_STATE);
        this->send_command_(SmCommandSend::GET_LIMIT_AND_PURCHASE_DATA);
      }
    });
#endif

#ifdef USE_PROTOCOL_TUYA
    this->set_interval("heartbeat", 15000, [this] { this->push_command_(SmCommand{.cmd = CommandType::HEARTBEAT, .payload = EMPTY_DATA}); });
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
    this->send_command_(SmCommandSend::GET_MEASUREMENT_DATA);
  }
#endif
}

void Dxs238xwComponent::dump_config() {
  LOG_UPDATE_INTERVAL(this)
  ESP_LOGCONFIG(TAG, "*** COMPONENT VERSION: %s ***", SM_STR_COMPONENT_VERSION);
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

bool Dxs238xwComponent::first_data_acquisition_() {
  switch (this->index_first_command) {
#ifdef USE_PROTOCOL_HEKR
    case 0:
      ESP_LOGI(TAG, "* Try to load GET_METER_ID");
      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_GET_METER_ID, .payload = EMPTY_DATA});

      break;
    case 1:
      ESP_LOGI(TAG, "* Try to load GET_POWER_STATE");
      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_GET_METER_STATE, .payload = EMPTY_DATA});

      break;
    case 2:
      ESP_LOGI(TAG, "* Try to load GET_LIMIT_AND_PURCHASE_DATA");
      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_GET_LIMIT_AND_PURCHASE, .payload = EMPTY_DATA});

      break;
    case 3:
      ESP_LOGI(TAG, "* Try to load GET_MEASUREMENT_DATA");
      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_GET_MEASUREMENT, .payload = EMPTY_DATA});

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
    } else {
      this->last_rx_char_timestamp_ = millis();
    }
  }
}

bool Dxs238xwComponent::validate_message_() {
  this->error_type_ = SmErrorType::NO_ERROR;
  this->error_code_ = SmErrorCode::NO_ERROR;

  // this->print_error_();

  // ESP_LOGV(TAG, "* Waiting answer:");
  // ESP_LOGV(TAG, "* Successful answer");
  // ESP_LOGV(TAG, "* Failed answer");

  uint32_t at = this->rx_message_.size() - 1;
  auto *data = &this->rx_message_[0];
  uint8_t new_byte = data[at];

#ifdef USE_PROTOCOL_HEKR

  // Byte 0: HEADER1 (always 0x55)
  if (at == 0) {
    return (new_byte == HEKR_HEADER);
  }

  uint8_t length = data[1];
  if (at == 1) {
    return true;
  }

  if (at == 2) {
    if (this->expected_confirmation_.has_value()) {
      return (new_byte == HEKR_TYPE_SEND);
    } else {
      return (new_byte == HEKR_TYPE_RECEIVE);
    }
  }

  if (at == 3) {
    return true;
  }

  uint8_t command = data[4];
  if (at == 4) {
    return true;
  }

  // wait until all data is read
  if (at < (length - 1)) {
    return true;
  }

  uint8_t rx_checksum = new_byte;
  uint8_t calc_checksum = 0;

  for (uint8_t i = 0; i < at; i++) {
    calc_checksum += data[i];
  }

  if (rx_checksum != calc_checksum) {
    ESP_LOGW(TAG, "Herk Received invalid message checksum %02X!=%02X", rx_checksum, calc_checksum);

    return false;
  }

  // valid message
  const uint8_t *message_data = data;
  // const uint8_t *message_data = data + 5;

  if (!this->expected_confirmation_.has_value() && !this->expected_response_.has_value()) {
    ESP_LOGD(TAG, "Received Hekr: DATA=[%s]", format_hex_pretty(message_data, length).c_str());
  }

  this->handle_command_(command, message_data, length - 1);
  // this->handle_command_(command, message_data, length - 6);

#endif

#ifdef USE_PROTOCOL_TUYA

  // Byte 0: HEADER1 (always 0x55)
  if (at == 0) {
    return (new_byte == 0x55);
  }

  // Byte 1: HEADER2 (always 0xAA)
  if (at == 1) {
    return (new_byte == 0xAA);
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

  // Byte 6+LEN: CHECKSUM - sum of all bytes (including header) modulo 256
  uint8_t rx_checksum = new_byte;
  uint8_t calc_checksum = 0;

  for (uint32_t i = 0; i < 6 + length; i++) {
    calc_checksum += data[i];
  }

  if (rx_checksum != calc_checksum) {
    ESP_LOGW(TAG, "Tuya Received invalid message checksum %02X!=%02X", rx_checksum, calc_checksum);

    return false;
  }

  // valid message
  const uint8_t *message_data = data + 6;

  ESP_LOGI(TAG, "Incoming message");
  ESP_LOGV(TAG, "Received Tuya: CMD=0x%02X VERSION=%u DATA=[%s] INIT_STATE=%u", command, version, format_hex_pretty(message_data, length).c_str(), static_cast<uint8_t>(this->init_state_));

  this->handle_command_(command, version, message_data, length);

#endif

  // return false to reset rx buffer
  return false;
}

#ifdef USE_PROTOCOL_HEKR

void Dxs238xwComponent::handle_command_(uint8_t command, const uint8_t *buffer, size_t len) {
  if (this->expected_confirmation_.has_value() && (this->expected_confirmation_ == (CommandType) command)) {
    ESP_LOGV(TAG, "Confirmacion recibida");

    this->expected_confirmation_.reset();

    switch ((CommandType) command) {
      case CommandType::HEKR_CMD_SEND_GET_METER_STATE:
      case CommandType::HEKR_CMD_SEND_SET_METER_STATE:
      case CommandType::HEKR_CMD_SEND_SET_DELAY:
        this->expected_response_ = ResponseType::HEKR_CMD_RECEIVE_METER_STATE;
        break;
      case CommandType::HEKR_CMD_SEND_GET_MEASUREMENT:
      case CommandType::HEKR_CMD_SEND_SET_RESET:
        this->expected_response_ = ResponseType::HEKR_CMD_RECEIVE_MEASUREMENT;
        break;
      case CommandType::HEKR_CMD_SEND_GET_LIMIT_AND_PURCHASE:
      case CommandType::HEKR_CMD_SEND_SET_LIMIT:
      case CommandType::HEKR_CMD_SEND_SET_PURCHASE:
        this->expected_response_ = ResponseType::HEKR_CMD_RECEIVE_LIMIT_AND_PURCHASE;
        break;
      case CommandType::HEKR_CMD_SEND_GET_METER_ID:
        this->expected_response_ = ResponseType::HEKR_CMD_RECEIVE_METER_ID;
        break;
      default:
        break;
    }

    return;
  } else if (this->expected_response_.has_value() && (this->expected_response_ == (ResponseType) command)) {
    ESP_LOGV(TAG, "Respuesta recibida");

    this->expected_response_.reset();

    bool is_null_old_response = this->command_queue_.front().null_old_response;

    // comprobamos si no se declaro el mensaje nulo
    if (is_null_old_response) {
      ESP_LOGD(TAG, "Anulamos respuesta recibida");

      // procesamos otra vez el mensaje con los datos actualizados.
      this->command_queue_.front().null_old_response = false;

      return;
    } else {
      // Liberamos el mensaje retenido y lo borramos ya que si resivio respuesta
      this->command_queue_.erase(command_queue_.begin());
    }
  }

  this->process_and_update_data_(command, buffer);
}

void Dxs238xwComponent::process_and_update_data_(uint8_t command, const uint8_t *buffer) {
  ResponseType command_type = (ResponseType) command;

  switch (command_type) {
    case ResponseType::HEKR_CMD_RECEIVE_METER_STATE: {
      ESP_LOGV(TAG, "HEKR_CMD_RECEIVE_METER_STATE");

      this->ms_data_.phase_count = buffer[5];
      this->ms_data_.meter_state = buffer[6];
      this->ms_data_.delay_state = buffer[18];
      this->ms_data_.delay_value_remaining = (buffer[16] << 8) | buffer[17];

      this->ms_data_.total_energy = ((buffer[7] << 24) | (buffer[8] << 16) | (buffer[9] << 8) | buffer[10]) * 0.01;

      if (this->ms_data_.meter_state) {
        this->ms_data_.warning_off_by_over_voltage = false;
        this->ms_data_.warning_off_by_under_voltage = false;
        this->ms_data_.warning_off_by_over_current = false;
        this->ms_data_.warning_off_by_end_purchase = false;
        this->ms_data_.warning_off_by_end_delay = false;
        this->ms_data_.warning_off_by_user = false;
      }

      if (!this->ms_data_.warning_off_by_over_voltage) {
        this->ms_data_.warning_off_by_over_voltage = (buffer[11] == 1);
      }

      if (!this->ms_data_.warning_off_by_under_voltage) {
        this->ms_data_.warning_off_by_under_voltage = (buffer[11] == 2);
      }

      if (!this->ms_data_.warning_off_by_over_current) {
        this->ms_data_.warning_off_by_over_current = buffer[15];
      }

      if (!this->ms_data_.warning_off_by_end_purchase) {
        this->ms_data_.warning_off_by_end_purchase = (buffer[1] == 21 ? buffer[19] : false);
      }

      if (!this->ms_data_.warning_off_by_end_delay && this->get_component_state() == COMPONENT_STATE_LOOP) {
        this->ms_data_.warning_off_by_end_delay = (this->ms_data_.delay_value_remaining == 0 && this->ms_data_.delay_state);
      }

      if (this->ms_data_.warning_off_by_end_delay && this->ms_data_.meter_state) {
        ESP_LOGD(TAG, "* End Delay, trying to set Power State to off");
        // REVISAR COMO NO PROCESAR LOS DATOS
        if (this->send_command_(SmCommandSend::SET_POWER_STATE, false)) {
          this->ms_data_.meter_state = buffer[6];
        }

        ESP_LOGD(TAG, "* New Power State = %s", ONOFF(this->ms_data_.meter_state));
      }

      if ((this->ms_data_.warning_off_by_end_delay && this->ms_data_.delay_state) || (this->get_component_state() == COMPONENT_STATE_SETUP && this->ms_data_.delay_state)) {
        ESP_LOGD(TAG, "* End Delay, trying to set Delay State to off");
        // REVISAR COMO NO PROCESAR LOS DATOS
        if (this->send_command_(SmCommandSend::SET_DELAY, false)) {
          this->ms_data_.delay_state = buffer[18];
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

      UPDATE_SENSOR(contract_total_energy, (this->ms_data_.starting_kWh * 0.1) + this->ms_data_.total_energy)
      UPDATE_SENSOR(total_energy_price, (this->ms_data_.price_kWh * 0.1) * this->ms_data_.total_energy)

      UPDATE_SWITCH(meter_state, this->ms_data_.meter_state)
      UPDATE_SWITCH(delay_state, this->ms_data_.delay_state)

      this->update_meter_state_detail_();

      break;
    }
    case ResponseType::HEKR_CMD_RECEIVE_MEASUREMENT: {
      ESP_LOGV(TAG, "HEKR_CMD_RECEIVE_MEASUREMENT");

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
      ESP_LOGV(TAG, "HEKR_CMD_RECEIVE_LIMIT_AND_PURCHASE");

      this->lp_data_.max_voltage_limit = (buffer[5] << 8) | buffer[6];
      this->lp_data_.min_voltage_limit = (buffer[7] << 8) | buffer[8];
      this->lp_data_.max_current_limit = ((buffer[9] << 8) | buffer[10]) * 0.01;

      UPDATE_NUMBER(max_voltage_limit, this->lp_data_.max_voltage_limit)
      UPDATE_NUMBER(min_voltage_limit, this->lp_data_.min_voltage_limit)
      UPDATE_NUMBER(max_current_limit, this->lp_data_.max_current_limit)

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      if (buffer[1] == 25) {
        this->lp_data_.energy_purchase_state = buffer[23];

        // this->lp_data_.energy_purchase_value = (((buffer[11] << 24) | (buffer[12] << 16) | (buffer[13] << 8) | buffer[14]) * 0.01);
        // this->lp_data_.energy_purchase_alarm = (((buffer[19] << 24) | (buffer[20] << 16) | (buffer[21] << 8) | buffer[22]) * 0.01);
        this->lp_data_.energy_purchase_balance = (((buffer[15] << 24) | (buffer[16] << 16) | (buffer[17] << 8) | buffer[18]) * 0.01);

        this->ms_data_.warning_purchase_alarm = ((this->lp_data_.energy_purchase_balance <= this->lp_data_.energy_purchase_alarm) && this->lp_data_.energy_purchase_state);
      }

      UPDATE_SENSOR(energy_purchase_balance, this->lp_data_.energy_purchase_balance)
      UPDATE_SENSOR(energy_purchase_price, this->lp_data_.energy_purchase_balance * (this->ms_data_.price_kWh * 0.1))

      UPDATE_SWITCH(energy_purchase_state, this->lp_data_.energy_purchase_state)

      UPDATE_BINARY_SENSOR(warning_purchase_alarm, this->ms_data_.warning_purchase_alarm)

      break;
    }
    case ResponseType::HEKR_CMD_RECEIVE_METER_ID: {
      ESP_LOGV(TAG, "HEKR_CMD_RECEIVE_METER_ID");

      char serial_number[20];

      sprintf(serial_number, "%02u%02u%02u %02u%02u%02u", buffer[5], buffer[6], buffer[7], buffer[8], buffer[9], buffer[10]);
      std::string string_serial_number(serial_number);

      ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      UPDATE_TEXT_SENSOR(meter_id, string_serial_number)

      break;
    }
    default:
      ESP_LOGE(TAG, "Invalid command (0x%02X) received", command);
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
  }

  switch (command_type) {
    case CommandType::HEARTBEAT:
      ESP_LOGV(TAG, "MCU Heartbeat (0x%02X)", buffer[0]);

      this->protocol_version_ = version;

      if (buffer[0] == 0) {
        ESP_LOGI(TAG, "MCU restarted");

        this->init_state_ = TuyaInitState::INIT_HEARTBEAT;
      }

      if (this->init_state_ == TuyaInitState::INIT_HEARTBEAT) {
        this->init_state_ = TuyaInitState::INIT_PRODUCT;

        this->push_command_(SmCommand{.cmd = CommandType::PRODUCT_QUERY, .payload = EMPTY_DATA});
      }

      break;
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
        this->push_command_(SmCommand{.cmd = CommandType::CONF_QUERY, .payload = EMPTY_DATA});
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
          this->push_command_(SmCommand{.cmd = CommandType::DATAPOINT_QUERY, .payload = EMPTY_DATA});

          bool is_pin_equals = this->status_pin_.has_value() && (this->status_pin_.value()->get_pin() == this->status_pin_reported_);

          // Configure status pin toggling (if reported and configured) or WIFI_STATE periodic send
          if (is_pin_equals) {
            ESP_LOGV(TAG, "Configured status pin %i", this->status_pin_reported_);

            this->set_interval("wifi", 1000, [this] {
              bool is_network_ready = network::is_connected() && remote_is_connected();
              this->status_pin_.value()->digital_write(is_network_ready);
            });
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
    case CommandType::WIFI_STATE:
      if (this->init_state_ == TuyaInitState::INIT_WIFI) {
        this->init_state_ = TuyaInitState::INIT_DATAPOINT;
        this->push_command_(SmCommand{.cmd = CommandType::DATAPOINT_QUERY, .payload = EMPTY_DATA});
      }

      break;
    case CommandType::WIFI_RESET:
      ESP_LOGE(TAG, "WIFI_RESET is not handled");

      break;
    case CommandType::WIFI_SELECT:
      ESP_LOGE(TAG, "WIFI_SELECT is not handled");

      break;
    case CommandType::DATAPOINT_DELIVER:
      break;
    case CommandType::DATAPOINT_REPORT:
      if (this->init_state_ == TuyaInitState::INIT_DATAPOINT) {
        this->init_state_ = TuyaInitState::INIT_DONE;

        this->set_timeout("datapoint_dump", 1000, [this] { this->dump_config(); });
      }

      this->process_and_update_data_(buffer, len);

      break;
    case CommandType::DATAPOINT_QUERY:
      break;
    case CommandType::WIFI_TEST:
      this->command_queue_.push_back(SmCommand{.cmd = CommandType::WIFI_TEST, .payload = std::vector<uint8_t>{0x00, 0x00}});

      break;
    case CommandType::WIFI_RSSI:
      this->command_queue_.push_back(SmCommand{.cmd = CommandType::WIFI_RSSI, .payload = std::vector<uint8_t>{0x00}});

      break;
    case CommandType::LOCAL_TIME_QUERY:
      ESP_LOGE(TAG, "LOCAL_TIME_QUERY is not handled");

      break;
    case CommandType::VACUUM_MAP_UPLOAD:
      this->command_queue_.push_back(SmCommand{.cmd = CommandType::VACUUM_MAP_UPLOAD, .payload = std::vector<uint8_t>{0x01}});

      ESP_LOGW(TAG, "Vacuum map upload requested, responding that it is not enabled.");

      break;
    case CommandType::GET_NETWORK_STATUS: {
      uint8_t wifi_status = this->get_wifi_status_code_();

      this->command_queue_.push_back(SmCommand{.cmd = CommandType::GET_NETWORK_STATUS, .payload = std::vector<uint8_t>{wifi_status}});

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
    datapoint.id = buffer[0];
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

    switch (datapoint.id) {
      case DATAPOINT_SWITCH_ENERGY_PURCHASE_STATE:
        ESP_LOGV(TAG, "MCU reported switch %u is: %s", DATAPOINT_SWITCH_ENERGY_PURCHASE_STATE, ONOFF(datapoint.value_bool));

        UPDATE_SWITCH(delay_state, datapoint.value_bool)
        break;
      default:
        return;
    }
  }
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

  ESP_LOGCONFIG(TAG, "* Product: '%s'", this->product_.c_str());
}

void Dxs238xwComponent::send_wifi_status_() {
  uint8_t status = this->get_wifi_status_code_();

  if (status == this->wifi_status_) {
    return;
  }

  ESP_LOGD(TAG, "Sending WiFi Status");
  this->wifi_status_ = status;
  this->command_queue_.push_back(SmCommand{.cmd = CommandType::WIFI_STATE, .payload = std::vector<uint8_t>{status}});
}

uint8_t Dxs238xwComponent::get_wifi_status_code_() {
  uint8_t status = 0x02;

  if (network::is_connected()) {
    status = 0x03;

    // Protocol version 3 also supports specifying when connected to "the cloud"
    if (this->protocol_version_ >= 0x03 && remote_is_connected()) {
      status = 0x04;
    }
  }

  return status;
}

void Dxs238xwComponent::set_boolean_datapoint_value_(uint8_t datapoint_id, bool value) {
  //
  this->set_numeric_datapoint_value_(datapoint_id, TuyaDatapointType::BOOLEAN, value, 1);
}

void Dxs238xwComponent::set_integer_datapoint_value_(uint8_t datapoint_id, uint32_t value) {
  //
  this->set_numeric_datapoint_value_(datapoint_id, TuyaDatapointType::INTEGER, value, 4);
}

void Dxs238xwComponent::set_numeric_datapoint_value_(uint8_t datapoint_id, TuyaDatapointType datapoint_type, const uint32_t value, uint8_t length) {
  ESP_LOGD(TAG, "Setting datapoint %u to %u", datapoint_id, value);

  optional<TuyaDatapoint> datapoint = {};

  for (auto &datapoint_index : this->datapoints_) {
    if (datapoint_index.id == datapoint_id) {
      datapoint = datapoint_index;
    }
  }

  if (!datapoint.has_value()) {
    ESP_LOGW(TAG, "Setting unknown datapoint %u", datapoint_id);
  } else if (datapoint->type != datapoint_type) {
    ESP_LOGE(TAG, "Attempt to set datapoint %u with incorrect type", datapoint_id);

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

void Dxs238xwComponent::send_datapoint_command_(uint8_t datapoint_id, TuyaDatapointType datapoint_type, std::vector<uint8_t> data) {
  std::vector<uint8_t> buffer;

  buffer.push_back(datapoint_id);
  buffer.push_back(static_cast<uint8_t>(datapoint_type));
  buffer.push_back(data.size() >> 8);
  buffer.push_back(data.size() >> 0);

  buffer.insert(buffer.end(), data.begin(), data.end());

  this->command_queue_.push_back(SmCommand{.cmd = CommandType::DATAPOINT_DELIVER, .payload = buffer});
}

#endif

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::process_command_queue_() {
  uint32_t now = millis();

  // Limpiamos el buffer de rx si ya paso mucho tiempo y no llego un mensaje valido
  if ((this->last_rx_char_timestamp_ != 0) && (now - this->last_rx_char_timestamp_) > SM_MAX_MILLIS_TO_RX_BYTE) {
    ESP_LOGD(TAG, "Se agoto tiempo de espera para un nuevo caracter");

    this->rx_message_.clear();
  }

  // verificamos si no hemos superado el tiempo de espera para resivir una respuesta
  // si superamos el tiempo podemos seguir procesando mensajes en cola
  if (this->is_message_timeout_()) {
    ESP_LOGD(TAG, "Se agoto el tiempo de espera para confirmacion o respuesta");

    if (this->get_component_state() == COMPONENT_STATE_SETUP) {
      ESP_LOGD(TAG, "No llego respuesta del MCU");

      this->status_set_error();

      this->count_error_data_acquisition_++;
      this->postpone_setup_time_ = 0;
    } else {
      // Borramos el mensaje retenido esperando respuesta ya que nunca resivio respuesta
      this->command_queue_.erase(this->command_queue_.begin());
    }
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
  ESP_LOGV(TAG, "Procesando comando en cola");

  this->last_command_timestamp_ = millis();

  std::vector<uint8_t> buffer;

#ifdef USE_PROTOCOL_HEKR
  this->expected_confirmation_ = command.cmd;

  buffer.push_back(HEKR_HEADER);
  buffer.push_back(5 + command.payload.size() + 1);
  buffer.push_back(HEKR_TYPE_SEND);
  buffer.push_back(this->version_msg++);
  buffer.push_back((uint8_t) command.cmd);
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

  ESP_LOGV(TAG, "* Message send: %s", format_hex_pretty(buffer).c_str());

  this->write_array(buffer);

#ifdef USE_PROTOCOL_HEKR
  ESP_LOGV(TAG, "* Waiting to confirmation:");
#endif

#ifdef USE_PROTOCOL_TUYA
  if (!this->expected_response_.has_value()) {
    ESP_LOGV(TAG, "* Waiting to response:");
  }
#endif
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

bool Dxs238xwComponent::is_expected_message() {
#ifdef USE_PROTOCOL_HEKR
  return (this->expected_confirmation_.has_value() || this->expected_response_.has_value());
#endif

#ifdef USE_PROTOCOL_TUYA
  return (this->expected_response_.has_value());
#endif
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

void Dxs238xwComponent::push_command_(const SmCommand &command, bool push_back) {
  uint8_t index = 0;

  // Buscamos si es que ya existe el mensaje
  for (auto &cmd : this->command_queue_) {
    if (cmd.cmd == command.cmd) {
      if (index == 0) {
        if (this->is_expected_message()) {
          // comprobamos si se estan adjuntando datos, si no descartamos el mensaje
          if (!command.payload.empty()) {
            cmd = command;

            cmd.null_old_response = true;
          }
        } else {
          cmd = command;
        }
      } else {
        cmd = command;
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

bool Dxs238xwComponent::send_command_(SmCommandSend cmd, bool state) {
  switch (cmd) {
    case SmCommandSend::GET_POWER_STATE: {
      ESP_LOGV(TAG, "Push Command - GET_POWER_STATE");

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_GET_METER_STATE, .payload = EMPTY_DATA});

      break;
    }
    case SmCommandSend::GET_MEASUREMENT_DATA: {
      ESP_LOGV(TAG, "Push Command - GET_MEASUREMENT_DATA");

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_GET_MEASUREMENT, .payload = EMPTY_DATA});

      break;
    }
    case SmCommandSend::GET_LIMIT_AND_PURCHASE_DATA: {
      ESP_LOGV(TAG, "Push Command - GET_LIMIT_AND_PURCHASE_DATA");

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_GET_LIMIT_AND_PURCHASE, .payload = EMPTY_DATA});

      break;
    }
    case SmCommandSend::GET_METER_ID: {
      ESP_LOGD(TAG, "Push Command - GET_METER_ID");

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_GET_METER_ID, .payload = EMPTY_DATA});

      break;
    }
    case SmCommandSend::SET_LIMIT_DATA: {
      ESP_LOGD(TAG, "Push Command - SET_LIMIT_DATA");

      uint16_t tmp_current_limit = this->lp_data_.max_current_limit * 100;

      std::vector<uint8_t> buffer;

      buffer.push_back(tmp_current_limit >> 8);
      buffer.push_back(tmp_current_limit & SM_GET_ONE_BYTE);

      buffer.push_back(this->lp_data_.max_voltage_limit >> 8);
      buffer.push_back(this->lp_data_.max_voltage_limit & SM_GET_ONE_BYTE);

      buffer.push_back(this->lp_data_.min_voltage_limit >> 8);
      buffer.push_back(this->lp_data_.min_voltage_limit & SM_GET_ONE_BYTE);

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_SET_LIMIT, .payload = buffer}, false);

      ESP_LOGD(TAG, "* Input Data: max_current_limit = %u, max_voltage_limit = %u, min_voltage_limit = %u", this->lp_data_.max_current_limit, this->lp_data_.max_voltage_limit, this->lp_data_.min_voltage_limit);

      break;
    }
    case SmCommandSend::SET_PURCHASE_DATA: {
      ESP_LOGD(TAG, "Push Command - SET_PURCHASE_DATA");

      uint32_t purchase_value = 0;
      uint32_t purchase_alarm = 0;

      if (state) {
        purchase_value = this->lp_data_.energy_purchase_value * 100;
        purchase_alarm = this->lp_data_.energy_purchase_alarm * 100;
      }

      std::vector<uint8_t> buffer;

      buffer.push_back(purchase_value >> 24);
      buffer.push_back(purchase_value >> 16);
      buffer.push_back(purchase_value >> 8);
      buffer.push_back(purchase_value & SM_GET_ONE_BYTE);

      buffer.push_back(purchase_alarm >> 24);
      buffer.push_back(purchase_alarm >> 16);
      buffer.push_back(purchase_alarm >> 8);
      buffer.push_back(purchase_alarm & SM_GET_ONE_BYTE);

      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_SET_PURCHASE, .payload = buffer}, false);

      ESP_LOGD(TAG, "* Input Data: purchase_value = %u, purchase_alarm = %u, state = %s", (state ? this->lp_data_.energy_purchase_value : 0), (state ? this->lp_data_.energy_purchase_alarm : 0), ONOFF(state));

      break;
    }
    case SmCommandSend::SET_POWER_STATE: {
      ESP_LOGD(TAG, "Push Command - SET_POWER_STATE");

      std::vector<uint8_t> buffer;

      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_SET_METER_STATE, .payload = buffer}, false);

      ESP_LOGD(TAG, "* Input Data: state = %s", ONOFF(state));

      break;
    }
    case SmCommandSend::SET_DELAY: {
      ESP_LOGD(TAG, "Push Command - SET_DELAY");

      uint16_t delay_value_set = 0;

      if (state) {
        delay_value_set = this->ms_data_.delay_value_set;
      }

      std::vector<uint8_t> buffer;

      buffer.push_back(delay_value_set >> 8);
      buffer.push_back(delay_value_set & SM_GET_ONE_BYTE);

      buffer.push_back(state);

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_SET_DELAY, .payload = buffer}, false);

      ESP_LOGD(TAG, "* Input Data: delay_value_set = %u, state = %s", delay_value_set, ONOFF(state));

      break;
    }
    case SmCommandSend::SET_RESET: {
      ESP_LOGD(TAG, "Push Command - SET_RESET");

      this->push_command_(SmCommand{.cmd = CommandType::HEKR_CMD_SEND_SET_RESET, .payload = EMPTY_DATA}, false);

      break;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

void Dxs238xwComponent::process_switch_state(SmIdEntity entity, bool state) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    switch (entity) {
      case SmIdEntity::SWITCH_ENERGY_PURCHASE_STATE: {
        this->send_command_(SmCommandSend::SET_PURCHASE_DATA, state);

        UPDATE_SWITCH_FORCE(energy_purchase_state, state)
        break;
      }
      case SmIdEntity::SWITCH_METER_STATE: {
        this->ms_data_.warning_off_by_user = !state;

        this->send_command_(SmCommandSend::SET_POWER_STATE, state);

        UPDATE_SWITCH_FORCE(meter_state, state)

        break;
      }
      case SmIdEntity::SWITCH_DELAY_STATE: {
        this->send_command_(SmCommandSend::SET_DELAY, state);

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

void Dxs238xwComponent::process_number_value(SmIdEntity entity, float value) {
  if (this->get_component_state() == COMPONENT_STATE_LOOP) {
    switch (entity) {
      case SmIdEntity::NUMBER_MAX_CURRENT_LIMIT: {
        uint32_t tmp_value = std::round(value);

        this->lp_data_.max_current_limit = tmp_value;

        this->send_command_(SmCommandSend::SET_LIMIT_DATA);

        UPDATE_NUMBER_FORCE(max_current_limit, this->lp_data_.max_current_limit)
        break;
      }
      case SmIdEntity::NUMBER_MAX_VOLTAGE_LIMIT: {
        uint32_t tmp_value = std::round(value);

        if (tmp_value > this->lp_data_.min_voltage_limit) {
          this->lp_data_.max_voltage_limit = tmp_value;

          this->send_command_(SmCommandSend::SET_LIMIT_DATA);
        } else {
          ESP_LOGW(TAG, "max_voltage_limit - Value %u must not be less than min_voltage_limit %u", tmp_value, this->lp_data_.min_voltage_limit);
        }

        UPDATE_NUMBER_FORCE(max_voltage_limit, this->lp_data_.max_voltage_limit)
        break;
      }
      case SmIdEntity::NUMBER_MIN_VOLTAGE_LIMIT: {
        uint32_t tmp_value = std::round(value);

        if (tmp_value < this->lp_data_.max_voltage_limit) {
          this->lp_data_.min_voltage_limit = tmp_value;

          this->send_command_(SmCommandSend::SET_LIMIT_DATA);
        } else {
          ESP_LOGW(TAG, "min_voltage_limit - Value %u must not be greater than max_voltage_limit %u", tmp_value, this->lp_data_.max_voltage_limit);
        }

        UPDATE_NUMBER_FORCE(min_voltage_limit, this->lp_data_.min_voltage_limit)
        break;
      }
      case SmIdEntity::NUMBER_ENERGY_PURCHASE_VALUE: {
        uint32_t tmp_value = std::round(value);

        this->lp_data_.energy_purchase_value = tmp_value;

        this->save_initial_number_value_(this->preference_energy_purchase_value_, this->lp_data_.energy_purchase_value);

        if (this->lp_data_.energy_purchase_state) {
          if (this->send_command_(SmCommandSend::SET_PURCHASE_DATA, false)) {
            this->send_command_(SmCommandSend::SET_PURCHASE_DATA, true);
          }
        }

        UPDATE_NUMBER_FORCE(energy_purchase_value, this->lp_data_.energy_purchase_value)
        break;
      }
      case SmIdEntity::NUMBER_ENERGY_PURCHASE_ALARM: {
        uint32_t tmp_value = std::round(value);

        this->lp_data_.energy_purchase_alarm = tmp_value;

        this->save_initial_number_value_(this->preference_energy_purchase_alarm_, this->lp_data_.energy_purchase_alarm);

        if (this->lp_data_.energy_purchase_state) {
          if (this->send_command_(SmCommandSend::SET_PURCHASE_DATA, false)) {
            this->send_command_(SmCommandSend::SET_PURCHASE_DATA, true);
          }
        }

        UPDATE_NUMBER_FORCE(energy_purchase_alarm, this->lp_data_.energy_purchase_alarm)
        break;
      }
      case SmIdEntity::NUMBER_DELAY_VALUE_SET: {
        uint32_t tmp_value = std::round(value);

        this->ms_data_.delay_value_set = tmp_value;

        this->save_initial_number_value_(this->preference_delay_value_set_, this->ms_data_.delay_value_set);

        if (this->ms_data_.delay_state) {
          this->send_command_(SmCommandSend::SET_DELAY, true);
        }

        UPDATE_NUMBER_FORCE(delay_value_set, this->ms_data_.delay_value_set)
        break;
      }
      case SmIdEntity::NUMBER_STARTING_KWH: {
        this->ms_data_.starting_kWh = std::round(value * 10);

        this->save_initial_number_value_(this->preference_starting_kWh_, this->ms_data_.starting_kWh);

        UPDATE_SENSOR(contract_total_energy, (this->ms_data_.starting_kWh * 0.1) + this->ms_data_.total_energy)
        UPDATE_NUMBER_FORCE(starting_kWh, this->ms_data_.starting_kWh * 0.1)
        break;
      }
      case SmIdEntity::NUMBER_PRICE_KWH: {
        this->ms_data_.price_kWh = std::round(value * 10);

        this->save_initial_number_value_(this->preference_price_kWh_, this->ms_data_.price_kWh);

        UPDATE_SENSOR(total_energy_price, (this->ms_data_.price_kWh * 0.1) * this->ms_data_.total_energy)

        UPDATE_NUMBER_FORCE(price_kWh, this->ms_data_.price_kWh * 0.1)
        UPDATE_SENSOR(price_kWh, this->ms_data_.price_kWh * 0.1)
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

//------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

#ifdef USE_RAW_MESSAGE
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
          if (this->calculate_crc_(send_array, length_array - 1) != send_array[length_array - 1]) {
            this->error_type_ = SmErrorType::INPUT_DATA;
            this->error_code_ = SmErrorCode::CRC;
          }
        } else {
          send_array[length_array - 1] = this->calculate_crc_(send_array, length_array - 1);
        }

        if (this->error_code_ == SmErrorCode::NO_ERROR) {
          ESP_LOGD(TAG, "* Message send: %s", format_hex_pretty(send_array, length_array).c_str());

          if (this->transmit_serial_data_(send_array, length_array)) {
            ESP_LOGD(TAG, "* Waiting answer:");

            if (this->receive_serial_data_(this->receive_array_, HEKR_TYPE_RECEIVE)) {
              ESP_LOGD(TAG, "* Successful answer: %s", format_hex_pretty(this->receive_array_, this->receive_array_[1]).c_str());

              // this->process_and_update_data_(this->receive_array_[4], this->receive_array_);

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

            if (this->calculate_crc_(array, array[1] - 1) != array[index_size]) {
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
#endif

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

}  // namespace dxs238xw
}  // namespace esphome
