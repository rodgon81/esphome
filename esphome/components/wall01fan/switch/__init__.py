from esphome.components import switch
import esphome.config_validation as cv
import esphome.codegen as cg

from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
    CONF_POWER,
)

from .. import wall01fan_ns, SmIdEntity, CONF_WALL01FAN_ID, WALL01FAN_COMPONENT_SCHEMA

DEPENDENCIES = ["wall01fan"]

Wall01fanSwitch = wall01fan_ns.class_("Wall01fanSwitch", switch.Switch)

CONF_OSCILLATION = "oscillation"
CONF_SPEED_LOW = "speed_low"
CONF_SPEED_MID = "speed_mid"
CONF_SPEED_HIGH = "speed_high"
CONF_MODE_NORMAL = "mode_normal"
CONF_MODE_NATURAL = "mode_natural"
CONF_MODE_SLEEP = "mode_sleep"
CONF_TIMER_05H = "timer_05h"
CONF_TIMER_1H = "timer_1h"
CONF_TIMER_2H = "timer_2h"
CONF_TIMER_4H = "timer_4h"
CONF_MORE_CONFIG = "more_config"


TYPES = {
    CONF_POWER: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:fan",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_POWER,
    ),
    CONF_OSCILLATION: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:rotate-3d-variant",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_OSCILLATION,
    ),
    CONF_SPEED_LOW: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:fan-speed-1",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_SPEED_LOW,
    ),
    CONF_SPEED_MID: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:fan-speed-2",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_SPEED_MID,
    ),
    CONF_SPEED_HIGH: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:fan-speed-3",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_SPEED_HIGH,
    ),
    CONF_MODE_NORMAL: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:cog-play-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_MODE_NORMAL,
    ),
    CONF_MODE_NATURAL: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:cog-play-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_MODE_NATURAL,
    ),
    CONF_MODE_SLEEP: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:cog-play-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_MODE_SLEEP,
    ),
    CONF_TIMER_05H: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:timer-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_TIMER_05H,
    ),
    CONF_TIMER_1H: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:timer-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_TIMER_1H,
    ),
    CONF_TIMER_2H: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:timer-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_TIMER_2H,
    ),
    CONF_TIMER_4H: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:timer-outline",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_TIMER_4H,
    ),
    CONF_MORE_CONFIG: (
        switch.switch_schema(
            Wall01fanSwitch,
            icon="mdi:cog",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        SmIdEntity.SWITCH_MORE_CONFIG,
    ),
}

CONFIG_SCHEMA = WALL01FAN_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): schema for type, (schema, _) in TYPES.items()}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_WALL01FAN_ID])

    for type, (_, number_id) in TYPES.items():
        if type in config:
            conf = config[type]
            var = await switch.new_switch(conf)
            cg.add(var.set_wall01fan_parent(paren))
            cg.add(var.set_entity_id(number_id))
            cg.add(getattr(paren, f"set_{type}_switch")(var))
