
// IR control constants
#define PORT_IR PIND
#define PIN_IR 2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 3000

// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20

#define START_PULSE_A 370
#define START_PULSE_B 180
#define START_PULSE_DELTA 20

#define END_PULSE_A 1680
#define END_PULSE_B 370
#define END_PULSE_DELTA 30

#define PULSE_0 15
#define PULSE_1 65
#define PULSE_DELTA 15

/** 
 * the main IR loop 
 * see https://learn.adafruit.com/ir-sensor/using-an-ir-sensor
 */
void loopIr() {
  uint16_t highpulse, lowpulse; // temporary storage timing
  highpulse = lowpulse = 0; // start out with no pulse length

  // while pin is still HIGH
  while (PORT_IR & (1 << PIN_IR)) {
    verifyPulse(highpulse++);
  }
  
  // we didn't time out so lets stash the reading
  pulses[currentpulse][0] = highpulse;

  // same as above for low pin
  while (! (PORT_IR & _BV(PIN_IR))) {
    verifyPulse(lowpulse++);
  }
  
  // we didn't time out so lets stash the reading
  pulses[currentpulse][1] = lowpulse;

  // we read one high-low pulse successfully, continue!
  currentpulse++;
}

// verify pulses if we need to do something and act on it
void verifyPulse(int pulse) {
    delayMicroseconds(RESOLUTION);
    
    // If the pulse is too long, we 'timed out' - either nothing
    // was received or the code is finished, so print what
    // we've grabbed so far, and then reset
    if ((pulse >= MAXPULSE) && (currentpulse != 0)) {
      actOnCode(parsePulses());
      currentpulse = 0;
      return;
    }
}

// IR control "business logic"
void actOnCode(String code) {
  int codeFound = -1;
  
  if(!code.startsWith(codeInit)) {
    logIr("unknown code init sequence: " + code, true);    
    return;
  }
  
  code = code.substring(codeInit.length());

  if(code.equals(codePower)) {
    codeFound = 1;

    if(powerOn) { 
      playerOn = false; 
      powerOn = false; 
    }
    else { 
      powerOn = true; 
    }
  }

  if(code.equals(codeVolumeUp)) {
    codeFound = 2;
    changeBrightness(BRIGHTNESS_STEP);
  }

  if(code.equals(codeVolumeDown)) {
    codeFound = 3;
    changeBrightness(-BRIGHTNESS_STEP);
  }

  if(code.equals(codePlay)) {
    codeFound = 4;
    
    if(playerOn) { 
      playerOn = false; 
    }
    else { 
      playerOn = true; 
      currentPixel = 0; 
    }
  }
  
  if(code.equals(codeColor1)) {
    codeFound = 10;
    setColorCode(presetColor(0));
  }
  if(code.equals(codeColor2)) {
    codeFound = 11;
    setColorCode(presetColor(1));
  }
  if(code.equals(codeColor3)) {
    codeFound = 12;
    setColorCode(presetColor(2));
  }
  if(code.equals(codeColor4)) {
    codeFound = 13;
    setColorCode(presetColor(3));
  }
  if(code.equals(codeColor5)) {
    codeFound = 14;
    setColorCode(presetColor(4));
  }
  if(code.equals(codeColor6)) {
    codeFound = 15;
    setColorCode(presetColor(5));
  }
  
  if(codeFound > 0) {
    logIr("actOnCode() IR: ", false); logIrInt(codeFound, true);
    
    if(powerOn) { 
      updateLedStrip();
    }
    else {
      setStripToColor(0, 0, 0, true);
    }
  }
  else {
    logIr("actOnCode() IR unknown: ", false); ; logIr(code, true);
  }
}

String parsePulses() {
  int code_start = findStart();
  int code_end = findEnd(code_start);
  String irCode = "";

  // uncomment the line below to do find values that work -> then update START_PULSE_* accordingly
  if(IR_DEBUG) {
    logIr("parsePulses()", true);
    logIr("start index ", false); logIrInt(code_start, true);
    logIr("end index ", false); logIrInt(code_end, true);
    
    if(code_start <= 0) {
      logIr("detect start/end failed -> fixt this", true);
    }
      
    for(int i = 0; i <= currentpulse; i++) {
      logIrInt(i, false); logIr(" pulse ", false); logIrInt(pulses[i][0], false); logIr(" ", false); logIrInt(pulses[i][1], true);
    }
  }

  if(code_start >= 0 && code_end > code_start) {
    irCode += '<';

    for(int i = code_start; i < code_end; i++) {
      irCode += mapToBit(i);
    }

    irCode += '>';
  }

  logIr(irCode, true);

  return irCode;
}

// searches for start pattern of logitech ir control
int findStart() {
  int i = 0;

  while(i < currentpulse - 2) {
    if(inRange(pulses[i][1], START_PULSE_A - START_PULSE_DELTA,  START_PULSE_A + START_PULSE_DELTA) &&
       inRange(pulses[i+1][0], START_PULSE_B - START_PULSE_DELTA,  START_PULSE_B + START_PULSE_DELTA)) 
    {
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
    if(inRange(pulses[i][0], END_PULSE_A - END_PULSE_DELTA,  END_PULSE_A + END_PULSE_DELTA) && 
       inRange(pulses[i][1], END_PULSE_B - END_PULSE_DELTA,  END_PULSE_B + END_PULSE_DELTA)) 
    {
      return i;
    }

    i++;
  }

  return currentpulse;
}

char mapToBit(int i) {
  if(inRange(pulses[i][0], PULSE_0 - PULSE_DELTA, PULSE_0 + PULSE_DELTA)) { 
    return '0'; 
  }
  if(inRange(pulses[i][0], PULSE_1 - PULSE_DELTA, PULSE_1 + PULSE_DELTA)) { 
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

void logIrInt(int i, boolean newline) {
  if(IR_DEBUG) {
    if(newline) { Serial.println(i); }
    else        { Serial.print(i); }
  }
}
