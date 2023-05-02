from esphome.const import (
    CONF_SENSOR,
    CONF_NUMBER,
    CONF_BINARY_SENSOR,
    CONF_FREQUENCY,
    CONF_IMPORT_ACTIVE_ENERGY,
    CONF_EXPORT_ACTIVE_ENERGY,
    CONF_CURRENT,
    CONF_VOLTAGE,
    CONF_REACTIVE_POWER,
    CONF_ACTIVE_POWER,
    CONF_POWER_FACTOR,
)

COMPONENT_NAME = "dxs238xw"

ICON_ALPHA_S_BOX_OUTLINE = "mdi:alpha-s-box-outline"
ICON_POWER_PLUG = "mdi:power-plug"
ICON_TIMER_COG_OUTLINE = "mdi:timer-cog-outline"
ICON_HOME_LIGHTNING_BOLT_OUTLINE = "mdi:home-lightning-bolt-outline"
ICON_LIGHTNING_BOLT = "mdi:lightning-bolt"
ICON_ALPHA_F_BOX_OUTLINE = "mdi:alpha-f-box-outline"
ICON_CASH = "mdi:cash"
ICON_SINE_WAVE = "mdi:sine-wave"
ICON_BACKUP_RESTORE = "mdi:backup-restore"

UNIT_CURRENCY_DOLLAR = "$"

MODE_BOX = "BOX"

RESTORE_MODE_RESTORE_DEFAULT_OFF = "RESTORE_DEFAULT_OFF"

PROTOCOL_TUYA = "tuya"
PROTOCOL_HEKR = "hekr"

MODEL_DDS238_2 = "DDS238_2"
MODEL_DDS238_4 = "DDS238_4"
MODEL_DTS238_7 = "DTS238_7"

MODEL_TYPES = [
    MODEL_DDS238_2,
    MODEL_DDS238_4,
    MODEL_DTS238_7,
]

PROTOCOL_TYPES = [
    PROTOCOL_TUYA,
    PROTOCOL_HEKR,
]

CONF_DXS238XW_ID = "dxs238xw_id"

CONF_MESSAGE_VALUE = "message"
CONF_CHECK_CRC = "check_crc"

CONF_TEXT_SENSOR = "text_sensor"
CONF_SWITCH = "switch"
CONF_BUTTON = "button"

CONF_ENABLE_RAW_MESSAGE = "enable_raw_message"

# --------SENSOR----------

CONF_CURRENT_PHASE_1 = "current_phase_1"
CONF_CURRENT_PHASE_2 = "current_phase_2"
CONF_CURRENT_PHASE_3 = "current_phase_3"

CONF_VOLTAGE_PHASE_1 = "voltage_phase_1"
CONF_VOLTAGE_PHASE_2 = "voltage_phase_2"
CONF_VOLTAGE_PHASE_3 = "voltage_phase_3"

CONF_REACTIVE_POWER_PHASE_1 = "reactive_power_phase_1"
CONF_REACTIVE_POWER_PHASE_2 = "reactive_power_phase_2"
CONF_REACTIVE_POWER_PHASE_3 = "reactive_power_phase_3"
CONF_REACTIVE_POWER_TOTAL = "reactive_power_total"

CONF_ACTIVE_POWER_PHASE_1 = "active_power_phase_1"
CONF_ACTIVE_POWER_PHASE_2 = "active_power_phase_2"
CONF_ACTIVE_POWER_PHASE_3 = "active_power_phase_3"
CONF_ACTIVE_POWER_TOTAL = "active_power_total"

CONF_POWER_FACTOR_PHASE_1 = "power_factor_phase_1"
CONF_POWER_FACTOR_PHASE_2 = "power_factor_phase_2"
CONF_POWER_FACTOR_PHASE_3 = "power_factor_phase_3"
CONF_POWER_FACTOR_TOTAL = "power_factor_total"

CONF_PHASE_COUNT = "phase_count"

CONF_ENERGY_PURCHASE_BALANCE = "energy_purchase_balance"
CONF_ENERGY_PURCHASE_PRICE = "energy_purchase_price"

CONF_TOTAL_ENERGY = "total_energy"
CONF_TOTAL_ENERGY_PRICE = "total_energy_price"
CONF_CONTRACT_TOTAL_ENERGY = "contract_total_energy"

CONF_PRICE_KWH = "price_kWh"

# --------TEXT SENSOR----------

