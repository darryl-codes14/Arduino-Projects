#include <Keypad.h>
#include <LiquidCrystal.h>

// 1. LCD Setup: (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// 2. Keypad Setup
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Row Pins: 10, 9, 8, 6  |  Col Pins: A0, A1, A2, A3
// Leave D13, D7, D1, and D0 totally empty!
byte rowPins[ROWS] = {10, 9, 8, 6}; 
byte colPins[COLS] = {A0, A1, A2, A3}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String secretPin = "67"; // Change this to whatever you want
String inputString = "";

void setup() {
  lcd.begin(16, 2);
  showPrompt();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    if (key == '#') { 
      checkPin();
    } 
    else if (key == '*') { 
      inputString = ""; // Reset button
      showPrompt();
    } 
    else {
      
      if (inputString.length() < 16) {
        inputString += key;
        lcd.setCursor(inputString.length() - 1, 1);
        lcd.print("*"); 
      }
    }
  }
}

void showPrompt() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter PIN:");
  lcd.setCursor(0, 1);
}

void checkPin() {
  lcd.clear();
  delay(150); 
  
  if (inputString == secretPin) {
    lcd.setCursor(0, 0);
    lcd.print("PIN Correct!    "); 
    lcd.setCursor(0, 1);
    lcd.print("Access Granted  ");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Incorrect PIN   ");
    lcd.setCursor(0, 1);
    lcd.print("Access Denied   ");
  }
  
  delay(3000);       // Show result for 3 seconds
  inputString = "";  // Clear the attempt
  showPrompt();      // Go back to the start
}