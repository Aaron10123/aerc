#include <Arduino.h>
#include <SoftwareSerial.h>
#include "motor_control.h"

const int motorLeftPWM = 5;  // 左輪馬達 PWM 控制, 轉速0~255
const int motorRightPWM = 6; // 右輪馬達 PWM 控制, 轉速0~255
const int motorLeftDir = 4;  // 左輪馬達方向
const int motorRightDir = 7; // 右輪馬達方向
const int buttonPin = 8;     // 按鈕連接到 Pin 2
int buttonState = 0;         // 變量來儲存按鈕狀態

volatile int IR_LL = 0;
volatile int IR_L = 0;
volatile int IR_M = 0;
volatile int IR_R = 0;
volatile int IR_RR = 0;

const int trigPin = 2;                // 超音波 trig 引腳
const int echoPin = 3;                // 超音波 echo 引腳
float distance = 0;                   // 超音波量測距離
volatile unsigned long echoStart = 0; // 超音波回波開始時間
volatile unsigned long echoEnd = 0;   // 超音波回波結束時間

// A1~A5為紅外線數值
const int IR[5] = {A1, A2, A3, A4, A5};
// ew
unsigned long startTime = 0; // 新增：記錄開始時間
unsigned long lapTime = 0;   // 新增：記錄單圈時間

