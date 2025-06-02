// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <stdarg.h>
#include <stdlib.h>

#include <az_core.h>
#include <az_iot.h>

#include "AzureIoT.h"
#include "Azure_IoT_PnP_Template.h"

#include <az_precondition_internal.h>

#include "sensors_config.h"

/* --- Defines --- */
#define AZURE_PNP_MODEL_ID "dtmi:azureiot:devkit:freertos:Esp32AzureIotKit;1"

#define SAMPLE_DEVICE_INFORMATION_NAME "deviceInformation"
#define SAMPLE_MANUFACTURER_PROPERTY_NAME "manufacturer"
#define SAMPLE_MODEL_PROPERTY_NAME "model"
#define SAMPLE_SOFTWARE_VERSION_PROPERTY_NAME "swVersion"
#define SAMPLE_OS_NAME_PROPERTY_NAME "osName"
#define SAMPLE_PROCESSOR_ARCHITECTURE_PROPERTY_NAME "processorArchitecture"
#define SAMPLE_PROCESSOR_MANUFACTURER_PROPERTY_NAME "processorManufacturer"
#define SAMPLE_TOTAL_STORAGE_PROPERTY_NAME "totalStorage"
#define SAMPLE_TOTAL_MEMORY_PROPERTY_NAME "totalMemory"

#define SAMPLE_MANUFACTURER_PROPERTY_VALUE "ESPRESSIF"
#define SAMPLE_MODEL_PROPERTY_VALUE "ESP32 Azure IoT Kit"
#define SAMPLE_VERSION_PROPERTY_VALUE "1.0.0"
#define SAMPLE_OS_NAME_PROPERTY_VALUE "FreeRTOS"
#define SAMPLE_ARCHITECTURE_PROPERTY_VALUE "ESP32 WROVER-B"
#define SAMPLE_PROCESSOR_MANUFACTURER_PROPERTY_VALUE "ESPRESSIF"
// The next couple properties are in KiloBytes.
#define SAMPLE_TOTAL_STORAGE_PROPERTY_VALUE 4096
#define SAMPLE_TOTAL_MEMORY_PROPERTY_VALUE 8192

#define TELEMETRY_PROP_NAME_TEMPERATURE "temperature"
#define TELEMETRY_PROP_NAME_HUMIDITY "humidity"
#define TELEMETRY_PROP_NAME_LIGHT "light"
#define TELEMETRY_PROP_NAME_PRESSURE "pressure"
#define TELEMETRY_PROP_NAME_ALTITUDE "altitude"
#define TELEMETRY_PROP_NAME_MAGNETOMETERX "magnetometerX"
#define TELEMETRY_PROP_NAME_MAGNETOMETERY "magnetometerY"
#define TELEMETRY_PROP_NAME_MAGNETOMETERZ "magnetometerZ"
#define TELEMETRY_PROP_NAME_PITCH "pitch"
#define TELEMETRY_PROP_NAME_ROLL "roll"
#define TELEMETRY_PROP_NAME_ACCELEROMETERX "accelerometerX"
#define TELEMETRY_PROP_NAME_ACCELEROMETERY "accelerometerY"
#define TELEMETRY_PROP_NAME_ACCELEROMETERZ "accelerometerZ"

static az_span COMMAND_NAME_TOGGLE_LED_1 = AZ_SPAN_FROM_STR("ToggleLed1");
static az_span COMMAND_NAME_TOGGLE_LED_2 = AZ_SPAN_FROM_STR("ToggleLed2");
static az_span COMMAND_NAME_DISPLAY_TEXT = AZ_SPAN_FROM_STR("DisplayText");
#define COMMAND_RESPONSE_CODE_ACCEPTED 202
#define COMMAND_RESPONSE_CODE_REJECTED 404

#define WRITABLE_PROPERTY_TELEMETRY_FREQ_SECS "telemetryFrequencySecs"
#define WRITABLE_PROPERTY_RESPONSE_SUCCESS "success"

#define DOUBLE_DECIMAL_PLACE_DIGITS 2

/* --- Function Checks and Returns --- */
#define RESULT_OK 0
#define RESULT_ERROR __LINE__

