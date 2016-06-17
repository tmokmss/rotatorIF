#define TEMPBUF_SIZE 64

extern int target_AZ;
extern int target_EL;
extern int allowed_error_AZ;
extern int allowed_error_EL;
extern int ctrl_interval_ms;

void init_com() {
  Serial.begin(9600);
  Serial.println("completed initialization");
}

char receive_order() {
  static char tempbuf[TEMPBUF_SIZE]; // 受信バッファ
  bool received = false; // ループフラグ
  volatile unsigned long counter = 0;
  // 無限ループ防止の為、タイムアウトできるようにする
  int index = 0; // バッファのポインタ
  char command = 0;

  while(!received && index < TEMPBUF_SIZE &&  counter++ < 10000L) {
    // 約6.25msでタイムアウト
    if(Serial.available()) {
      unsigned char rxletter = Serial.read();
      switch(rxletter) {
        case '\n':
          // LF(\n)で命令終了
          tempbuf[index] = 0x00;
          received = true;
          break;

        default:
          if (index >= 0)  {
            tempbuf[index] = rxletter;
            counter = 0;
            index++;
        }
      }
    }
    counter++;
  }
  if (received) {  // タイムアウトせずにループを抜けた場合
    command = parse(tempbuf);
  }
  return command;
}

char parse(char *buf) {
//       0       1~
// buf = command,[payload]
  char* commandstr = strtok(buf, ",");
  char command = commandstr[0];
  switch(command) {
    case ORDER_GET:
    break;
    case ORDER_SET_TARGET: {
      char* az_str = strtok(NULL, ",");
      char* el_str = strtok(NULL, ",");
      if (az_str != NULL && el_str != NULL) {
        target_AZ = atoi(az_str);
        target_EL = atoi(el_str);
      }
      else {
        // 受信失敗など
        command = 0;
      }
    break;
    }
    case ORDER_SET_ERROR: {
      char* az_str = strtok(NULL, ",");
      char* el_str = strtok(NULL, ",");
      if (az_str != NULL && el_str != NULL) {
        allowed_error_AZ = atoi(az_str);
        allowed_error_EL = atoi(el_str);
      }
      else {
        command = 0;
      }
    break;
    }
    case ORDER_SET_FREQ: {
      char* freq_str = strtok(NULL, ",");
      if (freq_str != NULL) {
        ctrl_interval_ms = atoi(freq_str);
      }
      else {
        command = 0;
      }
    break;
    }
  }
  return command;
}

void send_data(int est_AZ, int est_EL) {
  int error_AZ = target_AZ - est_AZ;
  int error_EL = target_EL - est_EL;
  Serial.print(ORDER_GET);
  Serial.print(',');
  Serial.print(est_AZ);
  Serial.print(',');
  Serial.print(est_EL);
  Serial.print(',');
  Serial.print(target_AZ);
  Serial.print(',');

  Serial.print(target_EL);
  Serial.print(',');
  Serial.print(error_AZ);
  Serial.print(',');
  Serial.print(error_EL);
  Serial.print(',');
  Serial.print(ctrl_interval_ms);
  Serial.print('\n');
}
