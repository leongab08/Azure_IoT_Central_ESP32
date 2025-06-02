Maceta Inteligente IoT - Implementación del Internet de las Cosas
🌱 Descripción
Este proyecto consiste en el desarrollo de un sistema automatizado de monitoreo y control ambiental aplicado a una maceta con planta de menta, integrando sensores de temperatura, humedad y luz solar con actuadores para riego, ventilación y control de luz, todo gestionado a través de una plataforma IoT basada en Azure IoT Hub y una ESP32.

El objetivo principal es optimizar el crecimiento y bienestar de la planta, utilizando la tecnología IoT para mejorar la eficiencia en el uso de recursos como agua, luz y energía.

🎯 Objetivos
Monitorizar las variables críticas para el cultivo de la menta:

Temperatura

Humedad del suelo

Radiación solar

Activar actuadores automáticamente para:

Riego automatizado

Ventilación automática

Control de luz solar

Integrar el sistema con Azure IoT Hub para el envío y visualización de datos en tiempo real.

Facilitar la supervisión remota y el análisis de datos.

Desarrollar una solución modular y escalable.

🪴 Planta Seleccionada
Menta: por su facilidad de cultivo, adaptabilidad y compatibilidad con IoT.

Rango óptimo:

Temperatura: 15°C a 25°C

Humedad: 50% - 70%

Radiación solar: 10,000 - 25,000 lux

🔌 Sensores Utilizados
Sensor	Descripción
Sensor capacitivo de humedad del suelo V2.0	Mide la humedad del suelo con salida analógica.
DS18B20	Sensor digital de temperatura con protocolo 1-Wire.
BH1750	Sensor de luz con comunicación I2C, mide iluminancia en lux.

⚙️ Actuadores Utilizados
Actuador	Uso
Electroválvula + Mini Bomba de Agua 3V-6V	Riego automático en función de la humedad del suelo.
Ventilador 5V	Control automático de temperatura ambiente.
Servomotor SG90	Control de compuerta para regular la radiación solar.

☁️ Plataforma IoT
ESP32: microcontrolador encargado de recolectar datos de sensores y controlar actuadores.

Azure IoT Hub: comunicación segura y centralizada de datos.

IoT Dashboard: visualización en tiempo real de las variables medidas y controladas.

🛠️ Metodología
Scrumban: para gestionar el flujo de trabajo y adaptarse a cambios.

Microsoft Planner: para asignar tareas, visualizar el flujo de trabajo y colaborar en equipo.

🗓️ Etapas del Proyecto
Semana 2: Definición del problema y objetivos.

Semana 3: Planificación del proyecto.

Semana 4: Investigación documental (sensores, actuadores, Azure).

Semana 5: Definición de variables y diseño preliminar.

Semanas 6-9: Integración de sensores y actuadores, conexión con Azure IoT Hub y prototipado físico.

Semana 10: Conclusión y entrega.

📦 Estructura del Código
Lectura de sensores: humedad, temperatura y luz.

Control de actuadores:

Riego automático mediante electroválvula y bomba de agua.

Activación de ventilador.

Movimiento del servomotor para la compuerta de luz.

Envío de telemetría a Azure IoT Hub en formato JSON.

Variables clave:

SOIL_MOISTURE_PIN

FAN_PIN

IN1_PIN, IN2_PIN

SERVO_PIN

TEMPERATURE_THRESHOLD, HUMIDITY_THRESHOLD, LIGHT_THRESHOLD

🔗 Conexión con Azure IoT Hub
Registro del dispositivo en Azure IoT Hub.

Configuración de la ESP32 (MQTT y credenciales).

Lectura y envío de datos como JSON.

Visualización en el IoT Dashboard.

📊 Resultados
Monitoreo en tiempo real de las variables ambientales.

Activación automática de actuadores en función de las variables medidas.

Integración efectiva con Azure IoT Hub y Dashboard.

Base física estable construida en MDF.

✅ Conclusiones
Conexión exitosa entre ESP32 y Azure IoT Hub.

Monitoreo y control centralizado y eficiente.

Sistema modular y escalable.

Automatización confiable de procesos de riego, ventilación y control de luz.

Superación de desafíos técnicos y adaptación ágil durante el desarrollo.

📝 Requerimientos
Hardware:

ESP32

Sensor capacitivo de humedad del suelo V2.0

Sensor DS18B20

Sensor BH1750

Bomba de agua 3V-6V

Servomotor SG90

Ventilador 5V

Protoboard, cables y MDF para base

Software:

Arduino IDE

Librerías:

OneWire

DallasTemperature

BH1750

Azure IoT SDK

Azure IoT Hub (cuenta gratuita para pruebas)

📌 Instalación
Instalar Arduino IDE y las librerías necesarias.

Conectar los sensores y actuadores a la ESP32 siguiendo las conexiones especificadas en el proyecto.

Configurar las credenciales de Azure IoT Hub en el código.

Subir el código a la ESP32.

Verificar la conexión con Azure IoT Hub y visualizar la telemetría en el IoT Dashboard.

👥 Autores
Sebastián Acosta Marín - A01278278

Yessica Lora Vázquez - A01273574

Mónica Sarahí Flores Lara - A07045944

Gabriel Esperilla León - A01277955