CONF_METER_STATE_DETAIL = "meter_state_detail"
CONF_DELAY_VALUE_REMAINING = "delay_value_remaining"
CONF_METER_ID = "meter_id"

# --------NUMBER----------

CONF_MAX_CURRENT_LIMIT = "max_current_limit"
CONF_MAX_VOLTAGE_LIMIT = "max_voltage_limit"
CONF_MIN_VOLTAGE_LIMIT = "min_voltage_limit"
CONF_ENERGY_PURCHASE_VALUE = "energy_purchase_value"
CONF_ENERGY_PURCHASE_ALARM = "energy_purchase_alarm"
CONF_DELAY_VALUE_SET = "delay_value_set"
CONF_STARTING_KWH = "starting_kWh"

# --------SWITCH----------

CONF_ENERGY_PURCHASE_STATE = "energy_purchase_state"
CONF_METER_STATE = "meter_state"
CONF_DELAY_STATE = "delay_state"

# --------BUTTON------------

CONF_RESET_DATA = "reset_data"

# -------BINARY SENSOR-------------

CONF_WARNING_OFF_BY_OVER_VOLTAGE = "warning_off_by_over_voltage"
CONF_WARNING_OFF_BY_UNDER_VOLTAGE = "warning_off_by_under_voltage"
CONF_WARNING_OFF_BY_OVER_CURRENT = "warning_off_by_over_current"
CONF_WARNING_OFF_BY_END_PURCHASE = "warning_off_by_end_purchase"
CONF_WARNING_OFF_BY_END_DELAY = "warning_off_by_end_delay"
CONF_WARNING_OFF_BY_USER = "warning_off_by_user"
CONF_WARNING_PURCHASE_ALARM = "warning_purchase_alarm"

# --------------------

