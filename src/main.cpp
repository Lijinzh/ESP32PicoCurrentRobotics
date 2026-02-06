/**
 * @file main.cpp
 * @brief ESP32-PICO-D4 多功能机器人控制系统 - 多传感器数据采集 + LCD屏幕显示
 * @note 集成DPS310气压传感器、TFT屏幕显示、RGB LED和蜂鸣器
 *
 * 硬件连接：
 * - DPS310（I2C） 链接到 I2C_SDA(GPIO22)、I2C_SCL(GPIO21)
 * - IMU(HiPNUC) 链接到 RS485_2（GPIO26 RX、GPIO27 TX、GPIO14 DE）
 * - LCD屏幕使用SPI和对应的控制引脚（GPIO18/23/5/13/4）
 * - RGB LED -> GPIO 15 (WS2812B)
 * - 蜂鸣器 -> GPIO 12
 */

#include <Arduino.h>
#include <FastLED.h>
#include <TFT_eSPI.h>
#include <Adafruit_DPS310.h>
#include "hipnuc_dec.h"
#include "pin_config.h"

// ==================== 配置常量 ====================
#define NUM_LEDS 1             // WS2812B LED数量
#define DISPLAY_INTERVAL 10    // 10Hz显示频率
#define LCD_UPDATE_INTERVAL 50 // LCD 20Hz刷新率

// ==================== 全局变量 ====================
TFT_eSPI tft = TFT_eSPI(); // TFT屏幕实例
Adafruit_DPS310 dps;       // DPS310传感器实例
CRGB leds[NUM_LEDS];
hipnuc_raw_t hipnuc_raw;

// DPS310数据
float dps_temp = 0.0;     // 温度(°C)
float dps_pressure = 0.0; // 气压(Pa)
float dps_altitude = 0.0; // 高度(m)

// 数据统计
unsigned long lastSecond = 0;
unsigned long frameCount = 0;
float currentFPS = 0;

// 显示控制
unsigned long lastDisplay = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lastDPSRead = 0;
const int DPS_READ_INTERVAL = 100; // DPS310采样100ms间隔

// 数据缓冲区（用于格式化输出）
char displayBuffer[512];

// ==================== LED状态指示 ====================
void setLEDStatus(uint8_t status)
{
    switch (status)
    {
    case 0:
        leds[0] = CRGB::Orange;
        break; // 初始化
    case 1:
        leds[0] = CRGB::Blue;
        break; // 等待数据
    case 2:
        leds[0] = CRGB::Green;
        break; // 接收IMU数据
    case 3:
        leds[0] = CRGB::Cyan;
        break; // 接收INS数据
    case 4:
        leds[0] = CRGB::Red;
        break; // 错误/无数据
    default:
        leds[0] = CRGB::Purple;
        break;
    }
    FastLED.show();
}

// ==================== 蜂鸣器功能 ====================
void playStartupSound()
{
    int melody[] = {1000, 1200, 1500};
    for (int i = 0; i < 3; i++)
    {
        tone(BUZZER_PIN, melody[i]);
        delay(80);
        noTone(BUZZER_PIN);
        delay(40);
    }
}

void playDataReceivedBeep()
{
    tone(BUZZER_PIN, 2000, 20); // 短促的嘀声
}

// ==================== LCD屏幕显示功能 ====================
void initLCD()
{
    // 初始化LCD屏幕
    tft.init();
    tft.setRotation(1); // 横屏显示
    tft.fillScreen(TFT_BLACK);

    // 显示初始化信息
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("System Init", 120, 60);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("Initializing DPS310...", 10, 100);
    tft.setTextDatum(TL_DATUM);

    delay(500);
}

