#include <Time.h>
#include <TimeLib.h>

byte displayContent[8][32];
const byte displayLength = 56;
long lastTextUpdate;

void setup() {
  Serial.begin(9600);
  initLED();
  
  setText("111", 10);
  lastTextUpdate = millis();
}


void loop() {
  updateLED();
  if (millis() - lastTextUpdate > 1000) {
    char text[10];
    sprintf(text, "%d %d", hour(), minute());
    Serial.println(text);
    setText(text, 10);
    lastTextUpdate = millis();
  }
}

