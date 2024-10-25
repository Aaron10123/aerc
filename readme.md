## 主要檔案說明

### [include/motor_control.h](include/motor_control.h)

此檔案定義了馬達控制相關的全域變數和函數宣告，包括：

- `motorLeftPWM`：左輪馬達 PWM 控制
- `motorRightPWM`：右輪馬達 PWM 控制
- `motorLeftDir`：左輪馬達方向
- `motorRightDir`：右輪馬達方向
- `IR[5]`：紅外線數值

函數宣告：

- `IR_update()`：更新IR感測器
- `updateLeftPulse()`：更新左輪的脈衝數
- `updateRightPulse()`：更新右輪的脈衝數
- `motor(int speedL, int speedR)`：控制左右輪的馬達
- `controlMotors(int speedL, int speedR, long targetPulses, bool autoSync)`：控制左右輪的馬達，並移動到指定的目標脈衝數
- `PID_trail(bool useFiveIR, bool (*exitCondition)(), float Kp, float Kd, float Ki, int baseSpeed, unsigned long ms)`：PID循跡
- `PID_trail_left(bool useFiveIR, bool (*exitCondition)(), float Kp, float Kd, float Ki, int baseSpeed, unsigned long ms)`：PID循跡（左）
- `trail()`：循跡
- `small_turn_left()`：小左轉
- `small_turn_right()`：小右轉
- `mid_turn_left()`：中左轉
- `mid_turn_right()`：中右轉
- `big_turn_left()`：大左轉
- `big_turn_right()`：大右轉
- `forward()`：前進
- `stop()`：停止
- `cmd_for_ms(void (*command)(), unsigned long ms)`：執行某個指令一段時間
- `trail_X()`：緩慢循跡
- `slow_trail()`：緩慢循跡
- `OLED_init()`：OLED 初始化
- `OLED_display()`：OLED 顯示

### [src/main.cpp](src/main.cpp)

此檔案是專案的進入點，包含了初始化和主循環邏輯。

### [src/motor_control.cpp](src/motor_control.cpp)

此檔案實現了 `motor_control.h` 中宣告的函數。

## 開發環境

- PlatformIO
- Arduino 框架

## 安裝與使用

1. 安裝 PlatformIO IDE。
2. 克隆此專案到本地端。
3. 使用 PlatformIO IDE 開啟專案目錄。
4. 編譯並上傳程式到 Arduino 板。
