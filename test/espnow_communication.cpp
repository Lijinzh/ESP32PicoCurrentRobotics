/**
 * @file espnow_communication.cpp
 * @brief ESP-NOW双向无线通信示例
 * @note 支持两个ESP32设备互相发送和接收数据
 *
 * 使用方法：
 * 1. 在第一个ESP32上定义 #define DEVICE_1 为1
 * 2. 在第二个ESP32上定义 #define DEVICE_1 为0
 * 3. 分别编译上传到两个设备
 */

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <FastLED.h>

// ==================== 设备配置 ====================
// 修改这个值来区分两个设备：设备1设为1，设备2设为0
#define DEVICE_1 1 // 设备1

// ==================== 引脚配置 ====================
#define WS2812_PIN 26
#define NUM_LEDS 1
#define BUZZER_PIN 2

// ==================== 通信数据结构 ====================
typedef struct
{
    char message[32];     // 文本消息
    float sensor_data[4]; // 传感器数据（例如：编码器角度）
    uint32_t counter;     // 计数器
    uint32_t timestamp;   // 时间戳
} DataPacket;

// ==================== 全局变量 ====================
CRGB leds[NUM_LEDS];

// 对方设备的MAC地址（需要根据实际情况修改）
#if DEVICE_1
// 设备1的配置：需要填写设备2的MAC地址
uint8_t peerMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // 替换为设备2的实际MAC
const char *deviceName = "Device 1";
#else
// 设备2的配置：需要填写设备1的MAC地址
uint8_t peerMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // 替换为设备1的实际MAC
const char *deviceName = "Device 2";
#endif

DataPacket sendData;
DataPacket receiveData;

// 发送统计
uint32_t sendCounter = 0;
uint32_t receiveCounter = 0;
uint32_t sendFailCounter = 0;

unsigned long lastSendTime = 0;
const uint32_t SEND_INTERVAL = 1000; // 发送间隔（毫秒）

bool peerAdded = false;

// ==================== ESP-NOW回调函数 ====================

/**
 * @brief 数据发送完成回调
 */
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    if (status == ESP_NOW_SEND_SUCCESS)
    {
        Serial.println("✓ 发送成功");
        leds[0] = CRGB::Green;
    }
    else
    {
        Serial.println("✗ 发送失败");
        sendFailCounter++;
        leds[0] = CRGB::Red;
        tone(BUZZER_PIN, 500, 100); // 失败时短鸣
    }
    FastLED.show();
}

/**
 * @brief 数据接收回调
 */
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&receiveData, incomingData, sizeof(receiveData));
    receiveCounter++;

    Serial.println("\n========== 收到数据 ==========");
    Serial.print("来自MAC: ");
    for (int i = 0; i < 6; i++)
    {
        Serial.printf("%02X", mac[i]);
        if (i < 5)
            Serial.print(":");
    }
    Serial.println();

    Serial.printf("消息: %s\n", receiveData.message);
    Serial.printf("计数器: %lu\n", receiveData.counter);
    Serial.printf("时间戳: %lu ms\n", receiveData.timestamp);
    Serial.print("传感器数据: [");
    for (int i = 0; i < 4; i++)
    {
        Serial.printf("%.2f", receiveData.sensor_data[i]);
        if (i < 3)
            Serial.print(", ");
    }
    Serial.println("]");
    Serial.printf("总接收次数: %lu\n", receiveCounter);
    Serial.println("==============================\n");

    // 接收成功时闪烁蓝灯
    leds[0] = CRGB::Blue;
    FastLED.show();
    tone(BUZZER_PIN, 1000, 50); // 接收时短鸣
    delay(100);
}

// ==================== 初始化ESP-NOW ====================
bool initESPNow()
{
    // 设置为Station模式
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    // 打印本机MAC地址
    Serial.print("本机MAC地址: ");
    Serial.println(WiFi.macAddress());

    // 初始化ESP-NOW
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("ESP-NOW初始化失败");
        return false;
    }
    Serial.println("ESP-NOW初始化成功");

    // 注册回调函数
    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    return true;
}

// ==================== 添加对等设备 ====================
bool addPeer()
{
    // 检查MAC地址是否已设置
    bool isDefaultMAC = true;
    for (int i = 0; i < 6; i++)
    {
        if (peerMAC[i] != 0xFF)
        {
            isDefaultMAC = false;
            break;
        }
    }

    if (isDefaultMAC)
    {
        Serial.println("⚠ 警告: 对方MAC地址未设置！");
        Serial.println("请先获取对方ESP32的MAC地址并修改代码中的peerMAC数组");
        return false;
    }

    // 配置对等设备信息
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerMAC, 6);
    peerInfo.channel = 0;     // 使用当前信道
    peerInfo.encrypt = false; // 不加密

    // 添加对等设备
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("添加对等设备失败");
        return false;
    }

    Serial.print("成功添加对等设备，MAC: ");
    for (int i = 0; i < 6; i++)
    {
        Serial.printf("%02X", peerMAC[i]);
        if (i < 5)
            Serial.print(":");
    }
    Serial.println();

    return true;
}

