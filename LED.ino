#include <SPI.h>

const int slaveSelectPin = 5;

byte currentOffset;
unsigned long lastUpdate;

void initLED() {
  pinMode(slaveSelectPin, OUTPUT);
  SPI.begin();
  digitalWrite(slaveSelectPin, HIGH);

  sendToAll(0x0C, 0x01); // enable display
  sendToAll(0x09, 0x00); // set decode mode to off
  sendToAll(0x0A, 0x0F); // Intensity to full on
  sendToAll(0x0B, 0x07); // Scan limit to 'scan all'
  sendToAll(0x0F, 0);    // set test to off
  clearScreen();

  currentOffset = 0;
  lastUpdate = millis();
}

void clearScreen() {
  for (byte i = 1; i <= 8; i++) {
    sendToAll(i, 0x00);
  }
}

void updateLED() {
  if(millis() - lastUpdate > 1000) {
    currentOffset = currentOffset + 1 % displayLength;
    for (byte y = 0; y < 8; y++) {
      byte addr = y + 1;
      byte addresses[4] = {addr, addr, addr, addr};
      byte values[4] = {0, 0, 0, 0};
      for (byte module = 0; module < 4; module++) {
        for (byte i = 0; i < 8; i++) {
          byte x = (currentOffset + i + module * 8) % displayLength;
          byte segment = x / 8;
          byte shiftRight = x % 8;
          byte shifted = displayContent[y][segment] >> shiftRight;
          byte masked = shifted & 0x01;
          values[3 - module] = values[3 - module] | (masked << i);
        }
      }
      sendData(addresses, values);
    }
  }
}

void sendToAll(byte address, byte value) {
  byte addresses[4] = {address, address, address, address};
  byte values[4] = {value, value, value, value};
  sendData(addresses, values);
}

void sendData(byte addresses[], byte values[]) {
  SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE0));
  digitalWrite(slaveSelectPin, LOW);
  for (int i = 0; i < 4; i++) {
    SPI.transfer(addresses[i]);
    SPI.transfer(values[i]);
  }
  digitalWrite(slaveSelectPin, HIGH);
  SPI.endTransaction();
}
