
byte displayContent[8][32];
const byte displayLength = 40;

void setup() {
  initLED();
  displayContent[6][0] = B00000000;
  displayContent[5][0] = B00100100;
  displayContent[4][0] = B01011010;
  displayContent[3][0] = B01000010;
  displayContent[2][0] = B00100100;
  displayContent[1][0] = B00011000;
}


void loop() {
  updateLED();
  delay(100);
}

