#include "src/Password/Password.h"
#include "src/Keypad/Keypad.h"
#include "src/TOTP/TOTP.h"
#include <Wire.h>
#include "src/DS1307RTC/DS1307RTC.h"

// Change it!
// http://www.lucadentella.it/OTP/

uint8_t hmacKey[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// And use this link for testing
// http://blog.tinisles.com/2011/10/google-authenticator-one-time-password-algorithm-in-javascript/

TOTP totp = TOTP(hmacKey, 10);
char code[7];

const byte ROWS = 4;
const byte COLS = 3;

// Define the Keymap
char keys[ROWS][COLS] = {
  {'1', '2', '3',},
  {'4', '5', '6',},
  {'7', '8', '9',},
  {'*', '0', '#',}
};

// initial password, will be overwriten
Password password = Password( "00000" );

byte rowPins[ROWS] = {28, 26, 24, 22}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {34, 32, 30}; // connect to the column pinouts of the keypad
const int buttonPin = 7;
int buttonState = 0;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define lockPin 8
int pinSpeaker = 10;

void setup() {
  
  // set RTC first
  // RTC.set(1479036691 );
  
  pinMode(buttonPin, INPUT);
  pinMode(lockPin, OUTPUT);
  digitalWrite(lockPin, HIGH);

  Serial.begin(9600);
  Serial2.begin(9600);

  keypad.addEventListener(keypadEvent); // add an event listener for this keypad
  keypad.setDebounceTime(250);

}


void loop() {
  long GMT = RTC.get() + 31; // RTC time + adjustment

  char* newCode = totp.getCode(GMT);

  if (strcmp(code, newCode) != 0) {
    strcpy(code, newCode);
    Serial.println(GMT);
    Serial.println(code);
    password.set(code);
  }

  keypad.getKey();
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH) {
    Serial.println();
  }
}


void keypadEvent(KeypadEvent eKey) {
  switch (keypad.getState()) {
    case PRESSED:
      Serial.print(eKey);
      switch (eKey) {
        case '#':
          guessPassword();
          Serial.println();
          break;
        default:
          password.append(eKey);
          tone(10, 4800, 100);
          delay(30);
      }
  }
}

void openDoor()
{
    tone(10, 5000, 100);
    delay(300);
    tone(10, 5000, 100);
    delay(300);
    tone(10, 5000, 100);
    delay(300);
    tone(10, 5000, 100);
    digitalWrite(lockPin, LOW);
    Serial.println("OPENING DOOR ");
    delay(1000);
    digitalWrite(lockPin, HIGH);
  }

void guessPassword() {
    
  if (password.evaluate()) {

    Serial.println("VALID PASSWORD ");
    Serial.println();
    password.reset(); // resets password after correct entry

    openDoor();
  }

  else {
    Serial.println("INVALID PASSWORD ");
    password.reset(); //resets password after INCORRECT entry
    tone(10, 1000, 600);
    delay(600);
    Serial.println();
  }
}

void tone(long duration, int freq) {
  duration *= 1000;
  int period = (1.0 / freq) * 1000000;
  long elapsed_time = 0;
  while (elapsed_time < duration) {
    digitalWrite(pinSpeaker, HIGH);
    delayMicroseconds(period / 2);
    digitalWrite(pinSpeaker, LOW);
    delayMicroseconds(period / 2);
    elapsed_time += (period);
  }
}
