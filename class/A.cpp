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

    // ! /////////////////////////////////////1-12/////////////////////////////////////

    PID_trail(true, []()
              { return (false); }, 75, 0, 0, 250, 500); // 1的循跡
    PID_trail(true, []()
              { return (false); }, 40, 5, 0, 90, 2900); // 2 - 4的循跡

    IR_update();
    while (!(IR_LL == 0 && IR_L == 0 && IR_M == 0 && IR_R == 0 && IR_RR == 0))
    {
        IR_update();
        motor(100, 100);
    }
    while (!(IR_R == 1))
    {
        IR_update();
        motor(90, -90);
    }

    PID_right(100, 100, -100); // 4的右直角
    PID_right(100, 100, -100); // 5的右直角
    delay(70);
    motor(100, 100);
    delay(150);                // 可能要調整 (越過6的十字)
    PID_left(100, -100, 100);  // 6的左直角
    PID_left(100, -100, 100);  // 7的左直角
    PID_right(100, 100, -100); // 8的右直角
    PID_left(100, -100, 100);  // 9的左直角
    PID_right(100, 100, -100); // 10的右直角
    PID_trail(true, []()
              { return (false); }, 30, 0, 0, 200, 300); // 10的循跡
    PID_right(100, 100, -100);                          // 10的右直角
    PID_left(100, -100, 100);                           // 11的左直角
    PID_left(100, -100, 100);                           // 11的銳角
    PID_left(100, -100, 100);                           // 11的左修正

    PID_trail(false, []()
              { return (false); }, 40, 0, 0, 100, 500); // 12的循跡, 讓車子盡量直行

    PID_trail(false, []()
              { return (IR_LL == 0 && IR_L == 0 && IR_M == 0 && IR_R == 0 && IR_RR == 0); }, 40, 0, 0, 100, 0); // 12的循跡, 讓車子盡量直行

    IR_update();
    while (!(IR_LL || IR_L || IR_M || IR_R || IR_RR))
    {
        IR_update();
        motor(255, 255);
    }
    cmd_for_ms(trail, 300);
    PID_left(90, -100, 90, 50, 50, true); // 12的左轉
    // ! /////////////////////////////////////13-24/////////////////////////////////////
    int U_250_time = 300;
    int U_speed_R = 120;
    int U_speed_L = 90;

    for (int i = 0; i < 3; i++)
    {
        // 右U開始
        PID_trail(false, []()
                  { return false; }, 100, 0, 0, 250, U_250_time);
        PID_right(100, U_speed_R, 0, 35, 0, true);
        // 左U開始
        PID_trail(false, []()
                  { return false; }, 100, 0, 0, 250, U_250_time);
        PID_left(100, 0, U_speed_L, 35, 0, true);
    } // 連續大U結束
    // ! /////////////////////////////////////小U開始/////////////////////////////////////
    for (int i = 0; i < 2; i++)
    {
        // TODO: VERY IMPORTANT (test the function): emotional_damage_weeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee();
        // ? ///////////////////////////////////// 測試中 (結束) /////////////////////////////////////
        // PID_trail(false, []()
        //           { return (IR_RR == 1); }, 35, 0, 0, 100, 0);
        // IR_update();
        // while (!(IR_RR == 0))
        // {
        //     IR_update();
        //     motor(100, 100);
        // }
        // IR_update();
        // while (!(IR_RR))
        // {
        //     IR_update();
        //     motor(90, -100);
        // }
        // IR_update();
        // while (!(IR_RR == 0))
        // {
        //     IR_update();
        //     motor(90, -100);
        // }

        // ? ///////////////////////////////////// 測試中 (結束) /////////////////////////////////////
        // PID_trail(false, []()
        //   { return (IR_RR == 1); }, 35, 0, 0, 100, 0);
        //     IR_update();
        //     while (!(IR_RR == 0))
        //     {
        //         IR_update();
        //         motor(100, 100);
        //     }
        //     IR_update();
        //     while (!(IR_RR))
        //     {
        //         IR_update();
        //         motor(90, -100);
        //     }
        //     IR_update();
        //     while (!(IR_RR == 0))
        //     {
        //         IR_update();
        //         motor(90, -100);
        //     }
        //     PID_trail(false, []()
        //               { return (IR_LL == 1); }, 35, 0, 0, 100, 0);
        //     IR_update();
        //     while (!(IR_LL == 0))
        //     {
        //         IR_update();
        //         motor(100, 100);
        //     }
        //     stop();
        //     IR_update();
        //     while (!(IR_LL))
        //     {
        //         IR_update();
        //         motor(0, U_speed_L);
        //     }
        //     while (!(IR_LL == 0))
        //     {
        //         IR_update();
        //         motor(0, U_speed_L);
        //     }
        PID_right(100, 90, -100, 35, 0);
        PID_right(100, 90, -100, 35, 0);
        PID_left(100, 20, U_speed_R, 35, 0, true);
    }

    PID_trail(false, []()
              { return false; }, 30, 10, 0, 100, 400);
    PID_left(100, -100, 100, 35, 0, true);

    distance = 0;
    PID_trail(false, []()
              { return (distance > 0 && distance <= 15); }, 40, 0, 0, 100, 0, true);
    stop();
    // PID_trail(false, []()
    //            { return (false); }, 100, 0, 0, 250, 200);
    // PID_trail(false, []()
    //           { return (IR_LL == 1); }, 35, 0, 0, 100, 0);
    // IR_update();
    // while (!(IR_LL == 0))
    // {
    //     IR_update();
    //     motor(100, 100);
    // }
    // ! /////////////////////////////////////小U結束/////////////////////////////////////
    // IR_update();
    // while (!(IR_LL)) // 左轉直到LL看到黑線
    // {
    //     IR_update();
    //     motor(-100, 90);
    // }
    // while (!(IR_LL == 0)) // 左轉直到LL看到白線
    // {
    //     IR_update();
    //     motor(-100, 90);
    // }
    // ! /////////////////////////////////////開始避障循跡///////////////////////////////////////
    while (!(IR_LL))
    {
        IR_update();
        motor(100, 0);
    }
    while (!(IR_LL == 0))
    {
        IR_update();
        motor(100, 0);
    }
    delay(100);
    while (!(IR_L))
    {
        IR_update();
        motor(40, 100);
    }
    // ! /////////////////////////////////////結束避障循跡///////////////////////////////////////
    PID_right(100, 100, -100);
    PID_right(100, 100, -100);
    PID_right(100, 100, -100);
    PID_trail(false, []()
              { return (IR_R && IR_M && IR_L); }, 35, 0, 0, 250, 0);
    IR_update();
    motor(-100, -100);
    delay(100);

    // PID_trail(false, []()
    //           { return (IR_RR == 1); }, 35, 0, 0, 100, 0);
    // IR_update();
    // while (!(IR_RR == 0))
    // {
    //     IR_update();
    //     motor(100, 100);
    // }
    // while (!(IR_RR))
    // {
    //     IR_update();
    //     motor(90, -100);
    // }
    // while (!(IR_RR == 0))
    // {
    //     IR_update();
    //     motor(90, -100);
    // }
    // PID_trail(false, []()
    //           { return (IR_RR == 1); }, 35, 0, 0, 100, 0);
    // IR_update();
    // while (!(IR_RR == 0))
    // {
    //     IR_update();
    //     motor(100, 100);
    // }
    // while (!(IR_RR))
    // {
    //     IR_update();
    //     motor(90, -100);
    // }
    // while (!(IR_RR == 0))
    // {
    //     IR_update();
    //     motor(90, -100);
    // }
    // PID_trail(false, []()
    //           { return (IR_L && IR_M && IR_R); }, 100, 0, 0, 250, 0);
    // ! /////////////////////////////////////finish///////////////////////////////////////

    stop();
}