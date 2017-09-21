#include <SPI.h>

const int slaveSelectPin = 5;

byte currentOffset;
unsigned long lastUpdate;

const byte digits[10][8] = {
  {
    B0000,
    B0110,
    B1001,
    B1001,
    B1001,
    B1001,
    B0110,
    B0000
  },
  {
    B0000,
    B0001,
    B0011,
    B0101,
    B1001,
    B0001,
    B0001,
    B0000
  },
  {
    B0000,
    B0110,
    B1001,
    B0001,
    B0010,
    B0100,
    B1111,
    B0000
  },
  {
    B0000,
    B1110,
    B0001,
    B0110,
    B0001,
    B0001,
    B1110,
    B0000
  },
  {
    B0000,
    B0010,
    B0110,
    B1010,
    B1111,
    B0010,
    B0010,
    B0000
  },
  {
    B0000,
    B1111,
    B1000,
    B1110,
    B0001,
    B0001,
    B1110,
    B0000
  },
  {
    B0000,
    B0110,
    B1000,
    B1110,
    B1001,
    B1001,
    B0110,
    B0000
  },
  {
    B0000,
    B1111,
    B0001,
    B0001,
    B0010,
    B0100,
    B1000,
    B0000
  },
  {
    B0000,
    B0110,
    B1001,
    B0110,
    B1001,
    B1001,
    B0110,
    B0000
  },
  {
    B0000,
    B0110,
    B1001,
    B1001,
    B0111,
    B0001,
    B1110,
    B0000
  }
};

const byte space[8] = {
  B0000,
  B0000,
  B0000,
  B0000,
  B0000,
  B0000,
  B0000,
  B0000
};


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
    byte *digit;
    
    if (text[i] < 0x30 || text[i] > 0x39) {
      digit = space;
    } else {
      digit = digits[text[i] - 0x30];
    }
    byte firstSegment = (i * 5 - 1) / 8;
    byte secondSegment = (i * 5 + 4) / 8;
    byte shiftLeft = 8 - ((i * 5 + 4) % 8);
    byte shiftRight = 8 - shiftLeft;
    
    for (int y = 0; y < 8; y++) {
      if (i != 0) {
        displayContent[7 - y][firstSegment] = displayContent[7 - y][firstSegment] | (digit[y] >> shiftRight);
      }
      displayContent[7 - y][secondSegment] = displayContent[7 - y][secondSegment] | (digit[y] << shiftLeft);
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
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  digitalWrite(slaveSelectPin, LOW);
  for (int i = 0; i < 4; i++) {
    SPI.transfer(addresses[i]);
    SPI.transfer(values[i]);
  }
  digitalWrite(slaveSelectPin, HIGH);
  SPI.endTransaction();
}
