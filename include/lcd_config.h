//
// LCD Pin Configuration
// IPS LCD Display Pin Mapping for ESP32-PICO-D4
//

#ifndef LCD_CONFIG_H
#define LCD_CONFIG_H

// LCD 引脚定义
#define LCD_BLK 21   // 背光控制 (Backlight)
#define LCD_RESET 22 // 复位引脚 (Reset)
#define LCD_RS 19    // 数据/命令选择 (Register Select/Data Command)
#define LCD_SDA 23   // 数据线 (SPI MOSI)
#define LCD_SCL 18   // 时钟线 (SPI SCLK)
#define LCD_CS 5     // 片选 (Chip Select)

#endif // LCD_CONFIG_H