#define EXIT_IF_TRUE(condition, retcode, message, ...) \
  do                                                   \
  {                                                    \
    if (condition)                                     \
    {                                                  \
      LogError(message, ##__VA_ARGS__);                \
      return retcode;                                  \
    }                                                  \
  } while (0)

#define EXIT_IF_AZ_FAILED(azresult, retcode, message, ...) \
  EXIT_IF_TRUE(az_result_failed(azresult), retcode, message, ##__VA_ARGS__)

/* --- Data --- */
#define DATA_BUFFER_SIZE 1024
static uint8_t data_buffer[DATA_BUFFER_SIZE];
static uint32_t telemetry_send_count = 0;

static size_t telemetry_frequency_in_seconds = 10; // With default frequency of once in 10 seconds.
static time_t last_telemetry_send_time = INDEFINITE_TIME;

static bool led1_on = false;
static bool led2_on = false;

/* --- Function Prototypes --- */
/* Please find the function implementations at the bottom of this file */
static int generate_telemetry_payload(
    uint8_t* payload_buffer,
    size_t payload_buffer_size,
    size_t* payload_buffer_length);
static int generate_device_info_payload(
    const az_iot_hub_client* hub_client,
    uint8_t* payload_buffer,
    size_t payload_buffer_size,
    size_t* payload_buffer_length) {
    az_json_writer jw;
    az_result rc;
    az_span payload_buffer_span = az_span_create(payload_buffer, payload_buffer_size);

    Serial.println("Iniciando generación de payload...");

    rc = az_json_writer_init(&jw, payload_buffer_span, NULL);
    if (az_result_failed(rc)) {
        Serial.println("Error al inicializar el escritor JSON.");
        return RESULT_ERROR;
    }

    rc = az_json_writer_append_begin_object(&jw);
    if (az_result_failed(rc)) {
        Serial.println("Error al abrir el objeto JSON.");
        return RESULT_ERROR;
    }

    Serial.println("Añadiendo propiedades...");
    rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR("manufacturer"));
    if (az_result_failed(rc)) {
        Serial.println("Error al añadir 'manufacturer'.");
        return RESULT_ERROR;
    }
    rc = az_json_writer_append_string(&jw, AZ_SPAN_FROM_STR("ESPRESSIF"));
    if (az_result_failed(rc)) {
        Serial.println("Error al añadir valor 'manufacturer'.");
        return RESULT_ERROR;
    }

    rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR("model"));
    if (az_result_failed(rc)) {
        Serial.println("Error al añadir 'model'.");
        return RESULT_ERROR;
    }
    rc = az_json_writer_append_string(&jw, AZ_SPAN_FROM_STR("ESP32 DevKitC"));
    if (az_result_failed(rc)) {
        Serial.println("Error al añadir valor 'model'.");
        return RESULT_ERROR;
    }

    rc = az_json_writer_append_end_object(&jw);
    if (az_result_failed(rc)) {
        Serial.println("Error al cerrar el objeto JSON.");
        return RESULT_ERROR;
    }

    *payload_buffer_length = az_span_size(az_json_writer_get_bytes_used_in_destination(&jw));
    Serial.print("Tamaño del payload generado: ");
    Serial.println(*payload_buffer_length);

    return RESULT_OK;
}

static int consume_properties_and_generate_response(
    azure_iot_t* azure_iot,
    az_span properties,
    uint8_t* buffer,
    size_t buffer_size,
    size_t* response_length);

/* --- Public Functions --- */
void azure_pnp_init() {}

const az_span azure_pnp_get_model_id() { return AZ_SPAN_FROM_STR(AZURE_PNP_MODEL_ID); }

void azure_pnp_set_telemetry_frequency(size_t frequency_in_seconds)
{
  telemetry_frequency_in_seconds = frequency_in_seconds;
  LogInfo("Telemetry frequency set to once every %d seconds.", telemetry_frequency_in_seconds);
}

/* Application-specific data section */

