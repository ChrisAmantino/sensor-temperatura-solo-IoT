#include <OneWire.h>
#include <DallasTemperature.h>
#include "EspMQTTClient.h"

// Inicializa o barramento OneWire no pino digital 4
OneWire barramento(4);

// Cria um objeto DallasTemperature associado ao barramento OneWire
DallasTemperature sensor(&barramento);

// Define o pino digital 5 como BUZZER
#define BUZZER 5

// Configura o cliente MQTT com as informações de conexão
EspMQTTClient client(
  "Wokwi-GUEST",         // SSID WiFi
  "",                    // Password WiFi
  "test.mosquitto.org",  // MQTT Broker
  "mqtt-wokwi",          // Client
  1883                   // MQTT port
);

void setup()
{
   // Inicializa a comunicação serial a 115200 bps
   Serial.begin(115200);

   // Inicializa o sensor DS18B20
   sensor.begin();

   // Configura o pino BUZZER como saída
   pinMode(BUZZER, OUTPUT);

   // Ativa mensagens de depuração no cliente MQTT
   client.enableDebuggingMessages();

   // Ativa a atualização via HTTP no cliente MQTT
   client.enableHTTPWebUpdater();

   // Ativa o OTA (Over-The-Air update) no cliente MQTT
   client.enableOTA();

   // Configura uma mensagem "Last Will" a ser publicada se a conexão MQTT for perdida
   client.enableLastWillMessage("TestClient/lastwill", "Vou ficar offline");
}

// Função para ler a temperatura e enviar dados
void lerEnviarDados()
{
  // Solicita a leitura da temperatura ao sensor
  Serial.println("Capturando Temperatura");
  sensor.requestTemperatures();

  // Lê a temperatura do sensor
  float temperatura = sensor.getTempCByIndex(0);

  // Imprime a temperatura no monitor serial
  Serial.println(temperatura);

  // Verifica se a temperatura é -127°C (valor de erro)
  if (temperatura == 25) {
    // Aciona o BUZZER por 1 segundo
    Serial.println("Temperatura limite atingida, emitindo sinal sonoro!");
    digitalWrite(BUZZER, HIGH);
    delay(1000);
    digitalWrite(BUZZER, LOW);

    // Publica uma mensagem MQTT informando a condição
    client.publish("topicowokwi/Temp", "Temperatura atingiu -127°C");
  }
}

// Função chamada quando a conexão MQTT é estabelecida
void onConnectionEstablished() {
  Serial.println("Conexão MQTT estabelecida com sucesso!");
}

void loop()
{
  // Chama a função para ler e enviar dados de temperatura
  lerEnviarDados();

  // Executa o loop MQTT para processar mensagens e manter a conexão
  client.loop();
}