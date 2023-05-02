from esphome.components import button
import esphome.config_validation as cv
import esphome.codegen as cg

from esphome.const import (
    CONF_ICON,
    CONF_ENTITY_CATEGORY,
    ENTITY_CATEGORY_CONFIG,
)

from .. import dxs238xw_ns, SmIdEntity, DXS238XW_COMPONENT_SCHEMA

from ..const import (
    CONF_DXS238XW_ID,

    CONF_RESET_DATA,

    ICON_BACKUP_RESTORE,
)

DEPENDENCIES = ["dxs238xw"]

Dxs238xwButton = dxs238xw_ns.class_("Dxs238xwButton", button.Button)


TYPES = {
    CONF_RESET_DATA: (
        button.BUTTON_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwButton),
                cv.Optional(CONF_ICON, default=ICON_BACKUP_RESTORE): cv.icon,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmIdEntity.BUTTON_RESET_DATA,
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
            var = await button.new_button(conf)
            cg.add(var.set_dxs238xw_parent(paren))
            cg.add(var.set_entity_id(number_id))
            cg.add(getattr(paren, f"set_{type}_button")(var))
