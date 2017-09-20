#include <SPI.h>

const int slaveSelectPin = 5;

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
}

void clearScreen() {
  for (int i = 1; i <= 8; i++) {
    sendToAll(i, 0x00);
  }
}

void showDisplay(byte content[8][32]) {
  for (int y = 0; y < 8; y++) {
    byte addr = y + 1;
    byte addresses[4] = {addr, addr, addr, addr};
    byte values[4];
    for (int module = 0; module < 4; module++) {
      byte value = 0;
      for (int x = 0; x < 8; x++) {
        value = value | (content[y][8 * module + x] << x);
      }
      values[3 - module] = value;
    }
    sendData(addresses, values);
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
