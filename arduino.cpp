#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h> // Librería para no hardcodear la red WiFi
#include <ESPmDNS.h>     // Librería para usar el dominio local casa.local

WebServer server(80);

const int pins[8] = {13, 12, 14, 27, 26, 25, 33, 32};

void setup() {
  Serial.begin(115200);
  
  // Configurar los pines de los relés como salidas
  for(int i = 0; i < 8; i++) { 
    pinMode(pins[i], OUTPUT); 
    digitalWrite(pins[i], LOW); // Iniciar apagados por defecto
  }
  
  // Iniciar WiFiManager
  WiFiManager wm;
  
  // Si el ESP32 no encuentra una red WiFi guardada, creará su propia red
  // llamada "Casa_Domotica_Config" para que te conectes y lo configures.
  bool res = wm.autoConnect("Casa_Domotica_Config");
  if(!res) {
    Serial.println("Fallo al conectar a la red WiFi");
  } else {
    Serial.println("¡Conectado exitosamente al WiFi!");
  }

  Serial.println("");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());

  // Iniciar el servicio mDNS
  if (!MDNS.begin("casa")) {
    Serial.println("Error al configurar mDNS");
  } else {
    Serial.println("mDNS iniciado. Puedes entrar desde tu navegador web a http://casa.local");
  }

  // Ruta /toggle: Recibe peticiones para encender o apagar relés
  server.on("/toggle", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*"); 
    
    // Si la página web solo está enviando un ping para verificar la conexión
    if (server.hasArg("id") && server.arg("id") == "ping") {
      server.send(200, "text/plain", "PONG");
      return; // Sale de la función sin tocar los pines
    }

    // Si se recibe un ID numérico, cambia el estado del relé correspondiente
    if (server.hasArg("id")) {
      int id = server.arg("id").toInt();
      if(id >= 0 && id < 8) { // Medida de seguridad
        digitalWrite(pins[id], !digitalRead(pins[id]));
      }
    }
    server.send(200, "text/plain", "OK");
  });

  // NUEVA RUTA /status: Devuelve el estado actual de todas las luces
  server.on("/status", []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    
    // Construye una respuesta en formato JSON, ejemplo: [0,1,0,0,1,0,0,0]
    String json = "[";
    for(int i = 0; i < 8; i++) {
      json += String(digitalRead(pins[i]));
      if(i < 7) json += ",";
    }
    json += "]";
    
    server.send(200, "application/json", json);
  });
  
  server.begin();
}

void loop() { 
  server.handleClient(); 
  delay(2); // Pausa corta para estabilidad del ESP32
}