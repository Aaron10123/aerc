#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

// 連接全域變數，用來在不同的檔案中共享變數

// 左右輪的馬達和編碼器引腳
// extern功能，將變數定義在其他檔案中
// const功能，將變數設定為唯讀
extern const int encoderLeftPinA;  // 左輪編碼器 A 相
extern const int encoderRightPinA; // 右輪編碼器 A 相
extern const int motorLeftPWM;     // 左輪馬達 PWM 控制
extern const int motorRightPWM;    // 右輪馬達 PWM 控制
extern const int motorLeftDir;     // 左輪馬達方向
extern const int motorRightDir;    // 右輪馬達方向
extern const int IR[5];            // A1~A5為紅外線數值

// 指令宣告區，用來快速查看指令的功能，案住Ctrl並點擊名稱即可跳轉到指令的功能

// 更新左輪的脈衝數
void updateLeftPulse();
// 更新右輪的脈衝數
void updateRightPulse();
// 控制左右輪的馬達
void motor(int speedL, int speedR);
// 控制左右輪的馬達，並移動到指定的目標脈衝數
void controlMotors(int speedL, int speedR, long targetPulses, bool autoSync);
// PD循跡
void PID_trail();
#endif // MOTOR_CONTROL_H