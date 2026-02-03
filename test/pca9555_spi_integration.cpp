/**
 * @file pca9555_spi_integration.cpp
 * @brief PCA9555与ESP32 SPI集成示例 - 实际应用场景
 * @note 演示如何使用PCA9555扩展16个SPI设备的片选
 *
 * 应用场景：
 * - 多个SPI传感器/显示屏
 * - 多个SPI ADC/DAC
 * - 多个SPI Flash/EEPROM
 * - 多个LCD显示屏（如多个ST7789）
 *
 * 硬件连接：
 * ESP32 SPI总线：
 * - MOSI  -> GPIO 23 (所有SPI设备共享)
 * - MISO  -> GPIO 19 (所有SPI设备共享)
 * - SCK   -> GPIO 18 (所有SPI设备共享)
 *
 * PCA9555 I2C：
 * - SDA   -> GPIO 21
 * - SCL   -> GPIO 22
 * - CS0-CS15 从PCA9555的P0.0-P1.7输出
 */

#include <Arduino.h>
#include <Wire.h>
#include <TCA9555.h>
#include <SPI.h>

// ==================== 引脚定义 ====================
// I2C
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// SPI
#define SPI_MOSI_PIN 23
#define SPI_MISO_PIN 19
#define SPI_SCK_PIN 18

// PCA9555地址
#define PCA9555_ADDR 0x20

// ==================== 全局对象 ====================
TCA9555 csExpander(PCA9555_ADDR);
SPIClass spi(VSPI); // 使用VSPI

// ==================== SPI设备类封装 ====================
class SPIDevice
{
private:
    uint8_t cs_pin;         // PCA9555上的CS引脚编号
    uint32_t spi_freq;      // SPI频率
    uint8_t spi_mode;       // SPI模式
    TCA9555 *gpio_expander; // GPIO扩展器指针

public:
    SPIDevice(uint8_t cs, uint32_t freq = 1000000, uint8_t mode = SPI_MODE0)
        : cs_pin(cs), spi_freq(freq), spi_mode(mode)
    {
        gpio_expander = &csExpander;
    }

    /**
     * @brief 开始SPI传输（选中设备）
     */
    void begin()
    {
        spi.beginTransaction(SPISettings(spi_freq, MSBFIRST, spi_mode));
        gpio_expander->digitalWrite(cs_pin, LOW);
        delayMicroseconds(1); // CS建立时间
    }

    /**
     * @brief 结束SPI传输（释放设备）
     */
    void end()
    {
        delayMicroseconds(1); // CS保持时间
        gpio_expander->digitalWrite(cs_pin, HIGH);
        spi.endTransaction();
    }

    /**
     * @brief 发送单字节数据
     */
    uint8_t transfer(uint8_t data)
    {
        return spi.transfer(data);
    }

    /**
     * @brief 批量传输数据
     */
    void transfer(uint8_t *buf, size_t len)
    {
        spi.transfer(buf, len);
    }

    /**
     * @brief 写入数据（不接收）
     */
    void write(uint8_t data)
    {
        spi.write(data);
    }

    /**
     * @brief 写入16位数据
     */
    void write16(uint16_t data)
    {
        spi.write16(data);
    }

    /**
     * @brief 写入32位数据
     */
    void write32(uint32_t data)
    {
        spi.write32(data);
    }
};

// ==================== 示例：多个SPI设备实例 ====================
SPIDevice flash_chip(0, 8000000); // CS0: SPI Flash, 8MHz
SPIDevice adc_chip(1, 2000000);   // CS1: ADC, 2MHz
SPIDevice dac_chip(2, 2000000);   // CS2: DAC, 2MHz
SPIDevice display1(3, 40000000);  // CS3: LCD Display #1, 40MHz
SPIDevice display2(4, 40000000);  // CS4: LCD Display #2, 40MHz
SPIDevice sensor1(5, 1000000);    // CS5: 传感器 #1, 1MHz
SPIDevice sensor2(6, 1000000);    // CS6: 传感器 #2, 1MHz

// ==================== 初始化函数 ====================
bool initSystem()
{
    // 初始化I2C
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    // 初始化PCA9555
    if (!csExpander.begin())
    {
        Serial.println("❌ PCA9555初始化失败！");
        return false;
    }
    Serial.println("✓ PCA9555初始化成功");

    // 配置所有CS引脚为输出并置高
    for (int i = 0; i < 16; i++)
    {
        csExpander.pinMode(i, OUTPUT);
        csExpander.digitalWrite(i, HIGH);
    }
    Serial.println("✓ 片选引脚初始化完成");

    // 初始化SPI总线
    spi.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, -1);
    Serial.println("✓ SPI总线初始化完成");

    return true;
}

// ==================== 示例应用函数 ====================

/**
 * @brief 示例：读取SPI Flash ID
 */
