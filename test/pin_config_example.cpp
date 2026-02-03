/**
 * @file pin_config_example.cpp
 * @brief 引脚配置头文件使用示例
 *
 * @details 演示如何使用 pin_config.h 管理硬件引脚
 * @note 这是示例代码，不会被编译到最终固件中
 */

#include <Arduino.h>
#include "pin_config.h"
#include <Wire.h>
#include <SPI.h>

// ============================================================================
// 示例 1: 基础引脚使用
// ============================================================================

void example_basic_usage()
{
    Serial.println("=== 示例 1: 基础引脚使用 ===");

    // 使用定义好的引脚常量
    pinMode(LED_BUILTIN_PIN, OUTPUT);
    digitalWrite(LED_BUILTIN_PIN, HIGH); // 点亮 LED

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, HIGH); // 打开蜂鸣器
    delay(100);
    digitalWrite(BUZZER_PIN, LOW); // 关闭蜂鸣器

    // 读取按键状态
    pinMode(EXT_BTN_1_PIN, INPUT);
    bool btn_pressed = digitalRead(EXT_BTN_1_PIN) == LOW;
    Serial.printf("按键 1 状态: %s\n", btn_pressed ? "按下" : "释放");
}

// ============================================================================
// 示例 2: RS485 通信初始化
// ============================================================================

#include <ModbusMaster.h>

ModbusMaster encoder1; // 编码器 1
ModbusMaster encoder2; // 编码器 2

void example_rs485_init()
{
    Serial.println("=== 示例 2: RS485 通信初始化 ===");

    // 使用引脚常量初始化 RS485
    Serial2.begin(RS485_BAUD_RATE, SERIAL_8N1, RS485_1_RX_PIN, RS485_1_TX_PIN);

    // 初始化方向控制引脚
    pin_init_rs485(1);

    // 配置 Modbus
    encoder1.begin(1, Serial2); // 地址 1

    // 切换到发送模式
    digitalWrite(RS485_1_DE_PIN, HIGH);
    delay(1);

    // 发送数据...

    // 切换回接收模式
    digitalWrite(RS485_1_DE_PIN, LOW);
}

// ============================================================================
// 示例 3: SPI 设备初始化（LCD 和 SD 卡）
// ============================================================================

#include <TFT_eSPI.h>
#include <SdFat.h>

TFT_eSPI tft = TFT_eSPI();
SdFat sd;

void example_spi_devices()
{
    Serial.println("=== 示例 3: SPI 设备初始化 ===");

    // 初始化 SPI 总线（使用定义的引脚）
    SPI.begin(SPI_SCLK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);

    // 初始化 LCD（TFT_eSPI 会自动使用 User_Setup.h 中的配置）
    // 但我们可以验证引脚是否一致
    Serial.printf("LCD 引脚 - CS: %d, DC: %d, BLK: %d\n",
                  LCD_CS_PIN, LCD_DC_PIN, LCD_BLK_PIN);
    tft.init();

    // 控制背光
    pinMode(LCD_BLK_PIN, OUTPUT);
    digitalWrite(LCD_BLK_PIN, HIGH); // 打开背光

    // 初始化 SD 卡
    if (sd_card_detected())
    {
        Serial.println("检测到 SD 卡");
        if (sd.begin(SD_CS_PIN, SD_SCK_MHZ(25)))
        {
            Serial.println("SD 卡初始化成功");
        }
    }
    else
    {
        Serial.println("未检测到 SD 卡");
    }
}

// ============================================================================
// 示例 4: I2C 设备初始化（PCA9555 GPIO 扩展器）
// ============================================================================

#include <TCA9555.h>

TCA9555 ioExpander;

void example_i2c_devices()
{
    Serial.println("=== 示例 4: I2C 设备初始化 ===");

    // 使用定义的 I2C 引脚初始化
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    // 初始化 PCA9555（使用定义的地址）
    if (ioExpander.begin(PCA9555_I2C_ADDR))
    {
        Serial.printf("PCA9555 初始化成功 (地址: 0x%02X)\n", PCA9555_I2C_ADDR);

        // 配置所有引脚为输出
        for (int i = 0; i < 16; i++)
        {
            ioExpander.pinMode(i, OUTPUT);
            ioExpander.digitalWrite(i, LOW);
        }
    }
    else
    {
        Serial.println("PCA9555 初始化失败");
    }
}

// ============================================================================
// 示例 5: WS2812B LED 控制
// ============================================================================

#include <FastLED.h>

CRGB leds[NUM_LEDS];

void example_ws2812b()
{
    Serial.println("=== 示例 5: WS2812B LED 控制 ===");

    // 使用定义的引脚和数量初始化
    FastLED.addLeds<WS2812B, WS2812B_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(LED_BRIGHTNESS);

    // 设置颜色
    leds[0] = CRGB::Red;
    FastLED.show();
    delay(500);

    leds[0] = CRGB::Green;
    FastLED.show();
    delay(500);

    leds[0] = CRGB::Blue;
    FastLED.show();
}

