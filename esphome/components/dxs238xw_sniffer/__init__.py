import logging
import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.final_validate as fv
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_RX_PIN,
    CONF_TX_PIN,
    CONF_BAUD_RATE,
    CONF_NUMBER,
    CONF_MODE,

    CONF_PROTOCOL,
)

COMPONENT_NAME = "dxs238xw_sniffer"

PROTOCOL_TUYA = "tuya"
PROTOCOL_HEKR = "hekr"

PROTOCOL_TYPES = [
    PROTOCOL_TUYA,
    PROTOCOL_HEKR,
]

MODE_MODULE = "module"
MODE_DEVICE = "device"

MODE_TYPES = [
    MODE_MODULE,
    MODE_DEVICE,
]

_LOGGER = logging.getLogger(__name__)

CODEOWNERS = ["@rodgon81"]

DEPENDENCIES = ["uart"]

dxs238xw_sniffer_ns = cg.esphome_ns.namespace(COMPONENT_NAME)

Dxs238xwSnifferComponent = dxs238xw_sniffer_ns.class_(
    "Dxs238xwSnifferComponent", cg.Component, uart.UARTDevice
)

CONFIG_SCHEMA = cv.All(
    cv.Schema({
        cv.GenerateID(): cv.declare_id(Dxs238xwSnifferComponent),
        cv.Required(CONF_PROTOCOL): cv.one_of(*PROTOCOL_TYPES, lower=True),
        cv.Required(CONF_MODE): cv.one_of(*MODE_TYPES, lower=True),
    })
    .extend(uart.UART_DEVICE_SCHEMA)
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
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if config[CONF_PROTOCOL] == PROTOCOL_HEKR:
        cg.add_define("USE_PROTOCOL_HEKR")
    elif config[CONF_PROTOCOL] == PROTOCOL_TUYA:
        cg.add_define("USE_PROTOCOL_TUYA")

    if config[CONF_MODE] == MODE_MODULE:
        cg.add_define("USE_MODE_MODULE")
    elif config[CONF_MODE] == MODE_DEVICE:
        cg.add_define("USE_MODE_DEVICE")
