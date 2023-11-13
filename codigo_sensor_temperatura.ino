#include <OneWire.h>
#include <DallasTemperature.h>
#include "EspMQTTClient.h"

OneWire barramento(4);
DallasTemperature sensor(&barramento);

#define BUZZER 5

EspMQTTClient client(
  "Wokwi-GUEST",         // SSID WiFi
  "",                    // Password WiFi
  "test.mosquitto.org",  // MQTT Broker
  "mqtt-wokwi",          // Client
  1883                   // MQTT port
);

void setup()
{
   Serial.begin(115200);
   sensor.begin();
   pinMode(BUZZER, OUTPUT);

   client.enableDebuggingMessages();
   client.enableHTTPWebUpdater();
   client.enableOTA();
   client.enableLastWillMessage("TestClient/lastwill", "Vou ficar offline");
}

void lerEnviarDados()
{
  sensor.requestTemperatures();
  float temperatura = sensor.getTempCByIndex(0);
  Serial.println(temperatura);

  if (temperatura == -127) {
    digitalWrite(BUZZER, HIGH);
    delay(1000);
    digitalWrite(BUZZER, LOW);
    // Envie um alerta MQTT quando a temperatura atingir -127°C no mesmo tópico
    client.publish("topicowokwi/Temp", "Temperatura atingiu -127°C");
  }
}

void onConnectionEstablished() {
  Serial.println("Conexão MQTT estabelecida com sucesso!");
  // Insira aqui o que deseja fazer quando a conexão é estabelecida.
}

void loop()
{
  lerEnviarDados();
  // Execute o loop MQTT
  client.loop();
}