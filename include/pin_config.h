/**
 * @file pin_config.h
 * @brief ESP32-PICO-D4 机器人开发平台 - 引脚配置定义
 *
 * @details 集中管理所有硬件引脚定义，便于代码复用和维护
 * @note 使用前请确保引脚配置与实际硬件连接一致
 * @version 1.0
 * @date 2026-02-03
 */

#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>

/* ====================================================================================
 *  RS485 通信引脚定义
 * ==================================================================================== */

// 第一路 RS485 通信
#define RS485_1_RX_PIN 32                    // 第一路 Modbus 数据接收
#define RS485_1_TX_PIN 33                    // 第一路 Modbus 数据发送
#define RS485_1_DE_PIN 25                    // 第一路收发控制 (DE/RE)
#define RS485_1_DIRECTION_PIN RS485_1_DE_PIN // 别名

// 第二路 RS485 通信
#define RS485_2_RX_PIN 26                    // 第二路 Modbus 数据接收
#define RS485_2_TX_PIN 27                    // 第二路 Modbus 数据发送
#define RS485_2_DE_PIN 14                    // 第二路收发控制 (DE/RE)
#define RS485_2_DIRECTION_PIN RS485_2_DE_PIN // 别名

/* ====================================================================================
 *  SPI 总线引脚定义（多设备共享）
 * ==================================================================================== */

#define SPI_MISO_PIN 19          // SPI 主机输入从机输出
#define SPI_MOSI_PIN 23          // SPI 主机输出从机输入
#define SPI_SCLK_PIN 18          // SPI 时钟信号
#define SPI_CLK_PIN SPI_SCLK_PIN // 别名

// SPI 片选信号
#define SPI_CS1_PIN 5  // LCD 显示屏片选
#define SPI_CS2_PIN 10 // SD 卡模块片选

/* ====================================================================================
 *  I2C 总线引脚定义
 * ==================================================================================== */

#define I2C_SDA_PIN 22 // I2C 数据线
#define I2C_SCL_PIN 21 // I2C 时钟线

/* ====================================================================================
 *  LCD 显示屏引脚定义
 * ==================================================================================== */

#define LCD_CS_PIN 5                  // LCD 片选（与 SPI_CS1 相同）
#define LCD_DC_PIN 13                 // LCD 数据/命令切换（RS）
#define LCD_RS_PIN LCD_DC_PIN         // 别名
#define LCD_BLK_PIN 4                 // LCD 背光控制
#define LCD_BACKLIGHT_PIN LCD_BLK_PIN // 别名

// LCD 使用共享 SPI 总线
#define LCD_MOSI_PIN SPI_MOSI_PIN
#define LCD_SCLK_PIN SPI_SCLK_PIN
// LCD RESET 连接到 ESP32 EN 引脚，不需要单独定义

/* ====================================================================================
 *  SD 卡模块引脚定义
 * ==================================================================================== */

#define SD_CS_PIN 10    // SD 卡片选（与 SPI_CS2 相同）
#define SD_DETECT_PIN 9 // SD 卡检测引脚（卡插入时为低电平）

// SD 卡使用共享 SPI 总线
#define SD_MISO_PIN SPI_MISO_PIN
#define SD_MOSI_PIN SPI_MOSI_PIN
#define SD_SCLK_PIN SPI_SCLK_PIN

/* ====================================================================================
 *  按键输入引脚定义
 * ==================================================================================== */

// 波轮开关（3 个按键）
#define ROTARY_BTN_1_PIN 38  // 波轮按键 1
#define ROTARY_BTN_2_PIN 36  // 波轮按键 2
#define ROTARY_SWITCH_PIN 37 // 波轮开关按键

// 外部按键（下拉触发）
#define EXT_BTN_1_PIN 35 // 外部按键 1（按下时下拉到地）
#define EXT_BTN_2_PIN 34 // 外部按键 2（按下时下拉到地）

/* ====================================================================================
 *  外设控制引脚定义
 * ==================================================================================== */

#define WS2812B_PIN 15          // WS2812B RGB LED 数据引脚
#define LED_RGB_PIN WS2812B_PIN // 别名

#define BUZZER_PIN 12 // 蜂鸣器 PWM 输出引脚

#define LED_BUILTIN_PIN 2       // 板载 LED 指示灯
#define LED_PIN LED_BUILTIN_PIN // 别名

/* ====================================================================================
 *  ADC 采集引脚定义
 * ==================================================================================== */

