#include <Time.h>
#include <TimeLib.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

byte displayContent[8][8];
const byte displayLength = 56;
unsigned long lastTextUpdate;
const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
IPAddress timeServer(134, 130, 5, 17);
const int timeZone = 2;
EthernetUDP Udp;
const unsigned int localPort = 8888;

int fetchedBusTime = 0;
char busTime[6];
char busDelay[7];
bool fetchNext = 1;

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
  if (millis() - lastTextUpdate > 5000) {
    if (fetchedBusTime == 2) {
      char hourBuf[3];
      char minBuf[3];
      hourBuf[0] = busTime[0];
      hourBuf[1] = busTime[1];
      hourBuf[2] = '\0';
      minBuf[0] = busTime[3];
      minBuf[1] = busTime[4];
      minBuf[2] = '\0';

      int hdiff = atoi(hourBuf) - hour();
      int mdiff = hdiff * 60 + atoi(minBuf) - minute();
      if (mdiff <= 1) {
        fetchNext = 1;
      } else if (mdiff <= 6) {
        fetchNext = 0;
      }
      char text[8];
      snprintf(text, 8, "%d%s", mdiff, busDelay);
      setText(text, 7);
    } else {
      setText("+++",3);
    }
    
    lastTextUpdate = millis();
  }
}