void updateLCDDisplay()
{
    // 清屏
    tft.fillScreen(TFT_BLACK);

    // 绘制标题栏
    tft.setTextColor(TFT_CYAN, TFT_DARKGREY);
    tft.setTextSize(2);
    tft.fillRect(0, 0, 240, 30, TFT_DARKGREY);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Sensor Monitor", 120, 15);
    tft.setTextDatum(TL_DATUM);

    // 显示帧率和运行时间
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextSize(1);
    tft.drawString("FPS: ", 10, 40);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawFloat(currentFPS, 1, 50, 40, 2);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("Time: ", 150, 40);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawFloat(millis() / 1000.0, 1, 195, 40, 2);
    tft.drawString("s", 220, 40);

    // 显示DPS310数据
    tft.setTextColor(TFT_BLACK, TFT_GREEN);
    tft.setTextSize(2);
    tft.fillRect(0, 60, 240, 25, TFT_GREEN);
    tft.drawString("DPS310 Active", 10, 65);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);

    // 第一行：温度
    tft.drawString("Temperature:", 10, 95);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawFloat(dps_temp, 2, 120, 95, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("C", 200, 95);

    // 第二行：气压
    tft.drawString("Pressure:", 10, 115);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawFloat(dps_pressure / 100.0, 1, 100, 115, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("hPa", 190, 115);

    // 第三行：高度
    tft.drawString("Altitude:", 10, 135);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawFloat(dps_altitude, 1, 100, 135, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("m", 210, 135);

    // 显示DPS310状态
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.drawString("[DPS310 I2C Ready]", 10, 180);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString("Temperature, Pressure, Altitude", 10, 200);
}

// ==================== 系统信息打印 ====================
void printSystemInfo()
{
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║   ESP32-PICO-D4 传感器监控系统        ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println("\n[系统信息]");
    Serial.printf("  芯片型号: ESP32-PICO-D4\n");
    Serial.printf("  CPU频率: %d MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("  空闲堆: %d bytes\n", ESP.getFreeHeap());
    Serial.println("  传感器: DPS310 (I2C)");
    Serial.println("  支持数据包: 0x91(IMU), 0x81(INS), 0x83(Flexible)");
    Serial.println("----------------------------------------");
}

// ==================== 启动倒计时 ====================
void startupCountdown()
{
    Serial.println("\n启动倒计时:");
    for (int i = 3; i > 0; i--)
    {
        Serial.printf("  %d...\n", i);
        setLEDStatus(0);
        tone(BUZZER_PIN, 800 + i * 200);
        delay(300);
        noTone(BUZZER_PIN);
        leds[0] = CRGB::Black;
        FastLED.show();
        delay(700);
    }
    playStartupSound();
    Serial.println("\n✓ 数据采集已启动");
    Serial.println("========================================\n");
}

// ==================== DPS310传感器函数 ====================
void initDPS310()
{
    if (!dps.begin_I2C(DPS310_I2C_ADDR, &Wire))
    {
        Serial.println("DPS310 初始化失败!");
        setLEDStatus(4);
        return;
    }
    Serial.println("DPS310 初始化成功");

    // 配置DPS310
    dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
    dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
    setLEDStatus(2);
}

void readDPS310()
{
    unsigned long now = millis();
    if (now - lastDPSRead < DPS_READ_INTERVAL)
    {
        return;
    }
    lastDPSRead = now;

    sensors_event_t temp_event, pressure_event;

    if (dps.getEvents(&temp_event, &pressure_event))
    {
        dps_temp = temp_event.temperature;
        dps_pressure = pressure_event.pressure;
        // 计算高度（简单公式，基于海平面气压101325 Pa）
        dps_altitude = 44330.0 * (1.0 - pow(dps_pressure / 101325.0, 1.0 / 5.255));
        frameCount++;
    }
}

// ==================== 数据显示函数 ====================
void displayCompactData()
{
    // 显示FPS和运行时间
    Serial.printf("[%.1f Hz | %.1fs] ", currentFPS, millis() / 1000.0);

    // 显示0x91 IMU数据（紧凑格式）
    if (hipnuc_raw.hi91.tag == 0x91)
    {
        hi91_t *imu = &hipnuc_raw.hi91;
        Serial.printf("IMU: Roll=%6.2f° Pitch=%6.2f° Yaw=%6.2f° ",
                      imu->roll, imu->pitch, imu->yaw);
        Serial.printf("| Acc=[%6.2f,%6.2f,%6.2f]m/s² ",
                      imu->acc[0] * 9.8, imu->acc[1] * 9.8, imu->acc[2] * 9.8);
        Serial.printf("| Gyr=[%6.1f,%6.1f,%6.1f]°/s",
                      imu->gyr[0], imu->gyr[1], imu->gyr[2]);
        setLEDStatus(2);
    }
    // 显示0x81 INS数据（紧凑格式）
    else if (hipnuc_raw.hi81.tag == 0x81)
    {
        hi81_t *ins = &hipnuc_raw.hi81;
        Serial.printf("INS: Lat=%.6f° Lon=%.6f° Alt=%.2fm ",
                      ins->ins_lat * 1e-7, ins->ins_lon * 1e-7, ins->ins_msl * 1e-3);
        Serial.printf("| Sats=%d Quality=%d ",
                      ins->nv_pos, ins->solq_pos);
        Serial.printf("| Roll=%6.2f° Pitch=%6.2f° Yaw=%6.2f°",
                      ins->roll * 0.01, ins->pitch * 0.01, ins->yaw * 0.01);
        setLEDStatus(3);
    }
    // 显示0x83数据
    else if (hipnuc_raw.hi83.tag == 0x83)
    {
        hi83_t *data = &hipnuc_raw.hi83;
        Serial.printf("HI83: Bitmap=0x%X ", data->data_bitmap);
        if (data->data_bitmap & HI83_BMAP_RPY)
        {
            Serial.printf("| RPY=[%.2f,%.2f,%.2f]° ",
                          data->rpy[0], data->rpy[1], data->rpy[2]);
        }
        if (data->data_bitmap & HI83_BMAP_ACC_B)
        {
            Serial.printf("| Acc=[%.2f,%.2f,%.2f]m/s² ",
                          data->acc_b[0] * 9.8, data->acc_b[1] * 9.8, data->acc_b[2] * 9.8);
        }
        setLEDStatus(2);
    }
    else
    {
        Serial.print("等待数据...");
        setLEDStatus(1);
    }

    Serial.println();
}

// ==================== 详细数据显示（JSON格式）====================
void displayDetailedData()
{
    if (hipnuc_raw.hi91.tag == 0x91 ||
        hipnuc_raw.hi81.tag == 0x81 ||
        hipnuc_raw.hi83.tag == 0x83)
    {

        int len = hipnuc_dump_packet(&hipnuc_raw, displayBuffer, sizeof(displayBuffer));
        if (len > 0)
        {
            Serial.println("\n========== 详细数据 ==========");
            Serial.print(displayBuffer);
            Serial.println("==============================\n");
        }
    }
}

// ==================== 串口命令处理 ====================
void processSerialCommand()
{
    if (Serial.available())
    {
        char cmd = Serial.read();
        while (Serial.available())
            Serial.read(); // 清空缓冲区

        switch (cmd)
        {
        case 'd':
        case 'D':
            Serial.println("\n执行详细数据输出...");
            displayDetailedData();
            break;

        case 'i':
        case 'I':
            printSystemInfo();
            break;

        case 'r':
        case 'R':
            Serial.println("正在重启ESP32...");
            delay(500);
            ESP.restart();
            break;

        case 's':
        case 'S':
            Serial.println("\n========== 统计信息 ==========");
            Serial.printf("当前帧率: %.1f Hz\n", currentFPS);
            Serial.printf("运行时间: %.1f 秒\n", millis() / 1000.0);
            Serial.printf("空闲堆: %d bytes\n", ESP.getFreeHeap());
            Serial.printf("接收到的数据包类型: ");
            if (hipnuc_raw.hi91.tag == 0x91)
                Serial.print("0x91(IMU) ");
            if (hipnuc_raw.hi81.tag == 0x81)
                Serial.print("0x81(INS) ");
            if (hipnuc_raw.hi83.tag == 0x83)
                Serial.print("0x83(Flex) ");
            Serial.println("\n==============================\n");
            break;

        case 'h':
        case 'H':
            Serial.println("\n========== 命令帮助 ==========");
            Serial.println("  d - 显示详细数据(JSON格式)");
            Serial.println("  i - 显示系统信息");
            Serial.println("  s - 显示统计信息");
            Serial.println("  r - 重启ESP32");
            Serial.println("  h - 显示帮助信息");
            Serial.println("==============================\n");
            break;

        default:
            Serial.println("未知命令，输入 'h' 查看帮助");
            break;
        }
    }
}

// ==================== Setup ====================
void setup()
{
    delay(500);

    // 初始化串口
    Serial.begin(115200);
    Serial.println("\n\n");

    // 初始化I2C（DPS310传感器）
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

    // 初始化IMU串口（Serial2，使用RS485_2引脚）
    Serial2.begin(IMU_BAUDRATE, SERIAL_8N1, RS485_2_RX_PIN, RS485_2_TX_PIN);
    pinMode(RS485_2_DE_PIN, OUTPUT);
    digitalWrite(RS485_2_DE_PIN, LOW); // 接收模式

    // 初始化LED和蜂鸣器
    pinMode(BUZZER_PIN, OUTPUT);
    FastLED.addLeds<WS2812B, WS2812B_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    setLEDStatus(0);

    // 初始化解码器
    memset(&hipnuc_raw, 0, sizeof(hipnuc_raw_t));

    // 初始化LCD屏幕
    initLCD();

    // 初始化DPS310传感器
    initDPS310();

    // 打印系统信息
    printSystemInfo();

    // 启动倒计时
    startupCountdown();

    setLEDStatus(1);
    lastSecond = millis();
    lastDisplay = millis();
    lastLCDUpdate = millis();
    lastDPSRead = millis();
}

// ==================== Loop ====================
void loop()
{
    unsigned long now = millis();

    // 读取DPS310数据
    readDPS310();

    // 读取并解码IMU数据
    while (Serial2.available())
    {
        uint8_t data = Serial2.read();

        // 输入解码器
        if (hipnuc_input(&hipnuc_raw, data) > 0)
        {
            frameCount++;
            // playDataReceivedBeep();  // 可选：每次接收数据时蜂鸣
        }
    }

    // 每秒计算一次FPS
    if (now - lastSecond >= 1000)
    {
        currentFPS = frameCount;
        frameCount = 0;
        lastSecond = now;

        // 如果FPS为0，显示警告
        if (currentFPS == 0)
        {
            setLEDStatus(4);
        }
    }

    // 定时显示数据到串口（10Hz）
    if (now - lastDisplay >= DISPLAY_INTERVAL)
    {
        displayCompactData();
        lastDisplay = now;
    }

    // 定时更新LCD显示（20Hz）
    if (now - lastLCDUpdate >= LCD_UPDATE_INTERVAL)
    {
        updateLCDDisplay();
        lastLCDUpdate = now;
    }

    // 处理串口命令
    processSerialCommand();

    delay(1);
}