int azure_pnp_send_telemetry(azure_iot_t* azure_iot) {
    Serial.println("Entrando a azure_pnp_send_telemetry...");
    int result;
    size_t payload_length;

    result = generate_telemetry_payload(data_buffer, DATA_BUFFER_SIZE, &payload_length);
    if (result != RESULT_OK) {
        Serial.println("Error generando payload de telemetría.");
        return RESULT_ERROR;
    }
    Serial.println("Payload de telemetría generado con éxito:");

    // Imprime el payload generado (para verificar qué se enviará)
    Serial.println((char*)data_buffer);

    result = azure_iot_send_telemetry(azure_iot, az_span_create(data_buffer, payload_length));
    if (result != 0) {
        Serial.println("Error enviando telemetría.");
        return RESULT_ERROR;
    }
    Serial.println("Telemetría enviada con éxito.");
    return RESULT_OK;
}


int azure_pnp_send_device_info(azure_iot_t* azure_iot, uint32_t request_id) {
    _az_PRECONDITION_NOT_NULL(azure_iot);

    Serial.println("Iniciando azure_pnp_send_device_info...");
    int result;
    size_t payload_length;

    // Generar el payload de información del dispositivo
    result = generate_device_info_payload(&azure_iot->iot_hub_client, data_buffer, DATA_BUFFER_SIZE, &payload_length);
    if (result != RESULT_OK) {
        Serial.println("Error al generar el payload de información del dispositivo.");
        return RESULT_ERROR;
    }
    Serial.println("Payload generado correctamente para azure_pnp_send_device_info.");
    Serial.print("Payload: ");
    Serial.println((char*)data_buffer);

    // Enviar el payload de información del dispositivo
    result = azure_iot_send_properties_update(azure_iot, request_id, az_span_create(data_buffer, payload_length));
    if (result != RESULT_OK) {
        Serial.println("Error al enviar el payload de información del dispositivo.");
        return RESULT_ERROR;
    }
    Serial.println("Payload enviado exitosamente para azure_pnp_send_device_info.");

    return RESULT_OK;
}



int azure_pnp_handle_command_request(azure_iot_t* azure_iot, command_request_t command)
{
  _az_PRECONDITION_NOT_NULL(azure_iot);

  uint16_t response_code;

  if (az_span_is_content_equal(command.command_name, COMMAND_NAME_TOGGLE_LED_1))
  {
    led1_on = !led1_on;
    LogInfo("LED 1 state: %s", (led1_on ? "ON" : "OFF"));
    response_code = COMMAND_RESPONSE_CODE_ACCEPTED;
  }
  else if (az_span_is_content_equal(command.command_name, COMMAND_NAME_TOGGLE_LED_2))
  {
    led2_on = !led2_on;
    LogInfo("LED 2 state: %s", (led2_on ? "ON" : "OFF"));
    response_code = COMMAND_RESPONSE_CODE_ACCEPTED;
  }
  else if (az_span_is_content_equal(command.command_name, COMMAND_NAME_DISPLAY_TEXT))
  {
    // The payload comes surrounded by quotes, so to remove them we offset the payload by 1 and its
    // size by 2.
    LogInfo(
        "OLED display: %.*s", az_span_size(command.payload) - 2, az_span_ptr(command.payload) + 1);
    response_code = COMMAND_RESPONSE_CODE_ACCEPTED;
  }
  else
  {
    LogError(
        "Command not recognized (%.*s).",
        az_span_size(command.command_name),
        az_span_ptr(command.command_name));
    response_code = COMMAND_RESPONSE_CODE_REJECTED;
  }

  return azure_iot_send_command_response(
      azure_iot, command.request_id, response_code, AZ_SPAN_EMPTY);
}

int azure_pnp_handle_properties_update(
    azure_iot_t* azure_iot,
    az_span properties,
    uint32_t request_id)
{
  _az_PRECONDITION_NOT_NULL(azure_iot);
  _az_PRECONDITION_VALID_SPAN(properties, 1, false);

  int result;
  size_t length;

  result = consume_properties_and_generate_response(
      azure_iot, properties, data_buffer, DATA_BUFFER_SIZE, &length);
  EXIT_IF_TRUE(result != RESULT_OK, RESULT_ERROR, "Failed generating properties ack payload.");

  result = azure_iot_send_properties_update(
      azure_iot, request_id, az_span_create(data_buffer, length));
  EXIT_IF_TRUE(result != RESULT_OK, RESULT_ERROR, "Failed sending reported properties update.");

  return RESULT_OK;
}

