/**
 * @file sdcard_demo.cpp
 * @brief SdFat库演示 - 高性能SD卡操作
 * @note 支持FAT16/FAT32/exFAT，性能优于Arduino SD库
 *
 * 硬件连接（SPI模式）：
 * - SD_CS   -> ESP32 GPIO 5
 * - SD_MOSI -> ESP32 GPIO 23
 * - SD_MISO -> ESP32 GPIO 19
 * - SD_SCK  -> ESP32 GPIO 18
 * - VCC     -> 3.3V
 * - GND     -> GND
 */

#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

// ==================== SD卡配置 ====================
#define SD_CS_PIN 5
#define SPI_SPEED SD_SCK_MHZ(25) // 25MHz SPI速度

// ==================== 全局对象 ====================
SdFat sd;
SdFile file;

// ==================== 文件路径定义 ====================
const char *TEST_FILE = "test.txt";
const char *DATA_DIR = "data";
const char *LOG_FILE = "data/sensor.log";
const char *CONFIG_FILE = "config.json";

// ==================== SD卡初始化 ====================
bool initSDCard()
{
    Serial.println("正在初始化SD卡...");

    if (!sd.begin(SD_CS_PIN, SPI_SPEED))
    {
        if (sd.card()->errorCode())
        {
            Serial.println("❌ SD卡初始化失败！");
            Serial.printf("错误代码: 0x%X, 0x%X\n",
                          sd.card()->errorCode(),
                          sd.card()->errorData());
        }
        else
        {
            Serial.println("❌ 未检测到SD卡");
        }
        return false;
    }

    Serial.println("✓ SD卡初始化成功");
    return true;
}

// ==================== SD卡信息 ====================
void printSDInfo()
{
    Serial.println("\n========== SD卡信息 ==========");

    // 卡类型
    Serial.print("卡类型: ");
    switch (sd.card()->type())
    {
    case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
    case SD_CARD_TYPE_SD2:
        Serial.println("SD2");
        break;
    case SD_CARD_TYPE_SDHC:
        Serial.println("SDHC");
        break;
    default:
        Serial.println("未知");
    }

    // 容量
    uint64_t cardSize = sd.card()->sectorCount() * 512ULL;
    Serial.printf("容量: %.2f GB\n", cardSize / 1024.0 / 1024.0 / 1024.0);

    // 文件系统类型
    Serial.print("文件系统: ");
    if (sd.vol()->fatType() == FAT_TYPE_FAT16)
    {
        Serial.println("FAT16");
    }
    else if (sd.vol()->fatType() == FAT_TYPE_FAT32)
    {
        Serial.println("FAT32");
    }
    else if (sd.vol()->fatType() == FAT_TYPE_EXFAT)
    {
        Serial.println("exFAT");
    }
    else
    {
        Serial.println("未知");
    }

    // 簇大小
    uint32_t clusterSize = sd.vol()->bytesPerCluster();
    Serial.printf("簇大小: %d KB\n", clusterSize / 1024);

    // 可用空间
    uint64_t freeSpace = sd.vol()->freeClusterCount() * clusterSize;
    Serial.printf("可用空间: %.2f MB\n", freeSpace / 1024.0 / 1024.0);

    Serial.println("==============================\n");
}

// ==================== 文件操作演示 ====================

// 1. 创建并写入文件
void demoWriteFile()
{
    Serial.println("\n【演示1】创建并写入文件");

    if (file.open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC))
    {
        file.println("Hello, SD Card!");
        file.println("This is a test file.");
        file.printf("Millis: %lu\n", millis());
        file.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
        file.close();
        Serial.printf("✓ 文件 '%s' 写入成功\n", TEST_FILE);
    }
    else
    {
        Serial.printf("❌ 无法创建文件 '%s'\n", TEST_FILE);
    }
}

// 2. 读取文件
void demoReadFile()
{
    Serial.println("\n【演示2】读取文件");

    if (file.open(TEST_FILE, O_RDONLY))
    {
        Serial.printf("--- %s 内容 ---\n", TEST_FILE);
        while (file.available())
        {
            Serial.write(file.read());
        }
        Serial.println("--- 文件结束 ---");
        file.close();
    }
    else
    {
        Serial.printf("❌ 无法打开文件 '%s'\n", TEST_FILE);
    }
}

