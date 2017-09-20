
byte content[8][32];

void setup() {
  initLED();
  
}


void loop() {
  sendToAll(1, B10101010);
  sendToAll(2, 0);
  delay(1000);
  sendToAll(1, 0);
  sendToAll(2, B01010101);
  delay(1000);
}

void clearContent() {
  for(int i = 0; i < 8; i++) {
    for (int j = 0; j < 32; j++) {
      content[i][j] = 0;
    }
  }
}
