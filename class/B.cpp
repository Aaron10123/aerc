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

    // * /////////////////////////////////////B圖/////////////////////////////////////
    // ! /////////////////////////////////////1號電池7.94~7.84  2號電池7.96~7.85/////////////////////////////////////
    // // 右U開始
    // error = PID_trail(true, []()
    //                   { return (IR_RR == 1); }, 100, 100, 0, 200, 0);
    // stop();
    // error = PID_trail(true, []()
    //                   { return (IR_RR == 0); }, 20, 0, 0, 70, 0, false, error);
    // error = PID_trail(false, []()
    //                   { return (IR_R == 1); }, 20, 0, 0, 70, 0, false, error);
    // stop();

    // // 左U開始
    // PID_trail(false, []()
    //           { return (false); }, 50, 0, 0, 100, 650);
    // error = PID_trail(true, []()
    //                   { return (IR_LL == 1); }, 100, 100, 0, 200, 0);
    // stop();
    // error = PID_trail(true, []()
    //                   { return (IR_LL == 0); }, 20, 0, 0, 70, 0, false, error);
    // error = PID_trail(false, []()
    //                   { return (IR_L == 1); }, 20, 0, 0, 70, 0, false, error);
    // stop();

    // PID_right(100, 125, -25, 50, 0, true);  //(4) U結束後右直角
    // PID_right(150, 125, -125, 75, 0, true); //(4) 右直角準備接虛線
    // delay(10);                              // ! 調整一下數值讓車子稍微正一些
    // stop();
    // delay(75); // ! 一定要等待車子穩定
    // PID_trail(false, []()
    //           { return (false); }, 60, 0, 0, 150, 800); // (5)虛線一開始穩定循跡
    // PID_left(150, -125, 125, 60, 0, true);              // (5)虛線後左直角
    // stop();
    // PID_left(100, -90, 125, 30, 0);  // (6)左直角
    // PID_right(100, 125, -25, 40, 0); //(6)
    // error = PID_trail(false, []()
    //                   { return (IR_RR == 1); }, 80, 0, 0, 250, 0, false, error); //(8)
    // error = PID_trail(false, []()
    //                   { return (IR_RR == 0); }, 80, 0, 0, 250, 0, false, error); //(8)
    // error = PID_trail(false, []()
    //                   { return (IR_RR == 1); }, 80, 0, 0, 250, 0, false, error); //(8)
    // error = PID_trail(false, []()
    //                   { return (IR_RR == 0); }, 80, 0, 0, 250, 0, false, error); //(8)
    // error = PID_trail(false, []()
    //                   { return (IR_RR == 1); }, 80, 0, 0, 250, 0, false, error); //(8)
    // error = PID_trail(false, []()
    //                   { return (IR_RR == 0); }, 80, 0, 0, 250, 0, false, error); //(8)

    // PID_left(100, -25, 90, 30, 0, true); //(8)的左直角
    // PID_trail(false, []()
    //           { return (false); }, 40, 0, 0, 100, 300); // 穩定一小段
    // PID_right(100, 100, -100, 30, 0);                   //(9)的右直角
    // PID_left(100, -100, 100);                           //(10)的左直角
    PID_right(100, 100, -100, 30, 0);                   //(10)的右直角
    // 右U開始
    error = PID_trail(true, []()
                      { return (IR_RR == 1); }, 80, 70, 0, 250, 0); //(11)的弧線
    stop();
    error = PID_trail(true, []()
                      { return (IR_RR == 0); }, 30, 0, 0, 100, 0, false, error); //(11)的弧線
    error = PID_trail(false, []()
                      { return (IR_R == 1); }, 30, 0, 0, 100, 0, false, error); //(11)的弧線

    error = PID_trail(true, []()
                      { return (IR_RR == 1); }, 80, 70, 0, 250, 0); //(11)的弧線
    stop();
    error = PID_trail(true, []()
                      { return (IR_RR == 0); }, 30, 0, 0, 100, 0, false, error); //(11)的弧線
    error = PID_trail(false, []()
                      { return (IR_R == 1); }, 30, 0, 0, 100, 0, false, error); //(11)的弧線

    // 左U開始
    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 100, 100, 0, 200, 0); //(12)的弧線
    stop();
    error = PID_trail(true, []()
                      { return (IR_LL == 0); }, 20, 0, 0, 70, 0, false, error); //(12)的弧線
    error = PID_trail(false, []()
                      { return (IR_L == 1); }, 20, 0, 0, 70, 0, false, error); //(12)的弧線

    PID_trail(false, []()
              { return (false); }, 30, 0, 0, 100, 500); //(12)的穩定循跡

    // ! ///////////////////////////////////////開始避障///////////////////////////////////////
    distance = 0;
    PID_trail(false, []()
              { return (false); }, 30, 0, 0, 100, 100, true);
    PID_trail(false, []()
              { return (distance <= 20 && distance > 0); }, 30, 0, 0, 100, 0, true);
    stop();
    IR_update();
    while (!(IR_LL))
    {
        motor(100, -100);
        IR_update();
    }

    while (!(IR_LL == 0))
    {
        motor(250, -250);
        IR_update();
    }
    while (!(IR_L))
    {
        IR_update();
        motor(100, 190);
    }
    motor(100, 100);
    delay(100);
    IR_update();
    while (!(IR_R))
    {
        IR_update();
        motor(125, -25); // ! 繞瓶子速度控制
    }

    distance = 0;
    PID_trail(false, []()
              { return (distance <= 20 && distance > 0); }, 30, 0, 0, 100, 0, true);
    stop();
    IR_update();
    while (!(IR_RR))
    {
        motor(-100, 100);
        IR_update();
    }

    while (!(IR_RR == 0))
    {
        motor(-250, 250);
        IR_update();
    }
    stop();
    delay(1000);
    while (!(IR_R))
    {
        IR_update();
        motor(210, 110); // ! 繞瓶子速度控制
    }
    while (!(IR_LL))
    {
        IR_update();
        motor(100, 100);
    }
    while (!(IR_LL == 0))
    {
        IR_update();
        motor(100, 100);
    }
    while (!(IR_L))
    {
        IR_update();
        motor(-25, 125);
    }
    stop();
    delay(100);
    // ! ///////////////////////////////////////結束避障///////////////////////////////////////
    PID_left(100, -100, 100); // 結束避障後左直角
    PID_left(100, -100, 100); //(15)
    PID_trail(false, []()
              { return (false); }, 40, 0, 0, 100, 400); //(16)讓車子穩定
    PID_trail(false, []()
              { return (IR_RR == 0 && IR_R == 0 && IR_M == 0 && IR_L == 0 && IR_LL == 0); }, 40, 0, 0, 100, 0); //(16)循跡到白色
    while (!(IR_RR || IR_R || IR_M || IR_L || IR_LL))
    {
        motor(255, 250); //(16)直走到黑色
        IR_update();
    }
    PID_right(100, 100, -100, 30, 0, true); //(16)的右直角
    error = PID_trail(true, []()
                      { return (false); }, 30, 0, 0, 100, 2000); //! 記得測試一下時間是否可以跑完2個U
    PID_trail(false, []()
              { return (IR_R && IR_M && IR_L); }, 80, 90, 0, 250, 0, false, error); // 直走直到終點
    // ! /////////////////////////////////////finish/////////////////////////////////////
    stop();
    lapTime = millis() - startTime; // 新增：計算單圈時間
}
