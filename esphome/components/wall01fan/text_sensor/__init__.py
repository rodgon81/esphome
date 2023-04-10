from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg

from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
)

from .. import CONF_WALL01FAN_ID, WALL01FAN_COMPONENT_SCHEMA

DEPENDENCIES = ["wall01fan"]

CONF_TIMER_REMAINING = "timer_remaining"

TYPES = {
    CONF_TIMER_REMAINING: text_sensor.text_sensor_schema(
        icon="mdi:timer-outline",
        entity_category=ENTITY_CATEGORY_CONFIG,
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
            var = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(paren, f"set_{type}_text_sensor")(var))
