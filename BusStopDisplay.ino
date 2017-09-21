#include <Time.h>
#include <TimeLib.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte displayContent[8][5];
const byte displayLength = 40;
long lastTextUpdate;
const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
IPAddress timeServer(132, 163, 4, 101);
const int timeZone = 2;
EthernetUDP Udp;
const unsigned int localPort = 8888;


void setup() {
  if (Ethernet.begin(mac) == 0) {
    // no point in carrying on, so do nothing forevermore:
    while (1) {
      Serial.println(F("Failed to configure Ethernet using DHCP"));
      delay(10000);
    }
  }
  delay(400);
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(60);
  delay(500);
  initLED();
  //setText("111", 10);
  lastTextUpdate = millis();
}


void loop() {
  updateLED();
  updateHttp();
  if (millis() - lastTextUpdate > 1000) {
    //char text[10];
    //sprintf(text, "%d %d", hour(), minute());
    //setText(text, 8);
    lastTextUpdate = millis();
  }
}

