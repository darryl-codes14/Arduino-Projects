#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         9          
#define SS_PIN          10         
#define HALL_SENSOR_PIN 4          
#define LED_PIN         5          

MFRC522 mfrc522(SS_PIN, RST_PIN);

unsigned long doorOpenTime = 0;  
bool timerActive = false;         
const long interval = 30000;     

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("System Active: 30s Grace Period Enabled.");
}

void loop() {
  int doorState = digitalRead(HALL_SENSOR_PIN);

  // 1. Check if the door just opened
  if (doorState == HIGH && !timerActive && digitalRead(LED_PIN) == LOW) {
    doorOpenTime = millis();       // Start the "stopwatch"
    timerActive = true;
    Serial.println("Door opened. 30 seconds to scan card...");
  }

  // 2. If 30 seconds pass and no card was scanned, turn on LED
  if (timerActive && (millis() - doorOpenTime >= interval)) {
    digitalWrite(LED_PIN, HIGH);
    timerActive = false;           // Stop the timer, LED is now latched ON
    Serial.println("ALARM: 30 seconds exceeded! LED ON.");
  }

  // 3. Check for RFID Card to reset everything
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Card Scanned: System Reset.");
    
    digitalWrite(LED_PIN, LOW);    // Turn off LED
    timerActive = false;           // Cancel the countdown if it was running
    
    delay(2000);                   
    mfrc522.PICC_HaltA();
  }
}