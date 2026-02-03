/**
 * @file pin_config.cpp
 * @brief ESP32-PICO-D4 机器人开发平台 - 引脚初始化实现
 *
 * @details 提供引脚初始化的便捷函数实现
 * @version 1.0
 * @date 2026-02-03
 */

#include "pin_config.h"

/**
 * @brief 初始化所有 GPIO 引脚为默认状态
 */
void pin_init_all(void)
{
    // 初始化各个子系统
    pin_init_rs485(1);
    pin_init_rs485(2);
    pin_init_spi();
    pin_init_i2c();
    pin_init_lcd();
    pin_init_sd();
    pin_init_buttons();
    pin_init_peripherals();

#ifdef DEBUG_PIN_CONFIG
    PRINT_PIN_CONFIG();
#endif
}

/**
 * @brief 初始化 RS485 通信引脚
 * @param port RS485 端口号（1 或 2）
 */
void pin_init_rs485(uint8_t port)
{
    if (port == 1)
    {
        pinMode(RS485_1_RX_PIN, INPUT);
        pinMode(RS485_1_TX_PIN, OUTPUT);
        pinMode(RS485_1_DE_PIN, OUTPUT);
        digitalWrite(RS485_1_DE_PIN, LOW); // 默认接收模式
    }
    else if (port == 2)
    {
        pinMode(RS485_2_RX_PIN, INPUT);
        pinMode(RS485_2_TX_PIN, OUTPUT);
        pinMode(RS485_2_DE_PIN, OUTPUT);
        digitalWrite(RS485_2_DE_PIN, LOW); // 默认接收模式
    }
}

/**
 * @brief 初始化 SPI 总线引脚
 */
void pin_init_spi(void)
{
    pinMode(SPI_MISO_PIN, INPUT);
    pinMode(SPI_MOSI_PIN, OUTPUT);
    pinMode(SPI_SCLK_PIN, OUTPUT);
    pinMode(SPI_CS1_PIN, OUTPUT);
    pinMode(SPI_CS2_PIN, OUTPUT);

    // 默认取消所有片选
    digitalWrite(SPI_CS1_PIN, HIGH);
    digitalWrite(SPI_CS2_PIN, HIGH);
}

/**
 * @brief 初始化 I2C 总线引脚
 */
void pin_init_i2c(void)
{
    // I2C 引脚通常由 Wire 库自动配置
    // 这里仅作为占位符，实际使用时调用 Wire.begin(SDA, SCL)
    pinMode(I2C_SDA_PIN, INPUT_PULLUP);
    pinMode(I2C_SCL_PIN, INPUT_PULLUP);
}

/**
 * @brief 初始化 LCD 显示屏引脚
 */
void pin_init_lcd(void)
{
    pinMode(LCD_CS_PIN, OUTPUT);
    pinMode(LCD_DC_PIN, OUTPUT);
    pinMode(LCD_BLK_PIN, OUTPUT);

    // 默认关闭背光，取消片选
    digitalWrite(LCD_CS_PIN, HIGH);
    digitalWrite(LCD_BLK_PIN, LOW);
}

/**
 * @brief 初始化 SD 卡引脚
 */
void pin_init_sd(void)
{
    pinMode(SD_CS_PIN, OUTPUT);
    pinMode(SD_DETECT_PIN, INPUT_PULLUP);

    // 默认取消片选
    digitalWrite(SD_CS_PIN, HIGH);
}

/**
 * @brief 初始化所有按键引脚
 */
void pin_init_buttons(void)
{
    // 波轮开关（内部上拉）
    pinMode(ROTARY_BTN_1_PIN, INPUT_PULLUP);
    pinMode(ROTARY_BTN_2_PIN, INPUT_PULLUP);
    pinMode(ROTARY_SWITCH_PIN, INPUT_PULLUP);

    // 外部按键（外部已有上拉电阻，按下时下拉到地）
    pinMode(EXT_BTN_1_PIN, INPUT);
    pinMode(EXT_BTN_2_PIN, INPUT);
}

/**
 * @brief 初始化外设控制引脚（LED、蜂鸣器）
 */
void pin_init_peripherals(void)
{
    // WS2812B LED（由 FastLED 库管理，这里仅设置为输出）
    pinMode(WS2812B_PIN, OUTPUT);
    digitalWrite(WS2812B_PIN, LOW);

    // 蜂鸣器（通过 PWM 控制）
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    // 板载 LED
    pinMode(LED_BUILTIN_PIN, OUTPUT);
    digitalWrite(LED_BUILTIN_PIN, LOW);

    // ADC 引脚无需初始化
}

/* ====================================================================================
 *  便捷辅助函数
 * ==================================================================================== */

/**
 * @brief 设置 RS485 收发模式
 * @param port RS485 端口号（1 或 2）
 * @param transmit true=发送模式, false=接收模式
 */
void rs485_set_mode(uint8_t port, bool transmit)
{
    if (port == 1)
    {
        digitalWrite(RS485_1_DE_PIN, transmit ? HIGH : LOW);
    }
    else if (port == 2)
    {
        digitalWrite(RS485_2_DE_PIN, transmit ? HIGH : LOW);
    }
}

/**
 * @brief 控制 LCD 背光
 * @param on true=开启, false=关闭
 */
void lcd_backlight(bool on)
{
    digitalWrite(LCD_BLK_PIN, on ? HIGH : LOW);
}

/**
 * @brief 检测 SD 卡是否插入
 * @return true=已插入, false=未插入
 */
bool sd_card_detected(void)
{
    return digitalRead(SD_DETECT_PIN) == LOW; // 低电平表示卡已插入
}

/**
 * @brief 读取电池电压（单位：伏特）
 * @return 电池电压值
 */
float read_battery_voltage(void)
{
    uint16_t adc_value = analogRead(ADC_VBAT_PIN);
    // 12位 ADC: 0-4095 对应 0-3.3V
    float voltage = (adc_value / 4095.0) * 3.3 * VBAT_DIVIDER_RATIO;
    return voltage;
}
