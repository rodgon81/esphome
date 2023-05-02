#pragma once

#include "esphome/components/uart/uart.h"
#include "esphome/core/application.h"
#include "esphome/components/network/util.h"
#include "esphome/core/util.h"

namespace esphome {
namespace dxs238xw {

// #define USE_PROTOCOL_HEKR
// #define USE_PROTOCOL_TUYA

// #define USE_MODEL_DDS238_2
// #define USE_MODEL_DDS238_4
// #define USE_MODEL_DTS238_7

// #define USE_RAW_MESSAGE

#define EMPTY_DATA \
  std::vector<uint8_t> {}

static const char *const SM_STR_COMPONENT_VERSION = "2.0.0000 beta";

#ifdef USE_MODEL_DDS238_2
static const char *const SM_STR_METER_MODEL = "DDS238_2";
#endif
#ifdef USE_MODEL_DDS238_4
static const char *const SM_STR_METER_MODEL = "DDS238_4";
#endif
#ifdef USE_MODEL_DTS238_7
static const char *const SM_STR_METER_MODEL = "DTS238_7";
#endif

#ifdef USE_PROTOCOL_HEKR
static const char *const SM_STR_PROTOCOL = "Hekr";
#endif

#ifdef USE_PROTOCOL_TUYA
static const char *const SM_STR_PROTOCOL = "Tuya";
#endif

//------------------------------------------------------------------------------
// DEFAULTS
//------------------------------------------------------------------------------

static const uint16_t SM_MIN_INTERVAL_TO_GET_DATA = 500;

#ifdef USE_API
static const uint16_t SM_POSTPONE_SETUP_TIME = 10000;
#else
static const uint16_t SM_POSTPONE_SETUP_TIME = 2500;
#endif

static const uint16_t SM_MAX_MILLIS_TO_RX_BYTE = 50;
static const uint16_t SM_MAX_MILLIS_TO_CONFIRM = 300;  // 200
static const uint16_t SM_MAX_MILLIS_TO_RESPONSE = 1000;

//------------------------------------------------------------------------------

static const uint8_t SM_MAX_BYTE_MSG_BUFFER = 96;
static const uint8_t SM_MAX_HEX_MSG_LENGTH = 255;
static const uint8_t SM_MAX_HEX_MSG_LENGTH_PARSE = 176;

//------------------------------------------------------------------------------

// Header
static const uint8_t HEKR_HEADER = 0x48;
static const uint8_t TUYA_HEADER_1 = 0x55;
static const uint8_t TUYA_HEADER_2 = 0xAA;

// Type message
static const uint8_t HEKR_TYPE_RECEIVE = 0x01;
static const uint8_t HEKR_TYPE_SEND = 0x02;

//------------------------------------------------------------------------------

static const uint8_t SM_GET_ONE_BYTE = 0xFF;

//------------------------------------------------------------------------------

// Error Meter State
static const char *const SM_STR_POWER_STATE_DETAILS_POWER_OK = "Power Ok";
static const char *const SM_STR_POWER_STATE_DETAILS_OVER_VOLTAGE = "Off by Over Voltage";
static const char *const SM_STR_POWER_STATE_DETAILS_UNDER_VOLTAGE = "Off by Under Voltage";
static const char *const SM_STR_POWER_STATE_DETAILS_OVER_CURRENT = "Off by Over Current";
static const char *const SM_STR_POWER_STATE_DETAILS_END_PURCHASE = "Off by End Purchase";
static const char *const SM_STR_POWER_STATE_DETAILS_END_DELAY = "Off by End Delay";
static const char *const SM_STR_POWER_STATE_DETAILS_END_BY_USER = "Off by User";
static const char *const SM_STR_POWER_STATE_DETAILS_UNKNOWN = "Off by Unknown";

// Error Type
static const char *const SM_STR_TYPE_NO_ERROR = "No error type";
static const char *const SM_STR_TYPE_COMMUNICATION_CONFIRMATION = "Communication (Confirmation Message)";
static const char *const SM_STR_TYPE_COMMUNICATION_ANSWER = "Communication (Answer Message)";
static const char *const SM_STR_TYPE_INPUT_DATA = "Input data";

// Error Description
static const char *const SM_STR_CODE_NO_ERROR = "No Errors";

static const char *const SM_STR_CODE_WRONG_BYTES_HEADER = "The bytes was received but are not correct (HEADER)";
static const char *const SM_STR_CODE_WRONG_BYTES_LENGTH = "The bytes was received but are not correct (LENGTH)";
static const char *const SM_STR_CODE_WRONG_BYTES_TYPE_MESSAGE = "The bytes was received but are not correct (TYPE_MESSAGE)";
static const char *const SM_STR_CODE_WRONG_BYTES_COMMAND = "The bytes was received but are not correct (COMMAND)";

static const char *const SM_STR_CODE_CRC = "CRC check failed";

static const char *const SM_STR_CODE_NOT_ENOUGH_BYTES = "Not enough bytes were received";
static const char *const SM_STR_CODE_TIMED_OUT = "Timed out";

static const char *const SM_STR_CODE_WRONG_MSG = "Wrong Message";
static const char *const SM_STR_CODE_MESSAGE_LENGTH = "Incorrect Message length";

// Preference Name
static const char *const SM_STR_DELAY_VALUE_SET = "delay_value_set";
static const char *const SM_STR_STARTING_KWH = "starting_kWh";
static const char *const SM_STR_PRICE_KWH = "price_kWh";
static const char *const SM_STR_ENERGY_PURCHASE_VALUE = "energy_purchase_value";
static const char *const SM_STR_ENERGY_PURCHASE_ALARM = "energy_purchase_alarm";

// Tuya Datapoint
static const uint8_t DATAPOINT_SWITCH_ENERGY_PURCHASE_STATE = 1;
static const uint8_t DATAPOINT_SWITCH_METER_STATE = 2;
static const uint8_t DATAPOINT_SWITCH_DELAY_STATE = 3;

enum class SmErrorMeterStateType : uint8_t {
  POWER_OK,
  OVER_VOLTAGE,
  UNDER_VOLTAGE,
  OVER_CURRENT,
  END_PURCHASE,
  END_DELAY,
  END_BY_USER,
  UNKNOWN,
};

enum class SmIdEntity : uint8_t {
  ID_NULL = 0,

