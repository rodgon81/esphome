#pragma once

#include "esphome/components/uart/uart.h"
#include "esphome/core/application.h"
#include "esphome/core/util.h"

namespace esphome {
namespace dxs238xw_sniffer {

//------------------------------------------------------------------------------
// THIS DEFINE IS ONLY FOR DEVELOPMENT
//------------------------------------------------------------------------------

// #define USE_PROTOCOL_HEKR
// #define USE_PROTOCOL_TUYA

// #define USE_MODE_MODULE
// #define USE_MODE_DEVICE

//*************************************************************************************

static const char *const SM_STR_COMPONENT_VERSION = "1.0.0000 beta";

//*************************************************************************************

#if !defined(USE_PROTOCOL_HEKR) && !defined(USE_PROTOCOL_TUYA)
enum class CommandType : uint8_t {
  NONE = 0x00,
};

static const char *const SM_STR_PROTOCOL = "None";
#endif

#if !defined(USE_MODE_MODULE) && !defined(USE_MODE_DEVICE)
static const char *const SM_STR_MODE = "None";
#endif

//------------------------------------------------------------------------------
// DEFAULTS
//------------------------------------------------------------------------------

#ifdef USE_API
static const uint16_t SM_POSTPONE_SETUP_TIME = 10000;
#else
static const uint16_t SM_POSTPONE_SETUP_TIME = 3500;
#endif

#ifdef USE_PROTOCOL_HEKR
static const char *const SM_STR_PROTOCOL = "Hekr";

static const uint8_t HEKR_HEADER = 0x48;
#endif

#ifdef USE_PROTOCOL_TUYA
static const char *const SM_STR_PROTOCOL = "Tuya";

static const uint8_t TUYA_HEADER_1 = 0x55;
static const uint8_t TUYA_HEADER_2 = 0xAA;
#endif

#ifdef USE_MODE_MODULE
static const char *const SM_STR_MODE = "Module";
#endif
#ifdef USE_MODE_DEVICE
static const char *const SM_STR_MODE = "Device";
#endif

//------------------------------------------------------------------------------

static const uint8_t SM_MAX_MILLIS_TO_RX_BYTE = 50;

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

//------------------------------------------------------------------------------

enum class SmInitState : uint8_t {
  INIT_PRE_SETUP = 0,
  INIT_CONNECTED,
  INIT_DONE,
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
};

struct SmCommand {
  std::vector<uint8_t> payload = std::vector<uint8_t>{};
};

class Dxs238xwSnifferComponent : public Component, public uart::UARTDevice {
 public:
  Dxs238xwSnifferComponent() = default;

  void setup() override;
  void loop() override;
  void dump_config() override;

  float get_setup_priority() const override;

 protected:
  SmInitState init_state_ = SmInitState::INIT_PRE_SETUP;

  uint32_t last_rx_char_timestamp_ = 0;

  uint32_t time_init_connected = 0;

  std::vector<uint8_t> rx_message_;
  std::vector<SmCommand> incoming_messages_offline_;

  SmErrorCode error_code_ = SmErrorCode::NO_ERROR;

  ////////////////////////////////////////////////////////////////////////

  void incoming_messages_();
  bool validate_message_();

  uint8_t calculate_crc_(const uint8_t *array, uint8_t size);
  void print_error_();
};

}  // namespace dxs238xw_sniffer
}  // namespace esphome
