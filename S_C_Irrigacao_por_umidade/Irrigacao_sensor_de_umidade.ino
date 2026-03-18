#include <WiFi.h>
#include <WebServer.h>

// ======== CONFIGURAÇÕES DE REDE ========
const char* ssid = "IRRIGACAO_AP";
const char* password = "12345678";

// ======== PINOS ========
const int pinoSensor = 34;   // Sensor HW-080
const int relePin = 16;      // Relê canal 1

// ======== CONFIGURAÇÕES INICIAIS ========
int tempoLigado = 10;         // Segundos que o relé ficará ligado
int tempoPausa = 60;          // Tempo de espera entre ativações
int umidadeLimite = 600;      // Valor analógico mínimo de umidade

bool modoAutomatico = true;   // Automático ou Manual
bool estadoReleManual = false;
bool irrigando = false;

unsigned long anterior = 0;
unsigned long tempoAtual = 0;

WebServer server(80);

// ======== FUNÇÕES ========

void controleIrrigacao() {
  int umidade = analogRead(pinoSensor);
  Serial.print("Umidade: ");
  Serial.println(umidade);

  tempoAtual = millis();

  if (!irrigando && umidade > umidadeLimite) {
    digitalWrite(relePin, LOW); // Liga o relé
    irrigando = true;
    anterior = tempoAtual;
    Serial.println(">> Irrigação INICIADA");
  }

  if (irrigando && tempoAtual - anterior >= tempoLigado * 1000) {
    digitalWrite(relePin, HIGH); // Desliga o relé
    Serial.println(">> Irrigação FINALIZADA");
    delay(tempoPausa * 1000); // Aguarda antes de permitir nova irrigação
    irrigando = false;
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html>";
  html += "<html lang=\"pt-BR\">";
  html += "<head>";
  html += "<meta charset=\"UTF-8\">";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  html += "<title>Configuração do Irrigador</title>";
  html += "<style>";
  html += "  body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; }";
  html += "  h1 { color: #2c3e50; }";
  html += "  form { background: #f9f9f9; padding: 20px; border-radius: 8px; }";
  html += "  input[type='number'] { width: 100px; padding: 5px; margin: 5px 0; }";
  html += "  input[type='submit'], button { background: #3498db; color: white; border: none; padding: 10px 15px; border-radius: 4px; cursor: pointer; }";
  html += "  input[type='submit']:hover, button:hover { background: #2980b9; }";
  html += "  .section { margin-top: 20px; padding: 15px; background: #ecf0f1; border-radius: 8px; }";
  html += "  .hidden { display: none; }";
  html += "</style>";
  html += "</head>";
  html += "<body>";

  html += "<h1>Configuração do Irrigador</h1>";

  // Leitura da umidade
  html += "<div class=\"section\">";
  html += "<h3>Leitura do Sensor</h3>";
  html += "<p>Umidade atual: <b id='valorUmidade'>---</b></p>";
  html += "</div>";

  // Formulário de configuração
  html += "<div class=\"section\">";
  html += "<form action='/set' method='POST'>";
  html += "<label>Tempo Ligado (s): <input type='number' name='ligado' min='1' value='" + String(tempoLigado) + "' required></label><br>";
  html += "<label>Tempo Pausa (s): <input type='number' name='pausa' min='1' value='" + String(tempoPausa) + "' required></label><br>";
  html += "<label>Umidade Limite: <input type='number' name='umidade' min='1' max='100' value='" + String(umidadeLimite) + "' required></label><br>";
  html += "<input type='submit' value='Salvar'>";
  html += "</form>";
  html += "</div>";

  // Controle de modo
  html += "<div class=\"section\">";
  html += "<h2>Modo de Operação</h2>";
  html += "<p>Modo atual: <b id='modoAtual'>" + String(modoAutomatico ? "Automático" : "Manual") + "</b></p>";
  html += "<button onclick='setModo(\"auto\")'>Modo Automático</button> ";
  html += "<button onclick='setModo(\"manual\")'>Modo Manual</button>";
  html += "</div>";

  // Controle manual (inicialmente visível apenas se modo manual)
  html += "<div id='secaoManual' class=\"section\" " + String(modoAutomatico ? "style='display:none;'" : "") + ">";
  html += "<h3>Controle Manual do Relé</h3>";
  html += "<p>Status do Relé: <b id='statusRele'>" + String(estadoReleManual ? "Ligado" : "Desligado") + "</b></p>";
  html += "<button onclick='ligar()'>Ligar Relé</button> ";
  html += "<button onclick='desligar()'>Desligar Relé</button>";
  html += "</div>";

  // Script para AJAX
  html += "<script>";
  html += "function atualizarUmidade() {";
  html += "  fetch('/umidade').then(r => r.text()).then(data => { document.getElementById('valorUmidade').innerText = data; });";
  html += "}";
  html += "function atualizarStatusRele() {";
  html += "  fetch('/status').then(r => r.text()).then(data => { document.getElementById('statusRele').innerText = data; });";
  html += "}";
  html += "function ligar() { fetch('/on').then(() => atualizarStatusRele()); }";
  html += "function desligar() { fetch('/off').then(() => atualizarStatusRele()); }";
  html += "function setModo(modo) {";
  html += "  fetch('/modo?tipo=' + modo).then(() => {";
  html += "    fetch('/getModo').then(r => r.text()).then(modo => {";
  html += "      document.getElementById('modoAtual').innerText = modo;";
  html += "      document.getElementById('secaoManual').style.display = (modo === 'Manual') ? 'block' : 'none';";
  html += "    });";
  html += "  });";
  html += "}";
  html += "function atualizarModo() {";
  html += "  fetch('/getModo').then(r => r.text()).then(modo => {";
  html += "    document.getElementById('modoAtual').innerText = modo;";
  html += "    document.getElementById('secaoManual').style.display = (modo === 'Manual') ? 'block' : 'none';";
  html += "  });";
  html += "}";
  html += "setInterval(atualizarUmidade, 1000);";
  html += "setInterval(atualizarStatusRele, 2000);";
  html += "setInterval(atualizarModo, 3000);"; // Atualiza o modo periodicamente
  html += "</script>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleSet() {
  if (server.hasArg("ligado")) tempoLigado = server.arg("ligado").toInt();
  if (server.hasArg("pausa")) tempoPausa = server.arg("pausa").toInt();
  if (server.hasArg("umidade")) umidadeLimite = server.arg("umidade").toInt();
  server.sendHeader("Location", "/");
  server.send(303);
}

void setupWiFi() {
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Iniciado");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", HTTP_POST, handleSet);

  // Rotas para controle via AJAX
  server.on("/modo", HTTP_GET, []() {
    if (server.hasArg("tipo")) {
      String tipo = server.arg("tipo");
      if (tipo == "auto") {
        modoAutomatico = true;
        digitalWrite(relePin, HIGH);
        estadoReleManual = false;
      } else if (tipo == "manual") {
        modoAutomatico = false;
      }
    }
    server.send(200, "text/plain", "ok");
  });

  server.on("/on", []() {
    if (!modoAutomatico) {
      digitalWrite(relePin, LOW);
      estadoReleManual = true;
    }
    server.send(200, "text/plain", "ok");
  });

  server.on("/off", []() {
    if (!modoAutomatico) {
      digitalWrite(relePin, HIGH);
      estadoReleManual = false;
    }
    server.send(200, "text/plain", "ok");
  });

  server.on("/umidade", []() {
    int leitura = analogRead(pinoSensor);
    server.send(200, "text/plain", String(leitura));
  });

  server.on("/status", []() {
    server.send(200, "text/plain", estadoReleManual ? "Ligado" : "Desligado");
  });

  server.on("/getModo", []() {
    server.send(200, "text/plain", modoAutomatico ? "Automático" : "Manual");
  });

  server.begin();
}

// ======== SETUP & LOOP ========

void setup() {
  Serial.begin(115200);
  pinMode(relePin, OUTPUT);
  digitalWrite(relePin, HIGH); // Inicia com relé desligado
  setupWiFi();
}

void loop() {
  server.handleClient();
  if (modoAutomatico) {
    controleIrrigacao();
  }
}