  NUMBER_MAX_CURRENT_LIMIT = 11,
  NUMBER_MAX_VOLTAGE_LIMIT = 12,
  NUMBER_MIN_VOLTAGE_LIMIT = 13,
  NUMBER_ENERGY_PURCHASE_VALUE = 14,
  NUMBER_ENERGY_PURCHASE_ALARM = 15,
  NUMBER_DELAY_VALUE_SET = 16,
  NUMBER_STARTING_KWH = 17,
  NUMBER_PRICE_KWH = 18,

  SWITCH_ENERGY_PURCHASE_STATE = 21,
  SWITCH_METER_STATE = 22,
  SWITCH_DELAY_STATE = 23,

  BUTTON_RESET_DATA = 31,
};

enum class SmCommandSend : uint8_t {
  GET_POWER_STATE,
  GET_MEASUREMENT_DATA,
  GET_LIMIT_AND_PURCHASE_DATA,
  GET_METER_ID,

  SET_LIMIT_DATA,
  SET_PURCHASE_DATA,
  SET_POWER_STATE,
  SET_DELAY,
  SET_RESET,
};

enum class SmErrorType : uint8_t {
  NO_ERROR,
  COMMUNICATION_CONFIRMATION,
  COMMUNICATION_ANSWER,
  INPUT_DATA,
};

enum class SmErrorCode : uint8_t {
  NO_ERROR,
  CRC,
  WRONG_BYTES_HEADER,
  WRONG_BYTES_LENGTH,
  WRONG_BYTES_TYPE_MESSAGE,
  WRONG_BYTES_COMMAND,
  NOT_ENOUGHT_BYTES,
  TIMEOUT,
  WRONG_MSG,
  MESSAGE_LENGTH,
};

enum SmLimitValue : uint32_t {
  MIN_VOLTAGE = 80,                    // V
  MAX_VOLTAGE = 300,                   // V
  STEP_VOLTAGE = 1,                    // Unit
  MIN_CURRENT = 1,                     // A
  MAX_CURRENT = 60,                    // A
  STEP_CURRENT = 1,                    // Unit
  MIN_DELAY_SET = 1,                   // minutes
  MAX_DELAY_SET = 1440,                // minutes
  STEP_DELAY_SET = 1,                  // Unit
  MIN_ENERGY_PURCHASE_ALARM = 0,       // kWh
  MAX_ENERGY_PURCHASE_ALARM = 999999,  // kWh
  STEP_ENERGY_PURCHASE_ALARM = 1,      // Unit
  MIN_ENERGY_PURCHASE_VALUE = 0,       // kWh
  MAX_ENERGY_PURCHASE_VALUE = 999999,  // kWh
  STEP_ENERGY_PURCHASE_VALUE = 1,      // Unit
  MIN_STARTING_KWH = 0,                // kWh
  MAX_STARTING_KWH = 999999,           // kWh
  STEP_STARTING_KWH = 0,               // Unit
  MIN_PRICE_KWH = 0,                   // $
  MAX_PRICE_KWH = 999999,              // $
  STEP_PRICE_KWH = 0,                  // Unit
};

enum class TuyaInitState : uint8_t {
  INIT_HEARTBEAT = 0x00,
  INIT_PRODUCT,
  INIT_CONF,
  INIT_WIFI,
  INIT_DATAPOINT,
  INIT_DONE,
};

#ifdef USE_PROTOCOL_HEKR
enum class ResponseType : uint8_t {
  HEKR_CMD_RECEIVE_METER_STATE = 0x01,
  HEKR_CMD_RECEIVE_MEASUREMENT = 0x0B,
  HEKR_CMD_RECEIVE_LIMIT_AND_PURCHASE = 0x08,
  HEKR_CMD_RECEIVE_METER_ID = 0x07,
};

enum class CommandType : uint8_t {
  HEKR_CMD_SEND_SET_LIMIT = 0x03,
  HEKR_CMD_SEND_SET_PURCHASE = 0x0D,
  HEKR_CMD_SEND_SET_METER_STATE = 0x09,
  HEKR_CMD_SEND_SET_DELAY = 0x0C,
  HEKR_CMD_SEND_SET_RESET = 0x05,
  HEKR_CMD_SEND_GET_METER_STATE = 0x00,
  HEKR_CMD_SEND_GET_MEASUREMENT = 0x0A,
  HEKR_CMD_SEND_GET_LIMIT_AND_PURCHASE = 0x02,
  HEKR_CMD_SEND_GET_METER_ID = 0x06,
};
#endif

#ifdef USE_PROTOCOL_TUYA
enum class CommandType : uint8_t {
  HEARTBEAT = 0x00,
  PRODUCT_QUERY = 0x01,
  CONF_QUERY = 0x02,
  WIFI_STATE = 0x03,
  WIFI_RESET = 0x04,
  WIFI_SELECT = 0x05,
  DATAPOINT_DELIVER = 0x06,
  DATAPOINT_REPORT = 0x07,
  DATAPOINT_QUERY = 0x08,
  WIFI_TEST = 0x0E,
  LOCAL_TIME_QUERY = 0x1C,
  WIFI_RSSI = 0x24,
  VACUUM_MAP_UPLOAD = 0x28,
  GET_NETWORK_STATUS = 0x2B,
};

enum class TuyaDatapointType : uint8_t {
  RAW = 0x00,      // variable length
  BOOLEAN = 0x01,  // 1 byte (0/1)
  INTEGER = 0x02,  // 4 byte
  STRING = 0x03,   // variable length
  ENUM = 0x04,     // 1 byte
  BITMASK = 0x05,  // 1/2/4 bytes
};

struct TuyaDatapoint {
  uint8_t id;
  TuyaDatapointType type;
  size_t len;