// ============================================================================
// 示例 6: 蜂鸣器 PWM 控制
// ============================================================================

void example_buzzer_pwm()
{
    Serial.println("=== 示例 6: 蜂鸣器 PWM 控制 ===");

    // 使用定义的引脚和配置初始化 PWM
    ledcSetup(BUZZER_PWM_CHANNEL, 2000, BUZZER_PWM_RESOLUTION); // 2kHz
    ledcAttachPin(BUZZER_PIN, BUZZER_PWM_CHANNEL);

    // 播放 Do Re Mi
    int frequencies[] = {262, 294, 330}; // C4, D4, E4
    for (int freq : frequencies)
    {
        ledcWriteTone(BUZZER_PWM_CHANNEL, freq);
        delay(300);
    }
    ledcWriteTone(BUZZER_PWM_CHANNEL, 0); // 停止
}

// ============================================================================
// 示例 7: 电池电压监测
// ============================================================================

void example_battery_monitor()
{
    Serial.println("=== 示例 7: 电池电压监测 ===");

    // 配置 ADC
    analogReadResolution(ADC_VBAT_RESOLUTION);
    analogSetAttenuation(ADC_11db); // 0-3.3V 范围

    // 读取电池电压（使用辅助函数）
    float voltage = read_battery_voltage();
    Serial.printf("电池电压: %.2f V\n", voltage);

    // 电压警告
    if (voltage < 3.0)
    {
        Serial.println("⚠️ 电池电压过低！");
        // 闪烁 LED 警告
        for (int i = 0; i < 5; i++)
        {
            digitalWrite(LED_BUILTIN_PIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN_PIN, LOW);
            delay(100);
        }
    }
}

// ============================================================================
// 示例 8: 完整初始化流程
// ============================================================================

void example_complete_init()
{
    Serial.println("=== 示例 8: 完整初始化流程 ===");

    // 方法 1: 使用便捷函数一次性初始化所有引脚
    pin_init_all();

    // 方法 2: 根据需要选择性初始化
    // pin_init_rs485(1);
    // pin_init_spi();
    // pin_init_i2c();
    // pin_init_lcd();
    // pin_init_sd();
    // pin_init_buttons();
    // pin_init_peripherals();

    Serial.println("所有引脚初始化完成");
}

// ============================================================================
// 示例 9: 调试模式打印引脚配置
// ============================================================================

void example_debug_print()
{
    Serial.println("=== 示例 9: 调试模式打印 ===");

    // 手动打印引脚配置
    Serial.println("\n========== 引脚配置信息 ==========");
    Serial.printf("RS485-1: RX=%d, TX=%d, DE=%d\n",
                  RS485_1_RX_PIN, RS485_1_TX_PIN, RS485_1_DE_PIN);
    Serial.printf("RS485-2: RX=%d, TX=%d, DE=%d\n",
                  RS485_2_RX_PIN, RS485_2_TX_PIN, RS485_2_DE_PIN);
    Serial.printf("SPI: MISO=%d, MOSI=%d, SCLK=%d, CS1=%d, CS2=%d\n",
                  SPI_MISO_PIN, SPI_MOSI_PIN, SPI_SCLK_PIN, SPI_CS1_PIN, SPI_CS2_PIN);
    Serial.printf("I2C: SDA=%d, SCL=%d\n", I2C_SDA_PIN, I2C_SCL_PIN);
    Serial.printf("LCD: CS=%d, DC=%d, BLK=%d\n", LCD_CS_PIN, LCD_DC_PIN, LCD_BLK_PIN);
    Serial.printf("SD: CS=%d, DET=%d\n", SD_CS_PIN, SD_DETECT_PIN);
    Serial.printf("按键: 波轮1=%d, 波轮2=%d, 开关=%d, 外部1=%d, 外部2=%d\n",
                  ROTARY_BTN_1_PIN, ROTARY_BTN_2_PIN, ROTARY_SWITCH_PIN,
                  EXT_BTN_1_PIN, EXT_BTN_2_PIN);
    Serial.printf("外设: LED=%d, WS2812B=%d, 蜂鸣器=%d\n",
                  LED_BUILTIN_PIN, WS2812B_PIN, BUZZER_PIN);
    Serial.printf("ADC: 电池电压=%d\n", ADC_VBAT_PIN);
    Serial.println("===================================\n");

    // 或使用宏（需要在编译时定义 DEBUG_PIN_CONFIG）
    // PRINT_PIN_CONFIG();
}

// ============================================================================
// 主程序（示例）
// ============================================================================

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n\n========================================");
    Serial.println("  pin_config.h 使用示例程序");
    Serial.println("========================================\n");

    // 取消注释你想测试的示例

    // example_basic_usage();
    // example_rs485_init();
    // example_spi_devices();
    // example_i2c_devices();
    // example_ws2812b();
    // example_buzzer_pwm();
    // example_battery_monitor();
    // example_complete_init();
    example_debug_print();

    Serial.println("\n示例程序执行完毕");
}

void loop()
{
    // 空循环
    delay(1000);
}
