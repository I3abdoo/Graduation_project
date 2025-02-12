#include <ESP8266WiFi.h>
#include <espnow.h>

// Struct to hold the message data
struct Message {
  float length;    // Length (e.g., distance in meters)
  float speed;     // Speed (e.g., in m/s)
  char location[50]; // Location (e.g., GPS coordinates as string)
};

// Global message variables
Message currentMessage = {100.5, 25.8, "35.6895° N, 139.6917° E"}; // Example values
Message receivedMessage; // Variable to hold received message

// Callback when data is sent
void onDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Send Status: ");
  Serial.println(sendStatus == 0 ? "Success" : "Fail");
}

// Callback when data is received
void onDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&receivedMessage, incomingData, sizeof(receivedMessage));
  Serial.print("Received - Length: ");
  Serial.print(receivedMessage.length);
  Serial.print(" meters, Speed: ");
  Serial.print(receivedMessage.speed);
  Serial.print(" m/s, Location: ");
  Serial.println(receivedMessage.location);
}

void setup() {
  Serial.begin(115200);

  // Initialize WiFi in station mode
  WiFi.mode(WIFI_STA);

  // Print the MAC address
  Serial.print("Device MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESPNOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESPNOW");
    return;
  }

  // Set up the send and receive callbacks
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(onDataSent);
  esp_now_register_recv_cb(onDataRecv);

  // Add broadcast peer
  uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
}

void loop() {
  // Update the struct dynamically if needed
  static unsigned long lastChange = 0;
  if (millis() - lastChange > 10000) {  // Update every 10 seconds
    currentMessage.length = random(50, 200);    // Random length (50 to 200 meters)
    currentMessage.speed = random(10, 50) / 10.0; // Random speed (1.0 to 5.0 m/s)
    snprintf(currentMessage.location, sizeof(currentMessage.location), "Latitude: %d° N, Longitude: %d° E", random(30, 40), random(70, 80)); // Random location
    Serial.println("Message updated!");
    lastChange = millis();
  }

  // Broadcast message
  esp_now_send(NULL, (uint8_t *)&currentMessage, sizeof(currentMessage));
  delay(5000); // Send every 5 seconds
}