/* --- Internal Functions --- */
static float simulated_get_temperature() { return 21.0; }

static float simulated_get_humidity() { return 88.0; }

static float simulated_get_ambientLight() { return 700.0; }

static void simulated_get_pressure_altitude(float* pressure, float* altitude)
{
  *pressure = 55.0;
  *altitude = 700.0;
}

static void simulated_get_magnetometer(
    int32_t* magneticFieldX,
    int32_t* magneticFieldY,
    int32_t* magneticFieldZ)
{
  *magneticFieldX = 2000;
  *magneticFieldY = 3000;
  *magneticFieldZ = 4000;
}

static void simulated_get_pitch_roll_accel(
    int32_t* pitch,
    int32_t* roll,
    int32_t* accelerationX,
    int32_t* accelerationY,
    int32_t* accelerationZ)
{
  *pitch = 30;
  *roll = 90;
  *accelerationX = 33;
  *accelerationY = 44;
  *accelerationZ = 55;
}

static int generate_telemetry_payload(
    uint8_t* payload_buffer,
    size_t payload_buffer_size,
    size_t* payload_buffer_length) {
    az_json_writer jw;
    az_result rc;
    az_span payload_buffer_span = az_span_create(payload_buffer, payload_buffer_size);

    // Leer valores de los sensores
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0); // Temperatura
    int rawHumedad = analogRead(soilMoisturePin);   // Humedad cruda
    float humedad = map(rawHumedad, 3000, 1000, 0, 100); // Convertir a porcentaje
    humedad = constrain(humedad, 0, 100); // Limitar a 0-100%
    float luminosidad = lightMeter.readLightLevel(); // Luminosidad en lux

    // Log para depuración
    Serial.print("Temperatura: "); Serial.println(temperature);
    Serial.print("Humedad: "); Serial.println(humedad);
    Serial.print("Luminosidad: "); Serial.println(luminosidad);

    if (temperature == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Sensor de temperatura desconectado.");
        return RESULT_ERROR;
    }

    rc = az_json_writer_init(&jw, payload_buffer_span, NULL);
    if (az_result_failed(rc)) {
        Serial.println("Error al inicializar el escritor JSON.");
        return RESULT_ERROR;
    }

    rc = az_json_writer_append_begin_object(&jw);
    if (az_result_failed(rc)) {
        Serial.println("Error al abrir el objeto JSON.");
        return RESULT_ERROR;
    }

    // Añadir temperatura al payload
    rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR("temperature"));
    if (az_result_failed(rc)) return RESULT_ERROR;
    rc = az_json_writer_append_double(&jw, temperature, 2);
    if (az_result_failed(rc)) return RESULT_ERROR;

    // Añadir humedad al payload
    rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR("humidity"));
    if (az_result_failed(rc)) return RESULT_ERROR;
    rc = az_json_writer_append_double(&jw, humedad, 2);
    if (az_result_failed(rc)) return RESULT_ERROR;

    // Añadir luminosidad al payload
    rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR("luminosity"));
    if (az_result_failed(rc)) return RESULT_ERROR;
    rc = az_json_writer_append_double(&jw, luminosidad, 2);
    if (az_result_failed(rc)) return RESULT_ERROR;

    rc = az_json_writer_append_end_object(&jw);
    if (az_result_failed(rc)) {
        Serial.println("Error al cerrar el objeto JSON.");
        return RESULT_ERROR;
    }

    *payload_buffer_length = az_span_size(az_json_writer_get_bytes_used_in_destination(&jw));

    Serial.print("Payload generado con éxito: ");
    Serial.println((char*)payload_buffer);

    return RESULT_OK;
}




