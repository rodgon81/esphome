from esphome.components import switch
import esphome.config_validation as cv
import esphome.codegen as cg

from esphome.const import (
    ENTITY_CATEGORY_CONFIG,
)

from .. import dxs238xw_ns, SmIdEntity, DXS238XW_COMPONENT_SCHEMA

from ..const import (
    CONF_DXS238XW_ID,

    CONF_ENERGY_PURCHASE_STATE,
    CONF_METER_STATE,
    CONF_DELAY_STATE,

    ICON_LIGHTNING_BOLT,
    ICON_POWER_PLUG,
    ICON_TIMER_COG_OUTLINE,
    RESTORE_MODE_RESTORE_DEFAULT_OFF,
)

DEPENDENCIES = ["dxs238xw"]

Dxs238xwSwitch = dxs238xw_ns.class_("Dxs238xwSwitch", switch.Switch)


TYPES = {
    CONF_ENERGY_PURCHASE_STATE: (
        switch.switch_schema(
            Dxs238xwSwitch,
            icon=ICON_LIGHTNING_BOLT,
            entity_category=ENTITY_CATEGORY_CONFIG,
            default_restore_mode=RESTORE_MODE_RESTORE_DEFAULT_OFF,
        ),
        SmIdEntity.SWITCH_ENERGY_PURCHASE_STATE,
    ),
    CONF_METER_STATE: (
        switch.switch_schema(
            Dxs238xwSwitch,
            icon=ICON_POWER_PLUG,
            entity_category=ENTITY_CATEGORY_CONFIG,
            default_restore_mode=RESTORE_MODE_RESTORE_DEFAULT_OFF,
        ),
        SmIdEntity.SWITCH_METER_STATE,
    ),
    CONF_DELAY_STATE: (
        switch.switch_schema(
            Dxs238xwSwitch,
            icon=ICON_TIMER_COG_OUTLINE,
            entity_category=ENTITY_CATEGORY_CONFIG,
            default_restore_mode=RESTORE_MODE_RESTORE_DEFAULT_OFF,
        ),
        SmIdEntity.SWITCH_DELAY_STATE,
    ),
}

CONFIG_SCHEMA = DXS238XW_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): schema for type, (schema, _) in TYPES.items()}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_DXS238XW_ID])

    for type, (_, number_id) in TYPES.items():
        if type in config:
            conf = config[type]
            var = await switch.new_switch(conf)
            cg.add(var.set_dxs238xw_parent(paren))
            cg.add(var.set_entity_id(number_id))
            cg.add(getattr(paren, f"set_{type}_switch")(var))
