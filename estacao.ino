// Bibliotecas necessárias para o Projeto
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

float temperatura, umidade, pressao, altitude;

// Nome da rede e senha para conexão
const char* ssid     = "NOME DA REDE";              // Inserir dados de Rede
const char* senha    = "SENHA";                     // Inserir dados de Rede

WebServer server(80);

// Informações de acesso para rede de internet / IP Fixo
IPAddress local_IP(192, 168, 0, 100);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

void setup() {
  Serial.begin(115200);
  delay(100);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  bme.begin(0x76);

  Serial.println("Conectando a ");
  Serial.println(ssid);

  //Conecta à Rede Wifi indicada anteriormente
  WiFi.begin(ssid, senha);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado ..!");
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  temperatura = bme.readTemperature();
  umidade = bme.readHumidity();
  pressao = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  server.send(200, "text/html", SendHTML(temperatura, umidade, pressao, altitude));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

// Informações da página Web criada
String SendHTML(float temperatura, float umidade, float pressao, float altitude) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";

// Título da Guia da Web.
  ptr += "<title>ESP32 + BME280</title>\n";

  // Configurações de cor e padrões de exibição
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";

  // Título impresso na Página Web Criada
  ptr += "<h1>ESP32 + BME280</h1>\n";

  // Informações de Temperatura
  ptr += "<p>Temperatura: ";
  ptr += temperatura;
  ptr += "&deg;C</p>";

  // Informações de Umidade
  ptr += "<p>Umidade: ";
  ptr += umidade;
  ptr += "%</p>";

  // Informações de Pressão
  ptr += "<p>Pressao: ";
  ptr += pressao;
  ptr += "hPa</p>";

  // Informações de altitude

  ptr += "<p>Altitude: ";
  ptr += altitude;
  ptr += "m</p>";
  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}
