//
// Zoom Buttons
// inspired by https://www.instructables.com/Zoom-Control-Box/
//
// Arduino Micro interrupt pins in order = 3, 2, 0, 1, 7 
// 
//#include "Keyboard.h"
#include "HID-Project.h"

#define ROTARY_A 5
#define ROTARY_B 6
boolean A, a, B, b, C, c;

// Define Arduino pin numbers for buttons and LEDs
#define LED 13
#define BTN1 0  // Alt-F2: Gallery view Alt-F1: Speaker view toggle
#define BTN2 1  // Alt-V: Turn video on/off
#define BTN3 2  // Alt-Y: Raise Hand
#define BTN4 3  // Alt-A: Mute self
#define BTN5 7  // MUTE computer
#define BTN6 9  // Alt-S: Share screen
#define NUMBUTTONS 6
int buttons[NUMBUTTONS] = {BTN1, BTN2, BTN3, BTN4, BTN5, BTN6};
unsigned long btntime[NUMBUTTONS];
boolean btnpress[NUMBUTTONS];


char line[80];
unsigned long t;
int n;
bool gallery = false; //keep track to toggle between gallery and speaker view

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  //volume knob
  pinMode(ROTARY_A, INPUT_PULLUP);
  pinMode(ROTARY_B, INPUT_PULLUP);
  a = b = c = false;

  for (int i=0; i < 3; i++) {
  digitalWrite(LED, HIGH); delay(20);
  digitalWrite(LED, LOW); delay(200);
  digitalWrite(LED, HIGH); delay(20);
  digitalWrite(LED, LOW); delay(200);
  digitalWrite(LED, HIGH); delay(20);
  digitalWrite(LED, LOW); delay(500);
  }

  //setup push buttons
  for (int i = 0; i < NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    btntime[i] = 0;
    btnpress[i] = false;
  }

  Keyboard.begin();
  Consumer.begin();
}

void loop() {
  //check volume knob and act
  A = digitalRead(ROTARY_A) == LOW;
  B = digitalRead(ROTARY_B) == LOW;
  if (A && !a) {
    if (B) {
      n++; if (n > 100) n = 100;
      Consumer.write(MEDIA_VOL_UP);
    }
    else {
      n--; if (n < 0) n = 0;
      Consumer.write(MEDIA_VOL_DOWN);
    }
//      sprintf(line, "%d\n", n);
//      Serial.write(line);
  }
  a = A;

  //check push buttons and act
  for (int i = 0; i < NUMBUTTONS; i++) {
    if (!digitalRead(buttons[i])) {
      // Button pressed (negative logic)
      if (btntime[i] == 0) {
        // Button has just been pressed
        btntime[i] = millis();
      }
      else {
        // Button is still pressed
        if (millis() - btntime[i] > 20 && !btnpress[i]) {
          // This is not just a glitch          
          btnpress[i] = true;
          // Now do your stuff!
          digitalWrite(LED, HIGH);
          RXLED0;
          switch (i) {
          case 0: // Alt-F2: Gallery view Alt-F1: Speaker view toggle
            Keyboard.press(KEY_LEFT_ALT);
            if (gallery == true){
              Keyboard.write(KEY_F2); //gallery view
            }
            else {
              Keyboard.write(KEY_F1); //speaker view
            }
            Keyboard.release(KEY_LEFT_ALT);
            gallery = !gallery; //toggle for next press cycle
            break;
          case 1: // Alt-V: video on/off
            Keyboard.press(KEY_LEFT_ALT);            
            Keyboard.write('v');
            Keyboard.release(KEY_LEFT_ALT);
            break;
          case 2: // Alt-Y: Raise / lower hand
            Keyboard.press(KEY_LEFT_ALT);            
            Keyboard.write('y');
            Keyboard.release(KEY_LEFT_ALT);
            break;
          case 3: // Alt-A: Mute self
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.write('a');
            Keyboard.release(KEY_LEFT_ALT);
            break;
          case 4: // Mute computer
            Consumer.write(MEDIA_VOL_MUTE);
            break;
          case 5: // Alt-S: Share screen
            Keyboard.press(KEY_LEFT_ALT);
            Keyboard.write('s');
            Keyboard.release(KEY_LEFT_ALT);
            break;
          }  
        }
      }
    }
    else {
      // Button not pressed
      if (btntime[i] != 0) {
        // Looks like it's just been released
        digitalWrite(LED, LOW);
        RXLED1;
        btntime[i] = 0;
        btnpress[i] = false;
      }
    }
  }
}
