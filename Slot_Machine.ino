#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int TILT_PIN = 2;
const int BUZZER_PIN = 8;

// Slot machine symbols
const char* symbols[] = {"777", "BAR", "CHERRY", "BELL", "LEMON"};
const int numSymbols = 5;

bool isSpinning = false;
int lastTiltState;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Power stabilization delay for OLED
  delay(500); 

  // Initialize Wire explicitly
  Wire.begin();
  Wire.setClock(100000); // Standard 100kHz I2C speed

  // Enable internal pull-up for tilt switch
  pinMode(TILT_PIN, INPUT_PULLUP);
  lastTiltState = digitalRead(TILT_PIN);

  // Auto-detect OLED address (0x3C or 0x3D)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      // Rapid blink on Pin 13 if OLED fails to initialize
      while(true) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
      }
    }
  }

  // Pin 13 LED stays solid ON when OLED connects successfully
  digitalWrite(LED_BUILTIN, HIGH);
  
  randomSeed(analogRead(0));
  playStartupSound();
  showIdleScreen();
}

void loop() {
  int currentTiltState = digitalRead(TILT_PIN);

  // Trigger spin on tilt change
  if (currentTiltState != lastTiltState && !isSpinning) {
    delay(50); // Debounce delay
    
    if (digitalRead(TILT_PIN) == currentTiltState) {
      isSpinning = true;
      playSlotMachine();
      
      lastTiltState = digitalRead(TILT_PIN); 
      isSpinning = false;
      showIdleScreen();
    }
  }
}

void showIdleScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 10);
  display.println("--- SLOT MACHINE ---");
  display.setCursor(22, 38);
  display.println("TILT TO SPIN!");
  display.display();
}

void playSlotMachine() {
  int reel1, reel2, reel3;

  for (int i = 0; i < 15; i++) {
    reel1 = random(0, numSymbols);
    reel2 = random(0, numSymbols);
    reel3 = random(0, numSymbols);

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(35, 5);
    display.println("SPINNING!");
    
    drawReels(symbols[reel1], symbols[reel2], symbols[reel3]);
    display.display();

    tone(BUZZER_PIN, 800 + (i * 20), 15);
    delay(50 + (i * 15)); 
  }

  display.setTextSize(1);
  if (reel1 == reel2 && reel2 == reel3) {
    display.clearDisplay();
    drawReels(symbols[reel1], symbols[reel2], symbols[reel3]);
    display.setCursor(40, 50);
    display.println("JACKPOT!");
    display.display();
    
    playJackpotSound();
  } else if (reel1 == reel2 || reel2 == reel3 || reel1 == reel3) {
    display.setCursor(35, 52);
    display.println("Nice Try!");
    display.display();
    
    playWinSound();
  } else {
    display.setCursor(38, 52);
    display.println("YOU LOSE");
    display.display();
    
    playLoseSound();
  }
  
  delay(2500);
}

void drawReels(const char* r1, const char* r2, const char* r3) {
  display.setTextSize(1);
  
  display.drawRect(5, 20, 36, 24, SSD1306_WHITE);
  display.setCursor(8, 28);
  display.print(r1);

  display.drawRect(46, 20, 36, 24, SSD1306_WHITE);
  display.setCursor(49, 28);
  display.print(r2);

  display.drawRect(87, 20, 36, 24, SSD1306_WHITE);
  display.setCursor(90, 28);
  display.print(r3);
}

// --- SOUND EFFECTS ---

void playStartupSound() {
  tone(BUZZER_PIN, 523, 100); delay(120);
  tone(BUZZER_PIN, 659, 100); delay(120);
  tone(BUZZER_PIN, 784, 150); delay(180);
}

void playJackpotSound() {
  int notes[] = {523, 659, 784, 1047, 784, 1047};
  int durations[] = {100, 100, 100, 200, 100, 400};
  
  for (int i = 0; i < 6; i++) {
    tone(BUZZER_PIN, notes[i], durations[i]);
    delay(durations[i] + 30);
  }
}

void playWinSound() {
  tone(BUZZER_PIN, 587, 100); delay(120);
  tone(BUZZER_PIN, 880, 250); delay(280);
}

void playLoseSound() {
  tone(BUZZER_PIN, 300, 150); delay(180);
  tone(BUZZER_PIN, 200, 300); delay(320);
}