#include "dxs238xw_button.h"

#include "esphome/core/log.h"

namespace esphome {
namespace dxs238xw {

static const char *const TAG = "dxs238xw.button";

void Dxs238xwButton::press_action() { this->parent_->process_button_action(this->entity_id_); }

}  // namespace dxs238xw
}  // namespace esphome
