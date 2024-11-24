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
    // 將IR設定為輸入
    for (int i = 0; i < 5; i++)
    {
        pinMode(IR[i], INPUT); // 數值為0~1023，白色為0，黑色為1023
    }
    attachInterrupt(digitalPinToInterrupt(echoPin), echoISR, CHANGE);
    OLED_init(); // OLED 初始化
    // Serial.begin(9600);
}

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

    // ! /////////////////////////////////////1號電池7.94~7.84  2號電池7.96~7.85/////////////////////////////////////

    // ! /////////////////////////////////////1-12/////////////////////////////////////

    // PID_trail(true, []()
    //           { return (false); }, 80, 0, 0, 250, 500); // 1的循跡
    // PID_trail(true, []()
    //           { return (false); }, 30, 0, 0, 100, 3400); // 2 - 4的循跡

    // PID_right(100, 125, -25); // 4的右直角
    // PID_right(100, 125, -25); // 4的右直角

    // motor(125, -25);
    // delay(150);
    // stop();
    // while (!(IR_LL))
    // {
    //     IR_update();
    //     motor(100, 100);
    // }
    // while (!(IR_LL == 0 && IR_L == 0 && IR_M == 0 && IR_R == 0 && IR_RR == 0))
    // {
    //     IR_update();
    //     motor(100, 100);
    // }
    // stop();
    // while (!(IR_RR == 1))
    // {
    //     IR_update();
    //     motor(60, 125);
    // }

    // while (!(IR_RR == 0))
    // {
    //     IR_update();
    //     motor(60, 125);
    // }
    // IR_update();
    // motor(100, 100);
    // delay(150);
    // while (!(IR_RR == 1))
    // {
    //     IR_update();
    //     motor(125, -100);
    // }
    // while (!(IR_RR == 0))
    // {
    //     IR_update();
    //     motor(125, -100);
    // }

    // PID_left(100, -100, 100);
    // PID_trail(true, []()
    //           { return (false); }, 40, 0, 0, 100, 500);
    // PID_right(100, 100, -100);
    // PID_left(100, -100, 100);
    // PID_trail(false, []()
    //           { return (false); }, 40, 0, 0, 100, 200);
    // PID_left(100, -100, 100, true);
    // PID_trail(false, []()
    //           { return (false); }, 40, 0, 0, 100, 1000);

    // PID_trail(false, []()
    //           { return (IR_LL == 0 && IR_L == 0 && IR_M == 0 && IR_R == 0 && IR_RR == 0); }, 40, 0, 0, 100, 0);
    // IR_update();

    // while (!(IR_LL|| IR_L || IR_M || IR_R || IR_RR))
    // {
    //     IR_update();
    //     motor(110, 100);  // ! //要調整速度
    // }
    // PID_trail(true, []()
    //           { return (false); }, 30, 0, 0, 100, 500);
    // PID_left(100, -25, 125);
    // ! /////////////////////////////////////13-24/////////////////////////////////////
    // for (int i = 0; i < 3; i++)
    // {
    int error = 0;
    PID_trail(false, []()
              { return (false); }, 40, 30, 0, 100, 300); // 13
    error = PID_trail(true, []()
                      { return (IR_RR == 1); }, 60, 0, 0, 150, 0); // 14
    stop();
    error = PID_trail(true, []()
                      { return (IR_RR == 0); }, 40, 0, 0, 100, 0, false, error);
    error = PID_trail(true, []()
                      { return (IR_RR == 1); }, 40, 0, 0, 100, 0, false, error);
    PID_trail(false, []()
              { return (false); }, 40, 30, 0, 100, 300);
    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 60, 0, 0, 150, 0); // 15
    stop();
    error = PID_trail(true, []()
                      { return (IR_LL == 0); }, 15, 0, 0, 50, 0, false, error); // 16
    error = PID_trail(true, []()
                      { return (IR_L == 1); }, 15, 0, 0, 50, 0, false, error); // 16
    PID_trail(false, []()
              { return (false); }, 40, 30, 0, 70, 400); // ! //修 //17
    error = PID_trail(true, []()
                      { return (IR_RR == 1); }, 60, 0, 0, 150, 0); // 17
    stop();
    error = PID_trail(true, []()
                      { return (IR_RR == 0); }, 15, 0, 0, 50, 0, false, error);
    error = PID_trail(true, []()
                      { return (IR_R == 1); }, 15, 0, 0, 50, 0, false, error);

    PID_trail(false, []()
              { return (false); }, 40, 30, 0, 100, 400); // ! //修 //18
    error = PID_trail(false, []()
                      { return (IR_LL == 1); }, 60, 0, 0, 150, 0);
    stop();
    error = PID_trail(true, []()
                      { return (IR_LL == 0); }, 15, 0, 0, 50, 0, false, error); //
    error = PID_trail(true, []()
                      { return (IR_L == 1); }, 15, 0, 0, 50, 0, false, error); //
    PID_trail(false, []()
              { return (false); }, 40, 30, 0, 70, 400); //
    error = PID_trail(true, []()
                      { return (IR_RR == 1); }, 60, 0, 0, 150, 0); //
    stop();
    error = PID_trail(true, []()
                      { return (IR_RR == 0); }, 15, 0, 0, 50, 0, false, error);
    error = PID_trail(true, []()
                      { return (IR_R == 1); }, 15, 0, 0, 50, 0, false, error);

    PID_trail(false, []()
              { return (false); }, 40, 30, 0, 100, 400); //

    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 60, 0, 0, 150, 0); //
    stop();
    error = PID_trail(true, []()
                      { return (IR_LL == 0); }, 40, 0, 0, 130, 0, false, error);
    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 40, 0, 0, 130, 0, false, error);
    PID_right(100, 125, -25);
    PID_right(100, 125, -25);

    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 60, 0, 0, 150, 0); //
    stop();

    //!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    error = PID_trail(true, []()
                      { return (IR_LL == 0); }, 40, 0, 0, 110, 0, false, error);
    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 40, 0, 0, 110, 0, false, error);

    PID_right(100, 125, -25, 40, 30, true); // 27
    PID_right(100, 145, -55, 40, 30, true); // 27

    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 60, 0, 0, 150, 0); //
    stop();
    error = PID_trail(true, []()
                      { return (IR_LL == 0); }, 40, 0, 0, 110, 0, false, error);
    error = PID_trail(true, []()
                      { return (IR_LL == 1); }, 40, 0, 0, 110, 0, false, error);

    PID_trail(false, []()
              { return (false); }, 60, 40, 0, 70, 400); //
    PID_left(100, -25, 155, 40, 30, true);              //
                                                        //!////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    IR_update();
    motor(100, 100);
    delay(100);
    while (!(IR_RR))
    {
        IR_update();
        motor(125, -25);
    }

    PID_right(100, 125, -25);
    PID_right(100, 125, -25);
    PID_right(100, 125, -25);

    PID_trail(false, []()
              { return (IR_R == 1 && IR_M == 1 && IR_L == 1); }, 80, 90, 0, 250, 0);

    stop();
}