LIST_SENSOR = {
    CONF_SENSOR: {
        MODEL_DDS238_2: [
            CONF_FREQUENCY,
            CONF_CURRENT,
            CONF_VOLTAGE,
            CONF_REACTIVE_POWER,
            CONF_ACTIVE_POWER,
            CONF_POWER_FACTOR,
            CONF_IMPORT_ACTIVE_ENERGY,
            CONF_EXPORT_ACTIVE_ENERGY,
            CONF_TOTAL_ENERGY,
            CONF_TOTAL_ENERGY_PRICE,
            CONF_CONTRACT_TOTAL_ENERGY,
            CONF_PRICE_KWH,
        ],
        MODEL_DDS238_4: [
            CONF_FREQUENCY,
            CONF_CURRENT,
            CONF_VOLTAGE,
            CONF_REACTIVE_POWER,
            CONF_ACTIVE_POWER,
            CONF_POWER_FACTOR,
            CONF_IMPORT_ACTIVE_ENERGY,
            CONF_EXPORT_ACTIVE_ENERGY,
            CONF_PHASE_COUNT,
            CONF_ENERGY_PURCHASE_BALANCE,
            CONF_ENERGY_PURCHASE_PRICE,
            CONF_TOTAL_ENERGY,
            CONF_TOTAL_ENERGY_PRICE,
            CONF_CONTRACT_TOTAL_ENERGY,
            CONF_PRICE_KWH,
        ],
        MODEL_DTS238_7: [
            CONF_FREQUENCY,
            CONF_CURRENT_PHASE_1,
            CONF_CURRENT_PHASE_2,
            CONF_CURRENT_PHASE_3,
            CONF_VOLTAGE_PHASE_1,
            CONF_VOLTAGE_PHASE_2,
            CONF_VOLTAGE_PHASE_3,
            CONF_REACTIVE_POWER_PHASE_1,
            CONF_REACTIVE_POWER_PHASE_2,
            CONF_REACTIVE_POWER_PHASE_3,
            CONF_REACTIVE_POWER_TOTAL,
            CONF_ACTIVE_POWER_PHASE_1,
            CONF_ACTIVE_POWER_PHASE_2,
            CONF_ACTIVE_POWER_PHASE_3,
            CONF_ACTIVE_POWER_TOTAL,
            CONF_POWER_FACTOR_PHASE_1,
            CONF_POWER_FACTOR_PHASE_2,
            CONF_POWER_FACTOR_PHASE_3,
            CONF_POWER_FACTOR_TOTAL,
            CONF_IMPORT_ACTIVE_ENERGY,
            CONF_EXPORT_ACTIVE_ENERGY,
            CONF_PHASE_COUNT,
            CONF_ENERGY_PURCHASE_BALANCE,
            CONF_ENERGY_PURCHASE_PRICE,
            CONF_TOTAL_ENERGY,
            CONF_TOTAL_ENERGY_PRICE,
            CONF_CONTRACT_TOTAL_ENERGY,
            CONF_PRICE_KWH,
        ],
    },
    CONF_TEXT_SENSOR: {
        MODEL_DDS238_2: [
            CONF_METER_STATE_DETAIL,
        ],
        MODEL_DDS238_4: [
            CONF_METER_STATE_DETAIL,
            CONF_DELAY_VALUE_REMAINING,
            CONF_METER_ID,
        ],
        MODEL_DTS238_7: [
            CONF_METER_STATE_DETAIL,
            CONF_DELAY_VALUE_REMAINING,
            CONF_METER_ID,
        ],
    },
    CONF_NUMBER: {
        MODEL_DDS238_2: [
            CONF_MAX_CURRENT_LIMIT,
            CONF_MAX_VOLTAGE_LIMIT,
            CONF_MIN_VOLTAGE_LIMIT,
            CONF_STARTING_KWH,
            CONF_PRICE_KWH,
        ],
        MODEL_DDS238_4: [
            CONF_MAX_CURRENT_LIMIT,
            CONF_MAX_VOLTAGE_LIMIT,
            CONF_MIN_VOLTAGE_LIMIT,
            CONF_ENERGY_PURCHASE_VALUE,
            CONF_ENERGY_PURCHASE_ALARM,
            CONF_DELAY_VALUE_SET,
            CONF_STARTING_KWH,
            CONF_PRICE_KWH,
        ],
        MODEL_DTS238_7: [
            CONF_MAX_CURRENT_LIMIT,
            CONF_MAX_VOLTAGE_LIMIT,
            CONF_MIN_VOLTAGE_LIMIT,
            CONF_ENERGY_PURCHASE_VALUE,
            CONF_ENERGY_PURCHASE_ALARM,
            CONF_DELAY_VALUE_SET,
            CONF_STARTING_KWH,
            CONF_PRICE_KWH,
        ],
    },
    CONF_SWITCH: {
        MODEL_DDS238_2: [
            CONF_METER_STATE,
        ],
        MODEL_DDS238_4: [
            CONF_ENERGY_PURCHASE_STATE,
            CONF_METER_STATE,
            CONF_DELAY_STATE,
        ],
        MODEL_DTS238_7: [
            CONF_ENERGY_PURCHASE_STATE,
            CONF_METER_STATE,
            CONF_DELAY_STATE,
        ],
    },
    CONF_BUTTON: {
        MODEL_DDS238_2: [
            CONF_RESET_DATA,
        ],
        MODEL_DDS238_4: [
            CONF_RESET_DATA,
        ],
        MODEL_DTS238_7: [
            CONF_RESET_DATA,
        ],
    },
    CONF_BINARY_SENSOR: {
        MODEL_DDS238_2: [
            CONF_WARNING_OFF_BY_OVER_VOLTAGE,
            CONF_WARNING_OFF_BY_UNDER_VOLTAGE,
            CONF_WARNING_OFF_BY_OVER_CURRENT,
            CONF_WARNING_OFF_BY_USER,
            CONF_METER_STATE,
        ],
        MODEL_DDS238_4: [
            CONF_WARNING_OFF_BY_OVER_VOLTAGE,
            CONF_WARNING_OFF_BY_UNDER_VOLTAGE,
            CONF_WARNING_OFF_BY_OVER_CURRENT,
            CONF_WARNING_OFF_BY_END_PURCHASE,
            CONF_WARNING_OFF_BY_END_DELAY,
            CONF_WARNING_OFF_BY_USER,
            CONF_WARNING_PURCHASE_ALARM,
            CONF_METER_STATE,
        ],
        MODEL_DTS238_7: [
            CONF_WARNING_OFF_BY_OVER_VOLTAGE,
            CONF_WARNING_OFF_BY_UNDER_VOLTAGE,
            CONF_WARNING_OFF_BY_OVER_CURRENT,
            CONF_WARNING_OFF_BY_END_PURCHASE,
            CONF_WARNING_OFF_BY_END_DELAY,
            CONF_WARNING_OFF_BY_USER,
            CONF_WARNING_PURCHASE_ALARM,
            CONF_METER_STATE,
        ],
    },
}
