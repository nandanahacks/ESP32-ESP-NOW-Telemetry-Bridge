/* AIR UNIT (Drone Side) - LOW LATENCY TELEMETRY
   Target: GROUND UNIT (04:83:08:0E:82:2C)
*/
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0x04, 0x83, 0x08, 0x0E, 0x82, 0x2C}; 

#define RXD2 16
#define TXD2 17

void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t *incomingData, int len) {
  Serial2.write(incomingData, len); // Forward over-the-air data to Flight Controller
}

void setup() {
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
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
  uint8_t data[250];
  int len = 0;

  // Read whatever the Flight Controller is spitting out right now
  while (Serial2.available() && len < 250) {
    data[len] = Serial2.read();
    len++;
    delayMicroseconds(100); 
  }

  if (len > 0) {
    esp_now_send(broadcastAddress, data, len);
  }
}