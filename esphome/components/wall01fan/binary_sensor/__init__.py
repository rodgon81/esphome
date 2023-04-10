import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.core import CORE
from esphome.const import (
    CONF_ID,
    CONF_PIN,
    CONF_INVERTED,
    CONF_INPUT,
    CONF_OUTPUT,
    CONF_OPEN_DRAIN,
    CONF_PULLUP,
    CONF_PULLDOWN,
)
from .. import wall01fan_ns, CONF_WALL01FAN_ID, WALL01FAN_COMPONENT_SCHEMA

GPIO_IO_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_INVERTED): cv.boolean,
    }
)


async def setup_gpio_io_platform_(obj, config):
    if CONF_INVERTED in config:
        cg.add(obj.set_inverted(config[CONF_INVERTED]))


async def register_gpio_io(var, config):
    if not CORE.has_id(config[CONF_ID]):
        var = cg.Pvariable(config[CONF_ID], var)
    await setup_gpio_io_platform_(var, config)


DEPENDENCIES = ["wall01fan"]

CONF_MODE_INPUT = "mode_input"
CONF_MODE_OUTPUT = "mode_output"

CONF_ROW_1 = "row_1"
CONF_ROW_2 = "row_2"
CONF_ROW_3 = "row_3"
CONF_ROW_4 = "row_4"
CONF_ROW_5 = "row_5"

Wall01fanBinaryInputOutput = wall01fan_ns.class_(
    "Wall01fanBinaryInputOutput", cg.Component)

TYPES = {
    CONF_ROW_1: GPIO_IO_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryInputOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_input_pullup_pin_schema,
            cv.Optional(CONF_MODE_INPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=True): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
            cv.Optional(CONF_MODE_OUTPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=False): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
        }
    ),
    CONF_ROW_2: GPIO_IO_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryInputOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_input_pullup_pin_schema,
            cv.Optional(CONF_MODE_INPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=True): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
            cv.Optional(CONF_MODE_OUTPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=False): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
        }
    ),
    CONF_ROW_3: GPIO_IO_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryInputOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_input_pullup_pin_schema,
            cv.Optional(CONF_MODE_INPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=True): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
            cv.Optional(CONF_MODE_OUTPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=False): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
        }
    ),
    CONF_ROW_4: GPIO_IO_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryInputOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_input_pullup_pin_schema,
            cv.Optional(CONF_MODE_INPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=True): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
            cv.Optional(CONF_MODE_OUTPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=False): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
        }
    ),
    CONF_ROW_5: GPIO_IO_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryInputOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_input_pullup_pin_schema,
            cv.Optional(CONF_MODE_INPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=True): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
            cv.Optional(CONF_MODE_OUTPUT, default={}): cv.Schema(
                {
                    cv.Optional(CONF_INPUT, default=False): cv.boolean,
                    cv.Optional(CONF_OUTPUT, default=True): cv.boolean,
                    cv.Optional(CONF_OPEN_DRAIN, default=False): cv.boolean,
                    cv.Optional(CONF_PULLUP, default=False): cv.boolean,
                    cv.Optional(CONF_PULLDOWN, default=False): cv.boolean,
                }
            ),
        }
    ),
}

CONFIG_SCHEMA = WALL01FAN_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): schema for type, schema in TYPES.items()}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_WALL01FAN_ID])

    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            var = cg.new_Pvariable(conf[CONF_ID])
            await register_gpio_io(var, conf)

            await cg.register_component(var, conf)
            cg.add(getattr(paren, f"set_{type}_binary_input_output")(var))
            pin = await cg.gpio_pin_expression(conf[CONF_PIN])
            cg.add(var.set_pin(pin))
            cg.add(var.set_mode_input(
                pins.gpio_flags_expr(conf[CONF_MODE_INPUT])))
            cg.add(var.set_mode_output(
                pins.gpio_flags_expr(conf[CONF_MODE_OUTPUT])))

            cg.add_define("USE_BINARY_INPUT_OUTPUT")