static int generate_properties_update_response(
    azure_iot_t* azure_iot,
    az_span component_name,
    int32_t frequency,
    int32_t version,
    uint8_t* buffer,
    size_t buffer_size,
    size_t* response_length)
{
  az_result azrc;
  az_json_writer jw;
  az_span response = az_span_create(buffer, buffer_size);

  azrc = az_json_writer_init(&jw, response, NULL);
  EXIT_IF_AZ_FAILED(
      azrc, RESULT_ERROR, "Failed initializing json writer for properties update response.");

  azrc = az_json_writer_append_begin_object(&jw);
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed opening json in properties update response.");

  // This Azure PnP Template does not have a named component,
  // so az_iot_hub_client_properties_writer_begin_component is not needed.

  azrc = az_iot_hub_client_properties_writer_begin_response_status(
      &azure_iot->iot_hub_client,
      &jw,
      AZ_SPAN_FROM_STR(WRITABLE_PROPERTY_TELEMETRY_FREQ_SECS),
      (int32_t)AZ_IOT_STATUS_OK,
      version,
      AZ_SPAN_FROM_STR(WRITABLE_PROPERTY_RESPONSE_SUCCESS));
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed appending status to properties update response.");

  azrc = az_json_writer_append_int32(&jw, frequency);
  EXIT_IF_AZ_FAILED(
      azrc, RESULT_ERROR, "Failed appending frequency value to properties update response.");

  azrc = az_iot_hub_client_properties_writer_end_response_status(&azure_iot->iot_hub_client, &jw);
  EXIT_IF_AZ_FAILED(
      azrc, RESULT_ERROR, "Failed closing status section in properties update response.");

  // This Azure PnP Template does not have a named component,
  // so az_iot_hub_client_properties_writer_end_component is not needed.

  azrc = az_json_writer_append_end_object(&jw);
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed closing json in properties update response.");

  *response_length = az_span_size(az_json_writer_get_bytes_used_in_destination(&jw));

  return RESULT_OK;
}

static int consume_properties_and_generate_response(
    azure_iot_t* azure_iot,
    az_span properties,
    uint8_t* buffer,
    size_t buffer_size,
    size_t* response_length)
{
  int result;
  az_json_reader jr;
  az_span component_name;
  int32_t version = 0;

  az_result azrc = az_json_reader_init(&jr, properties, NULL);
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed initializing json reader for properties update.");

  const az_iot_hub_client_properties_message_type message_type
      = AZ_IOT_HUB_CLIENT_PROPERTIES_MESSAGE_TYPE_WRITABLE_UPDATED;

  azrc = az_iot_hub_client_properties_get_properties_version(
      &azure_iot->iot_hub_client, &jr, message_type, &version);
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed writable properties version.");

  azrc = az_json_reader_init(&jr, properties, NULL);
  EXIT_IF_AZ_FAILED(
      azrc, RESULT_ERROR, "Failed re-initializing json reader for properties update.");

  while (az_result_succeeded(
      azrc = az_iot_hub_client_properties_get_next_component_property(
          &azure_iot->iot_hub_client,
          &jr,
          message_type,
          AZ_IOT_HUB_CLIENT_PROPERTY_WRITABLE,
          &component_name)))
  {
    if (az_json_token_is_text_equal(
            &jr.token, AZ_SPAN_FROM_STR(WRITABLE_PROPERTY_TELEMETRY_FREQ_SECS)))
    {
      int32_t value;
      azrc = az_json_reader_next_token(&jr);
      EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed getting writable properties next token.");

      azrc = az_json_token_get_int32(&jr.token, &value);
      EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed getting writable properties int32_t value.");

      azure_pnp_set_telemetry_frequency((size_t)value);

      result = generate_properties_update_response(
          azure_iot, component_name, value, version, buffer, buffer_size, response_length);
      EXIT_IF_TRUE(
          result != RESULT_OK, RESULT_ERROR, "generate_properties_update_response failed.");
    }
    else
    {
      LogError(
          "Unexpected property received (%.*s).",
          az_span_size(jr.token.slice),
          az_span_ptr(jr.token.slice));
    }

    azrc = az_json_reader_next_token(&jr);
    EXIT_IF_AZ_FAILED(
        azrc, RESULT_ERROR, "Failed moving to next json token of writable properties.");

    azrc = az_json_reader_skip_children(&jr);
    EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed skipping children of writable properties.");

    azrc = az_json_reader_next_token(&jr);
    EXIT_IF_AZ_FAILED(
        azrc, RESULT_ERROR, "Failed moving to next json token of writable properties (again).");
  }

  return RESULT_OK;
}
