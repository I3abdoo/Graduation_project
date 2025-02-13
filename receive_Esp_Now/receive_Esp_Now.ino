#include <ESP8266WiFi.h>
#include <espnow.h>

#define LED_PIN 4  // GPIO4 (D2 on NodeMCU)

typedef struct struct_message {
    int buttonState;
} struct_message;

struct_message receivedData;

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    Serial.print("Received Button State: ");
    Serial.println(receivedData.buttonState);
    
    digitalWrite(LED_PIN, receivedData.buttonState == 1 ? HIGH : LOW);
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);  // Turn off LED initially

    if (esp_now_init() != 0) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
}
