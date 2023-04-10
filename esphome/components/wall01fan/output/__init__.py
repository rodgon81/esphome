import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import output
from esphome.const import CONF_ID, CONF_PIN
from .. import wall01fan_ns, CONF_WALL01FAN_ID, WALL01FAN_COMPONENT_SCHEMA

DEPENDENCIES = ["wall01fan"]

CONF_OSCILLATION = "oscillation"
CONF_SPEED_LOW = "speed_low"
CONF_SPEED_MID = "speed_mid"
CONF_SPEED_HIGH = "speed_high"

CONF_COLUMN_A = "column_a"
CONF_COLUMN_B = "column_b"
CONF_COLUMN_C = "column_c"
CONF_COLUMN_D = "column_d"
CONF_COLUMN_E = "column_e"
CONF_COLUMN_F = "column_f"
CONF_COLUMN_G = "column_g"
CONF_COLUMN_H = "column_h"
CONF_COLUMN_I = "column_i"
CONF_COLUMN_J = "column_j"

Wall01fanBinaryOutput = wall01fan_ns.class_(
    "Wall01fanBinaryOutput", output.BinaryOutput, cg.Component)

TYPES = {
    CONF_OSCILLATION: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_SPEED_LOW: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_SPEED_MID: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_SPEED_HIGH: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_A: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_B: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),

    CONF_COLUMN_C: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_D: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_E: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_F: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_G: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_H: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_I: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
        }
    ),
    CONF_COLUMN_J: output.BINARY_OUTPUT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(Wall01fanBinaryOutput),
            cv.Required(CONF_PIN): pins.internal_gpio_output_pin_schema,
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
            await output.register_output(var, conf)

            await cg.register_component(var, conf)
            cg.add(getattr(paren, f"set_{type}_binary_output")(var))
            pin = await cg.gpio_pin_expression(conf[CONF_PIN])
            cg.add(var.set_pin(pin))
            cg.add_define("USE_BINARY_OUTPUT")
