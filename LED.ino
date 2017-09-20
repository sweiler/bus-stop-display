#include <SPI.h>

const int slaveSelectPin = 5;

byte currentOffset;
unsigned long lastUpdate;

byte digits[10][8];



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
  lastUpdate = 0;

  digits[1][7] = B0000;
  digits[1][6] = B0001;
  digits[1][5] = B0011;
  digits[1][4] = B0101;
  digits[1][3] = B1001;
  digits[1][2] = B0001;
  digits[1][1] = B0001;
  digits[1][0] = B0000;

  digits[2][7] = B0000;
  digits[2][6] = B0110;
  digits[2][5] = B1001;
  digits[2][4] = B0001;
  digits[2][3] = B0010;
  digits[2][2] = B0100;
  digits[2][1] = B1111;
  digits[2][0] = B0000;

  digits[3][7] = B0000;
  digits[3][6] = B1110;
  digits[3][5] = B0001;
  digits[3][4] = B0110;
  digits[3][3] = B0001;
  digits[3][2] = B0001;
  digits[3][1] = B1110;
  digits[3][0] = B0000;

  digits[9][7] = B0000;
  digits[9][6] = B0110;
  digits[9][5] = B1001;
  digits[9][4] = B1001;
  digits[9][3] = B0111;
  digits[9][2] = B0001;
  digits[9][1] = B1110;
  digits[9][0] = B0000;

  digits[4][7] = B0000;
  digits[4][6] = B0010;
  digits[4][5] = B0110;
  digits[4][4] = B1010;
  digits[4][3] = B1111;
  digits[4][2] = B0010;
  digits[4][1] = B0010;
  digits[4][0] = B0000;

  digits[5][7] = B0000;
  digits[5][6] = B1111;
  digits[5][5] = B1000;
  digits[5][4] = B1110;
  digits[5][3] = B0001;
  digits[5][2] = B0001;
  digits[5][1] = B1110;
  digits[5][0] = B0000;

  digits[6][7] = B0000;
  digits[6][6] = B0110;
  digits[6][5] = B1000;
  digits[6][4] = B1110;
  digits[6][3] = B1001;
  digits[6][2] = B1001;
  digits[6][1] = B0110;
  digits[6][0] = B0000;

  digits[7][7] = B0000;
  digits[7][6] = B1111;
  digits[7][5] = B0001;
  digits[7][4] = B0001;
  digits[7][3] = B0010;
  digits[7][2] = B0100;
  digits[7][1] = B1000;
  digits[7][0] = B0000;
}

void clearScreen() {
  for (byte i = 1; i <= 8; i++) {
    sendToAll(i, 0x00);
  }
}

void setText(char* text, byte len) {
  for (int i = 0; i < 32; i++) {
    for (int y = 0; y < 8; y++) {
      displayContent[i][y] = 0;
    }
  }
  for (int i = 0; i < len; i++) {
    byte *digit = digits[text[i] - 0x30];
    byte firstSegment = (i * 5) / 8;
    byte secondSegment = (i * 5 + 5) / 8;
    byte shiftLeft = 8 - ((i * 5 + 4) % 8);
    byte shiftRight = 8 - shiftLeft;
    
    for (int y = 0; y < 8; y++) {
      displayContent[y][firstSegment] = displayContent[y][firstSegment] | (digit[y] >> shiftRight);
      displayContent[y][secondSegment] = displayContent[y][secondSegment] | (digit[y] << shiftLeft);
    }
  }
}

void updateLED() {
  if(millis() - lastUpdate > 100) {
    for (byte y = 0; y < 8; y++) {
      byte addr = y + 1;
      byte addresses[4] = {addr, addr, addr, addr};
      byte values[4] = {0, 0, 0, 0};
      for (byte module = 0; module < 4; module++) {
        for (byte i = 0; i < 8; i++) {
          byte x = (currentOffset + i + module * 8) % displayLength;
          byte segment = x / 8;
          byte shiftRight = x % 8;
          byte shifted = displayContent[y][segment] >> (7 - shiftRight);
          byte masked = shifted & 0x01;
          values[3 - module] = values[3 - module] | (masked << i);
        }
      }
      sendData(addresses, values);
    }
    currentOffset = (currentOffset + 1) % displayLength;
    lastUpdate = millis();
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