  union {
    bool value_bool;
    int value_int;
    uint32_t value_uint;
    uint8_t value_enum;
    uint32_t value_bitmask;
  };

  std::string value_string;
  std::vector<uint8_t> value_raw;
};
#endif

struct SmCommand {
  CommandType cmd;
  std::vector<uint8_t> payload;
  bool null_old_response = false;
};

struct LimitAndPurchaseData {
  uint32_t energy_purchase_value = 0;
  uint32_t energy_purchase_alarm = 0;
  uint32_t energy_purchase_balance = 0;  // old float

  bool energy_purchase_state = false;

  uint8_t max_current_limit = 0;
  uint16_t max_voltage_limit = 0;
  uint16_t min_voltage_limit = 0;
};

struct MeterStateData {
  uint8_t phase_count = 0;

  bool warning_off_by_over_voltage = false;
  bool warning_off_by_under_voltage = false;
  bool warning_off_by_over_current = false;
  bool warning_off_by_end_purchase = false;
  bool warning_off_by_end_delay = false;
  bool warning_off_by_user = false;

  bool warning_purchase_alarm = false;

  bool meter_state = false;
  bool delay_state = false;

  uint32_t starting_kWh = 0;  // old float
  uint32_t price_kWh = 0;     // old float

  float total_energy = 0;

