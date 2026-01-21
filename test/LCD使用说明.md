# IPS LCD 显示屏使用说明

## 📋 目录
- [硬件信息](#硬件信息)
- [引脚连接](#引脚连接)
- [快速开始](#快速开始)
- [代码示例](#代码示例)
- [API参考](#api参考)
- [故障排查](#故障排查)

---

## 🔌 硬件信息

### 屏幕规格
- **驱动芯片**: ST7789
- **分辨率**: 135 x 240 像素
- **接口**: SPI
- **显示类型**: IPS LCD (全视角)
- **工作电压**: 3.3V

### 引脚连接

| LCD引脚 | ESP32引脚 | 功能说明 |
|---------|----------|---------|
| BLK     | IO21     | 背光控制 (Backlight) |
| RESET   | IO22     | 硬件复位 |
| RS      | IO19     | 数据/命令选择 (DC) |
| SDA     | IO23     | SPI 数据线 (MOSI) |
| SCL     | IO18     | SPI 时钟线 (SCLK) |
| CS      | IO5      | 片选 (Chip Select) |
| VCC     | 3.3V     | 电源正极 |
| GND     | GND      | 电源地 |

### 连接示意图
```
ESP32-PICO-D4          ST7789 LCD
    IO21  -----------> BLK
    IO22  -----------> RESET
    IO19  -----------> RS (DC)
    IO23  -----------> SDA (MOSI)
    IO18  -----------> SCL (SCLK)
    IO5   -----------> CS
    3.3V  -----------> VCC
    GND   -----------> GND
```

---

## 🚀 快速开始

### 1. 库依赖
在 `platformio.ini` 中已包含:
```ini
lib_deps = 
    bodmer/TFT_eSPI @ ^2.5.43
```

### 2. 配置文件
- **引脚配置**: `include/lcd_config.h`
- **TFT_eSPI配置**: `lib/TFT_eSPI/User_Setup.h`

### 3. 编译测试代码
```bash
# 复制测试代码到src目录
cp test/lcd_display_driver.cpp src/main.cpp

# 编译
pio run

# 上传
pio run --target upload

# 查看串口输出
pio device monitor
```

---

## 📝 代码示例

### 基础初始化
```cpp
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
    // 初始化屏幕
    tft.init();
    tft.setRotation(1);  // 横屏 (0-3可选)
    tft.fillScreen(TFT_BLACK);
    
    // 显示文本
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 10);
    tft.println("Hello ESP32!");
}
```

### 显示IMU数据
```cpp
void displayIMUData(float ax, float ay, float az) {
    tft.fillScreen(TFT_BLACK);
    
    // 标题
    tft.setTextSize(2);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.println("Accelerometer");
    
    // 数据
    int y = 40;
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setCursor(10, y); tft.printf("X: %+6.2f", ax); y += 25;
    tft.setCursor(10, y); tft.printf("Y: %+6.2f", ay); y += 25;
    tft.setCursor(10, y); tft.printf("Z: %+6.2f", az);
}
```

### 显示编码器角度
```cpp
void displayEncoderAngles(float angle1, float angle2, float angle3, float angle4) {
    tft.fillScreen(TFT_BLACK);
    
    // 状态栏
    tft.fillRect(0, 0, 240, 25, TFT_NAVY);
    tft.setTextSize(2);
    tft.setTextColor(TFT_WHITE, TFT_NAVY);
    tft.setCursor(5, 5);
    tft.print("Encoder Angles");
    
    // 显示4个编码器数据
    int y = 35;
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, y); tft.printf("E1: %6.1f deg", angle1); y += 30;
    tft.setCursor(10, y); tft.printf("E2: %6.1f deg", angle2); y += 30;
    tft.setCursor(10, y); tft.printf("E3: %6.1f deg", angle3); y += 30;
    tft.setCursor(10, y); tft.printf("E4: %6.1f deg", angle4);
}
```

### 绘制图形
```cpp
void drawShapes() {
    tft.fillScreen(TFT_BLACK);
    
    // 绘制矩形
    tft.drawRect(10, 10, 100, 50, TFT_GREEN);
    tft.fillRect(120, 10, 100, 50, TFT_RED);
    
    // 绘制圆形
    tft.drawCircle(60, 100, 30, TFT_BLUE);
    tft.fillCircle(170, 100, 30, TFT_YELLOW);
    
    // 绘制线条
    tft.drawLine(10, 150, 230, 150, TFT_WHITE);
    tft.drawLine(10, 180, 230, 180, TFT_CYAN);
}
```

---

## 🛠 API参考

### 屏幕控制
```cpp
tft.init()                    // 初始化屏幕
tft.setRotation(r)            // 设置旋转 (0-3)
tft.fillScreen(color)         // 清屏
tft.invertDisplay(bool)       // 反色显示
```

### 文本显示
```cpp
tft.setTextColor(fg, bg)      // 设置文本颜色
tft.setTextSize(size)         // 设置文本大小 (1-8)
tft.setCursor(x, y)           // 设置光标位置
tft.print(text)               // 打印文本
tft.println(text)             // 打印并换行
tft.printf(format, ...)       // 格式化打印
```

### 图形绘制
```cpp
tft.drawPixel(x, y, color)              // 画点
tft.drawLine(x0, y0, x1, y1, color)     // 画线
tft.drawRect(x, y, w, h, color)         // 画矩形
tft.fillRect(x, y, w, h, color)         // 填充矩形
tft.drawCircle(x, y, r, color)          // 画圆
tft.fillCircle(x, y, r, color)          // 填充圆
tft.drawTriangle(x0,y0,x1,y1,x2,y2,c)   // 画三角形
```

### 颜色定义
```cpp
TFT_BLACK       0x0000   // 黑色
TFT_NAVY        0x000F   // 深蓝
TFT_DARKGREEN   0x03E0   // 深绿
TFT_DARKCYAN    0x03EF   // 深青
TFT_MAROON      0x7800   // 栗色
TFT_PURPLE      0x780F   // 紫色
TFT_OLIVE       0x7BE0   // 橄榄绿
TFT_LIGHTGREY   0xC618   // 浅灰
TFT_DARKGREY    0x7BEF   // 深灰
TFT_BLUE        0x001F   // 蓝色
TFT_GREEN       0x07E0   // 绿色
TFT_CYAN        0x07FF   // 青色
TFT_RED         0xF800   // 红色
TFT_MAGENTA     0xF81F   // 品红
TFT_YELLOW      0xFFE0   // 黄色
TFT_WHITE       0xFFFF   // 白色
TFT_ORANGE      0xFD20   // 橙色
TFT_GREENYELLOW 0xAFE5   // 黄绿
TFT_PINK        0xF81F   // 粉色
```

---

## 🔧 故障排查

### 问题1: 屏幕无显示
**可能原因:**
- 电源未连接 (VCC/GND)
- 背光未开启 (IO21)
- 引脚连接错误

**解决方法:**
1. 检查电源连接
2. 确认 IO21 (BLK) 输出高电平
3. 验证引脚定义与实际接线一致

### 问题2: 显示颜色异常
**可能原因:**
- RGB顺序配置错误
- 屏幕型号不匹配

**解决方法:**
在 `User_Setup.h` 中尝试调整:
```cpp
#define TFT_RGB_ORDER TFT_BGR  // 或 TFT_RGB
```

### 问题3: 显示偏移或花屏
**可能原因:**
- 屏幕初始化参数不匹配
- SPI速度过高

**解决方法:**
1. 降低SPI频率:
```cpp
#define SPI_FREQUENCY  27000000  // 改为27MHz
```
2. 检查 `TFT_WIDTH` 和 `TFT_HEIGHT` 配置

### 问题4: 编译错误
**错误信息**: `User_Setup.h: No such file or directory`

**解决方法:**
确认文件路径正确:
```
lib/TFT_eSPI/User_Setup.h
include/lcd_config.h
```

### 问题5: 屏幕刷新慢
**优化建议:**
1. 使用 `tft.startWrite()` 和 `tft.endWrite()` 包裹批量操作
2. 只更新变化的区域,避免全屏刷新
3. 提高SPI频率 (最高80MHz)

---

## 📚 参考资料

### TFT_eSPI 库文档
- GitHub: https://github.com/Bodmer/TFT_eSPI
- 示例代码: `lib/TFT_eSPI/examples/`

### 本项目参考
- TactileGlove项目: `TFT_DISP_FUNC.cpp`
- 编码器驱动: `test/编码器数据批量快速读取备份`
- IMU驱动: `test/hipnuc_imu_reader.cpp`

---

## 💡 使用技巧

### 提高刷新率
```cpp
void fastUpdate() {
    tft.startWrite();  // 开始事务
    
    // 批量绘制操作
    tft.fillRect(0, 0, 100, 50, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print("Fast!");
    
    tft.endWrite();    // 结束事务
}
```

### 局部更新
```cpp
void updateTemperature(float temp) {
    // 只更新温度显示区域
    tft.fillRect(100, 50, 120, 25, TFT_BLACK);
    tft.setCursor(100, 50);
    tft.printf("%.1f C", temp);
}
```

### 双缓冲 (精灵图)
```cpp
TFT_eSprite spr = TFT_eSprite(&tft);

void setup() {
    spr.createSprite(240, 135);  // 创建精灵图
}

void loop() {
    spr.fillSprite(TFT_BLACK);   // 在内存中绘制
    spr.drawString("Hello", 10, 10);
    spr.pushSprite(0, 0);        // 一次性推送到屏幕
}
```

---

**作者**: ESP32PicoCurrentRobotics Project  
**最后更新**: 2025年1月  
**许可证**: MIT
