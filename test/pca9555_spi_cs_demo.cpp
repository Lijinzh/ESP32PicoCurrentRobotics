/**
 * @file pca9555_spi_cs_demo.cpp
 * @brief PCA9555/TCA9555 GPIO扩展器演示 - 用于扩展SPI片选线
 * @note PCA9555是16位I2C GPIO扩展器，可提供16个额外的GPIO引脚
 *
 * 硬件连接：
 * - PCA9555 SDA -> ESP32 GPIO 21
 * - PCA9555 SCL -> ESP32 GPIO 22
 * - PCA9555 VCC -> 3.3V
 * - PCA9555 GND -> GND
 * - PCA9555 A0/A1/A2 -> GND (I2C地址 = 0x20)
 *
 * 使用场景：
 * - 16个SPI设备片选（CS0-CS15）
 * - Port 0 (P0.0-P0.7): CS0-CS7
 * - Port 1 (P1.0-P1.7): CS8-CS15
 */

#include <Arduino.h>
#include <Wire.h>
#include <TCA9555.h>

// ==================== I2C配置 ====================
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define I2C_FREQ 400000 // 400kHz快速模式

// PCA9555 I2C地址 (A0=A1=A2=GND)
#define PCA9555_ADDR 0x20

// ==================== SPI片选管理 ====================
#define MAX_CS_PINS 16 // PCA9555提供16个GPIO

// ==================== 全局对象 ====================
TCA9555 gpio(PCA9555_ADDR); // 创建TCA9555对象

// ==================== SPI片选控制函数 ====================

/**
 * @brief 初始化PCA9555作为SPI片选扩展器
 * @return true 初始化成功, false 初始化失败
 */
bool initSpiChipSelect()
{
    // 初始化I2C总线
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(I2C_FREQ);

    // 初始化PCA9555
    if (!gpio.begin())
    {
        Serial.println("❌ PCA9555初始化失败！检查I2C连接和地址");
        return false;
    }

    Serial.println("✓ PCA9555初始化成功");

    // 配置所有16个引脚为输出模式
    for (int i = 0; i < MAX_CS_PINS; i++)
    {
        gpio.pinMode(i, OUTPUT);
    }

    // 初始状态：所有片选置高（未选中）
    for (int i = 0; i < MAX_CS_PINS; i++)
    {
        gpio.digitalWrite(i, HIGH);
    }

    Serial.println("✓ 所有SPI片选引脚配置完成（默认HIGH/未选中）");
    return true;
}

/**
 * @brief 选中指定的SPI设备（拉低对应CS引脚）
 * @param cs_num 片选编号 (0-15)
 */
void spiSelect(uint8_t cs_num)
{
    if (cs_num >= MAX_CS_PINS)
    {
        Serial.printf("⚠️  无效的CS编号: %d\n", cs_num);
        return;
    }
    gpio.digitalWrite(cs_num, LOW);
    Serial.printf("📍 SPI_CS%d = LOW (设备已选中)\n", cs_num);
}

/**
 * @brief 取消选中指定的SPI设备（拉高对应CS引脚）
 * @param cs_num 片选编号 (0-15)
 */
void spiDeselect(uint8_t cs_num)
{
    if (cs_num >= MAX_CS_PINS)
    {
        Serial.printf("⚠️  无效的CS编号: %d\n", cs_num);
        return;
    }
    gpio.digitalWrite(cs_num, HIGH);
    Serial.printf("📍 SPI_CS%d = HIGH (设备已释放)\n", cs_num);
}

/**
 * @brief 取消所有SPI设备选中（所有CS拉高）
 */
void spiDeselectAll()
{
    for (int i = 0; i < MAX_CS_PINS; i++)
    {
        gpio.digitalWrite(i, HIGH);
    }
    Serial.println("📍 所有SPI设备已释放");
}

/**
 * @brief 读取PCA9555的输入状态（调试用）
 */
void readGpioStatus()
{
    Serial.println("\n========== PCA9555 GPIO状态 ==========");

    // 读取Port 0 (P0.0-P0.7)
    uint8_t port0 = gpio.read8(0);
    Serial.print("Port 0 (CS0-CS7):  ");
    for (int i = 7; i >= 0; i--)
    {
        Serial.print((port0 & (1 << i)) ? "1" : "0");
        if (i == 4)
            Serial.print(" ");
    }
    Serial.println();

    // 读取Port 1 (P1.0-P1.7)
    uint8_t port1 = gpio.read8(1);
    Serial.print("Port 1 (CS8-CS15): ");
    for (int i = 7; i >= 0; i--)
    {
        Serial.print((port1 & (1 << i)) ? "1" : "0");
        if (i == 4)
            Serial.print(" ");
    }
    Serial.println();

    Serial.println("======================================\n");
}