void readFlashID()
{
    Serial.println("\n【读取Flash芯片ID】");

    flash_chip.begin();
    flash_chip.write(0x9F); // Read JEDEC ID命令
    uint8_t mfg = flash_chip.transfer(0x00);
    uint8_t type = flash_chip.transfer(0x00);
    uint8_t capacity = flash_chip.transfer(0x00);
    flash_chip.end();

    Serial.printf("制造商ID: 0x%02X\n", mfg);
    Serial.printf("类型ID: 0x%02X\n", type);
    Serial.printf("容量ID: 0x%02X\n", capacity);
}

/**
 * @brief 示例：向LCD显示屏发送命令
 */
void sendLcdCommand(SPIDevice &lcd, uint8_t cmd)
{
    lcd.begin();
    lcd.write(cmd);
    lcd.end();
}

/**
 * @brief 示例：同时操作多个显示屏
 */
void demoMultipleDisplays()
{
    Serial.println("\n【演示：同时控制两个LCD】");

    // 向Display1发送命令
    Serial.println("向Display1发送初始化命令...");
    sendLcdCommand(display1, 0x11); // Sleep Out
    delay(120);

    // 向Display2发送命令
    Serial.println("向Display2发送初始化命令...");
    sendLcdCommand(display2, 0x11); // Sleep Out
    delay(120);

    Serial.println("✓ 两个显示屏初始化完成");
}

/**
 * @brief 示例：轮询读取多个传感器
 */
void pollSensors()
{
    Serial.println("\n【读取多个传感器】");

    // 读取传感器1
    sensor1.begin();
    sensor1.write(0x00); // 发送读取命令
    uint16_t value1 = (sensor1.transfer(0x00) << 8) | sensor1.transfer(0x00);
    sensor1.end();
    Serial.printf("传感器1数值: %d\n", value1);

    // 读取传感器2
    sensor2.begin();
    sensor2.write(0x00);
    uint16_t value2 = (sensor2.transfer(0x00) << 8) | sensor2.transfer(0x00);
    sensor2.end();
    Serial.printf("传感器2数值: %d\n", value2);
}

/**
 * @brief 示例：DAC输出控制
 */
void setDacOutput(uint16_t value)
{
    dac_chip.begin();
    dac_chip.write(0x30);    // DAC写命令
    dac_chip.write16(value); // 12位数值
    dac_chip.end();
    Serial.printf("DAC输出设置为: %d\n", value);
}

/**
 * @brief 性能测试：快速切换片选
 */
void performanceTest()
{
    Serial.println("\n【性能测试：片选切换速度】");

    unsigned long start = micros();
    for (int i = 0; i < 1000; i++)
    {
        csExpander.digitalWrite(0, LOW);
        csExpander.digitalWrite(0, HIGH);
    }
    unsigned long elapsed = micros() - start;

    Serial.printf("1000次片选切换耗时: %lu μs\n", elapsed);
    Serial.printf("单次切换平均耗时: %.2f μs\n", elapsed / 1000.0);
    Serial.printf("理论最大切换频率: %.1f kHz\n", 1000000.0 / (elapsed / 1000.0));
}

/**
 * @brief 批量端口操作演示
 */
void demoBulkOperation()
{
    Serial.println("\n【批量端口操作演示】");

    // 一次性设置Port 0的CS0,CS2,CS4,CS6为低电平
    // 位掩码: 0b10101010 = 0xAA (奇数位为高，偶数位为低)
    Serial.println("同时选中CS1,CS3,CS5,CS7...");
    csExpander.write8(0, 0xAA);
    delay(500);

    // 恢复
    csExpander.write8(0, 0xFF);
    Serial.println("释放所有片选");
}

// ==================== Setup ====================
void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║  PCA9555 + SPI 多设备集成演示         ║");
    Serial.println("╚════════════════════════════════════════╝\n");

    if (!initSystem())
    {
        Serial.println("系统初始化失败！");
        while (1)
            delay(1000);
    }

    Serial.println("\n[已配置的SPI设备]");
    Serial.println("  CS0: SPI Flash (8MHz)");
    Serial.println("  CS1: ADC (2MHz)");
    Serial.println("  CS2: DAC (2MHz)");
    Serial.println("  CS3: LCD Display #1 (40MHz)");
    Serial.println("  CS4: LCD Display #2 (40MHz)");
    Serial.println("  CS5: Sensor #1 (1MHz)");
    Serial.println("  CS6: Sensor #2 (1MHz)");
    Serial.println("  CS7-CS15: 备用");
    Serial.println("========================================\n");

    delay(2000);
}

// ==================== Loop ====================
void loop()
{
    Serial.println("\n========== 开始应用演示 ==========\n");

    // 演示1: 读取Flash ID
    readFlashID();
    delay(1000);

    // 演示2: 控制多个显示屏
    demoMultipleDisplays();
    delay(1000);

    // 演示3: 读取传感器
    pollSensors();
    delay(1000);

    // 演示4: DAC输出
    setDacOutput(2048); // 中间值
    delay(1000);

    // 演示5: 批量操作
    demoBulkOperation();
    delay(1000);

    // 演示6: 性能测试
    performanceTest();

    Serial.println("\n========== 演示结束 ==========");
    Serial.println("等待10秒后重复...\n");
    delay(10000);
}
