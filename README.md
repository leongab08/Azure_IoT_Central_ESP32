# 🌱 Maceta Inteligente IoT - Implementación del Internet de las Cosas

## Descripción

Este proyecto desarrolla un sistema automatizado para monitorear y controlar las condiciones ambientales de una maceta con planta de menta. Utiliza sensores de temperatura, humedad y luz solar junto con actuadores para riego, ventilación y control de luz solar. Está conectado a Azure IoT Hub a través de una ESP32, permitiendo supervisión y control remoto en tiempo real.

---

## Objetivos

- Monitorear temperatura, humedad del suelo y radiación solar.
- Activar actuadores automáticamente:
  - Riego automático (electroválvula y bomba).
  - Ventilación automática (ventilador).
  - Control de luz solar (servomotor).
- Integrar con Azure IoT Hub para visualización y análisis de datos.
- Diseñar un sistema modular y escalable.
- Mejorar el bienestar de la planta de menta.

---

## Planta seleccionada

- **Menta**
  - Temperatura: 15°C - 25°C
  - Humedad: 50% - 70%
  - Radiación solar: 10,000 - 25,000 lux

---

## Sensores utilizados

| Sensor                                      | Función                                     |
|---------------------------------------------|---------------------------------------------|
| Sensor capacitivo de humedad V2.0           | Medición de la humedad del suelo            |
| DS18B20                                     | Medición digital de la temperatura          |
| BH1750                                      | Medición digital de la luminosidad en lux   |

---

## Actuadores utilizados

| Actuador                     | Función                                        |
|------------------------------|-----------------------------------------------|
| Electroválvula + bomba de agua 3V-6V | Riego automático según humedad del suelo |
| Ventilador 5V                 | Control automático de temperatura ambiente   |
| Servomotor SG90               | Control automático de compuerta para luz     |

---

## Plataforma IoT

- **ESP32**: Microcontrolador central.
- **Azure IoT Hub**: Plataforma de nube para monitoreo y control.
- **IoT Dashboard**: Visualización de datos en tiempo real.

---

## Metodología

- **Scrumban** para organización ágil y flexible.
- **Microsoft Planner** como herramienta de gestión.

---

## Estructura del Código

- Lectura de sensores de temperatura, humedad y luz.
- Control de actuadores en función de umbrales:
  - Temperatura: 25°C
  - Humedad: 50%
  - Luz: 25,000 lux (demo: 2,000 lux)
- Envío de datos JSON a Azure IoT Hub.
- Variables clave:
  - `SOIL_MOISTURE_PIN`, `FAN_PIN`, `IN1_PIN`, `IN2_PIN`, `SERVO_PIN`
  - `TEMPERATURE_THRESHOLD`, `HUMIDITY_THRESHOLD`, `LIGHT_THRESHOLD`

---

## Conexión con Azure IoT Hub

1. Registrar la ESP32 como dispositivo en Azure IoT Hub.
2. Configurar la ESP32 con MQTT y credenciales.
3. Enviar datos JSON desde ESP32.
4. Visualizar datos en IoT Dashboard.

---

## Resultados

- Monitorización en tiempo real de las variables críticas.
- Activación automática de actuadores.
- Integración efectiva con Azure IoT Hub.
- Construcción física estable en MDF.

---

## Conclusiones

- Integración exitosa de hardware y software.
- Supervisión centralizada y eficiente con Azure IoT Dashboard.
- Sistema escalable y modular.
- Automatización confiable de procesos agrícolas.
- Capacidad de adaptación y resolución de problemas técnicos.

---

## Requerimientos

**Hardware:**
- ESP32
- Sensor capacitivo de humedad V2.0
- DS18B20
- BH1750
- Bomba de agua 3V-6V
- Servomotor SG90
- Ventilador 5V
- Protoboard, cables y MDF

**Software:**
- Arduino IDE
- Librerías: OneWire, DallasTemperature, BH1750, Azure IoT SDK
- Azure IoT Hub (cuenta gratuita para pruebas)

---

## Instalación

1. Instalar Arduino IDE y las librerías necesarias.
2. Conectar sensores y actuadores a la ESP32.
3. Configurar credenciales de Azure IoT Hub en el código.
4. Subir el código a la ESP32.
5. Verificar conexión y visualización en IoT Dashboard.

---

## Autores

- Sebastián Acosta Marín - A01278278
- Yessica Lora Vázquez - A01273574
- Mónica Sarahí Flores Lara - A07045944
- Gabriel Esperilla León - A01277955

---


