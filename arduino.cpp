#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "FLIA_CONDORI";
const char* password = "27071988"; 

WebServer server(80);

const int pins[8] = {13, 12, 14, 27, 26, 25, 33, 32};

void setup() {
  Serial.begin(115200);
  for(int i=0; i<8; i++) { 
    pinMode(pins[i], OUTPUT); 
    digitalWrite(pins[i], LOW); 
  }
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print("."); 
  }
  
  Serial.println("");
  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());

  server.on("/toggle", [](){
    server.sendHeader("Access-Control-Allow-Origin", "*"); 
    if (server.hasArg("id")) {
      int id = server.arg("id").toInt();
      digitalWrite(pins[id], !digitalRead(pins[id]));
    }
    server.send(200, "text/plain", "OK");
  });
  
  server.begin();
}

void loop() { 
  server.handleClient(); 
}