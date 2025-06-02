#ifndef SENSORS_CONFIG_H
#define SENSORS_CONFIG_H

#include <BH1750.h>
#include <Arduino.h>
#include <ESP32Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Configuración del sensor de temperatura DS18B20
#define ONE_WIRE_BUS 2  // Pin donde está conectado el DS18B20
extern OneWire oneWire;
extern DallasTemperature sensors;

// Configuración del sensor de humedad del suelo
extern const int soilMoisturePin;

// Configuración del sensor de luminosidad BH1750
extern BH1750 lightMeter;

#endif // SENSORS_CONFIG_H
