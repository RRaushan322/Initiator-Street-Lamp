#include <ESP8266WiFi.h>
#include <espnow.h>

// Variables for test data
  int led_status=0;
  int change_data=0;
#define LED_PIN 5
#define BUTTON_PIN 6
byte lastButtonState = LOW;
byte ledState = LOW;
unsigned long debounceDuration = 50; // millis
unsigned long lastTimeButtonStateChanged = 0;
// REPLACE WITH RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xB8, 0xD6, 0x1A, 0x43, 0x5A, 0x00};
// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
} struct_message;

// Create a struct_message called myData
struct_message myData;


// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
void ledchange(){
  digitalWrite(LED_PIN, ledState);
  change_data=0;
}

 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
   pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
 
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ERR_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {
   // Create test data
   if (millis() - lastTimeButtonStateChanged > debounceDuration) {
    byte buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState) {
      lastTimeButtonStateChanged = millis();
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        ledState = (ledState == HIGH) ? LOW: HIGH;
        change_data=1;
      }
    }
  }
    // Send message via ESP-NOW
    if(change_data==1){
 ledchange();
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    }
}