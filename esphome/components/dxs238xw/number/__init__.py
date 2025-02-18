from esphome.components import number
import esphome.config_validation as cv
import esphome.codegen as cg

from esphome.components.number import NUMBER_MODES

from esphome.const import (
    CONF_ICON,
    CONF_ENTITY_CATEGORY,
    CONF_MODE,
    ENTITY_CATEGORY_CONFIG,
    CONF_UNIT_OF_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_VOLT,
    UNIT_KILOWATT_HOURS,
    UNIT_MINUTE,
)

from .. import (
    dxs238xw_ns,
    CONF_DXS238XW_ID,
    SmIdEntity,
    SmLimitValue,
    DXS238XW_COMPONENT_SCHEMA,
)

DEPENDENCIES = ["dxs238xw"]

Dxs238xwNumber = dxs238xw_ns.class_("Dxs238xwNumber", number.Number)

MAX_CURRENT_LIMIT = "max_current_limit"
MAX_VOLTAGE_LIMIT = "max_voltage_limit"
MIN_VOLTAGE_LIMIT = "min_voltage_limit"
ENERGY_PURCHASE_VALUE = "energy_purchase_value"
ENERGY_PURCHASE_ALARM = "energy_purchase_alarm"
DELAY_VALUE_SET = "delay_value_set"
STARTING_KWH = "starting_kWh"
PRICE_KWH = "price_kWh"

UNIT_CURRENCY_DOLLAR = "$"

TYPES = {
    MAX_CURRENT_LIMIT: (
        number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwNumber),
                cv.Optional(CONF_ICON, default="mdi:current-ac"): cv.icon,
                cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True),
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_AMPERE
                ): cv.string_strict,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmLimitValue.MIN_CURRENT,
        SmLimitValue.MAX_CURRENT,
        SmLimitValue.STEP_CURRENT,
        SmIdEntity.NUMBER_MAX_CURRENT_LIMIT,
    ),
    MAX_VOLTAGE_LIMIT: (
        number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwNumber),
                cv.Optional(CONF_ICON, default="mdi:sine-wave"): cv.icon,
                cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True),
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT
                ): cv.string_strict,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmLimitValue.MIN_VOLTAGE,
        SmLimitValue.MAX_VOLTAGE,
        SmLimitValue.STEP_VOLTAGE,
        SmIdEntity.NUMBER_MAX_VOLTAGE_LIMIT,
    ),
    MIN_VOLTAGE_LIMIT: (
        number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwNumber),
                cv.Optional(CONF_ICON, default="mdi:sine-wave"): cv.icon,
                cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True),
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT
                ): cv.string_strict,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmLimitValue.MIN_VOLTAGE,
        SmLimitValue.MAX_VOLTAGE,
        SmLimitValue.STEP_VOLTAGE,
        SmIdEntity.NUMBER_MIN_VOLTAGE_LIMIT,
    ),
    ENERGY_PURCHASE_VALUE: (
        number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwNumber),
                cv.Optional(CONF_ICON, default="mdi:lightning-bolt"): cv.icon,
                cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True),
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_KILOWATT_HOURS
                ): cv.string_strict,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmLimitValue.MIN_ENERGY_PURCHASE_VALUE,
        SmLimitValue.MAX_ENERGY_PURCHASE_VALUE,
        SmLimitValue.STEP_ENERGY_PURCHASE_VALUE,
        SmIdEntity.NUMBER_ENERGY_PURCHASE_VALUE,
    ),
    ENERGY_PURCHASE_ALARM: (
        number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwNumber),
                cv.Optional(CONF_ICON, default="mdi:lightning-bolt"): cv.icon,
                cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True),
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_KILOWATT_HOURS
                ): cv.string_strict,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmLimitValue.MIN_ENERGY_PURCHASE_ALARM,
        SmLimitValue.MAX_ENERGY_PURCHASE_ALARM,
        SmLimitValue.STEP_ENERGY_PURCHASE_ALARM,
        SmIdEntity.NUMBER_ENERGY_PURCHASE_ALARM,
    ),
    DELAY_VALUE_SET: (
        number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwNumber),
                cv.Optional(CONF_ICON, default="mdi:timer-cog-outline"): cv.icon,
                cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True),
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_MINUTE
                ): cv.string_strict,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmLimitValue.MIN_DELAY_SET,
        SmLimitValue.MAX_DELAY_SET,
        SmLimitValue.STEP_DELAY_SET,
        SmIdEntity.NUMBER_DELAY_VALUE_SET,
    ),
    STARTING_KWH: (
        number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwNumber),
                cv.Optional(CONF_ICON, default="mdi:home-lightning-bolt-outline"): cv.icon,
                cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True),
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_KILOWATT_HOURS
                ): cv.string_strict,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmLimitValue.MIN_STARTING_KWH + 0.0,
        SmLimitValue.MAX_STARTING_KWH + 0.9,
        SmLimitValue.STEP_STARTING_KWH + 0.1,
        SmIdEntity.NUMBER_STARTING_KWH,
    ),
    PRICE_KWH: (
        number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(Dxs238xwNumber),
                cv.Optional(CONF_ICON, default="mdi:cash"): cv.icon,
                cv.Optional(CONF_MODE, default="BOX"): cv.enum(NUMBER_MODES, upper=True),
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_CURRENCY_DOLLAR
                ): cv.string_strict,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
                ): cv.entity_category,
            }
        ),
        SmLimitValue.MIN_PRICE_KWH + 0.0,
        SmLimitValue.MAX_PRICE_KWH + 0.9,
        SmLimitValue.STEP_PRICE_KWH + 0.1,
        SmIdEntity.NUMBER_PRICE_KWH,
    ),
}

CONFIG_SCHEMA = DXS238XW_COMPONENT_SCHEMA.extend(
    {cv.Optional(type): schema for type, (schema, _, _, _, _) in TYPES.items()}
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_DXS238XW_ID])

    for type, (_, min, max, step_value, number_id) in TYPES.items():
        if type in config:
            conf = config[type]
            var = await number.new_number(
                conf, min_value=min, max_value=max, step=step_value
            )
            cg.add(var.set_dxs238xw_parent(paren))
            cg.add(var.set_entity_id(number_id))
            cg.add(getattr(paren, f"set_{type}_number")(var))
