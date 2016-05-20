#define CTRL_INTERVAL_MS 500
#define ORDER_GET 'G'
#define ORDER_SET_TARGET 'T'
#define ORDER_SET_ERROR 'E'

#include "CtrlSystem.h"

// AZコネクタピン1
// ポテンショメータの出力
int meter_AZ_pin = A0;

// AZコネクタピン2
// LOWならローターが右回転
int CW_AZ_pin = 2; 

// AZコネクタピン3→NC
// コネクタピン4→11VDC電源

// AZコネクタピン5
// LOWならローターが左回転
int CCW_AZ_pin = 3;

// AZコネクタピン6
// LOWなら回転速度低下
int slow_AZ_pin = 4;

// ELコネクタピン1
// ポテンショメータの出力
int meter_EL_pin = A1;

// ELコネクタピン2
// LOWならローターが下回転
int CW_EL_pin = 8; 

// ELコネクタピン3→NC
// コネクタピン4→11VDC電源

// ELコネクタピン5
// LOWならローターが上回転
int CCW_EL_pin = 9;

// ELコネクタピン6
// LOWなら回転速度低下
int slow_EL_pin = 10;

int target_AZ = 0;
int target_EL = 0;
int allowed_error_AZ = 2;
int allowed_error_EL = 2;

CtrlSystem AZ_control(CW_AZ_pin, CCW_AZ_pin, slow_AZ_pin, meter_AZ_pin);
CtrlSystem EL_control(CW_EL_pin, CCW_EL_pin, slow_EL_pin, meter_EL_pin);

void setup() {
  init_com();
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  AZ_control.set_adc_values(0.1, 5, -180, 180);
  EL_control.set_adc_values(0.4, 2.8, 0, 90);
  AZ_control.set_allowed_error(allowed_error_AZ);
  EL_control.set_allowed_error(allowed_error_EL);
  AZ_control.myName = "AZ_control";
  EL_control.myName = "EL_control";
}

void loop() {
  static long previous_time = 0;
  long timenow = millis();
  if (timenow - previous_time > CTRL_INTERVAL_MS) {
    // 制御周期分時間経過したら制御
    previous_time  = timenow;
    int AZnow = AZ_control.start_control();
    //delay(500);
    int ELnow = EL_control.start_control();
    //Serial.println("control");
    send_data(AZnow, ELnow);
  }

  char command = receive_order();
  switch (command) {
    case ORDER_GET: {
      int est_AZ = AZ_control.get_estimation();
      int est_EL = EL_control.get_estimation();
      send_data(est_AZ, est_EL);
    break;
    }
    case ORDER_SET_TARGET:
      AZ_control.set_target(target_AZ);
      EL_control.set_target(target_EL);
    break;
    case ORDER_SET_ERROR:
      AZ_control.set_allowed_error(allowed_error_AZ);
      EL_control.set_allowed_error(allowed_error_EL);
    break;
  }
}
