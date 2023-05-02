from esphome.components import text_sensor
import esphome.config_validation as cv
import esphome.codegen as cg

from esphome.const import (
    ENTITY_CATEGORY_DIAGNOSTIC,
)

from .. import DXS238XW_COMPONENT_SCHEMA

from ..const import (
    CONF_DXS238XW_ID,

    CONF_METER_STATE_DETAIL,
    CONF_DELAY_VALUE_REMAINING,
    CONF_METER_ID,

    ICON_POWER_PLUG,
    ICON_TIMER_COG_OUTLINE,
    ICON_ALPHA_S_BOX_OUTLINE,
)

DEPENDENCIES = ["dxs238xw"]


TYPES = {
    CONF_METER_STATE_DETAIL: text_sensor.text_sensor_schema(
        icon=ICON_POWER_PLUG,
    ),
    CONF_DELAY_VALUE_REMAINING: text_sensor.text_sensor_schema(
        icon=ICON_TIMER_COG_OUTLINE,
    ),
    CONF_METER_ID: text_sensor.text_sensor_schema(
        icon=ICON_ALPHA_S_BOX_OUTLINE,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
    ),
}

CONFIG_SCHEMA = DXS238XW_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): schema for type, schema in TYPES.items()}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_DXS238XW_ID])

    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            var = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(paren, f"set_{type}_text_sensor")(var))
