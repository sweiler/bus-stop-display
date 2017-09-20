
byte displayContent[8][32];
const byte displayLength = 56;

void setup() {
  initLED();
  



  setText("1239456780", 10);
}


void loop() {
  updateLED();
}

