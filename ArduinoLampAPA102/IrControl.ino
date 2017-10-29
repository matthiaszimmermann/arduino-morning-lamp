// IR control "business logic"
void actOnCode(String code) {
  int codeFound = -1;
  
  if(!code.startsWith(codeInit)) {
    logIr("unknown code: " + code, true);    
    return;
  }
  
  code = code.substring(codeInit.length());

  if(code == codePower) {
    codeFound = 1;

    if(powerOn) { 
      playerOn = false; 
      powerOn = false; 
    }
    else { 
      powerOn = true; 
    }
  }

  if(code == codeVolumeUp) {
    codeFound = 2;
    changeBrightness(BRIGHTNESS_STEP);
  }

  if(code == codeVolumeDown) {
    codeFound = 3;
    changeBrightness(-BRIGHTNESS_STEP);
  }

  if(code == codePlay) {
    codeFound = 4;
    
    if(playerOn) { 
      playerOn = false; 
    }
    else { 
      playerOn = true; 
      currentPixel = 0; 
    }
  }
  
  if(code == codeColor1) {
    codeFound = 10;
    setColorCode(presetColor(0));
  }
  if(code == codeColor2) {
    codeFound = 11;
    setColorCode(presetColor(1));
  }
  if(code == codeColor3) {
    codeFound = 12;
    setColorCode(presetColor(2));
  }
  if(code == codeColor4) {
    codeFound = 13;
    setColorCode(presetColor(3));
  }
  if(code == codeColor5) {
    codeFound = 14;
    setColorCode(presetColor(4));
  }
  if(code == codeColor6) {
    codeFound = 15;
    setColorCode(presetColor(5));
  }
  
  if(codeFound > 0) {
    logIr("actOnCode() IR code: " + codeFound, true);
    
    if(powerOn) { 
      updateLedStrip();
    }
    else {
      setStripToColor(0, 0, 0, true);
    }
  }
  else {
    logIr("actOnCode() Unknown code: " + code, true);    
  }
}

/** 
 * the main IR loop 
 * see https://learn.adafruit.com/ir-sensor/using-an-ir-sensor
 */
void loopIr() {
  uint16_t highpulse, lowpulse; // temporary storage timing
  highpulse = lowpulse = 0; // start out with no pulse length

  // while pin is still HIGH
  while (PORT_IR & (1 << PIN_IR)) {
    highpulse++;
    delayMicroseconds(RESOLUTION);

    // If the pulse is too long, we 'timed out' - either nothing
    // was received or the code is finished, so print what
    // we've grabbed so far, and then reset
    if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
      actOnCode(parsePulses());
      currentpulse = 0;
      return;
    }
  }
  // we didn't time out so lets stash the reading
  pulses[currentpulse][0] = highpulse;

  // same as above for low pin
  while (! (PORT_IR & _BV(PIN_IR))) {
    lowpulse++;
    delayMicroseconds(RESOLUTION);
    if ((lowpulse >= MAXPULSE) && (currentpulse != 0)) {
      actOnCode(parsePulses());
      currentpulse = 0;
      return;
    }
  }
  // we didn't time out so lets stash the reading
  pulses[currentpulse][1] = lowpulse;

  // we read one high-low pulse successfully, continue!
  currentpulse++;
}

String parsePulses() {
  int code_start = findStart();
  int code_end = findEnd(code_start);
  String irCode = "";

  if(code_start >= 0 && code_end >= 0) {
    irCode += '<';

    for(int i = code_start; i < code_end; i++) {
      irCode += mapToBit(i);
    }

    irCode += '>';
  }

  return irCode;
}

// searches for start pattern of logitech ir control
int findStart() {
  int i = 0;

  while(i < currentpulse - 2) {
    if(inRange(pulses[i][1], 440 - 10,  440 + 5) && inRange(pulses[i+1][0], 220 - 10,  220 + 5)) {
      return i + 2;
    }

    i++;
  }

  return -1;
}

// searches for end pattern of logitech ir control
int findEnd(int i) {
  if(i < 0) {
    return -1;
  }

  while(i < currentpulse) {
    if(inRange(pulses[i][0], 2000 - 25,  2000 + 7) && inRange(pulses[i][1], 440 - 10,  440 + 5)) {
      return i;
    }

    i++;
  }

  return currentpulse;
}

char mapToBit(int i) {
  if(inRange(pulses[i][0], 26 - 10, 26 + 5)) { 
    return '0'; 
  }
  if(inRange(pulses[i][0], 80 - 10, 80 + 32)) { 
    return '1'; 
  }
  
  logIr("mapToBit() pulses[" + i, false);
  logIr("][0]: " + pulses[i][0], true);
  
  return '?';
}

boolean inRange(int value, int min_value, int max_value) {
  if(value >= min_value && value <= max_value) {
    return true;
  }
  
  return false;
}

void logIr(String text, boolean newline) {
  if(IR_DEBUG) {
    if(newline) { Serial.println(text); }
    else        { Serial.print(text); }
  }
}