/**
 * @brief 演示：批量设置多个片选（用于多设备同步通信）
 */
void demoMultipleCS()
{
    Serial.println("\n=== 演示：同时选中CS0和CS5 ===");
    gpio.digitalWrite(0, LOW);
    gpio.digitalWrite(5, LOW);
    delay(500);
    readGpioStatus();

    Serial.println("=== 释放所有片选 ===");
    spiDeselectAll();
    delay(500);
    readGpioStatus();
}

/**
 * @brief 演示：快速切换片选（模拟SPI多设备通信）
 */
void demoFastSwitching()
{
    Serial.println("\n=== 演示：快速切换片选（CS0-CS7循环）===");

    for (int i = 0; i < 8; i++)
    {
        spiSelect(i);
        delayMicroseconds(100); // 模拟SPI传输
        spiDeselect(i);
        delay(200);
    }

    Serial.println("✓ 快速切换完成\n");
}

/**
 * @brief 演示：使用位操作快速设置整个端口
 */
void demoPortWrite()
{
    Serial.println("\n=== 演示：端口批量写入 ===");

    // 设置Port 0所有引脚为0（选中CS0-CS7）
    Serial.println("Port 0 全部拉低 (CS0-CS7选中)");
    gpio.write8(0, 0x00);
    delay(1000);
    readGpioStatus();

    // 设置Port 1所有引脚为0（选中CS8-CS15）
    Serial.println("Port 1 全部拉低 (CS8-CS15选中)");
    gpio.write8(1, 0x00);
    delay(1000);
    readGpioStatus();

    // 恢复所有为高电平
    Serial.println("所有片选恢复HIGH");
    gpio.write8(0, 0xFF);
    gpio.write8(1, 0xFF);
    delay(500);
    readGpioStatus();
}

/**
 * @brief 演示：读取GPIO状态（可用于检测从设备的BUSY/READY信号）
 */
void demoReadGpio()
{
    Serial.println("\n=== 演示：GPIO读取功能 ===");
    Serial.println("提示：可将某些引脚配置为输入，用于读取SPI设备的BUSY/READY信号");

    // 将P1.7配置为输入（示例：读取设备状态）
    gpio.pinMode(15, INPUT);
    Serial.println("✓ P1.7 (CS15) 配置为输入模式");

    bool state = gpio.digitalRead(15);
    Serial.printf("P1.7 当前状态: %s\n", state ? "HIGH" : "LOW");

    // 恢复为输出
    gpio.pinMode(15, OUTPUT);
    gpio.digitalWrite(15, HIGH);
    Serial.println("✓ P1.7 恢复为输出模式\n");
}

// ==================== Setup ====================
void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║   PCA9555 SPI片选扩展器 演示程序      ║");
    Serial.println("╚════════════════════════════════════════╝\n");

    // 初始化PCA9555
    if (!initSpiChipSelect())
    {
        Serial.println("初始化失败，停止运行");
        while (1)
        {
            delay(1000);
        }
    }

    Serial.println("\n[系统信息]");
    Serial.printf("  芯片型号: ESP32-PICO-D4\n");
    Serial.printf("  I2C引脚: SDA=%d, SCL=%d\n", I2C_SDA_PIN, I2C_SCL_PIN);
    Serial.printf("  PCA9555地址: 0x%02X\n", PCA9555_ADDR);
    Serial.printf("  可用片选数: %d (CS0-CS%d)\n", MAX_CS_PINS, MAX_CS_PINS - 1);
    Serial.println("----------------------------------------\n");

    // 读取初始状态
    readGpioStatus();

    delay(2000);
}

// ==================== Loop ====================
void loop()
{
    Serial.println("\n========== 开始演示循环 ==========\n");

    // 演示1: 单个片选控制
    Serial.println("【演示1】单个片选控制");
    for (int i = 0; i < 4; i++)
    {
        spiSelect(i);
        delay(300);
        spiDeselect(i);
        delay(300);
    }

    delay(1000);

    // 演示2: 快速切换
    demoFastSwitching();

    delay(1000);

    // 演示3: 多片选同时激活
    demoMultipleCS();

    delay(1000);

    // 演示4: 端口批量写入
    demoPortWrite();

    delay(1000);

    // 演示5: GPIO读取
    demoReadGpio();

    Serial.println("\n========== 演示循环结束 ==========");
    Serial.println("等待10秒后重新开始...\n");
    delay(10000);
}
