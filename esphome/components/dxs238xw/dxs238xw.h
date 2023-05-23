#pragma once

#include "esphome/components/uart/uart.h"
#include "esphome/core/application.h"
#include "esphome/core/util.h"

#ifdef USE_TIME
#include "esphome/components/time/real_time_clock.h"
#endif

namespace esphome {
namespace dxs238xw {

//------------------------------------------------------------------------------
// THIS DEFINE IS ONLY FOR DEVELOPMENT
//------------------------------------------------------------------------------

// #define USE_PROTOCOL_HEKR
// #define USE_PROTOCOL_TUYA

// #define USE_MODEL_DDS238_2
// #define USE_MODEL_DDS238_4
// #define USE_MODEL_DTS238_7

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//*************************************************************************************

static const char *const SM_STR_COMPONENT_VERSION = "2.0.0000 beta";

//*************************************************************************************

#if !defined(USE_PROTOCOL_HEKR) && !defined(USE_PROTOCOL_TUYA)
enum class CommandType : uint8_t {
  NONE = 0x00,
};

static const uint8_t SM_MIN_COMMAND_LENGHT = 0;

static const char *const SM_STR_PROTOCOL = "None";
#endif

#if !defined(USE_MODEL_DDS238_2) && !defined(USE_MODEL_DDS238_4) && !defined(USE_MODEL_DTS238_7)
static const char *const SM_STR_METER_MODEL = "None";
#endif

//------------------------------------------------------------------------------
// DEFAULTS
//------------------------------------------------------------------------------

#ifdef USE_MODEL_DDS238_2
static const char *const SM_STR_METER_MODEL = "DDS238_2";
#endif
#ifdef USE_MODEL_DDS238_4
static const char *const SM_STR_METER_MODEL = "DDS238_4";
#endif
#ifdef USE_MODEL_DTS238_7
static const char *const SM_STR_METER_MODEL = "DTS238_7";
#endif

#ifdef USE_API
static const uint16_t SM_POSTPONE_SETUP_TIME = 10000;
#else
static const uint16_t SM_POSTPONE_SETUP_TIME = 3500;
#endif

#ifdef USE_PROTOCOL_HEKR
static const char *const SM_STR_PROTOCOL = "Hekr";

static const uint8_t HEKR_HEADER = 0x48;

static const uint16_t SM_MAX_MILLIS_TO_CONFIRM = 200;
static const uint16_t SM_MAX_MILLIS_TO_RESPONSE = 1000;

static const uint8_t SM_MIN_COMMAND_LENGHT = 6;
#endif

#ifdef USE_PROTOCOL_TUYA
static const char *const SM_STR_PROTOCOL = "Tuya";

static const uint8_t TUYA_HEADER_1 = 0x55;
static const uint8_t TUYA_HEADER_2 = 0xAA;

static const uint16_t SM_MAX_MILLIS_TO_RESPONSE = 300;

static const uint8_t SM_MIN_COMMAND_LENGHT = 7;
#endif

//------------------------------------------------------------------------------

static const uint16_t SM_MIN_INTERVAL_TO_GET_DATA = 500;
static const uint8_t SM_MAX_MILLIS_TO_RX_BYTE = 50;

static const uint8_t SM_COMMAND_DELAY = 50;

static const uint8_t SM_MAX_HEX_MSG_LENGTH = 255;

static const uint8_t SM_GET_ONE_BYTE = 0xFF;

static const uint8_t SM_MAX_RETRIES = 5;

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
static const char *const SM_STR_TYPE_COMMUNICATION = "Communication";
static const char *const SM_STR_TYPE_COMMUNICATION_CONFIRMATION = "Communication (Confirmation Message)";
static const char *const SM_STR_TYPE_COMMUNICATION_RESPONSE = "Communication (Answer Message)";
static const char *const SM_STR_TYPE_INPUT_DATA = "Input data";

// Error Description
static const char *const SM_STR_CODE_WRONG_BYTES_HEADER = "The bytes was received but are not correct (HEADER)";
static const char *const SM_STR_CODE_WRONG_BYTES_LENGTH = "The bytes was received but are not correct (LENGTH)";
static const char *const SM_STR_CODE_WRONG_BYTES_TYPE_MESSAGE = "The bytes was received but are not correct (TYPE_MESSAGE)";
static const char *const SM_STR_CODE_WRONG_BYTES_COMMAND = "The bytes was received but are not correct (COMMAND)";
static const char *const SM_STR_CODE_WRONG_BYTES_VERSION_PROTOCOL = "Tuya Protocol send 0x00 version";
static const char *const SM_STR_CODE_WRONG_BYTES_VERSION_MESSAGE = "Hekr Protocol invalid version confirmation message";

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

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                        "abcdefghijklmnopqrstuvwxyz"
                                        "0123456789+/";

enum class SmInitState : uint8_t {
  INIT_PRE_SETUP = 0,
  INIT_CONNECTED,
  INIT_GET_PREFERENCES,

#ifdef USE_PROTOCOL_HEKR
  INIT_GET_METER_ID,
  INIT_GET_METER_STATE,
  INIT_GET_LIMIT_AND_PURCHASE,
  INIT_GET_MEASUREMENT,
#endif

#ifdef USE_PROTOCOL_TUYA
  INIT_HEARTBEAT,
  INIT_PRODUCT_QUERY,
  INIT_WORKING_MODE_QUERY,
  INIT_WIFI_STATE,
  INIT_DATAPOINT_QUERY,
#endif

