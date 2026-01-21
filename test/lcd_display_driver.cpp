/**
 * IPS LCD 显示驱动程序
 * ESP32-PICO-D4 + ST7789 135x240 IPS LCD
 *
 * 硬件连接:
 * - LCD_BLK   (IO21) -> 背光控制
 * - LCD_RESET (IO22) -> 复位
 * - LCD_RS    (IO19) -> 数据/命令选择
 * - LCD_SDA   (IO23) -> SPI MOSI 数据
 * - LCD_SCL   (IO18) -> SPI 时钟
 * - LCD_CS    (IO5)  -> 片选
 *
 * 参考自: TactileGlove 项目的 TFT_DISP_FUNC.cpp
 */

#include <Arduino.h>
#include <TFT_eSPI.h>

// 创建TFT对象 (配置在 lib/TFT_eSPI/User_Setup.h)
TFT_eSPI tft = TFT_eSPI();

// =================================================================
// ==================== 辅助函数 ===================================
// =================================================================

/**
 * 绘制WiFi图标
 * @param x X坐标
 * @param y Y坐标
 * @param connected 是否已连接
 */
void drawWifiIcon(int x, int y, bool connected)
{
    uint16_t color = connected ? TFT_GREEN : TFT_DARKGREY;

    // 绘制WiFi信号波纹
    tft.fillCircle(x + 8, y + 10, 2, color);
    tft.drawArc(x + 8, y + 10, 5, 4, 225, 315, color, TFT_BLACK);
    tft.drawArc(x + 8, y + 10, 6, 5, 225, 315, color, TFT_BLACK);
    tft.drawArc(x + 8, y + 10, 9, 8, 225, 315, color, TFT_BLACK);
    tft.drawArc(x + 8, y + 10, 10, 9, 225, 315, color, TFT_BLACK);
    tft.drawArc(x + 8, y + 10, 13, 12, 225, 315, color, TFT_BLACK);
    tft.drawArc(x + 8, y + 10, 14, 13, 225, 315, color, TFT_BLACK);

    // 如果未连接,画红叉
    if (!connected)
    {
        tft.drawLine(x, y, x + 16, y + 12, TFT_RED);
        tft.drawLine(x, y + 1, x + 17, y + 13, TFT_RED);
    }
}

/**
 * 绘制电池图标
 * @param x X坐标
 * @param y Y坐标
 * @param percentage 电量百分比 (0-100)
 */
void drawBatteryIcon(int x, int y, int percentage)
{
    // 电池外框
    tft.drawRect(x, y, 22, 12, TFT_WHITE);
    tft.fillRect(x + 22, y + 3, 2, 6, TFT_WHITE);

    // 清空内部
    tft.fillRect(x + 2, y + 2, 18, 8, TFT_BLACK);

    // 根据电量绘制电池条
    int bars = map(percentage, 0, 100, 0, 4);
    bars = constrain(bars, 0, 4);

    if (bars > 0)
    {
        uint16_t color = TFT_RED;
        if (bars > 3)
            color = TFT_GREEN;
        else if (bars > 1)
            color = TFT_YELLOW;

        for (int i = 0; i < bars; i++)
        {
            tft.fillRect(x + 3 + i * 5, y + 3, 3, 6, color);
        }
    }
}

/**
 * 绘制进度条
 * @param x X坐标
 * @param y Y坐标
 * @param w 宽度
 * @param h 高度
 * @param percentage 百分比 (0-100)
 * @param color 颜色
 */
void drawProgressBar(int x, int y, int w, int h, int percentage, uint16_t color)
{
    percentage = constrain(percentage, 0, 100);

    // 绘制外框
    tft.drawRect(x, y, w, h, TFT_WHITE);

    // 填充进度
    int fillWidth = map(percentage, 0, 100, 0, w - 4);
    if (fillWidth > 0)
    {
        tft.fillRect(x + 2, y + 2, fillWidth, h - 4, color);
    }

    // 清空剩余部分
    if (fillWidth < w - 4)
    {
        tft.fillRect(x + 2 + fillWidth, y + 2, w - 4 - fillWidth, h - 4, TFT_BLACK);
    }
}

// =================================================================
// ==================== 主程序 =====================================
// =================================================================

void setup()
{
    Serial.begin(115200);
    delay(100);
    Serial.println("\n=== IPS LCD Display Driver Test ===");

    // 初始化TFT屏幕
    Serial.println("初始化LCD...");
    tft.init();
    tft.setRotation(1); // 横屏模式 (0-3可选)
    tft.fillScreen(TFT_BLACK);

    // 设置文本属性
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);

    // 显示欢迎信息
    tft.setCursor(10, 10);
    tft.println("ESP32 PICO D4");
    tft.setCursor(10, 30);
    tft.println("LCD Test");

    Serial.println("LCD 初始化完成!");
    delay(2000);
}

void loop()
{
    static unsigned long lastUpdate = 0;
    static int counter = 0;

    // 每500ms更新一次显示
    if (millis() - lastUpdate >= 500)
    {
        lastUpdate = millis();
        counter++;

        // 清屏
        tft.fillScreen(TFT_BLACK);

        // ===== 顶部状态栏 =====
        tft.fillRect(0, 0, 240, 25, TFT_NAVY);
        tft.setTextSize(2);
        tft.setTextColor(TFT_WHITE, TFT_NAVY);
        tft.setCursor(5, 5);
        tft.print("ESP32 PICO");

        // 运行时间
        unsigned long runtime = millis() / 1000;
        tft.setCursor(150, 5);
        tft.printf("%02lu:%02lu", runtime / 60, runtime % 60);

        // ===== 数据显示区域 =====
        int y_pos = 35;

        // 计数器
        tft.setTextSize(3);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.setCursor(10, y_pos);
        tft.printf("Count: %d", counter);
        y_pos += 30;

        // 模拟传感器数据
        tft.setTextSize(2);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.setCursor(10, y_pos);
        float temp = 25.5 + (counter % 10) * 0.1;
        tft.printf("Temp: %.1f C", temp);
        y_pos += 25;

        // 模拟湿度数据
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.setCursor(10, y_pos);
        int humidity = 50 + (counter % 20);
        tft.printf("Humi: %d%%", humidity);
        y_pos += 30;

        // 进度条演示
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(10, y_pos);
        tft.print("Progress:");
        y_pos += 12;

        int progress = (counter * 5) % 101;
        drawProgressBar(10, y_pos, 220, 15, progress, TFT_GREEN);
        y_pos += 20;

        // ===== 底部图标栏 =====
        tft.fillRect(0, 215, 240, 25, TFT_NAVY);

        // WiFi图标 (模拟连接状态)
        bool wifiConnected = (counter % 4) < 3;
        drawWifiIcon(10, 218, wifiConnected);

        // 电池图标
        int batteryLevel = 100 - (counter % 101);
        drawBatteryIcon(50, 218, batteryLevel);

        // 电池百分比
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE, TFT_NAVY);
        tft.setCursor(80, 220);
        tft.printf("%d%%", batteryLevel);

        // FPS显示
        tft.setCursor(180, 220);
        tft.print("2 FPS");

        Serial.printf("更新显示 - 计数器: %d\n", counter);
    }
}
