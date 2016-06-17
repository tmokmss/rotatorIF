#ifndef CtrlSystem_h
#define CtrlSystem_h

#include "Arduino.h"

class CtrlSystem {
  public:
    CtrlSystem(int cwPin, int ccwPin, int slowPin, int meterPin);
    int set_target(int target);
    int start_control();
    void set_adc_values(float lowV, float highV, int lowDeg, int highDeg);
    void set_allowed_error(int error);
    int get_estimation();
    int get_current_error();
    String myName;
  private:
    int cw_pin;
    int ccw_pin;
    int slow_pin;
    int meter_pin;
    float fromLow, fromHigh;
    int toLow, toHigh;
    int target_deg;
    int allowed_error;
    int is_rotating_CW;
    void start_CW_rotation();
    void start_CCW_rotation();
    void set_slow_rotation(bool isOn);
    void stop_rotation();
    int determine();
};

#endif