  INIT_ERROR,
  INIT_DONE,
};

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

enum class SmErrorType : uint8_t {
  NO_ERROR,
  COMMUNICATION,
  COMMUNICATION_CONFIRMATION,
  COMMUNICATION_RESPONSE,
  INPUT_DATA,
};

enum class SmErrorCode : uint8_t {
  NO_ERROR,
  CRC,
  WRONG_BYTES_HEADER,
  WRONG_BYTES_LENGTH,
  WRONG_BYTES_VERSION_PROTOCOL,
  WRONG_BYTES_VERSION_MESSAGE,
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

enum class SmCommandType : uint8_t {
  SET_LIMIT,
  SET_ENERGY_PURCHASE,
  SET_METER_STATE,
  SET_DELAY,
  SET_RESET,
};

#ifdef USE_PROTOCOL_HEKR
enum class ModuleOperation : uint8_t {
  STATUS = 0x01,            // status query
  SOFT_REBOOT = 0x02,       // module soft restart
  FACTORY_RESET = 0x03,     // Restore Factory
  CONFIG = 0x04,            // One-key configuration
  SET_SLEEP = 0x05,         // go to sleep
  WEAKUP = 0x06,            // wake up from sleep
  FIRMWARE_VERSION = 0x10,  // version query
  PRODKEY_GET = 0x11,       // ProdKey query
  FACTORY_TEST = 0x20,      // enter factory test
  SET_PRODKEY = 0x21,       // ProdKey setting
};

enum class ErrorValue : uint8_t {
  ERROR_OPERATION = 0x01,
  ERROR_SUM_CHECK = 0x02,
  ERROR_DATA_RANGE = 0x03,
  ERROR_CMD_NO_SUPPORTED = 0x04,
  ERROR_DATA_IS_INCONSISTENT = 0x05,
};

enum class ResponseType : uint8_t {
  RECEIVE_METER_STATE = 0x01,
  RECEIVE_MEASUREMENT = 0x0B,
  RECEIVE_LIMIT_AND_ENERGY_PURCHASE = 0x08,
  RECEIVE_METER_ID = 0x07,
};

enum class CommandData : uint8_t {
  GET_METER_STATE = 0x00,
  GET_MEASUREMENT = 0x0A,
  GET_LIMIT_AND_ENERGY_PURCHASE = 0x02,
  GET_METER_ID = 0x06,