// 3. 追加写入
void demoAppendFile()
{
    Serial.println("\n【演示3】追加写入");

    if (file.open(TEST_FILE, O_WRONLY | O_APPEND))
    {
        file.printf("Append at: %lu ms\n", millis());
        file.close();
        Serial.println("✓ 内容追加成功");
    }
    else
    {
        Serial.println("❌ 追加失败");
    }
}

// 4. 创建目录
void demoCreateDirectory()
{
    Serial.println("\n【演示4】创建目录");

    if (!sd.exists(DATA_DIR))
    {
        if (sd.mkdir(DATA_DIR))
        {
            Serial.printf("✓ 目录 '%s' 创建成功\n", DATA_DIR);
        }
        else
        {
            Serial.printf("❌ 目录 '%s' 创建失败\n", DATA_DIR);
        }
    }
    else
    {
        Serial.printf("目录 '%s' 已存在\n", DATA_DIR);
    }
}

// 5. 列出目录内容
void listDirectory(const char *dirname)
{
    Serial.printf("\n【目录列表】%s\n", dirname);

    SdFile dir;
    SdFile entry;

    if (!dir.open(dirname, O_RDONLY))
    {
        Serial.printf("❌ 无法打开目录 '%s'\n", dirname);
        return;
    }

    Serial.println("类型     大小(B)    文件名");
    Serial.println("-----------------------------");

    while (entry.openNext(&dir, O_RDONLY))
    {
        char name[64];
        entry.getName(name, sizeof(name));

        if (entry.isDir())
        {
            Serial.printf("DIR      %-10s %s/\n", "-", name);
        }
        else
        {
            Serial.printf("FILE     %-10lu %s\n", entry.fileSize(), name);
        }
        entry.close();
    }

    dir.close();
    Serial.println("-----------------------------\n");
}

// 6. 数据日志记录（高性能写入）
void demoDataLogging()
{
    Serial.println("\n【演示5】数据日志记录");

    if (file.open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND))
    {
        unsigned long start = millis();

        // 模拟传感器数据记录
        for (int i = 0; i < 100; i++)
        {
            file.printf("%lu,%d,%d,%d\n",
                        millis(),
                        random(0, 100),    // 传感器1
                        random(100, 200),  // 传感器2
                        random(200, 300)); // 传感器3
        }

        file.sync(); // 确保数据写入
        unsigned long elapsed = millis() - start;
        file.close();

        Serial.printf("✓ 记录100条数据，耗时: %lu ms\n", elapsed);
        Serial.printf("写入速度: %.2f 条/秒\n", 100000.0 / elapsed);
    }
    else
    {
        Serial.println("❌ 无法打开日志文件");
    }
}

// 7. 删除文件
void demoDeleteFile(const char *path)
{
    Serial.printf("\n【删除文件】%s\n", path);

    if (sd.exists(path))
    {
        if (sd.remove(path))
        {
            Serial.println("✓ 文件删除成功");
        }
        else
        {
            Serial.println("❌ 文件删除失败");
        }
    }
    else
    {
        Serial.println("文件不存在");
    }
}

// 8. 重命名文件
void demoRenameFile()
{
    Serial.println("\n【演示6】重命名文件");

    const char *oldName = "test.txt";
    const char *newName = "renamed.txt";

    if (sd.exists(oldName))
    {
        if (sd.rename(oldName, newName))
        {
            Serial.printf("✓ '%s' -> '%s'\n", oldName, newName);
        }
        else
        {
            Serial.println("❌ 重命名失败");
        }
    }
    else
    {
        Serial.printf("文件 '%s' 不存在\n", oldName);
    }
}

// 9. 二进制数据读写
void demoBinaryData()
{
    Serial.println("\n【演示7】二进制数据读写");

    const char *binFile = "data.bin";

    // 写入二进制数据
    if (file.open(binFile, O_WRONLY | O_CREAT | O_TRUNC))
    {
        struct SensorData
        {
            uint32_t timestamp;
            float temperature;
            float humidity;
            uint16_t pressure;
        } data;

        for (int i = 0; i < 10; i++)
        {
            data.timestamp = millis();
            data.temperature = 20.0 + i * 0.5;
            data.humidity = 50.0 + i * 1.0;
            data.pressure = 1000 + i * 10;

            file.write(&data, sizeof(data));
        }
        file.close();
        Serial.println("✓ 二进制数据写入完成");
    }

    // 读取二进制数据
    if (file.open(binFile, O_RDONLY))
    {
        struct SensorData
        {
            uint32_t timestamp;
            float temperature;
            float humidity;
            uint16_t pressure;
        } data;

        Serial.println("\n读取的二进制数据：");
        Serial.println("时间戳    温度   湿度   气压");

        while (file.read(&data, sizeof(data)) == sizeof(data))
        {
            Serial.printf("%lu  %.1f°C  %.1f%%  %d\n",
                          data.timestamp, data.temperature,
                          data.humidity, data.pressure);
        }
        file.close();
    }
}

