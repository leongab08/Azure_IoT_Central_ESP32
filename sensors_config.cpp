#include "sensors_config.h"

// Sensor de temperatura
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Sensor de humedad del suelo
const int soilMoisturePin = 34;

// Sensor de luminosidad
BH1750 lightMeter;
