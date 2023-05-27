#include "dxs238xw_sniffer.h"

namespace esphome {
namespace dxs238xw_sniffer {

static const char *const TAG = "dxs238xw_sniffer";

float Dxs238xwSnifferComponent::get_setup_priority() const {
  //
  return setup_priority::PROCESSOR;
}

void Dxs238xwSnifferComponent::setup() {
  this->incoming_messages_();

  switch (this->init_state_) {
    case SmInitState::INIT_PRE_SETUP:
      if (remote_is_connected()) {
        this->init_state_ = SmInitState::INIT_CONNECTED;

        this->time_init_connected = millis();
      }

      break;
    case SmInitState::INIT_CONNECTED:
      if ((millis() - this->time_init_connected) > SM_POSTPONE_SETUP_TIME) {
        this->init_state_ = SmInitState::INIT_DONE;
      }

      break;
    case SmInitState::INIT_DONE:
      ESP_LOGI(TAG, "Print '%s' Messages offline mode", SM_STR_PROTOCOL);

      for (auto &message : this->incoming_messages_offline_) {
        ESP_LOGI(TAG, "* %s message: %s", SM_STR_MODE, format_hex_pretty(message.payload).c_str());
      }

      this->incoming_messages_offline_.clear();

      ESP_LOGI(TAG, "Print '%s' Messages online mode", SM_STR_PROTOCOL);

      break;
    default:
      break;
  }

  if (this->init_state_ != SmInitState::INIT_DONE) {
    this->component_state_ &= ~COMPONENT_STATE_MASK;
    this->component_state_ |= COMPONENT_STATE_CONSTRUCTION;
  }
}

void Dxs238xwSnifferComponent::loop() {
  //
  this->incoming_messages_();
}

void Dxs238xwSnifferComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "dxs238xw_sniffer:");
  ESP_LOGCONFIG(TAG, "  Component Version: %s", SM_STR_COMPONENT_VERSION);
  ESP_LOGCONFIG(TAG, "  Protocol: %s", SM_STR_PROTOCOL);
  ESP_LOGCONFIG(TAG, "  Mode: %s", SM_STR_MODE);
}

void Dxs238xwSnifferComponent::incoming_messages_() {
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

  if ((this->last_rx_char_timestamp_ != 0) && (millis() - this->last_rx_char_timestamp_) > SM_MAX_MILLIS_TO_RX_BYTE) {
    ESP_LOGW(TAG, "* Time ran out waiting for a new char");

    this->rx_message_.clear();

    this->last_rx_char_timestamp_ = 0;

    this->error_code_ = SmErrorCode::NOT_ENOUGHT_BYTES;

    this->print_error_();
  }
}

bool Dxs238xwSnifferComponent::validate_message_() {
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
    return true;
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

  uint8_t calc_checksum = this->calculate_crc_(data, at);

  if (new_byte != calc_checksum) {
    ESP_LOGW(TAG, "* WRONG_BYTES: CRC / Expected = %02X, Receive = %02X", calc_checksum, new_byte);
    this->error_code_ = SmErrorCode::CRC;
    return false;
  }

  size_t data_length = 0;

#ifdef USE_PROTOCOL_HEKR
  data_length = length;
#endif

#ifdef USE_PROTOCOL_TUYA
  data_length = length + 6;
#endif

  if (this->init_state_ == SmInitState::INIT_DONE) {
    ESP_LOGI(TAG, "* %s message: %s", SM_STR_MODE, format_hex_pretty(data, data_length).c_str());
  } else {
    this->incoming_messages_offline_.push_back(SmCommand{.payload = std::vector<uint8_t>(data, data + data_length)});
  }

  return false;
}

uint8_t Dxs238xwSnifferComponent::calculate_crc_(const uint8_t *array, uint8_t size) {
  uint16_t tmp_crc = 0;

  for (uint8_t n = 0; n < size; n++) {
    tmp_crc = tmp_crc + array[n];
  }

  return tmp_crc & 0xFF;
}

void Dxs238xwSnifferComponent::print_error_() {
  std::string string_code;

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
  }

  ESP_LOGE(TAG, "* Error, Description: %s", string_code.c_str());

  this->error_code_ = SmErrorCode::NO_ERROR;
}

}  // namespace dxs238xw_sniffer
}  // namespace esphome