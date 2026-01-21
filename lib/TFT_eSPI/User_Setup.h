//                            USER DEFINED SETTINGS
// TFT_eSPI 配置文件 - ESP32 PICO D4 with ST7789 IPS LCD
//
// 参考引脚配置: include/lcd_config.h
// LCD_BLK   = 21  背光控制
// LCD_RESET = 22  复位
// LCD_RS    = 19  数据/命令选择
// LCD_SDA   = 23  SPI MOSI
// LCD_SCL   = 18  SPI SCLK
// LCD_CS    = 5   片选

#ifndef USER_SETUP_H
#define USER_SETUP_H

#include "lcd_config.h" // 引入LCD引脚配置

#define USER_SETUP_INFO "ESP32_PICO_ST7789_135x240"

// ##################################################################################
// Section 1. 驱动选择
// ##################################################################################

#define ST7789_2_DRIVER // ST7789 驱动 (轻量级配置)

// ##################################################################################
// Section 2. 显示屏参数
// ##################################################################################

#define TFT_WIDTH 135 // ST7789 135 x 240
#define TFT_HEIGHT 240

#define CGRAM_OFFSET // 使用库自动偏移

// ##################################################################################
// Section 3. 引脚定义 (使用 lcd_config.h 中的定义)
// ##################################################################################

#define TFT_MOSI LCD_SDA  // GPIO23 - SPI MOSI
#define TFT_SCLK LCD_SCL  // GPIO18 - SPI SCLK
#define TFT_CS LCD_CS     // GPIO5  - 片选
#define TFT_DC LCD_RS     // GPIO19 - 数据/命令选择
#define TFT_RST LCD_RESET // GPIO22 - 复位
#define TFT_BL LCD_BLK    // GPIO21 - 背光控制

#define TFT_BACKLIGHT_ON HIGH // 背光高电平开启

// ##################################################################################
// Section 4. 字体加载
// ##################################################################################

#define LOAD_GLCD  // 原始 Adafruit 8 像素字体
#define LOAD_FONT2 // 16 像素小字体
#define LOAD_FONT4 // 26 像素中等字体
#define LOAD_FONT6 // 48 像素大字体
#define LOAD_FONT7 // 7 段数字字体
#define LOAD_FONT8 // 75 像素超大字体
#define LOAD_GFXFF // Adafruit GFX 字体支持

#define SMOOTH_FONT // 平滑字体支持

// ##################################################################################
// Section 5. SPI 速度配置
// ##################################################################################

#define SPI_FREQUENCY 40000000 // 40MHz SPI 速度 (稳定)
// #define SPI_FREQUENCY  80000000  // 80MHz 可尝试 (部分屏幕支持)

#define SPI_READ_FREQUENCY 20000000 // 读取速度
#define SPI_TOUCH_FREQUENCY 2500000 // 触摸控制器速度

// ##################################################################################
// Section 6. 其他配置
// ##################################################################################

// #define SUPPORT_TRANSACTIONS  // 如果有其他SPI设备共享总线,取消注释

#endif // USER_SETUP_H
