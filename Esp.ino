#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "lab14";
const char* password = "expert1234";

WebServer server(80);  // Set up server on port 80

void handleGridSelection() {
  if (server.hasArg("grid")) {
    String grid = server.arg("grid");
    Serial.println("Received grid selection: " + grid);
    server.send(200, "text/plain", "Received: " + grid);
  } else {
    server.send(400, "text/plain", "Grid parameter missing");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/button", handleGridSelection);  // Define route
  server.begin();
}

void loop() {
  server.handleClient();
}
