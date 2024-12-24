#include <Wire.h>
#include <U8g2lib.h>

// Initialize OLED display using I2C communication
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

const int dotButtonPin = 6; // Pin for dot button input
const int dashButtonPin = 9; // Pin for dash button input
String morseCode = ""; // Stores the current Morse code sequence
String fullMessage = ""; // Stores the decoded full message

bool lastDotButtonState = LOW; // Keeps track of the previous state of the dot button
bool lastDashButtonState = LOW; // Keeps track of the previous state of the dash button
unsigned long lastInputTime = 0; // Records the time of the last input
const unsigned long inputTimeout = 1000; // Timeout duration for detecting the end of a letter (1 second)
const unsigned long finalDelay = 3000; // Delay before resetting to a new message (3 seconds)

// Morse code table for letters A-Z and numbers 0-9
const String morseAlphabet[36] = {
  ".-",   "-...", "-.-.", "-..",  ".",    "..-.", "--.",   "....", "..",   ".---", // A-J
  "-.-",  ".-..", "--",   "-.",   "---",  ".--.", "--.-",  ".-.",  "...",  "-",    // K-T
  "..-",  "...-", ".--",  "-..-", "-.--", "--..", // U-Z
  "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----." // 0-9
};

void setup() {
  u8g2.begin(); // Initialize the OLED display
  pinMode(dotButtonPin, INPUT_PULLUP); // Configure the dot button pin as input with a pull-up resistor
  pinMode(dashButtonPin, INPUT_PULLUP); // Configure the dash button pin as input with a pull-up resistor

  displayIntroScreen(); // Show the introductory screen
}

void loop() {
  // Read the state of the dot button
  bool currentDotButtonState = digitalRead(dotButtonPin);
  if (currentDotButtonState == LOW && lastDotButtonState == HIGH) { // Check for button press
    morseCode += "."; // Add a dot to the Morse code sequence
    updateDisplay(); // Update the OLED display
    lastInputTime = millis(); // Update the time of the last input
    delay(300); // Debounce delay
  }
  lastDotButtonState = currentDotButtonState; // Update the previous button state

  // Read the state of the dash button
  bool currentDashButtonState = digitalRead(dashButtonPin);
  if (currentDashButtonState == LOW && lastDashButtonState == HIGH) { // Check for button press
    morseCode += "-"; // Add a dash to the Morse code sequence
    updateDisplay(); // Update the OLED display
    lastInputTime = millis(); // Update the time of the last input
    delay(300); // Debounce delay
  }
  lastDashButtonState = currentDashButtonState; // Update the previous button state

  // Check if there is a timeout for the current letter
  if (millis() - lastInputTime > inputTimeout && morseCode.length() > 0) {
    fullMessage += decodeMorse(morseCode); // Decode the Morse code and append the character to the full message
    fullMessage += " "; // Add a space after the letter
    updateDisplay(); // Update the OLED display with the full message
    morseCode = ""; // Clear the Morse code sequence
  }
}

// Display an introductory animation on the OLED screen
void displayIntroScreen() {
  // Display "Morse Decoder" for 2 seconds
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(25, 32, "Morse Decoder");
  u8g2.sendBuffer();
  delay(2000);

  // Display "Loading..." with a blinking effect for 2 seconds
  for (int i = 0; i < 4; i++) { // Blink 4 times, 500ms per cycle
    u8g2.clearBuffer();
    if (i % 2 == 0) {
      u8g2.drawStr(35, 32, "Loading...");
    }
    u8g2.sendBuffer();
    delay(500);
  }
}

// Update the OLED display with the current Morse code and message
void updateDisplay() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  // Display the current Morse code sequence
  u8g2.drawStr(0, 10, "Morse Code:");
  u8g2.setCursor(0, 20);
  u8g2.print(morseCode.c_str());

  // Display the decoded message
  u8g2.drawStr(0, 40, "Message:");
  u8g2.setCursor(0, 50);
  u8g2.print(fullMessage.c_str());

  u8g2.sendBuffer(); // Send buffer to the OLED
}

// Decode the given Morse code sequence into a character
String decodeMorse(String morse) {
  String decoded = "";
  int index = -1; // Initialize index to -1 (not found)

  // Find the Morse code in the table
  for (int i = 0; i < 36; i++) {
    if (morseAlphabet[i] == morse) {
      index = i;
      break;
    }
  }

  // If a matching code is found, decode it
  if (index != -1) {
    if (index < 26) {
      decoded += char(index + 'A'); // Convert index to letter (A-Z)
    } else {
      decoded += char(index - 26 + '0'); // Convert index to number (0-9)
    }
  }
  return decoded; // Return the decoded character
}