  SET_LIMIT = 0x03,
  SET_ENERGY_PURCHASE = 0x0D,
  SET_METER_STATE = 0x09,
  SET_DELAY = 0x0C,
  SET_RESET = 0x05,
};

enum class CommandType : uint8_t {
  RECEIVE = 0x01,
  SEND = 0x02,
  MODULE_OPERATION = 0xFE,
  ERROR_FRAME = 0xFF,
};
#endif

#ifdef USE_PROTOCOL_TUYA
enum class CommandType : uint8_t {
  HEARTBEAT = 0x00,
  PRODUCT_QUERY = 0x01,
  WORKING_MODE_QUERY = 0x02,
  WIFI_STATE = 0x03,
  DATAPOINT_DELIVER = 0x06,
  DATAPOINT_QUERY = 0x08,
  DATAPOINT_REPORT = 0x07,
  WIFI_TEST = 0x0E,
  WIFI_RSSI = 0x24,
  LOCAL_TIME_QUERY = 0x1C,
  GET_NETWORK_STATUS = 0x2B,
};

enum class TuyaDatapointType : uint8_t {
  RAW = 0x00,     // variable length
  BOOL = 0x01,    // 1 byte (0/1)
  UINT32 = 0x02,  // 4 byte
  STRING = 0x03,  // variable length
  UINT8 = 0x04,   // 1 byte
  BITMAP = 0x05,  // 1/2/4 bytes
};

#ifdef USE_MODEL_DDS238_2
enum class DatapointId : uint8_t {
  //  FREQUENCY = 0,
  CURRENT = 18,
  VOLTAGE = 20,
  //  POWER_FACTOR = 0,

  REACTIVE_POWER = 51,  /////????
  ACTIVE_POWER = 19,

  IMPORT_ACTIVE_ENERGY = 101,
  EXPORT_ACTIVE_ENERGY = 9,
  //  TOTAL_ENERGY = 0,

  // ENERGY_PURCHASE_ALARM = 0,
  //  ENERGY_PURCHASE_BALANCE = 0,

  //  DELAY_VALUE_REMAINING = 0,

  //  SET_LIMIT = 0,
  //  SET_ENERGY_PURCHASE = 0,
  METER_STATE = 1,
  //  SET_DELAY = 0,
  //  SET_RESET_DATA = 0,
  GET_CONFIG = 17,
  GET_DATA = 6,
};
#endif

#ifdef USE_MODEL_DDS238_4
enum class DatapointId : uint8_t {
  FREQUENCY = 100,
  CURRENT = 101,
  VOLTAGE = 105,
  REACTIVE_POWER = 109,
  ACTIVE_POWER = 114,
  POWER_FACTOR = 119,
  IMPORT_ACTIVE_ENERGY = 124,
  EXPORT_ACTIVE_ENERGY = 125,
  ENERGY_PURCHASE_BALANCE = 127,
  TOTAL_ENERGY = 128,
  DELAY_VALUE_REMAINING = 129,
  MAX_CURRENT_LIMIT = 130,
  MAX_VOLTAGE_LIMIT = 131,
  MIN_VOLTAGE_LIMIT = 132,
  ENERGY_PURCHASE_VALUE = 133,
  ENERGY_PURCHASE_ALARM = 134,
  DELAY_VALUE_SET = 135,
  ENERGY_PURCHASE_STATE = 136,
  METER_STATE = 137,
  DELAY_STATE = 138,
  RESET_DATA = 139,
};
#endif

#ifdef USE_MODEL_DTS238_7
enum class DatapointId : uint8_t {
  FREQUENCY = 100,
  CURRENT_PHASE_1 = 102,
  CURRENT_PHASE_2 = 103,
  CURRENT_PHASE_3 = 104,
  VOLTAGE_PHASE_1 = 106,
  VOLTAGE_PHASE_2 = 107,
  VOLTAGE_PHASE_3 = 108,
  REACTIVE_POWER_PHASE_1 = 110,
  REACTIVE_POWER_PHASE_2 = 111,
  REACTIVE_POWER_PHASE_3 = 112,
  REACTIVE_POWER_TOTAL = 113,
  ACTIVE_POWER_PHASE_1 = 115,
  ACTIVE_POWER_PHASE_2 = 116,
  ACTIVE_POWER_PHASE_3 = 117,
  ACTIVE_POWER_TOTAL = 118,
  POWER_FACTOR_PHASE_1 = 120,
  POWER_FACTOR_PHASE_2 = 121,
  POWER_FACTOR_PHASE_3 = 122,
  POWER_FACTOR_TOTAL = 123,
  IMPORT_ACTIVE_ENERGY = 124,
  EXPORT_ACTIVE_ENERGY = 125,
  ENERGY_PURCHASE_BALANCE = 127,
  TOTAL_ENERGY = 128,
  DELAY_VALUE_REMAINING = 129,
  MAX_CURRENT_LIMIT = 130,
  MAX_VOLTAGE_LIMIT = 131,
  MIN_VOLTAGE_LIMIT = 132,
  ENERGY_PURCHASE_VALUE = 133,
  ENERGY_PURCHASE_ALARM = 134,
  DELAY_VALUE_SET = 135,
  ENERGY_PURCHASE_STATE = 136,
  METER_STATE = 137,
  DELAY_STATE = 138,
  RESET_DATA = 139,
};
#endif

struct TuyaDatapoint {
  DatapointId id;
  TuyaDatapointType type;
  size_t len;

