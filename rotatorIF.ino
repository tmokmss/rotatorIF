/* 
 * ローテーターコントローラ<->PCのインターフェースとなるマイコンのプログラム
 * シリアル通信によりローテータコントローラを操作することを可能にする
 * 詳細な仕様はspecification.mdを参照
 * 20160530 友岡
 */

#define ORDER_GET 'G'
#define ORDER_SET_TARGET 'T'
#define ORDER_SET_ERROR 'E'
#define ORDER_SET_FREQ 'F'

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

int target_AZ = 0;  // デフォルトの角度
int target_EL = 0;  // デフォルトの角度
int allowed_error_AZ = 5;
int allowed_error_EL = 5;
int ctrl_interval_ms = 500;

CtrlSystem AZ_control(CW_AZ_pin, CCW_AZ_pin, slow_AZ_pin, meter_AZ_pin);
CtrlSystem EL_control(CW_EL_pin, CCW_EL_pin, slow_EL_pin, meter_EL_pin);

void setup() {
  init_com();
  AZ_control.set_adc_values(0.24, 4.72, -180, 180);
  EL_control.set_adc_values(0.50, 4.53,  -10, 100);
  AZ_control.set_allowed_error(allowed_error_AZ);
  EL_control.set_allowed_error(allowed_error_EL);
  AZ_control.myName = "AZ_control";
  EL_control.myName = "EL_control";
}

void loop() {
  static long previous_time = 0;
  long timenow = millis();
  if (timenow - previous_time > ctrl_interval_ms) {
    // 制御周期分時間経過したら制御
    previous_time  = timenow;
    int AZnow = AZ_control.start_control();
    int ELnow = EL_control.start_control();
    //send_data(AZnow, ELnow);
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
      target_AZ = AZ_control.set_target(target_AZ);
      target_EL = EL_control.set_target(target_EL);
    break;
    case ORDER_SET_ERROR:
      AZ_control.set_allowed_error(allowed_error_AZ);
      EL_control.set_allowed_error(allowed_error_EL);
    break;
  }
}