// 10. 性能测试
void performanceTest()
{
    Serial.println("\n【性能测试】");

    const char *perfFile = "perf.dat";
    const size_t bufSize = 512;
    uint8_t buf[bufSize];

    // 填充测试数据
    for (size_t i = 0; i < bufSize; i++)
    {
        buf[i] = i & 0xFF;
    }

    // 写入测试
    if (file.open(perfFile, O_WRONLY | O_CREAT | O_TRUNC))
    {
        unsigned long start = millis();
        for (int i = 0; i < 1000; i++)
        {
            file.write(buf, bufSize);
        }
        file.sync();
        unsigned long elapsed = millis() - start;
        file.close();

        float speed = (1000.0 * bufSize) / (elapsed / 1000.0) / 1024.0;
        Serial.printf("写入速度: %.2f KB/s\n", speed);
    }

    // 读取测试
    if (file.open(perfFile, O_RDONLY))
    {
        unsigned long start = millis();
        while (file.read(buf, bufSize) > 0)
        {
            // 读取数据
        }
        unsigned long elapsed = millis() - start;
        file.close();

        float speed = (1000.0 * bufSize) / (elapsed / 1000.0) / 1024.0;
        Serial.printf("读取速度: %.2f KB/s\n", speed);
    }

    // 清理
    sd.remove(perfFile);
}

// ==================== 菜单函数 ====================
void printMenu()
{
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║      SdFat SD卡操作演示程序           ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println("\n【基础操作】");
    Serial.println("  i - 显示SD卡信息");
    Serial.println("  l - 列出根目录文件");
    Serial.println("  d - 列出data目录文件");
    Serial.println("\n【文件操作】");
    Serial.println("  1 - 创建并写入文件");
    Serial.println("  2 - 读取文件");
    Serial.println("  3 - 追加写入文件");
    Serial.println("  4 - 重命名文件");
    Serial.println("  5 - 删除测试文件");
    Serial.println("\n【高级功能】");
    Serial.println("  6 - 创建目录");
    Serial.println("  7 - 数据日志记录(100条)");
    Serial.println("  8 - 二进制数据读写");
    Serial.println("  9 - 性能测试");
    Serial.println("\n【其他】");
    Serial.println("  h - 显示此帮助");
    Serial.println("  r - 重新初始化SD卡");
    Serial.println("========================================");
}

// ==================== Setup ====================
void setup()
{
    delay(1000);
    Serial.begin(115200);
    Serial.println("\n\n");

    if (!initSDCard())
    {
        Serial.println("SD卡初始化失败，请检查连接后重启");
        while (1)
            delay(1000);
    }

    printSDInfo();
    printMenu();
}

// ==================== Loop ====================
void loop()
{
    if (Serial.available())
    {
        char cmd = Serial.read();
        while (Serial.available())
            Serial.read();

        Serial.printf("\n>>> 执行命令: %c\n", cmd);

        switch (cmd)
        {
        case 'i':
        case 'I':
            printSDInfo();
            break;

        case 'l':
        case 'L':
            listDirectory("/");
            break;

        case 'd':
        case 'D':
            listDirectory(DATA_DIR);
            break;

        case '1':
            demoWriteFile();
            break;

        case '2':
            demoReadFile();
            break;

        case '3':
            demoAppendFile();
            break;

        case '4':
            demoRenameFile();
            break;

        case '5':
            demoDeleteFile("renamed.txt");
            break;

        case '6':
            demoCreateDirectory();
            break;

        case '7':
            demoDataLogging();
            break;

        case '8':
            demoBinaryData();
            break;

        case '9':
            performanceTest();
            break;

        case 'r':
        case 'R':
            initSDCard();
            printSDInfo();
            break;

        case 'h':
        case 'H':
            printMenu();
            break;

        default:
            Serial.println("⚠️  未知命令，输入 'h' 查看帮助");
            break;
        }

        Serial.println("✓ 完成\n");
    }
}
