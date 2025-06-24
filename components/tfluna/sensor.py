import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_DISTANCE,
    CONF_SIGNAL_STRENGTH,
    CONF_TEMPERATURE,
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_SIGNAL_STRENGTH,
    DEVICE_CLASS_TEMPERATURE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ICON_ARROW_EXPAND_VERTICAL,
    ICON_THERMOMETER,
    ICON_SIGNAL,
    STATE_CLASS_MEASUREMENT,
    UNIT_CENTIMETER,
    UNIT_CELSIUS,
)

from . import CONF_TFLUNA_ID, TFLunaComponent

DEPENDENCIES = ["tfluna"]

CONFIG_SCHEMA = {
        cv.GenerateID(CONF_TFLUNA_ID): cv.use_id(TFLunaComponent),
        cv.Optional(CONF_DISTANCE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CENTIMETER,
            icon=ICON_ARROW_EXPAND_VERTICAL,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_DISTANCE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_THERMOMETER,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
			entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_SIGNAL_STRENGTH): sensor.sensor_schema(
            icon=ICON_SIGNAL,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
            state_class=STATE_CLASS_MEASUREMENT,
			entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }

async def to_code(config):
    tfluna_component = await cg.get_variable(config[CONF_TFLUNA_ID])
    if distance_config := config.get(CONF_DISTANCE):
        sens = await sensor.new_sensor(distance_config)
        cg.add(tfluna_component.set_distance_sensor(sens))
    if temperature_config := config.get(CONF_TEMPERATURE):
        sens = await sensor.new_sensor(temperature_config)
        cg.add(tfluna_component.set_temperature_sensor(sens))
    if signal_strength_config := config.get(CONF_SIGNAL_STRENGTH):
        sens = await sensor.new_sensor(signal_strength_config)
        cg.add(tfluna_component.set_signal_sensor(sens))