  union {
    bool value_bool;
    uint8_t value_uint8;
    uint32_t value_uint32;
    uint32_t value_bitmap;
  };

  std::string value_string;
  std::vector<uint8_t> value_raw;
};
#endif

struct SmCommand {
  CommandType cmd;
  std::vector<uint8_t> payload = std::vector<uint8_t>{};
  bool null_old_response = false;
  bool raw = false;
  bool priority_cmd = false;
};

struct MeterData {
  uint32_t energy_purchase_value = 0;
  uint32_t energy_purchase_alarm = 0;
  float energy_purchase_balance = 0;

  bool energy_purchase_state = false;

  uint8_t max_current_limit = 0;
  uint16_t max_voltage_limit = 0;
  uint16_t min_voltage_limit = 0;

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

  float starting_kWh = 0;
  float price_kWh = 0;

  float total_energy = 0;
  float active_power_total = 0;

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
  DXS238XW_SENSOR(frequency)
  DXS238XW_SENSOR(current)
  DXS238XW_SENSOR(current_phase_1)
  DXS238XW_SENSOR(current_phase_2)
  DXS238XW_SENSOR(current_phase_3)
  DXS238XW_SENSOR(voltage)
  DXS238XW_SENSOR(voltage_phase_1)
  DXS238XW_SENSOR(voltage_phase_2)
  DXS238XW_SENSOR(voltage_phase_3)
  DXS238XW_SENSOR(reactive_power)
  DXS238XW_SENSOR(reactive_power_phase_1)
  DXS238XW_SENSOR(reactive_power_phase_2)
  DXS238XW_SENSOR(reactive_power_phase_3)
  DXS238XW_SENSOR(reactive_power_total)
  DXS238XW_SENSOR(active_power)
  DXS238XW_SENSOR(active_power_phase_1)
  DXS238XW_SENSOR(active_power_phase_2)
  DXS238XW_SENSOR(active_power_phase_3)
  DXS238XW_SENSOR(active_power_total)
  DXS238XW_SENSOR(power_factor)
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

  void hex_message(std::string message, bool check_crc = true);

  void process_switch_state(SmIdEntity entity, bool state);
  void process_button_action(SmIdEntity entity);
  void process_number_value(SmIdEntity entity, float value);

