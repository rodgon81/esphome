import logging
import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.final_validate as fv
from esphome import automation
from esphome.automation import maybe_simple_id
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_RX_PIN,
    CONF_TX_PIN,
    CONF_BAUD_RATE,
    CONF_PLATFORM,
    CONF_NUMBER,

    CONF_MODEL,
    CONF_PROTOCOL,
)

from .const import (
    COMPONENT_NAME,

    CONF_DXS238XW_ID,

    CONF_MESSAGE_VALUE,
    CONF_CHECK_CRC,

    LIST_SENSOR,
    MODEL_TYPES,
    PROTOCOL_TYPES,

    PROTOCOL_TUYA,
    PROTOCOL_HEKR,

    MODEL_DDS238_2,
    MODEL_DDS238_4,
    MODEL_DTS238_7,
)

_LOGGER = logging.getLogger(__name__)

CODEOWNERS = ["@rodgon81"]

DEPENDENCIES = ["uart"]

dxs238xw_ns = cg.esphome_ns.namespace(COMPONENT_NAME)

Dxs238xwComponent = dxs238xw_ns.class_(
    "Dxs238xwComponent", cg.PollingComponent, uart.UARTDevice
)

SmIdEntity = dxs238xw_ns.enum("SmIdEntity", True)
SmLimitValue = dxs238xw_ns.enum("SmLimitValue")

DXS238XW_COMPONENT_SCHEMA = cv.Schema(
    {cv.GenerateID(CONF_DXS238XW_ID): cv.use_id(Dxs238xwComponent), }
)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(Dxs238xwComponent),
        cv.Required(CONF_MODEL): cv.one_of(*MODEL_TYPES, upper=True),
        cv.Required(CONF_PROTOCOL): cv.one_of(*PROTOCOL_TYPES, lower=True),
    })
    .extend(uart.UART_DEVICE_SCHEMA)
    .extend(cv.polling_component_schema("3s"))
)


def validate_uart(
    component: str,
    tx_pin: int,
    rx_pin: int,
    baud_rate: int,
    parity: str,
    rx_buffer_size: int,
    stop_bits: int,
    data_bits: int,
):
    def validate_data(opt, required_value):
        def validator(value):
            if value != required_value:
                raise cv.Invalid(
                    f"Component {component} required {opt} {required_value} for the uart bus"
                )
            return value

        return validator

    def validate_pin(opt, device, pin, value_other_pin):
        def validator(value):
            if value[CONF_NUMBER] != pin:
                _LOGGER.warning(
                    "Component %s usually uart %s is GPIO%d", component, opt, pin
                )
            if value[CONF_NUMBER] == value_other_pin[CONF_NUMBER]:
                raise cv.Invalid(
                    f"Component {component} required {CONF_TX_PIN} and {CONF_RX_PIN} not be the same"
                )
            if opt in device:
                raise cv.Invalid(
                    f"The uart {opt} is used both by {component} and {device[opt]}, "
                    f"but can only be used by one. Please create a new uart bus for {component}."
                )
            device[opt] = component
            return value

        return validator

    def validate_hub(conf):
        if CONF_TX_PIN not in conf:
            raise cv.Invalid(
                f"Component {component} requires this uart bus to declare a {CONF_TX_PIN}"
            )

        if CONF_RX_PIN not in conf:
            raise cv.Invalid(
                f"Component {component} requires this uart bus to declare a {CONF_RX_PIN}"
            )

        hub_schema = {}
        uart_id = conf[CONF_ID]
        devices = fv.full_config.get().data.setdefault(uart.KEY_UART_DEVICES, {})
        device = devices.setdefault(uart_id, {})

        hub_schema[cv.Required(CONF_TX_PIN)] = validate_pin(
            CONF_TX_PIN, device, tx_pin, conf[CONF_RX_PIN]
        )
        hub_schema[cv.Required(CONF_RX_PIN)] = validate_pin(
            CONF_RX_PIN, device, rx_pin, conf[CONF_TX_PIN]
        )
        hub_schema[cv.Required(CONF_BAUD_RATE)] = validate_data(
            CONF_BAUD_RATE, baud_rate
        )
        hub_schema[cv.Optional(uart.CONF_PARITY)] = validate_data(
            uart.CONF_PARITY, parity
        )
        hub_schema[cv.Optional(uart.CONF_RX_BUFFER_SIZE)] = validate_data(
            uart.CONF_RX_BUFFER_SIZE, rx_buffer_size
        )
        hub_schema[cv.Optional(uart.CONF_STOP_BITS)] = validate_data(
            uart.CONF_STOP_BITS, stop_bits
        )
        hub_schema[cv.Optional(uart.CONF_DATA_BITS)] = validate_data(
            uart.CONF_DATA_BITS, data_bits
        )

        return cv.Schema(hub_schema, extra=cv.ALLOW_EXTRA)(conf)

    return cv.Schema(
        {cv.Required(uart.CONF_UART_ID): fv.id_declaration_match_schema(validate_hub)},
        extra=cv.ALLOW_EXTRA,
    )


