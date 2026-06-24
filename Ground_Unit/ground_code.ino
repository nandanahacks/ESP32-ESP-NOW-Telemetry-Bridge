/* GROUND UNIT (USB Side) - LOW LATENCY TELEMETRY
   Target: AIR UNIT (04:83:08:0E:7E:38)
*/
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x04, 0x83, 0x08, 0x0E, 0x7E, 0x38}; 

void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t *incomingData, int len) {
  Serial.write(incomingData, len); // Forward over-the-air data to USB
}

void setup() {
  Serial.begin(115200); 
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) return;

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo)); 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA; 
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) return;

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  uint8_t data[250]; // ESP-NOW max payload size is 250 bytes
  int len = 0;

  // Read whatever is available right now without waiting
  while (Serial.available() && len < 250) {
    data[len] = Serial.read();
    len++;
    // Small micro-delay to let fast-incoming serial bytes accumulate
    delayMicroseconds(100); 
  }

  // If we collected any data, blast it immediately
  if (len > 0) {
    esp_now_send(broadcastAddress, data, len);
  }
}