void setup()
{
    // 設置馬達和編碼器的引腳
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(motorLeftPWM, OUTPUT);
    pinMode(motorRightPWM, OUTPUT);
    pinMode(motorLeftDir, OUTPUT);
    pinMode(motorRightDir, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(A0, INPUT); // 設定A0為輸入

    // 將IR設定為輸入
    for (int i = 0; i < 5; i++)
    {
        pinMode(IR[i], INPUT); // 數值為0~1023，白色為0，黑色為1023
    }
    attachInterrupt(digitalPinToInterrupt(echoPin), echoISR, CHANGE);
    OLED_init(); // OLED 初始化
    // Serial.begin(9600);
}
int error = 0;
void loop()
{
    // 按按鈕
    buttonState = digitalRead(buttonPin);
    while (buttonState)
    {
        buttonState = digitalRead(buttonPin);
        OLED_display(); // OLED 顯示IR真實數值
    }
    delay(1000);

    startTime = millis(); // 新增：記錄開始時間

    // * /////////////////////////////////////C圖/////////////////////////////////////
    // ! /////////////////////////////////////1號電池7.94~7.84  2號電池7.96~7.85/////////////////////////////////////
    PID_right(100, 125, -25); //(1)的右轉
    PID_right(100, 125, -25); //(1)的右轉
    PID_left(100, -100, 100); //(2)的銳角左轉
    PID_trail(false, []()
              { return (false); }, 40, 0, 0, 100, 500); // 讓車子盡量直行
    PID_trail(false, []()
              { return (IR_L == 0 && IR_M == 0 && IR_R == 0); }, 30, 0, 0, 100, 0);

    unsigned long startTime = millis();
    while (true)
    {
        IR_update();
        motor(110, -110);

        if (millis() - startTime >= 200)
        {
            break;
        }
    }
    stop(); //! 一定要停止
    delay(500);
    while (!(IR_L || IR_M || IR_R))
    {
        IR_update();
        motor(110, 110);
    }

    error = PID_trail_left(false, []()
                           { return (false); }, 55, 0, 0, 110, 1000); //(4↑)  1250
    error = PID_trail_left(false, []()
                           { return (IR_RR == 1); }, 55, 0, 0, 110, 0, error); //(4↑)  1250
    error = PID_trail_left(false, []()
                           { return (IR_RR == 0); }, 55, 0, 0, 110, 0, error); //(4↑)  1250
    stop();
    while (!(IR_RR == 1))
    {
        IR_update();
        motor(165, -50);
    }
    while (!(IR_RR == 0))
    {
        IR_update();
        motor(165, -50);
    }
    while (!(IR_LL == 1))
    {
        IR_update();
        motor(110, 110);
    }
    while (!(IR_LL == 0))
    {
        IR_update();
        motor(110, 110); // 經過(5)左邊岔路
    }
    PID_left(100, -100, 100, 30, 0, true); //(5)的左轉
    PID_right(100, 100, -100);             //(6)的右轉
    PID_right(100, 100, -100);             //(6)->(7)的右轉
    PID_left(100, -100, 100, 40, 0);       //(7)的左轉
    PID_trail(false, []()
              { return (false); }, 40, 0, 0, 100, 500); //(8)的穩定一小段
    PID_left(150, -90, 90, 60, 0, true);                //(8)>(9)的左轉
    error = PID_trail(false, []()
                      { return (IR_RR == 1); }, 30, 0, 0, 100, 0);
    PID_trail(false, []()
              { return (IR_RR == 0); }, 30, 0, 0, 100, 0, false, error); //(9↑)
    PID_right(100, 125, -25);                                            //(9)>(10)的右轉
    PID_right(150, 100, -100, 60, 0, true);                              //(10)>(12)的T右轉
    PID_left(150, -90, 90, 60, 0, true);                                 //(12)的左轉
    error = PID_trail(false, []()
                      { return (false); }, 40, 0, 0, 100, 500); //(12)的穩定一小段
    PID_trail(false, []()
              { return (IR_LL == 0 && IR_L == 0 && IR_M == 0 && IR_R == 0 && IR_RR == 0); }, 40, 0, 0, 100, 0, false, error); //(12↑)
    IR_update();
    while (!(IR_LL || IR_L || IR_M || IR_R || IR_RR))
    {
        IR_update();
        motor(105, 100); // ! //要調整速度
    }
    PID_left(100, -100, 100, 40, 0, true);  // (12)>(13)的左轉
    PID_left(100, -100, 100, 40, 0, true);  // (13)的左轉
    PID_right(100, 100, -100, 40, 0, true); // (14)的右轉
    error = PID_trail(false, []()
                      { return (false); }, 40, 0, 0, 100, 500); //(15)的穩定一小段
    PID_right(100, 100, -100, 40, 0, true);                     // (15)的右轉
    PID_right(100, 100, -100, 40, 0, true);                     // (15)>(16)的右轉
    error = PID_trail(false, []()
                      { return (false); }, 40, 0, 0, 100, 500); //(16)的穩定一小段
    // 右U開始
    error = PID_trail(true, []()
                      { return (IR_RR == 1); }, 80, 70, 0, 250, 0); //(17)的弧線
    stop();
    error = PID_trail(true, []()
                      { return (IR_RR == 0); }, 30, 0, 0, 100, 0, false, error); //(17)的弧線
    error = PID_trail(false, []()
                      { return (IR_R == 1); }, 30, 0, 0, 100, 0, false, error); //(17)的弧線

    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 80, 70, 0, 250, 0); //(17)的弧線
    stop();
    error = PID_trail(true, []()
                      { return (IR_LL == 0); }, 30, 0, 0, 70, 0, false, error); //(17)的弧線
    while (!(IR_LL == 1))
    {
        IR_update();
        motor(-100, 100);
    }
    while (!(IR_LL == 0))
    {
        IR_update();
        motor(-100, 100);
    }

    error = PID_trail(false, []()
                      { return (false); }, 40, 0, 0, 100, 500); //(18)的穩定一小段
    PID_left(150, -90, 90, 60, 0, true);                        // (18)的左轉
    PID_trail(false, []()
              { return (IR_R && IR_M && IR_L); }, 80, 90, 0, 250, 0, false, error); // 直走直到終點
    // ! /////////////////////////////////////finish/////////////////////////////////////
    stop();
    lapTime = millis() - startTime; // 新增：計算單圈時間
}