  SmErrorMeterStateType meter_state_detail = SmErrorMeterStateType::UNKNOWN;

  uint16_t delay_value_remaining = 0;
  uint32_t delay_value_set = 0;
};

#ifdef USE_SENSOR
#define DXS238XW_SENSOR(name) \
 protected: \
  sensor::Sensor *name##_sensor_; \
\
 public: \
  void set_##name##_sensor(sensor::Sensor *name##_sensor) { this->name##_sensor_ = name##_sensor; }
#else
#define DXS238XW_SENSOR(name)
#endif

#ifdef USE_BINARY_SENSOR
#define DXS238XW_BINARY_SENSOR(name) \
 protected: \
  binary_sensor::BinarySensor *name##_binary_sensor_; \
\
 public: \
  void set_##name##_binary_sensor(binary_sensor::BinarySensor *name##_binary_sensor) { this->name##_binary_sensor_ = name##_binary_sensor; }
#else
#define DXS238XW_BINARY_SENSOR(name)
#endif

#ifdef USE_TEXT_SENSOR
#define DXS238XW_TEXT_SENSOR(name) \
 protected: \
  text_sensor::TextSensor *name##_text_sensor_; \
\
 public: \
  void set_##name##_text_sensor(text_sensor::TextSensor *name##_text_sensor) { this->name##_text_sensor_ = name##_text_sensor; }
#else
#define DXS238XW_TEXT_SENSOR(name)
#endif

#ifdef USE_NUMBER
#define DXS238XW_NUMBER(name) \
 protected: \
  number::Number *name##_number_; \
\
 public: \
  void set_##name##_number(number::Number *name##_number) { this->name##_number_ = name##_number; }
#else
#define DXS238XW_NUMBER(name)
#endif

#ifdef USE_SWITCH
#define DXS238XW_SWITCH(name) \
 protected: \
  switch_::Switch *name##_switch_; \
\
 public: \
  void set_##name##_switch(switch_::Switch *name##_switch) { this->name##_switch_ = name##_switch; }
#else
#define DXS238XW_SWITCH(name)
#endif

#ifdef USE_BUTTON
#define DXS238XW_BUTTON(name) \
 protected: \
  button::Button *name##_button_; \
\
 public: \
  void set_##name##_button(button::Button *name##_button) { this->name##_button_ = name##_button; }
#else
#define DXS238XW_BUTTON(name)
#endif

class Dxs238xwComponent : public PollingComponent, public uart::UARTDevice {
#ifdef USE_MODEL_DDS238_2
  DXS238XW_SENSOR(frequency)
  DXS238XW_SENSOR(current)
  DXS238XW_SENSOR(voltage)
  DXS238XW_SENSOR(reactive_power)
  DXS238XW_SENSOR(active_power)
  DXS238XW_SENSOR(power_factor)
  DXS238XW_SENSOR(import_active_energy)
  DXS238XW_SENSOR(export_active_energy)
  DXS238XW_SENSOR(total_energy)
  DXS238XW_SENSOR(total_energy_price)
  DXS238XW_SENSOR(contract_total_energy)
  DXS238XW_SENSOR(price_kWh)

