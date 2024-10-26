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
volatile long pulseLeft = 0;  // 左輪的脈衝數
volatile long pulseRight = 0; // 右輪的脈衝數

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
    // 將IR設定為輸入
    for (int i = 0; i < 5; i++)
    {
        pinMode(IR[i], INPUT); // 數值為0~1023，白色為0，黑色為1023
    }

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

    while (!(IR_RR)) // 直到RR紅外線看到黑
    {
        trail();
    }
    while (!(IR_RR == 0))
    {
        forward();
    }
    IR_update();
    while (!(IR_RR)) // 直到RR紅外線看到黑(1↑)
    {
        IR_update();
        motor(80, -110);
    }

    while (!(IR_RR == 0))
    {
        IR_update();
    }

    IR_update();
    while (!(IR_RR)) //(2↑)
    {
        trail();
    }

    while (IR_RR)
    {
        forward();
    }

    while (!(IR_RR))
    {
        IR_update();
        motor(70, -100);
    }
    while (!(IR_LL))
    {
        forward();
    }
    while (!(IR_LL == 0 && IR_L == 0 && IR_M == 0 && IR_R == 0 && IR_RR == 0))
    {
        forward();
    }
    while (!(IR_LL))
    {
        IR_update();
        motor(-100, 70); // 左迴轉
    }
    forward();
    delay(100);

    PID_trail(false, []()
              { return (false); }, 40, 0, 0, 100, 500); //(3↑)  1250
    PID_trail(false, []()
              { return (IR_L == 0 && IR_M == 0 && IR_R == 0); }, 40, 0, 0, 100, 0); //(3↑)  1250
    stop();

    unsigned long startTime = millis();
    // while (!(IR_M))
    while (true)
    {
        IR_update();
        motor(70, -100);

        if (millis() - startTime >= 300)
        {
            break;
        }
    }

    PID_trail_left(false, []()
                   { return (false); }, 50, 90, 0, 100, 1250); //(4↑)  1250

    stop();
}