// ==================== 发送数据 ====================
void sendDataPacket()
{
    // 准备发送数据
    sendCounter++;

#if DEVICE_1
    snprintf(sendData.message, sizeof(sendData.message), "Hello from Device 1");
    // 模拟传感器数据
    sendData.sensor_data[0] = random(0, 360) * 1.0;
    sendData.sensor_data[1] = random(0, 360) * 1.0;
    sendData.sensor_data[2] = random(0, 360) * 1.0;
    sendData.sensor_data[3] = random(0, 360) * 1.0;
#else
    snprintf(sendData.message, sizeof(sendData.message), "Hello from Device 2");
    // 模拟传感器数据
    sendData.sensor_data[0] = random(0, 100) * 0.1;
    sendData.sensor_data[1] = random(0, 100) * 0.1;
    sendData.sensor_data[2] = random(0, 100) * 0.1;
    sendData.sensor_data[3] = random(0, 100) * 0.1;
#endif

    sendData.counter = sendCounter;
    sendData.timestamp = millis();

    // 发送数据
    esp_err_t result = esp_now_send(peerMAC, (uint8_t *)&sendData, sizeof(sendData));

    if (result == ESP_OK)
    {
        Serial.printf("📤 发送数据包 #%lu...\n", sendCounter);
    }
    else
    {
        Serial.printf("❌ 发送启动失败，错误代码: %d\n", result);
        sendFailCounter++;
    }
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

// ==================== 打印帮助信息 ====================
void printHelp()
{
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.printf("║  ESP-NOW 通信测试 - %s     ║\n", deviceName);
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println("\n串口命令:");
    Serial.println("  s - 手动发送一次数据");
    Serial.println("  i - 显示设备信息");
    Serial.println("  r - 重启ESP32");
    Serial.println("  h - 显示帮助信息");
    Serial.println();
}

// ==================== Setup ====================
void setup()
{
    delay(500);
    Serial.begin(115200);
    Serial.println("\n\n");

    // 初始化蜂鸣器和LED
    pinMode(BUZZER_PIN, OUTPUT);
    FastLED.addLeds<WS2812B, WS2812_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);
    leds[0] = CRGB::Orange;
    FastLED.show();

    playStartupSound();

    printHelp();

    // 初始化ESP-NOW
    leds[0] = CRGB::Yellow;
    FastLED.show();

    if (!initESPNow())
    {
        Serial.println("❌ ESP-NOW初始化失败，系统停止");
        leds[0] = CRGB::Red;
        FastLED.show();
        while (1)
        {
            tone(BUZZER_PIN, 500);
            delay(500);
            noTone(BUZZER_PIN);
            delay(500);
        }
    }

    // 添加对等设备
    if (addPeer())
    {
        peerAdded = true;
        leds[0] = CRGB::Green;
        Serial.println("✓ 系统就绪，开始通信");
    }
    else
    {
        leds[0] = CRGB::Purple;
        Serial.println("⚠ 对等设备未配置，仅接收模式");
    }

    FastLED.show();
    tone(BUZZER_PIN, 2000, 100);
    delay(1000);
}

// ==================== Loop ====================
void loop()
{
    // 定时自动发送数据
    if (peerAdded && (millis() - lastSendTime >= SEND_INTERVAL))
    {
        lastSendTime = millis();
        sendDataPacket();
    }

    // 处理串口命令
    if (Serial.available())
    {
        char cmd = Serial.read();
        while (Serial.available())
            Serial.read(); // 清空缓冲区

        switch (cmd)
        {
        case 's':
        case 'S':
            if (peerAdded)
            {
                Serial.println("\n手动发送数据...");
                sendDataPacket();
            }
            else
            {
                Serial.println("⚠ 对等设备未配置");
            }
            break;

        case 'i':
        case 'I':
            Serial.println("\n========== 设备信息 ==========");
            Serial.printf("设备名称: %s\n", deviceName);
            Serial.printf("本机MAC: %s\n", WiFi.macAddress().c_str());
            Serial.print("对方MAC: ");
            for (int i = 0; i < 6; i++)
            {
                Serial.printf("%02X", peerMAC[i]);
                if (i < 5)
                    Serial.print(":");
            }
            Serial.println();
            Serial.printf("发送计数: %lu\n", sendCounter);
            Serial.printf("接收计数: %lu\n", receiveCounter);
            Serial.printf("发送失败: %lu\n", sendFailCounter);
            Serial.printf("运行时间: %lu 秒\n", millis() / 1000);
            Serial.println("==============================\n");
            break;

        case 'r':
        case 'R':
            Serial.println("正在重启...");
            delay(500);
            ESP.restart();
            break;

        case 'h':
        case 'H':
            printHelp();
            break;

        default:
            Serial.println("未知命令，输入 'h' 查看帮助");
            break;
        }
    }

    delay(10);
}
