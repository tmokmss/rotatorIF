#include "Arduino.h"
#include "CtrlSystem.h"

CtrlSystem::CtrlSystem(int cwPin, int ccwPin, int slowPin, int meterPin) {
  cw_pin = cwPin;
  ccw_pin = ccwPin;
  slow_pin = slowPin;
  meter_pin = meterPin;
 
  pinMode(cw_pin, OUTPUT);
  pinMode(ccw_pin, OUTPUT);
  pinMode(slow_pin, OUTPUT);
  //pinMode(meter_pin, INPUT);

  fromLow = 0.1;
  fromHigh = 5;
  toLow = -180;
  toHigh = 180;

  set_target(0);
  set_allowed_error(10);
  myName = "CtrlSystem";
}

void CtrlSystem::set_target(int target) {
  if (target >= toLow && target <= toHigh) {
    target_deg = target;
  }
  else {
    target_deg = 0; //(toLow + toHigh) / 2;
  }
  // これからの回転方向を記憶
  int est = determine();
  is_rotating_CW = est > target_deg;
}

int CtrlSystem::start_control() {
  static bool reached = false;
  int estimation = determine();
  int distance = target_deg - estimation ;

  if (reached && abs(distance) < allowed_error){
    return estimation;
  }
  else {
    reached = false;
  }

  // 目標近傍では回転速度を下げる
  if (abs(distance) < allowed_error * 2) {
    set_slow_rotation(true);
  }
  else {
    set_slow_rotation(false);
  }

  Serial.print("control switch: ");
  // deadbandのあるon-off制御
  if (is_rotating_CW) {
    if (distance < -allowed_error) {
      reached = true;
      stop_rotation();
    }
    else {
      start_CW_rotation();
    }
  }
  else {
    if (distance > allowed_error) {
      reached = true;
      stop_rotation();
    }
    else {
      start_CCW_rotation();
    }
  }

  return estimation;
}

void CtrlSystem::set_adc_values(float lowV, float highV, int lowDeg, int highDeg) {
  fromLow = lowV;
  fromHigh = highV;
  toLow = lowDeg;
  toHigh = highDeg; 
}

// ±error [deg]の許容制御誤差
void CtrlSystem::set_allowed_error(int error) {
  allowed_error = error;
}

int CtrlSystem::get_current_error() {
  int estimation = determine();
  return target_deg-estimation;
}

int CtrlSystem::get_estimation() {
  return determine();
}

void CtrlSystem::start_CW_rotation() {
  digitalWrite(ccw_pin, LOW);
  digitalWrite(cw_pin, HIGH);
  Serial.print(myName);
  Serial.println(" Start CW");
}

void CtrlSystem::start_CCW_rotation() {
  digitalWrite(cw_pin, LOW);
  digitalWrite(ccw_pin, HIGH);
  Serial.print(myName);
  Serial.println(" Start CCW");
}

void CtrlSystem::set_slow_rotation(bool isOn) {
  if (isOn) {
    digitalWrite(slow_pin, HIGH); 
  }
  else {
    digitalWrite(slow_pin, LOW); 
  }
}

void CtrlSystem::stop_rotation() {
  digitalWrite(cw_pin, LOW);
  digitalWrite(ccw_pin, LOW);
  Serial.print(myName);
  Serial.println(" stopped");
}

int CtrlSystem::determine() {
  int anaraw = analogRead(meter_pin);
  float anav = anaraw * 5.0 / 1024;
  int estimation_deg = (anav - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
  return estimation_deg;
}