  DXS238XW_TEXT_SENSOR(meter_state_detail)

  DXS238XW_NUMBER(max_current_limit)
  DXS238XW_NUMBER(max_voltage_limit)
  DXS238XW_NUMBER(min_voltage_limit)
  DXS238XW_NUMBER(starting_kWh)
  DXS238XW_NUMBER(price_kWh)

  DXS238XW_SWITCH(meter_state)

  DXS238XW_BUTTON(reset_data)

  DXS238XW_BINARY_SENSOR(warning_off_by_over_voltage)
  DXS238XW_BINARY_SENSOR(warning_off_by_under_voltage)
  DXS238XW_BINARY_SENSOR(warning_off_by_over_current)
  DXS238XW_BINARY_SENSOR(warning_off_by_user)
  DXS238XW_BINARY_SENSOR(meter_state)
#endif

#ifdef USE_MODEL_DDS238_4
  DXS238XW_SENSOR(frequency)
  DXS238XW_SENSOR(current)
  DXS238XW_SENSOR(voltage)
  DXS238XW_SENSOR(reactive_power)
  DXS238XW_SENSOR(active_power)
  DXS238XW_SENSOR(power_factor)
  DXS238XW_SENSOR(import_active_energy)
  DXS238XW_SENSOR(export_active_energy)
  DXS238XW_SENSOR(phase_count)
  DXS238XW_SENSOR(energy_purchase_balance)
  DXS238XW_SENSOR(energy_purchase_price)
  DXS238XW_SENSOR(total_energy)
  DXS238XW_SENSOR(total_energy_price)
  DXS238XW_SENSOR(contract_total_energy)
  DXS238XW_SENSOR(price_kWh)

  DXS238XW_TEXT_SENSOR(meter_state_detail)
  DXS238XW_TEXT_SENSOR(delay_value_remaining)
  DXS238XW_TEXT_SENSOR(meter_id)

  DXS238XW_NUMBER(max_current_limit)
  DXS238XW_NUMBER(max_voltage_limit)
  DXS238XW_NUMBER(min_voltage_limit)
  DXS238XW_NUMBER(energy_purchase_value)
  DXS238XW_NUMBER(energy_purchase_alarm)
  DXS238XW_NUMBER(delay_value_set)
  DXS238XW_NUMBER(starting_kWh)
  DXS238XW_NUMBER(price_kWh)

  DXS238XW_SWITCH(energy_purchase_state)
  DXS238XW_SWITCH(meter_state)
  DXS238XW_SWITCH(delay_state)

  DXS238XW_BUTTON(reset_data)

