#include <ESP8266WiFi.h>
#include <espnow.h>

// Receiver MAC Address
uint8_t receiverMAC[] = {0xEC, 0xFA, 0xBC, 0xC2, 0xD6, 0x4E};  // Change this to your receiver's MAC address

#define BUTTON_PIN 5  // GPIO5 (D1 on NodeMCU)

typedef struct struct_message {
    int buttonState;
} struct_message;

struct_message dataToSend;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    Serial.print("Send Status: ");
    Serial.println(sendStatus == 0 ? "Success" : "Fail");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Internal pull-up resistor

    if (esp_now_init() != 0) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);
    esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

void loop() {
    int buttonState = digitalRead(BUTTON_PIN);
    dataToSend.buttonState = (buttonState == LOW) ? 1 : 0;  // Button pressed -> LED ON, otherwise LED OFF

    esp_now_send(receiverMAC, (uint8_t *)&dataToSend, sizeof(dataToSend));
    delay(500);  // Send data every 500ms
}