def validate_entities(config):
    def raise_error(property: str, component: str, model: str):
        raise cv.Invalid(
            f"Entity '{property}' in component '{component}' is not valid for the model '{model}'"
        )

    for component in LIST_SENSOR:
        exist_component = component in fv.full_config.get()

        if not exist_component:
            continue

        platform = fv.full_config.get()[component]

        for n in platform:
            if n[CONF_PLATFORM] == COMPONENT_NAME:
                for x in n:
                    if x == CONF_PLATFORM or x == CONF_DXS238XW_ID:
                        continue

                    if x not in LIST_SENSOR[component][config[CONF_MODEL]]:
                        raise_error(x , component, config[CONF_MODEL])

    return config


def validate_protocol_and_model(config):
    if config[CONF_PROTOCOL] == PROTOCOL_HEKR:
        if config[CONF_MODEL] == MODEL_DDS238_2:
            raise cv.Invalid(
                f"This configuration has not been tested, if you have this meter and want support contact the developer"
            )
        elif config[CONF_MODEL] == MODEL_DDS238_4:
            _LOGGER.info(
                "Model '%s' and protocol '%s' are supported", config[CONF_MODEL], config[CONF_PROTOCOL]
            )
        elif config[CONF_MODEL] == MODEL_DTS238_7:
            _LOGGER.info(
                "Model '%s' and protocol '%s' are supported", config[CONF_MODEL], config[CONF_PROTOCOL]
            )
    elif config[CONF_PROTOCOL] == PROTOCOL_TUYA:
        if config[CONF_MODEL] == MODEL_DDS238_2:
            _LOGGER.info(
                "Model '%s' and protocol '%s' are supported", config[CONF_MODEL], config[CONF_PROTOCOL]
            )
        elif config[CONF_MODEL] == MODEL_DDS238_4:
            raise cv.Invalid(
                f"This configuration has not been tested, if you have this meter and want support contact the developer"
            )
        elif config[CONF_MODEL] == MODEL_DTS238_7:
            raise cv.Invalid(
                f"This configuration has not been tested, if you have this meter and want support contact the developer"
            )

    return config


FINAL_VALIDATE_SCHEMA = cv.All(
    validate_uart(
        component=COMPONENT_NAME,
        tx_pin=1,
        rx_pin=3,
        baud_rate=9600,
        parity="NONE",
        rx_buffer_size=256,
        stop_bits=1,
        data_bits=8,
    ),
    validate_entities,
    validate_protocol_and_model,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if config[CONF_MODEL] == MODEL_DDS238_2:
        cg.add_define("USE_MODEL_DDS238_2")
    elif config[CONF_MODEL] == MODEL_DDS238_4:
        cg.add_define("USE_MODEL_DDS238_4")
    elif config[CONF_MODEL] == MODEL_DTS238_7:
        cg.add_define("USE_MODEL_DTS238_7")

    if config[CONF_PROTOCOL] == PROTOCOL_HEKR:
        cg.add_define("USE_PROTOCOL_HEKR")
    elif config[CONF_PROTOCOL] == PROTOCOL_TUYA:
        cg.add_define("USE_PROTOCOL_TUYA")


# ////////////////////////////////////////////////////////////////////////////////////////////////////

MeterStateToggleAction = dxs238xw_ns.class_("MeterStateToggleAction", automation.Action)


@automation.register_action(
    "dxs238xw.meter_state_toggle",
    MeterStateToggleAction,
    maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(Dxs238xwComponent),
        }
    ),
)
async def meter_state_toggle_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


# ////////////////////////////////////////////////////////////////////////////////////////////////////

MeterStateOnAction = dxs238xw_ns.class_("MeterStateOnAction", automation.Action)


@automation.register_action(
    "dxs238xw.meter_state_on",
    MeterStateOnAction,
    maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(Dxs238xwComponent),
        }
    ),
)
async def meter_state_on_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


# ////////////////////////////////////////////////////////////////////////////////////////////////////

MeterStateOffAction = dxs238xw_ns.class_("MeterStateOffAction", automation.Action)


@automation.register_action(
    "dxs238xw.meter_state_off",
    MeterStateOffAction,
    maybe_simple_id(
        {
            cv.GenerateID(): cv.use_id(Dxs238xwComponent),
        }
    ),
)
async def meter_state_off_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)


# ////////////////////////////////////////////////////////////////////////////////////////////////////

HexMessageAction = dxs238xw_ns.class_("HexMessageAction", automation.Action)


@automation.register_action(
    "dxs238xw.hex_message",
    HexMessageAction,
    cv.Schema(
        {
            cv.GenerateID(): cv.use_id(Dxs238xwComponent),
            cv.Required(CONF_MESSAGE_VALUE): cv.templatable(cv.string),
            cv.Optional(CONF_CHECK_CRC, default=True): cv.templatable(cv.boolean),
        }
    ),
)
async def hex_message_action_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)

    template_message = await cg.templatable(config[CONF_MESSAGE_VALUE], args, str)
    cg.add(var.set_message(template_message))

    template_check_crc = await cg.templatable(config[CONF_CHECK_CRC], args, bool)
    cg.add(var.set_check_crc(template_check_crc))

    return var
