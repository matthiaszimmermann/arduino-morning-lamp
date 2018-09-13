
/**
 * returns the preset color for the provided index
 */
const char* presetColor(int index) {
  switch(index) {
    case 0:
      return "34db7a";
    case 1:
      return "2e33ba";
    case 2:
      return "d127c2";
    case 3:
      return "dd4400";
    case 4:
      return "aa2200";
    case 5:
      return "660000";
    default:
      return "000000";
  }
}

/**
 * returns the cycle color for the provided index
 */
const char* color(int index) {
  if(LED_DEBUG) { return colorDebug(index); }
  else          { return colorMorning(index); }
}

/**
 * return the number of colors available for colors(int index)
 */
int colors() {
  if(LED_DEBUG) { return 4; }
  else          { return 5; }
}

/**
 * returns the color for the provided index (production case)
 */
const char* colorMorning(int index) {
  switch(index % 5) {
    case 0:
      return "000000"; // schwarz
    case 1:
      return "000018"; // dunkelblau
    case 2:
      return "1e0070"; // violett
    case 3:
      return "a02000"; // orange
    case 4:
      return "ff6600"; // gelborange
    default:
      return "000000";
  }
}

/**
 * returns the color for the provided index (debugging case)
 */
const char* colorDebug(int index) {
  switch(index) {
    case 0:
      return "020000";
    case 1:
      return "ff0000";
    case 2:
      return "00ff00";
    case 3:
      return "0000ff";
    default:
      return "000000";
  }
}
