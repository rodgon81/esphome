from esphome.components import sensor
import esphome.config_validation as cv
import esphome.codegen as cg

from esphome.const import (
    CONF_FREQUENCY,
    CONF_IMPORT_ACTIVE_ENERGY,
    CONF_EXPORT_ACTIVE_ENERGY,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_REACTIVE_POWER,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_POWER_FACTOR,
    DEVICE_CLASS_ENERGY,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    STATE_CLASS_TOTAL,
    UNIT_AMPERE,
    UNIT_VOLT,
    UNIT_HERTZ,
    UNIT_KILOVOLT_AMPS_REACTIVE,
    UNIT_KILOWATT,
    UNIT_KILOWATT_HOURS,
    ENTITY_CATEGORY_DIAGNOSTIC,
    DEVICE_CLASS_MONETARY,
)

from .. import CONF_DXS238XW_ID, DXS238XW_COMPONENT_SCHEMA

DEPENDENCIES = ["dxs238xw"]

CONF_CURRENT_PHASE_1 = "current_phase_1"
CONF_CURRENT_PHASE_2 = "current_phase_2"
CONF_CURRENT_PHASE_3 = "current_phase_3"

CONF_VOLTAGE_PHASE_1 = "voltage_phase_1"
CONF_VOLTAGE_PHASE_2 = "voltage_phase_2"
CONF_VOLTAGE_PHASE_3 = "voltage_phase_3"

CONF_REACTIVE_POWER_TOTAL = "reactive_power_total"
CONF_REACTIVE_POWER_PHASE_1 = "reactive_power_phase_1"
CONF_REACTIVE_POWER_PHASE_2 = "reactive_power_phase_2"
CONF_REACTIVE_POWER_PHASE_3 = "reactive_power_phase_3"

CONF_ACTIVE_POWER_TOTAL = "active_power_total"
CONF_ACTIVE_POWER_PHASE_1 = "active_power_phase_1"
CONF_ACTIVE_POWER_PHASE_2 = "active_power_phase_2"
CONF_ACTIVE_POWER_PHASE_3 = "active_power_phase_3"

CONF_POWER_FACTOR_TOTAL = "power_factor_total"
CONF_POWER_FACTOR_PHASE_1 = "power_factor_phase_1"
CONF_POWER_FACTOR_PHASE_2 = "power_factor_phase_2"
CONF_POWER_FACTOR_PHASE_3 = "power_factor_phase_3"

CONF_TOTAL_ENERGY = "total_energy"

CONF_ENERGY_PURCHASE_BALANCE = "energy_purchase_balance"

CONF_PHASE_COUNT = "phase_count"

CONF_ENERGY_PURCHASE_PRICE = "energy_purchase_price"
CONF_TOTAL_ENERGY_PRICE = "total_energy_price"
CONF_CONTRACT_TOTAL_ENERGY = "contract_total_energy"

CONF_PRICE_KWH = "price_kWh"

UNIT_CURRENCY_DOLLAR = "$"

TYPES = {
    CONF_CURRENT_PHASE_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_CURRENT_PHASE_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_CURRENT_PHASE_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_VOLTAGE_PHASE_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_VOLTAGE_PHASE_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_VOLTAGE_PHASE_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_FREQUENCY: sensor.sensor_schema(
        unit_of_measurement=UNIT_HERTZ,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_FREQUENCY,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_REACTIVE_POWER_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOVOLT_AMPS_REACTIVE,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_REACTIVE_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_REACTIVE_POWER_PHASE_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOVOLT_AMPS_REACTIVE,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_REACTIVE_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_REACTIVE_POWER_PHASE_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOVOLT_AMPS_REACTIVE,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_REACTIVE_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_REACTIVE_POWER_PHASE_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOVOLT_AMPS_REACTIVE,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_REACTIVE_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_ACTIVE_POWER_TOTAL: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_ACTIVE_POWER_PHASE_1: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_ACTIVE_POWER_PHASE_2: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_ACTIVE_POWER_PHASE_3: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_POWER,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_POWER_FACTOR_TOTAL: sensor.sensor_schema(
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_POWER_FACTOR,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_POWER_FACTOR_PHASE_1: sensor.sensor_schema(
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_POWER_FACTOR,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_POWER_FACTOR_PHASE_2: sensor.sensor_schema(
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_POWER_FACTOR,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_POWER_FACTOR_PHASE_3: sensor.sensor_schema(
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_POWER_FACTOR,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    CONF_IMPORT_ACTIVE_ENERGY: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ),
    CONF_EXPORT_ACTIVE_ENERGY: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL_INCREASING,
    ),
    CONF_TOTAL_ENERGY: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL,
    ),
    CONF_ENERGY_PURCHASE_BALANCE: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        icon="mdi:lightning-bolt",
        accuracy_decimals=2,
    ),
    CONF_PHASE_COUNT: sensor.sensor_schema(
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        icon="mdi:alpha-f-box-outline",
        accuracy_decimals=0,
    ),
    CONF_ENERGY_PURCHASE_PRICE: sensor.sensor_schema(
        unit_of_measurement=UNIT_CURRENCY_DOLLAR,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_MONETARY,
    ),
    CONF_TOTAL_ENERGY_PRICE: sensor.sensor_schema(
        unit_of_measurement=UNIT_CURRENCY_DOLLAR,
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_MONETARY,
    ),
    CONF_CONTRACT_TOTAL_ENERGY: sensor.sensor_schema(
        unit_of_measurement=UNIT_KILOWATT_HOURS,
        icon="mdi:home-lightning-bolt-outline",
        accuracy_decimals=2,
        device_class=DEVICE_CLASS_ENERGY,
        state_class=STATE_CLASS_TOTAL,
    ),
    CONF_PRICE_KWH: sensor.sensor_schema(
        unit_of_measurement=UNIT_CURRENCY_DOLLAR,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_MONETARY,
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
            var = await sensor.new_sensor(conf)
            cg.add(getattr(paren, f"set_{type}_sensor")(var))
