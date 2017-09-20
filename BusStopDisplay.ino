
byte displayContent[8][32];
const byte displayLength = 56;

void setup() {
  initLED();
  



  setText("12394567", 8);
}


void loop() {
  updateLED();
}

