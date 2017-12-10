#!/bin/bash

ARDUINO_EXEC="/Applications/Arduino.app/Contents/MacOS/Arduino"

function compile {
  $ARDUINO_EXEC --verify BusStopDisplay.ino
}

compile
