#include <Arduino.h>
#include <main.h>

const int RING_LENGTH = 2000; //ring duration in milliseconds
const int RING_DELAY = 5000; //ring off in milliseconds
const int MOTION_TRIGGER_RING_COUNT = 4; //times the phone should ring when triggered
const int MOTION_PIN = PB4;
const int HOOK_PIN = PB6;
const int RINGER_PIN = PB7;

int ringTimer = 0;
volatile bool ringState = false;
int ringCount = 0;
bool hookState = false;

void motionDetected() {
  if(!digitalRead(HOOK_PIN)){
    if(digitalRead(MOTION_PIN)) {
      Serial.println("Ringer Started");
      ringState = true;
    }
  }
}

void setup() {
  pinMode(HOOK_PIN, INPUT_PULLUP);
  pinMode(MOTION_PIN, INPUT_PULLUP);
  pinMode(PB5, INPUT_PULLDOWN);
  attachInterrupt(MOTION_PIN, motionDetected, FALLING);
  Serial.begin();
  Serial.println("Serial Begun");
}

void loop() {
  updateState();
  if(digitalRead(PB5)) {
    Serial.println("PB5 HIGH");
  }
  int loopTime = millis();
  if(!hookState && ringState && (loopTime - ringTimer > RING_DELAY)) {
    tone(RINGER_PIN, 20, RING_LENGTH);
    ringTimer = loopTime;
    ringCount++;
  } else if(hookState && ringState) {
    stopRinger();
  }
  if(ringCount > MOTION_TRIGGER_RING_COUNT) {
    stopRinger();
  }
}

void stopRinger() {
  ringTimer = 0;
  ringCount = 0;
  noTone(RINGER_PIN);
  ringState = false;
  Serial.println("Ringer Stopped");
}


void updateState() {
  hookState = digitalRead(HOOK_PIN);

}
