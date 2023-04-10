import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID

CODEOWNERS = ["@rodgon81"]

CONF_WALL01FAN_ID = "wall01fan_id"
COMPONENT_NAME = "wall01fan"

CONF_ON_BEEP = "on_beep"

wall01fan_ns = cg.esphome_ns.namespace(COMPONENT_NAME)
Wall01fanComponent = wall01fan_ns.class_(
    "Wall01fanComponent", cg.Component
)
SmIdEntity = wall01fan_ns.enum("SmIdEntity", True)

BeepTrigger = wall01fan_ns.class_(
    "BeepTrigger", automation.Trigger.template()
)

WALL01FAN_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_WALL01FAN_ID): cv.use_id(Wall01fanComponent),
    }
)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(Wall01fanComponent),
        cv.Optional(CONF_ON_BEEP): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(BeepTrigger),
            }
        ),
    })
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    for config in config.get(CONF_ON_BEEP, []):
        trigger = cg.new_Pvariable(config[CONF_TRIGGER_ID], var)
        await automation.build_automation(trigger, [], config)

ACTION_SCHEMA = automation.maybe_simple_id(
    {
        cv.GenerateID(): cv.use_id(Wall01fanComponent),
    }
)

CONDITION_SCHEMA = cv.All(
    {
        cv.GenerateID(): cv.use_id(Wall01fanComponent),
    }
)

# ////////////////////////////////////////////////////////////////////////////////////////////////////

RemoteReceiverAction = wall01fan_ns.class_(
    "RemoteReceiverAction", automation.Action)

CONF_PROTOCOL = "protocol"
CONF_CODE = "code"


@automation.register_action(
    "wall01fan.remote_receiver",
    RemoteReceiverAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(Wall01fanComponent),
            cv.Required(CONF_PROTOCOL): cv.templatable(cv.uint8_t),
            cv.Optional(CONF_CODE): cv.templatable(cv.uint64_t),
        }
    ),
)
async def remote_receiver_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)

    template_protocol = await cg.templatable(config[CONF_PROTOCOL], args, str)
    cg.add(var.set_message(template_protocol))

    template_code = await cg.templatable(config[CONF_CODE], args, bool)
    cg.add(var.set_check_crc(template_code))

    return var

# ////////////////////////////////////////////////////////////////////////////////////////////////////

PowerTurnOnAction = wall01fan_ns.class_(
    "PowerTurnOnAction", automation.Action)


@automation.register_action("wall01fan.power_turn_on", PowerTurnOnAction, ACTION_SCHEMA)
async def turn_on_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

PowerTurnOffAction = wall01fan_ns.class_(
    "PowerTurnOffAction", automation.Action)


@automation.register_action("wall01fan.power_turn_off", PowerTurnOffAction, ACTION_SCHEMA)
async def power_turn_off_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

PowerToggleAction = wall01fan_ns.class_(
    "PowerToggleAction", automation.Action)


@automation.register_action("wall01fan.power_toggle", PowerToggleAction, ACTION_SCHEMA)
async def power_toggle_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ////////////////////////////////////////////////////////////////////////////////////////////////////

OscillationTurnOnAction = wall01fan_ns.class_(
    "OscillationTurnOnAction", automation.Action)


@automation.register_action("wall01fan.oscillation_turn_on", OscillationTurnOnAction, ACTION_SCHEMA)
async def oscillation_turn_on_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

OscillationTurnOffAction = wall01fan_ns.class_(
    "OscillationTurnOffAction", automation.Action)


@automation.register_action("wall01fan.oscillation_turn_off", OscillationTurnOffAction, ACTION_SCHEMA)
async def oscillation_turn_off_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

OscillationToggleAction = wall01fan_ns.class_(
    "OscillationToggleAction", automation.Action)


@automation.register_action("wall01fan.oscillation_toggle", OscillationToggleAction, ACTION_SCHEMA)
async def oscillation_toggle_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ////////////////////////////////////////////////////////////////////////////////////////////////////

SpeedNoneAction = wall01fan_ns.class_(
    "SpeedNoneAction", automation.Action)


@automation.register_action("wall01fan.speed_none", SpeedNoneAction, ACTION_SCHEMA)
async def speed_none_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

SpeedLowAction = wall01fan_ns.class_(
    "SpeedLowAction", automation.Action)


@automation.register_action("wall01fan.speed_low", SpeedLowAction, ACTION_SCHEMA)
async def speed_low_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

SpeedMidAction = wall01fan_ns.class_(
    "SpeedMidAction", automation.Action)


@automation.register_action("wall01fan.speed_mid", SpeedMidAction, ACTION_SCHEMA)
async def speed_mid_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

SpeedHighAction = wall01fan_ns.class_(
    "SpeedHighAction", automation.Action)


@automation.register_action("wall01fan.speed_high", SpeedHighAction, ACTION_SCHEMA)
async def speed_high_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

SpeedToggleAction = wall01fan_ns.class_(
    "SpeedToggleAction", automation.Action)


@automation.register_action("wall01fan.speed_toggle", SpeedToggleAction, ACTION_SCHEMA)
async def speed_toggle_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ////////////////////////////////////////////////////////////////////////////////////////////////////

ModeNoneAction = wall01fan_ns.class_(
    "ModeNoneAction", automation.Action)


@automation.register_action("wall01fan.mode_none", ModeNoneAction, ACTION_SCHEMA)
async def mode_none_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

ModeNormalAction = wall01fan_ns.class_(
    "ModeNormalAction", automation.Action)


@automation.register_action("wall01fan.mode_normal", ModeNormalAction, ACTION_SCHEMA)
async def mode_normal_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

ModeNaturalAction = wall01fan_ns.class_(
    "ModeNaturalAction", automation.Action)


@automation.register_action("wall01fan.mode_natural", ModeNaturalAction, ACTION_SCHEMA)
async def mode_natural_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

ModeSleepAction = wall01fan_ns.class_(
    "ModeSleepAction", automation.Action)


@automation.register_action("wall01fan.mode_sleep", ModeSleepAction, ACTION_SCHEMA)
async def mode_sleep_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

ModeToggleAction = wall01fan_ns.class_(
    "ModeToggleAction", automation.Action)


@automation.register_action("wall01fan.mode_toggle", ModeToggleAction, ACTION_SCHEMA)
async def mode_toggle_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# ////////////////////////////////////////////////////////////////////////////////////////////////////

IsPowerOnCondition = wall01fan_ns.class_(
    "IsPowerOnCondition", automation.Condition)


@automation.register_condition("wall01fan.is_power_on", IsPowerOnCondition, CONDITION_SCHEMA)
async def is_power_on_condition_to_code(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)

# ---------------------------------------------------------------------------------------------------

IsTemperatureModeCondition = wall01fan_ns.class_(
    "IsTemperatureModeCondition", automation.Condition)


@automation.register_condition("wall01fan.is_temperature_mode", IsTemperatureModeCondition, CONDITION_SCHEMA)
async def is_temperature_mode_condition_to_code(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)
