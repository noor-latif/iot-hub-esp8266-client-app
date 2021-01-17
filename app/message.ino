#include <ArduinoJson.h>

// Start simulation setup if enabled. Else setup real DHT functions.
#if SIMULATED_DATA
void initSensor()
{
    // use SIMULATED_DATA, no sensor need to be inited
}

float readTemperature(){
    return random(20, 30);
}

float readHumidity(){
    return random(30, 40);
}

#else
float readTemperature(){
    return dht.readTemperature();
}

float readHumidity(){
    return dht.readHumidity();
}
#endif
// End function setup

bool readMessage(int messageId, char *payload)
{
  float temperature = readTemperature();
  float humidity = readHumidity();
  StaticJsonDocument<MESSAGE_MAX_LEN> jsonBuffer;
  JsonObject root = jsonBuffer.to<JsonObject>();
  root["deviceId"] = DEVICE_ID;
  root["messageId"] = messageId;
  bool temperatureAlert = false;

  // NAN is not the valid json, change it to NULL
  if (std::isnan(temperature))
  {
    root["temperature"] = NULL;
  }
  else
  {
    root["temperature"] = temperature;
    if (temperature > TEMPERATURE_ALERT)
    {
      temperatureAlert = true;
    }
  }

  if (std::isnan(humidity))
  {
    root["humidity"] = NULL;
  }
  else
  {
    root["humidity"] = humidity;
  }
  serializeJson(root, payload, MESSAGE_MAX_LEN);
  return temperatureAlert;
}

void parseTwinMessage(char *message)
{
  DynamicJsonDocument root(MESSAGE_MAX_LEN);
  DeserializationError error = deserializeJson(root, message);
  if (!error)
  {
    Serial.printf("Parse %s failed.\r\n", message);
    return;
  }

  if (root["desired"]["interval"].isNull())
  {
    interval = root["desired"]["interval"];
  }
  else if (root.containsKey("interval"))
  {
    interval = root["interval"];
  }
}
