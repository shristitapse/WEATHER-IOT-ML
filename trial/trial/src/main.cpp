// trial code to connect to wifi and send hardcoded data to thingsboard and display on dashboard

#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi Credentials
const char* ssid = "tapse";        
const char* pass = "haikyuuu";    

// ThingsBoard Cloud Configuration
const char* thingsboardServer = "thingsboard.cloud";  
const char* accessToken = "ll6efb2mwWipNJR7QKXl";        

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int attempts = 0; // Counter for Wi-Fi connection attempts

// Function to connect to Wi-Fi
void connectWiFi() {
  Serial.println("Starting WiFi connection...");
  WiFi.mode(WIFI_STA);

  while ((WiFi.status() != WL_CONNECTED) && (attempts < 36)) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    delay(5000);
    attempts++;
    Serial.print(".");
    if (attempts == 35) {
      Serial.println("NO WIFI. Restarting...");
      ESP.restart();
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WIFI.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

// Function to connect to ThingsBoard
void connectThingsBoard() {
  Serial.println("Connecting to ThingsBoard...");
  client.setServer(thingsboardServer, 1883); // ThingsBoard MQTT broker on port 1883

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ThingsBoard...");
    if (client.connect("NodeMCU", accessToken, NULL)) {
      Serial.println("Connected to ThingsBoard!");
    } else {
      Serial.print("Failed to connect. MQTT Error code: ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}

// Function to send telemetry data to ThingsBoard
void sendTelemetry() {
  
  float temperature = 29.5; 
  float humidity = 40.0;    

  // Create telemetry payload in JSON format
  String payload = "{\"temperature\":";
  payload += temperature;
  payload += ",\"humidity\":";
  payload += humidity;
  payload += "}";

  // Publish telemetry to ThingsBoard
  if (client.publish("v1/devices/me/telemetry", payload.c_str())) {
    Serial.println("Telemetry sent successfully: " + payload);
  } else {
    Serial.println("Failed to send telemetry.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  connectWiFi();         // Connect to Wi-Fi
  connectThingsBoard();  // Connect to ThingsBoard
}

void loop() {
  if (!client.connected()) {
    connectThingsBoard(); // Reconnect to ThingsBoard if disconnected
  }
  client.loop(); // Keep MQTT connection alive

  sendTelemetry(); // Send telemetry data to ThingsBoard
  delay(10000);    // Send data every 10 seconds
}
