#include <Arduino.h>
#include <main.h>

const int RING_LENGTH = 2000; //ring duration in milliseconds
const int RING_DELAY = 6000; //ring off in milliseconds
const int RING_TIMEOUT = 60; //time to wait until next ring in seconds
const int MOTION_TRIGGER_RING_COUNT = 4; //times the phone should ring when triggered
const int MOTION_PIN = PB4;
const int HOOK_PIN = PB6;
const float RING_POWER = 0.27;
const int RINGER_PIN = PA8;
const int RINGER_PIN_COMP = PB14;
const int RINGER_PIN_V = PA9;
const int RING_COUNT = 4;

volatile bool ringState = false;
volatile int ringCount = 0;

bool ringerEnabled = false;
int ringTimer = 0;
bool hookState = false;
int duty_100 = 0;
int ringTimerOverflow = 0;

void setup() {
  // setup pins
  pinMode(HOOK_PIN, INPUT_PULLUP);
  pinMode(MOTION_PIN, INPUT_PULLUP);
  pinMode(PB5, INPUT_PULLDOWN);
  pinMode(RINGER_PIN, PWM);
  pinMode(RINGER_PIN_COMP, PWM);
  pinMode(PB8, OUTPUT);
  // setup GPIO interrupts
  attachInterrupt(MOTION_PIN, motionDetected, FALLING);
  attachInterrupt(HOOK_PIN, hookLifted, RISING);
  // setup timers
  HardwareTimer timer1 = HardwareTimer(1);
  HardwareTimer timer2 = HardwareTimer(2);
  timer1.pause();
  timer2.pause();
  timer2.setPrescaleFactor(36000);
  timer2.setOverflow(RING_DELAY * 2);
  timer2.setMode(1, TIMER_OUTPUT_COMPARE);
  timer2.setCompare(1, 0);
  timer2.attachInterrupt(1, ringerOn);
  timer2.setMode(2, TIMER_OUTPUT_COMPARE);
  timer2.setCompare(2, RING_LENGTH * 2);
  timer2.attachInterrupt(2, ringerOff);
  duty_100 = timer1.setPeriod(25000);
  timer_dev *t = TIMER1; //refers t to Timer 8 memory location, how to read back?
  timer_reg_map r = t->regs;
  bitSet(r.adv->CCER,0); //this should enable complimentary outputs
  bitSet(r.adv->CCER,2);
  bitSet(r.adv->CCER,7);
  bitSet(r.adv->CCER,6);
  bitSet(r.bas->CR1,5);
  timer1.refresh();
  timer2.refresh();
  timer1.resume();
  timer2.resume();
  // Make sure we start with the ringer disabled
  ringerOff();
}

void loop() {
  // startRinger();
  //int loopTime = millis();
  // if(!ringState) {
  //   startRinger();
  // }
  // if (loopTime - ringTimer > RING_LENGTH){
  //   stopRinger();

  // }
  // updateState();
  // if(!hookState && ringState && (loopTime - ringTimer > RING_DELAY)) {
  //   tone(RINGER_PIN, 20, RING_LENGTH);
  //   ringTimer = loopTime;
  //   ringCount++;
  // } else if(hookState && ringState) {
  //   stopRinger();
  // }
  // if(ringCount > MOTION_TRIGGER_RING_COUNT) {
  //   stopRinger();
  // }
  // delay(10000);
}
void motionDetected() {
  if(!digitalRead(HOOK_PIN)){
      startRinger();
  }
}

void startRinger() {
  if(millis() - ringTimer > (RING_TIMEOUT * 1000) || millis() < (RING_TIMEOUT * 1000)) {
    HardwareTimer timer2 = HardwareTimer(2);
    timer2.setCount(0);
    ringerEnabled = true;
    ringTimer = millis();
    ringCount = 0;
    ringerOn();
  }
}

void stopRinger() {
  ringerOff();
  ringTimer = millis();
  ringerEnabled = false;
  ringCount = RING_COUNT + 1;
}

void ringerOn() {
  if (ringerEnabled && (ringCount <= RING_COUNT)) {
    ringCount ++;
    pwmWrite(RINGER_PIN, RING_POWER * duty_100);
    pwmWrite(RINGER_PIN_V, (1 - RING_POWER) * duty_100);
    ringState = true;
  }
}

void ringerOff() {
  pwmWrite(RINGER_PIN, 0);
  pwmWrite(RINGER_PIN_V, duty_100);
  ringState = false;
}

void updateState() {
  hookState = digitalRead(HOOK_PIN);
}

void hookLifted() {
  stopRinger();
}