#include <Arduino.h>
#include <SoftwareSerial.h>
#include "motor_control.h"

// 左右輪的馬達和編碼器引腳
const int encoderLeftPinA = 2;  // 左輪編碼器 A 相
const int encoderRightPinA = 3; // 右輪編碼器 A 相
const int motorLeftPWM = 5;     // 左輪馬達 PWM 控制
const int motorRightPWM = 6;    // 右輪馬達 PWM 控制
const int motorLeftDir = 4;     // 左輪馬達方向
const int motorRightDir = 7;    // 右輪馬達方向
const int buttonPin = 8;        // 按鈕連接到 Pin 2
int buttonState = 0;            // 變量來儲存按鈕狀態

// A1~A5為紅外線數值
const int IR[5] = {A1, A2, A3, A4, A5};

void setup()
{
    // 設置馬達和編碼器的引腳
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(encoderLeftPinA, INPUT);
    pinMode(encoderRightPinA, INPUT);
    pinMode(motorLeftPWM, OUTPUT);
    pinMode(motorRightPWM, OUTPUT);
    pinMode(motorLeftDir, OUTPUT);
    pinMode(motorRightDir, OUTPUT);
    // 將IR設定為輸入
    for (int i = 0; i < 5; i++)
    {
        pinMode(IR[i], INPUT); // 數值為0~1023，白色為0，黑色為1023
    }

    // 設置編碼器中斷
    // attachInterrupt(digitalPinToInterrupt(encoderLeftPinA), updateLeftPulse, RISING);
    // attachInterrupt(digitalPinToInterrupt(encoderRightPinA), updateRightPulse, RISING);

    Serial.begin(9600);
}

void loop()
{
    buttonState = digitalRead(buttonPin);
    if (buttonState == LOW)
    {
        // PD循跡
        PID_trail();
    }
}
