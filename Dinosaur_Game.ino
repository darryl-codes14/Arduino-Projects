#include <LiquidCrystal.h>

LiquidCrystal lcd(11, 10, 5, 4, 3, 2);

const int joyX = A0; // Changed to A0 for your vertical axis
const int buzzer = 8;

byte dinoChar[8] = { B00111, B00101, B00111, B10110, B11111, B01010, B01010, B00000 };
byte cactusChar[8] = { B00100, B00101, B10101, B10101, B10111, B00100, B00100, B00100 };
byte birdChar[8] = { B00100, B01011, B11111, B01011, B00100, B00000, B00000, B00000 };
byte dodgeChar[8] = { B00000, B00000, B00000, B01111, B11111, B11010, B00000, B00000 };

int score = 0;
int dinoRow = 1;
int obsCol = 15;
int obsType = 0; 
bool isDodging = false;
bool gameOver = false;
unsigned long jumpTimer = 0;

void setup() {
  randomSeed(analogRead(A5)); 
  
  lcd.begin(16, 2);
  lcd.createChar(0, dinoChar);
  lcd.createChar(1, cactusChar);
  lcd.createChar(2, birdChar);
  lcd.createChar(3, dodgeChar);
  
  pinMode(joyX, INPUT);
  pinMode(buzzer, OUTPUT);
  
  lcd.setCursor(4, 0);
  lcd.print("DINO ELITE");
  delay(2000);
}

void loop() {
  if (!gameOver) {
    int val = analogRead(joyX);

    if (val > 800 && dinoRow == 1) {
      dinoRow = 0;
      isDodging = false;
      jumpTimer = millis();
      tone(buzzer, 1500, 50); 
    }
    else if (val < 200 && dinoRow == 1) {
      isDodging = true;
      static unsigned long lastBeep = 0;
      if(millis() - lastBeep > 250){
        tone(buzzer, 300, 30);
        lastBeep = millis();
      }
    } 
    else {
      isDodging = false;
    }

    if (dinoRow == 0 && millis() - jumpTimer > 600) {
      dinoRow = 1;
    }

    obsCol--;
    if (obsCol < 0) {
      obsCol = 15;
      obsType = random(0, 2); 
      score++;
      delay(random(0, 50)); 
    }

    if (obsType == 0 && obsCol == 0 && dinoRow == 1 && !isDodging) gameOver = true;
    if (obsType == 1 && obsCol == 0 && dinoRow == 0) gameOver = true;
    if (obsType == 1 && obsCol == 0 && dinoRow == 1 && !isDodging) gameOver = true;

    lcd.clear();
    lcd.setCursor(14, 0);
    lcd.print(score);

    lcd.setCursor(0, dinoRow);
    if (isDodging) lcd.write(byte(3)); 
    else lcd.write(byte(0));           

    lcd.setCursor(obsCol, (obsType == 0 ? 1 : 0)); 
    lcd.write(byte(obsType == 0 ? 1 : 2));

    delay(max(30, 130 - (score * 4))); 
    
  } else {
    int notes[] = {400, 300, 200, 150};
    for (int i = 0; i < 4; i++) {
      tone(buzzer, notes[i], 150);
      delay(200);
    }
    
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("GAME OVER!");
    lcd.setCursor(4, 1);
    lcd.print("Score: ");
    lcd.print(score);
    
    while(analogRead(joyX) < 800) { 
      delay(10); 
    }
    
    score = 0;
    obsCol = 15;
    gameOver = false;
    delay(500);
  }
}