 protected:
  SmInitState init_state_ = SmInitState::INIT_PRE_SETUP;

  std::string log_command_queue_str_ = "log_command_queue";

  uint32_t last_command_timestamp_ = 0;
  uint32_t last_rx_char_timestamp_ = 0;

  uint8_t init_retries_ = 0;

  uint32_t time_init_connected = 0;

#ifdef USE_PROTOCOL_HEKR
  uint8_t version_msg = 0;

  optional<CommandType> expected_confirmation_{};
  optional<CommandType> expected_response_{};
#endif

#ifdef USE_PROTOCOL_TUYA
  std::string heartbeat_interval_str_ = "heartbeat_interval";

  std::string product_ = "";

  int8_t status_pin_reported_ = -1;
  int8_t reset_pin_reported_ = -1;

  uint8_t protocol_version_ = 0x00;

  optional<CommandType> expected_response_{};

  std::vector<TuyaDatapoint> datapoints_;
#endif

  std::vector<uint8_t> rx_message_;
  std::vector<SmCommand> command_queue_;

  MeterData data_;

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

  //////////////////////////////////////////////////////////////////////////////////////////////////

  void incoming_messages_();
  bool validate_message_();

#ifdef USE_PROTOCOL_HEKR
  void handle_command_(uint8_t command, const uint8_t *buffer, size_t len);
  void process_and_update_data_(const uint8_t *buffer, size_t len);
#endif

#ifdef USE_PROTOCOL_TUYA
  void handle_command_(uint8_t command, uint8_t version, const uint8_t *buffer, size_t len);
  void process_and_update_data_(const uint8_t *buffer, size_t len);

  void set_bool_datapoint_value_(DatapointId datapoint_id, bool value, bool priority_cmd = false);
  void set_uint8_datapoint_value_(DatapointId datapoint_id, uint8_t value, bool priority_cmd = false);
  void set_uint32_datapoint_value_(DatapointId datapoint_id, uint32_t value, bool priority_cmd = false);
  void set_bitmap_datapoint_value_(DatapointId datapoint_id, uint32_t value, uint8_t length, bool priority_cmd = false);
  void set_raw_datapoint_value_(DatapointId datapoint_id, const std::vector<uint8_t> &value, bool priority_cmd = false);
  void set_string_datapoint_value_(DatapointId datapoint_id, const std::string &value, bool priority_cmd = false);

  void numeric_datapoint_value_(DatapointId datapoint_id, TuyaDatapointType datapoint_type, uint32_t value, uint8_t length, bool priority_cmd);
  void send_datapoint_command_(DatapointId datapoint_id, TuyaDatapointType datapoint_type, std::vector<uint8_t> data, bool priority_cmd);
  optional<TuyaDatapoint> get_datapoint_(DatapointId datapoint_id);
  void show_info_datapoints_();
#endif

#ifdef USE_TIME
  void send_local_time_();
  optional<time::RealTimeClock *> time_id_{};
#endif

  void process_command_queue_();
  void send_raw_command_(SmCommand command);

  void push_command_(const SmCommand &command);
  void send_command_(SmCommandType cmd, bool state = false);

  bool is_expected_message();
  bool is_message_timeout_();
  void set_meter_state_(bool state);
  void update_meter_state_detail_();
  std::string get_delay_value_remaining_string_(uint16_t value);
  std::string get_meter_state_string_(SmErrorMeterStateType error);
  uint8_t calculate_crc_(const uint8_t *array, uint8_t size);
  void print_error_();
  uint32_t read_initial_number_value_(ESPPreferenceObject &preference, const std::string &preference_string, uint32_t default_value);
  void save_initial_number_value_(ESPPreferenceObject &preference, uint32_t value);

  std::string base64_encode_(const uint8_t *buf, size_t bufLen);
  std::vector<uint8_t> base64_decode_(const std::string &encoded_string);
};

}  // namespace dxs238xw
}  // namespace esphome