#define ADC_VBAT_PIN 39                  // 电池电压检测引脚
#define BATTERY_VOLTAGE_PIN ADC_VBAT_PIN // 别名

/* ====================================================================================
 *  硬件配置宏定义
 * ==================================================================================== */

// RS485 通信配置
#define RS485_BAUD_RATE 115200 // RS485 波特率
#define MODBUS_TIMEOUT_MS 100  // Modbus 超时时间（毫秒）

// I2C 设备地址
#define PCA9555_I2C_ADDR 0x20 // PCA9555 GPIO 扩展器默认地址
#define DPS310_I2C_ADDR 0x77  // DPS310 气压传感器地址（可能为 0x76）

// SPI 时钟频率
#define SPI_CLOCK_LCD 40000000 // LCD SPI 时钟 40MHz
#define SPI_CLOCK_SD 25000000  // SD 卡 SPI 时钟 25MHz

// WS2812B LED 配置
#define NUM_LEDS 1        // WS2812B LED 数量
#define LED_BRIGHTNESS 50 // LED 亮度（0-255）

// 蜂鸣器 PWM 配置
#define BUZZER_PWM_CHANNEL 0    // PWM 通道
#define BUZZER_PWM_RESOLUTION 8 // PWM 分辨率（位）
#define BUZZER_DEFAULT_DUTY 128 // 默认占空比（0-255）

// ADC 采集配置
#define ADC_VBAT_RESOLUTION 12 // ADC 分辨率（12位）
#define VBAT_DIVIDER_RATIO 2.0 // 电池电压分压比（根据实际电路调整）

/* ====================================================================================
 *  引脚初始化函数声明（可选）
 * ==================================================================================== */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief 初始化所有 GPIO 引脚为默认状态
     * @note 调用此函数会将所有输出引脚设置为输出模式，输入引脚设置为输入模式
     */
    void pin_init_all(void);

    /**
     * @brief 初始化 RS485 通信引脚
     * @param port RS485 端口号（1 或 2）
     */
    void pin_init_rs485(uint8_t port);

    /**
     * @brief 初始化 SPI 总线引脚
     */
    void pin_init_spi(void);

    /**
     * @brief 初始化 I2C 总线引脚
     */
    void pin_init_i2c(void);

    /**
     * @brief 初始化 LCD 显示屏引脚
     */
    void pin_init_lcd(void);

    /**
     * @brief 初始化 SD 卡引脚
     */
    void pin_init_sd(void);

    /**
     * @brief 初始化所有按键引脚
     */
    void pin_init_buttons(void);

    /**
     * @brief 初始化外设控制引脚（LED、蜂鸣器）
     */
    void pin_init_peripherals(void);

#ifdef __cplusplus
}
#endif

/* ====================================================================================
 *  引脚功能检查宏（调试用）
 * ==================================================================================== */

#ifdef DEBUG_PIN_CONFIG
#define PRINT_PIN_CONFIG()                                                                               \
    do                                                                                                   \
    {                                                                                                    \
        Serial.println("=== ESP32-PICO-D4 引脚配置 ===");                                                \
        Serial.printf("RS485_1: RX=%d, TX=%d, DE=%d\n", RS485_1_RX_PIN, RS485_1_TX_PIN, RS485_1_DE_PIN); \
        Serial.printf("RS485_2: RX=%d, TX=%d, DE=%d\n", RS485_2_RX_PIN, RS485_2_TX_PIN, RS485_2_DE_PIN); \
        Serial.printf("SPI: MISO=%d, MOSI=%d, SCLK=%d\n", SPI_MISO_PIN, SPI_MOSI_PIN, SPI_SCLK_PIN);     \
        Serial.printf("I2C: SDA=%d, SCL=%d\n", I2C_SDA_PIN, I2C_SCL_PIN);                                \
        Serial.printf("LCD: CS=%d, DC=%d, BLK=%d\n", LCD_CS_PIN, LCD_DC_PIN, LCD_BLK_PIN);               \
        Serial.printf("SD: CS=%d, DET=%d\n", SD_CS_PIN, SD_DETECT_PIN);                                  \
        Serial.printf("LED: WS2812B=%d, BUILTIN=%d\n", WS2812B_PIN, LED_BUILTIN_PIN);                    \
        Serial.printf("BUZZER: %d\n", BUZZER_PIN);                                                       \
        Serial.printf("ADC: VBAT=%d\n", ADC_VBAT_PIN);                                                   \
        Serial.println("==============================");                                                \
    } while (0)
#else
#define PRINT_PIN_CONFIG()
#endif

#endif // PIN_CONFIG_H