  DXS238XW_BINARY_SENSOR(warning_off_by_over_voltage)
  DXS238XW_BINARY_SENSOR(warning_off_by_under_voltage)
  DXS238XW_BINARY_SENSOR(warning_off_by_over_current)
  DXS238XW_BINARY_SENSOR(warning_off_by_end_purchase)
  DXS238XW_BINARY_SENSOR(warning_off_by_end_delay)
  DXS238XW_BINARY_SENSOR(warning_off_by_user)
  DXS238XW_BINARY_SENSOR(warning_purchase_alarm)
  DXS238XW_BINARY_SENSOR(meter_state)
#endif

#ifdef USE_MODEL_DTS238_7
  DXS238XW_SENSOR(frequency)
  DXS238XW_SENSOR(current_phase_1)
  DXS238XW_SENSOR(current_phase_2)
  DXS238XW_SENSOR(current_phase_3)
  DXS238XW_SENSOR(voltage_phase_1)
  DXS238XW_SENSOR(voltage_phase_2)
  DXS238XW_SENSOR(voltage_phase_3)
  DXS238XW_SENSOR(reactive_power_phase_1)
  DXS238XW_SENSOR(reactive_power_phase_2)
  DXS238XW_SENSOR(reactive_power_phase_3)
  DXS238XW_SENSOR(reactive_power_total)
  DXS238XW_SENSOR(active_power_phase_1)
  DXS238XW_SENSOR(active_power_phase_2)
  DXS238XW_SENSOR(active_power_phase_3)
  DXS238XW_SENSOR(active_power_total)
  DXS238XW_SENSOR(power_factor_phase_1)
  DXS238XW_SENSOR(power_factor_phase_2)
  DXS238XW_SENSOR(power_factor_phase_3)
  DXS238XW_SENSOR(power_factor_total)
  DXS238XW_SENSOR(import_active_energy)
  DXS238XW_SENSOR(export_active_energy)
  DXS238XW_SENSOR(phase_count)
  DXS238XW_SENSOR(energy_purchase_balance)
  DXS238XW_SENSOR(energy_purchase_price)
  DXS238XW_SENSOR(total_energy)
  DXS238XW_SENSOR(total_energy_price)
  DXS238XW_SENSOR(contract_total_energy)
  DXS238XW_SENSOR(price_kWh)

  DXS238XW_TEXT_SENSOR(meter_state_detail)
  DXS238XW_TEXT_SENSOR(delay_value_remaining)
  DXS238XW_TEXT_SENSOR(meter_id)

  DXS238XW_NUMBER(max_current_limit)
  DXS238XW_NUMBER(max_voltage_limit)
  DXS238XW_NUMBER(min_voltage_limit)
  DXS238XW_NUMBER(energy_purchase_value)
  DXS238XW_NUMBER(energy_purchase_alarm)
  DXS238XW_NUMBER(delay_value_set)
  DXS238XW_NUMBER(starting_kWh)
  DXS238XW_NUMBER(price_kWh)

  DXS238XW_SWITCH(energy_purchase_state)
  DXS238XW_SWITCH(meter_state)
  DXS238XW_SWITCH(delay_state)

  DXS238XW_BUTTON(reset_data)

  DXS238XW_BINARY_SENSOR(warning_off_by_over_voltage)
  DXS238XW_BINARY_SENSOR(warning_off_by_under_voltage)
  DXS238XW_BINARY_SENSOR(warning_off_by_over_current)
  DXS238XW_BINARY_SENSOR(warning_off_by_end_purchase)
  DXS238XW_BINARY_SENSOR(warning_off_by_end_delay)
  DXS238XW_BINARY_SENSOR(warning_off_by_user)
  DXS238XW_BINARY_SENSOR(warning_purchase_alarm)
  DXS238XW_BINARY_SENSOR(meter_state)
#endif

 public:
  Dxs238xwComponent() = default;

  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;

  float get_setup_priority() const override;

  void meter_state_toggle();
  void meter_state_on();
  void meter_state_off();

  void hex_message(std::string message, bool check_crc);

  void process_switch_state(SmIdEntity entity, bool state);
  void process_button_action(SmIdEntity entity);
  void process_number_value(SmIdEntity entity, float value);

 protected:
  bool load_preferences = false;

  uint32_t postpone_setup_time_ = 0;
  uint8_t index_first_command = 0;

  TuyaInitState init_state_ = TuyaInitState::INIT_HEARTBEAT;

  uint32_t last_command_timestamp_ = 0;
  uint32_t last_rx_char_timestamp_ = 0;

  uint32_t count_error_data_acquisition_ = 0;

#ifdef USE_PROTOCOL_HEKR
  uint8_t version_msg = 0;

