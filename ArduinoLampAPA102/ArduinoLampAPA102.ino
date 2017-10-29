#include <APA102.h>
#include <TimerOne.h>

//--- debug settings ---//
#define LED_DEBUG false
#define IR_DEBUG false

//--- LED pixel constants ---//
#define PIXELS 120
#define PIN_LED_STRIP_DATA 11
#define PIN_LED_STRIP_CLOCK 12

#define LED_MAX 127
#define LED_MIN 0
#define LED_STEP 40

#define BRIGHTNESS_MAX 1.0
#define BRIGHTNESS_MIN 0.05
#define BRIGHTNESS_LOW 0.1
#define BRIGHTNESS_STEP 0.2
#define GLOBAL_DIM 2.0

#define STEPS 3
#define PIXEL_STEPS 4

#define TIMER_DEBUG 10000
#define TIMER_PRODUCTION 1200000

//--- IR control constants ---//
#define PORT_IR PIND
#define PIN_IR 2

// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 2500

// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20

//--- LED variables ---//
int currentColor;
int nextColor;
int currentStep;
int currentPixel;

boolean powerOn = true;
boolean playerOn = true;

int colorRed = LED_MAX;
int colorGreen = LED_MAX;
int colorBlue = LED_MAX;

float brightness = BRIGHTNESS_MAX;
float brightness_previous = BRIGHTNESS_MAX;

APA102<PIN_LED_STRIP_DATA, PIN_LED_STRIP_CLOCK> strip;
rgb_color stripColors[PIXELS];

//--- IR control variables --//

// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[100][2]; // pair is high and low pulse
uint8_t currentpulse = 0; // index for pulses we're storing

// logitec codes
String codeInit         = "<0000110011110011";
String codePower        = "0011000011001111>";
String codeVolumeUp     = "0101000010101111>";
String codeVolumeDown   = "1000000001111111>";
String codePlay         = "1001000001101111>";

String codeColor1       = "0011100011000111>";
String codeColor2       = "0111000010001111>";
String codeColor3       = "1010100001010111>";

String codeColor4       = "1100100000110111>";
String codeColor5       = "1000100001110111>";
String codeColor6       = "0100100010110111>";

// String codeMoon         = "0001000011101111>";
// String codeSkipForward  = "0110000010011111>";
// String codeSkipBackward = "1110000000011111>";

void setup() {
  Serial.begin(9600);
  setupLedStrip();
}

void loop() {
  loopIr();
}

