#include <Arduino.h>
#include "motor_control.h"

// volatile功能，告訴編譯器，這個變數可能會在程式的不同部分被更改
// 所以編譯器要保持從記憶體中讀取變數的最新值，而不是從暫存器中讀取
volatile long pulseLeft = 0;  // 左輪的脈衝數
volatile long pulseRight = 0; // 右輪的脈衝數

// 更新IR感測器
void IR_update()
{
    // 讀取IR感測器，白色為0，黑色為1
    IR_LL = analogRead(IR[0]) > 450 ? 1 : 0;
    IR_L = analogRead(IR[1]) > 450 ? 1 : 0;
    IR_M = analogRead(IR[2]) > 450 ? 1 : 0;
    IR_R = analogRead(IR[3]) > 450 ? 1 : 0;
    IR_RR = analogRead(IR[4]) > 450 ? 1 : 0;
}

// 更新左輪的脈衝數
void updateLeftPulse()
{
    pulseLeft++;
}

// 更新右輪的脈衝數
void updateRightPulse()
{
    pulseRight++;
}

// 控制左右輪的馬達
void motor(int speedL, int speedR)
{
    digitalWrite(motorLeftDir, speedL < 0 ? LOW : HIGH);
    digitalWrite(motorRightDir, speedR < 0 ? LOW : HIGH);
    analogWrite(motorLeftPWM, abs(speedL));
    analogWrite(motorRightPWM, abs(speedR));
}

// 馬達脈衝對齊控制函數
void controlMotors(int initialSpeedL, int initialSpeedR, long targetPulses, bool autoSync)
{
    const int minimumSpeed = -255; // 最小速度
    const int maximumSpeed = 255;  // 最大速度
    float Kp = 10;                 // 比例增益，需要根據實際情況調整
    float Kd = 5;                  // 微分增益，需要根據實際情況調整

    int speedL = initialSpeedL;
    int speedR = initialSpeedR;

    pulseLeft = 0;
    pulseRight = 0;

    long lastPulseDifference = 0;

    while (pulseLeft < targetPulses && pulseRight < targetPulses)
    {
        if (autoSync)
        {
            long pulseDifference = pulseLeft - pulseRight;
            long pulseDifferenceChange = pulseDifference - lastPulseDifference;

            int adjustment = Kp * pulseDifference + Kd * pulseDifferenceChange;

            // 根據速度的正負號進行調整
            if (initialSpeedL >= 0)
                speedL = initialSpeedL - adjustment;
            else
                speedL = initialSpeedL + adjustment;

            if (initialSpeedR >= 0)
                speedR = initialSpeedR + adjustment;
            else
                speedR = initialSpeedR - adjustment;

            speedL = constrain(speedL, minimumSpeed, maximumSpeed);
            speedR = constrain(speedR, minimumSpeed, maximumSpeed);

            lastPulseDifference = pulseDifference;
        }
        motor(speedL, speedR);
    }

    motor(0, 0); // 停止馬達
    Serial.println("運行完成");
    Serial.print("Left Pulses: ");
    Serial.println(pulseLeft);
    Serial.print("Right Pulses: ");
    Serial.println(pulseRight);
    Serial.print("speedL: ");
    Serial.println(speedL);
    Serial.print("speedR: ");
    Serial.println(speedR);
    Serial.println("==================================");
}

// 沿著黑線走，紅外線感測器的數值為0~1023，白色為0，黑色為1023
// 排列方式為IR[0]~IR[4]，IR[0]為最左邊的感測器，IR[4]為最右邊的感測器
// 讀取IR感測器，白色為0，黑色為1
void PID_trail()
{
    const int minimumSpeed = -255; // 最小速度
    const int maximumSpeed = 255;  // 最大速度
    float Kp = 310;                // 70=速度150,
    float Kd = 0;                  // 50 數值越大對於大轉彎反應越慢
    float Ki = 0;                  // 0.1積分增益，需要根據實際情況調整

    int baseSpeed = 250; // 基本速度
    int lastError = 0;   // 上一次的偏差值
    int integral = 0;    // 積分項

    while (true)
    {
        // 讀取IR感測器，白色為0，黑色為1
        int IR_LL = analogRead(IR[0]) > 450 ? 1 : 0;
        int IR_L = analogRead(IR[1]) > 450 ? 1 : 0;
        int IR_M = analogRead(IR[2]) > 450 ? 1 : 0;
        int IR_R = analogRead(IR[3]) > 450 ? 1 : 0;
        int IR_RR = analogRead(IR[4]) > 450 ? 1 : 0;

        if (IR_LL == 1 || IR_RR == 1)
        {
            break;
        }
        // 計算偏差值
        // int error = IR_LL * -4 + IR_L * -1 + IR_M * 0 + IR_R * 1 + IR_RR * 4;
        int error = IR_L * -1 + IR_M * 0 + IR_R * 1;
        if (IR_M == 1 && IR_L == 0 && IR_R == 0)
        {
            int error = 0;
        }

        // 計算積分項
        integral += error;

        // 計算微分項
        int derivative = error - lastError;

        // 計算調整值
        int adjustment = Kp * error + Ki * integral + Kd * derivative;

        // 計算新的馬達速度
        int speedL = baseSpeed + adjustment;
        int speedR = baseSpeed - adjustment;

        // 限制速度在最小和最大速度之間
        speedL = constrain(speedL, minimumSpeed, maximumSpeed);
        speedR = constrain(speedR, minimumSpeed, maximumSpeed);

        // 設置馬達速度
        motor(speedL, speedR);

        // 更新上一次的偏差值
        lastError = error;
    }
    stop();
}

void trail()
{
    // 讀取IR感測器，白色為0，黑色為1
    int IR_LL = analogRead(IR[0]) > 450 ? 1 : 0;
    int IR_L = analogRead(IR[1]) > 450 ? 1 : 0;
    int IR_M = analogRead(IR[2]) > 450 ? 1 : 0;
    int IR_R = analogRead(IR[3]) > 450 ? 1 : 0;
    int IR_RR = analogRead(IR[4]) > 450 ? 1 : 0;
    if (IR_M)
    {
        if (IR_L)
        {
            small_turn_left();
        }
        else if (IR_R)
        {
            small_turn_right();
        }
        else if (IR_L == 0 && IR_R == 0)
        {
            forward();
        }
    }
    else
    {
        if (IR_L)
        {
            // while (!(analogRead(IR[2]) > 450))
            {
                mid_turn_left();
            }
        }
        else if (IR_R)
        {
            // while (!(analogRead(IR[2]) > 450))
            {
                mid_turn_right();
            }
        }
    }
}

void forward()
{
    motor(100, 100);
}

void small_turn_left()
{
    motor(80, 100);
}

void small_turn_right()
{
    motor(100, 80);
}

void mid_turn_left()
{
    motor(0, 100);
}
void mid_turn_right()
{
    motor(100, 0);
}

void big_turn_left()
{
    motor(-100, 100);
}

void big_turn_right()
{
    motor(100, -100);
}

void stop()
{
    motor(0, 0);
}