  optional<CommandType> expected_confirmation_{};
  optional<ResponseType> expected_response_{};
#endif

#ifdef USE_PROTOCOL_TUYA
  std::string product_ = "";

  int status_pin_reported_ = -1;
  int reset_pin_reported_ = -1;

  uint8_t wifi_status_ = -1;
  uint8_t protocol_version_ = -1;

  optional<InternalGPIOPin *> status_pin_{};

  optional<CommandType> expected_response_{};

  std::vector<TuyaDatapoint> datapoints_;
#endif

  std::vector<uint8_t> rx_message_;
  std::vector<SmCommand> command_queue_;

  LimitAndPurchaseData lp_data_;
  MeterStateData ms_data_;

  uint32_t hash_delay_value_set_ = fnv1_hash(SM_STR_DELAY_VALUE_SET);
  uint32_t hash_starting_kWh_ = fnv1_hash(SM_STR_STARTING_KWH);
  uint32_t hash_price_kWh_ = fnv1_hash(SM_STR_PRICE_KWH);
  uint32_t hash_energy_purchase_value_ = fnv1_hash(SM_STR_ENERGY_PURCHASE_VALUE);
  uint32_t hash_energy_purchase_alarm_ = fnv1_hash(SM_STR_ENERGY_PURCHASE_ALARM);

  ESPPreferenceObject preference_delay_value_set_;
  ESPPreferenceObject preference_starting_kWh_;
  ESPPreferenceObject preference_price_kWh_;
  ESPPreferenceObject preference_energy_purchase_value_;
  ESPPreferenceObject preference_energy_purchase_alarm_;

  SmErrorType error_type_ = SmErrorType::NO_ERROR;
  SmErrorCode error_code_ = SmErrorCode::NO_ERROR;

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  void process_command_queue_();
  void send_raw_command_(SmCommand command);

  bool validate_message_();

#ifdef USE_PROTOCOL_HEKR
  void handle_command_(uint8_t command, const uint8_t *buffer, size_t len);
  void process_and_update_data_(uint8_t command, const uint8_t *buffer);
#endif

#ifdef USE_PROTOCOL_TUYA
  void handle_command_(uint8_t command, uint8_t version, const uint8_t *buffer, size_t len);
  void process_and_update_data_(const uint8_t *buffer, size_t len);

  void set_numeric_datapoint_value_(uint8_t datapoint_id, TuyaDatapointType datapoint_type, uint32_t value, uint8_t length);

  void send_datapoint_command_(uint8_t datapoint_id, TuyaDatapointType datapoint_type, std::vector<uint8_t> data);

  void set_boolean_datapoint_value_(uint8_t datapoint_id, bool value);
  void set_integer_datapoint_value_(uint8_t datapoint_id, uint32_t value);

  uint8_t get_wifi_status_code_();
  void send_wifi_status_();

  void show_info_datapoints_();
#endif

#ifdef USE_RAW_MESSAGE
  void hex_message(std::string message, bool check_crc = true);
  bool transmit_serial_data_(uint8_t *array, uint8_t size);
  bool receive_serial_data_(uint8_t *array, uint8_t type_message, uint8_t cmd = 0, uint8_t size_expected = 0);
#endif

  void push_command_(const SmCommand &command, bool push_back = true);

  bool is_expected_message();

  bool first_data_acquisition_();

  void set_meter_state_(bool state);

  bool send_command_(SmCommandSend cmd, bool state = false);

  void update_meter_state_detail_();
  bool is_message_timeout_();

  void incoming_messages_();

  std::string get_delay_value_remaining_string_(uint16_t value);
  std::string get_meter_state_string_(SmErrorMeterStateType error);

  uint8_t calculate_crc_(const uint8_t *array, uint8_t size);

  void print_error_();

  uint32_t read_initial_number_value_(ESPPreferenceObject &preference, const std::string preference_string, uint32_t default_value);
  void save_initial_number_value_(ESPPreferenceObject &preference, uint32_t value);
};

}  // namespace dxs238xw
}  // namespace esphome
