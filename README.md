# ESP32-PICO-D4 机器人开发平台

<div align="center">

![ESP32](https://img.shields.io/badge/ESP32-PICO--D4-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Ready-green)
![License](https://img.shields.io/badge/License-MIT-yellow)
![Language](https://img.shields.io/badge/Language-C%2B%2B-orange)

**专业级嵌入式机器人开发平台，集成通信、传感器、UI、存储于一体**

[快速开始](#-快速开始) • [功能特性](#-核心功能模块) • [使用文档](#-使用文档) • [硬件配置](#-硬件配置)

</div>

---

## 📋 项目简介

基于 ESP32-PICO-D4 (240MHz) 的多功能机器人开发平台，提供完整的硬件驱动、通信协议、图形界面和数据存储解决方案。适用于移动机器人、智能设备、物联网应用等场景。

### 🎯 核心优势

- ✅ **即插即用** - 开箱即用的完整示例代码和详细中文文档
- ✅ **高性能** - 240MHz 主频，100+ Hz 数据采集，<10ms 通信延迟
- ✅ **模块化** - 独立的功能模块，可灵活组合使用
- ✅ **专业级** - 完整的错误处理、性能优化和故障排查方案
- ✅ **开源免费** - MIT 协议，文档齐全，持续更新

---

## 🚀 核心功能模块

### 1️⃣ 通信系统

#### 📡 Modbus RTU 编码器采集
- **高速通信**: 115200 波特率，支持 100+ Hz 数据更新
- **多设备支持**: 同时管理多个 Modbus 编码器（已验证 4 个）
- **优化流程**: 批量读取模式，最小化通信延迟
- **实时输出**: CSV 格式数据流，便于分析和记录

#### 📶 ESP-NOW 无线通信
- **点对点传输**: 无需路由器，直连 200 米范围
- **低延迟**: <10ms 典型延迟，适合实时控制
- **双向通信**: 支持数据收发和状态反馈
- **自动重连**: 连接中断自动恢复机制

#### 🧭 HiPNUC IMU 姿态传感器
- **硬件串口**: 使用 Serial2 实现稳定 100 Hz 数据采集
- **多协议支持**: 0x91(IMU) / 0x81(INS) / 0x83(灵活) 数据包
- **高精度姿态**: Roll/Pitch/Yaw 实时姿态角
- **传感器融合**: 加速度、陀螺仪、磁力计、GPS 数据

### 2️⃣ 扩展与外设

#### 🔌 PCA9555 GPIO 扩展器
- **16 路 I/O 扩展**: I2C 总线控制，节省引脚资源
- **SPI 片选管理**: 支持 16 路 SPI 设备片选扩展
- **中断支持**: 硬件中断检测 I/O 状态变化
- **灵活配置**: 每个引脚独立配置输入/输出/上拉

#### 💾 SD 卡存储
- **高性能**: 支持 FAT16/FAT32/exFAT 文件系统
- **SdFat 库**: 专业级 SD 卡读写库
- **数据日志**: 传感器数据持久化存储
- **性能测试**: 内置读写速度测试工具

#### 🔊 蜂鸣器控制
- **音乐播放**: 支持乐谱格式，内置 4 首示例曲目
- **音效库**: 6 种预设音效（启动、警告、成功等）
- **PWM 驱动**: 精确频率控制，音质清晰
- **非阻塞**: 后台播放，不影响主程序运行

### 3️⃣ 图形界面系统

#### 🎨 LVGL 图形库集成
- **专业 GUI**: 业界领先的嵌入式图形库（⭐16.7k）
- **丰富组件**: 40+ 内置组件（按钮、滑块、图表等）
- **流畅动画**: 60 FPS 渲染，支持缓动函数
- **主题系统**: 可定制主题，深色/浅色模式
- **TFT_eSPI 集成**: 与现有显示库无缝对接
- **内存优化**: 仅需 64KB RAM，支持双缓冲

### 4️⃣ 状态指示

#### 💡 WS2812B RGB LED
- **实时状态**: 不同颜色表示系统运行状态
- **动画效果**: 流水、呼吸、闪烁等动态效果
- **低功耗**: 单线控制，节省引脚

---

## ✨ 主要特性

| 特性 | 说明 |
|------|------|
| 🚀 **高性能** | 240MHz 主频，100+ Hz 数据采集，实时控制 |
| 📡 **多协议** | Modbus RTU、ESP-NOW、HiPNUC、I2C、SPI |
| 🎨 **现代 UI** | LVGL 图形库，60 FPS 流畅动画 |
| 💾 **数据存储** | SD 卡支持，FAT32 文件系统 |
| 🔌 **易扩展** | GPIO 扩展器，支持 16 路 I/O 和 SPI CS |
| 🔊 **人机交互** | 蜂鸣器音效、RGB LED 状态、串口命令 |
| 📚 **完整文档** | 详细中文使用指南和示例代码 |
| 🛠️ **开发友好** | PlatformIO 工程，一键编译上传 |

---

## 🔧 硬件配置

### 开发板规格
- **主控芯片**: ESP32-PICO-D4 (集成 4MB Flash)
- **CPU 频率**: 240 MHz (Dual-core Tensilica LX6)
- **内存**: 520 KB SRAM
- **Flash 频率**: 80 MHz
- **无线**: WiFi 802.11 b/g/n + Bluetooth 4.2

### 完整引脚分配

| 功能模块 | 引脚 | 方向 | 说明 |
|----------|------|------|------|
| **RS485 通信** ||||
| RS485 RX | GPIO 32 | IN | Modbus 数据接收 |
| RS485 TX | GPIO 33 | OUT | Modbus 数据发送 |
| RS485 DE/RE | GPIO 25 | OUT | 收发控制（高=发送，低=接收）|
| **IMU 传感器** ||||
| IMU RX | GPIO 16 | IN | HiPNUC 数据接收（Serial2）|
| IMU TX | GPIO 17 | OUT | HiPNUC 配置发送（Serial2）|
| **I2C 总线** ||||
| SDA | GPIO 21 | INOUT | PCA9555 数据线 |
| SCL | GPIO 22 | OUT | PCA9555 时钟线 |
| **SPI 总线** ||||
| MISO | GPIO 19 | IN | SPI 主机输入 |
| MOSI | GPIO 23 | OUT | SPI 主机输出 |
| SCK | GPIO 18 | OUT | SPI 时钟 |
| CS (SD Card) | GPIO 5 | OUT | SD 卡片选 |
| **TFT 显示** ||||
| TFT CS | GPIO 5 | OUT | 显示屏片选 |
| TFT DC | GPIO 2 | OUT | 数据/命令切换 |
| TFT RST | GPIO 4 | OUT | 显示屏复位 |
| **外设控制** ||||
| WS2812B LED | GPIO 26 | OUT | RGB 状态指示灯 |
| 蜂鸣器 | GPIO 27 | OUT | PWM 音频输出 |
| 按键 1 | GPIO 0 | IN | 功能按键（上拉）|
| 按键 2 | GPIO 35 | IN | 功能按键（上拉）|

### 外部设备配置

#### Modbus RTU 编码器
- **通信协议**: Modbus RTU
- **波特率**: 115200 bps
- **数据格式**: 8N1
- **默认地址**: 1, 2, 3, 5（可修改）
- **读取寄存器**: 0x0001（角度数据）
- **数据范围**: 0-360°

#### PCA9555 GPIO 扩展器
- **I2C 地址**: 0x20（默认，A0/A1/A2 接地）
- **电源电压**: 2.3V - 5.5V
- **I2C 速度**: 100 kHz (标准) / 400 kHz (快速)
- **中断引脚**: 可选配置

#### SD 卡模块
- **文件系统**: FAT16 / FAT32 / exFAT
- **容量支持**: 最大 32GB（FAT32）
- **SPI 模式**: 标准 SPI 通信
- **电压**: 3.3V (使用电平转换模块)

---

## 📦 依赖库

```ini
[env:pico32]
platform = espressif32
board = pico32
framework = arduino

lib_deps = 
    # 通信协议
    emelianov/modbus-esp8266 @ ^4.1.0      # Modbus RTU 通信
    
    # 显示与 UI
    bodmer/TFT_eSPI @ ^2.5.43              # TFT 显示驱动
    lvgl/lvgl @ ^9.2.0                      # 图形界面库
    
    # 外设控制
    fastled/FastLED @ ^3.10.3              # WS2812B LED 控制
    mathertel/OneButton @ ^2.6.1           # 按键处理
    arduinogetstarted/ezBuzzer @ ^1.0.2    # 蜂鸣器控制
    
    # 传感器
    adafruit/Adafruit DPS310 @ ^1.1.5      # 气压传感器
    
    # 扩展模块
    robtillaart/TCA9555 @ ^0.4.4           # I/O 扩展器
    greiman/SdFat @ ^2.2.3                 # SD 卡高性能库
    
    # 其他
    plerup/EspSoftwareSerial @ ^8.2.0      # 软串口（备用）
```

---

## 🚀 快速开始

### 1. 环境准备

#### 安装 PlatformIO
```bash
# 方法 1: VS Code 插件（推荐）
# 在 VS Code 扩展商店搜索 "PlatformIO IDE"

# 方法 2: 命令行安装
pip install platformio
```

#### 克隆项目
```bash
git clone https://github.com/Lijinzh/ESP32PicoCurrentRobotics.git
cd ESP32PicoCurrentRobotics
```

### 2. 功能模块选择

本项目包含多个独立演示程序，根据需求选择对应模块：

#### 🎨 LVGL 图形界面 Demo（推荐新手）
```bash
# 完整的 UI 演示，包含按钮、滑块、图表等组件
cp test/lvgl_demo.cpp src/main.cpp
pio run --target upload
```
📖 详细文档：[test/LVGL使用指南.md](test/LVGL使用指南.md)

#### 📡 Modbus 编码器采集
```bash
# 高速批量读取多个编码器数据
cp test/编码器数据批量快速读取备份 src/main.cpp
pio run --target upload
```

#### 📶 ESP-NOW 无线通信
```bash
# 点对点无线数据传输
cp test/espnow_communication.cpp src/main.cpp
# ⚠️ 需要修改设备角色和对方 MAC 地址
pio run --target upload
```
📖 详细文档：[test/ESP-NOW使用说明.md](test/ESP-NOW使用说明.md)

#### 🧭 HiPNUC IMU 姿态传感器
```bash
# 实时姿态数据采集
# 已经是 src/main.cpp 默认程序
pio run --target upload
```
📖 详细文档：[test/HiPNUC-IMU使用说明.md](test/HiPNUC-IMU使用说明.md)  
📖 性能分析：[test/软串口性能分析.md](test/软串口性能分析.md)

#### 🔌 PCA9555 GPIO 扩展
```bash
# 基础演示
cp test/pca9555_spi_cs_demo.cpp src/main.cpp

# 高级集成（SPI 设备管理）
cp test/pca9555_spi_integration.cpp src/main.cpp
pio run --target upload
```
📖 详细文档：[test/PCA9555使用指南.md](test/PCA9555使用指南.md)

#### 💾 SD 卡存储
```bash
# 文件读写和性能测试
cp test/sdcard_demo.cpp src/main.cpp
pio run --target upload
```

#### 🔊 蜂鸣器音效
```bash
# 音乐播放和音效演示
cp test/buzzer_demo.cpp src/main.cpp
pio run --target upload
```

### 3. 编译与上传

```bash
# 编译项目
platformio run

# 上传到开发板
platformio run --target upload

# 打开串口监视器（115200 波特率）
platformio device monitor

# 一键编译、上传、监视
platformio run --target upload && platformio device monitor
```

### 4. 验证运行

根据选择的功能模块，观察以下现象：

| 模块 | 预期现象 | 串口输出 |
|------|----------|----------|
| LVGL Demo | RGB LED 显示，屏幕显示 UI | `LVGL Initialization Complete!` |
| Modbus 编码器 | LED 绿色（正常）/红色（异常）| CSV 格式角度数据流 |
| ESP-NOW | LED 绿色（发送成功）| `Data sent, Delivery: Success` |
| IMU 传感器 | LED 蓝色闪烁 | `FPS: 100, Roll: xx, Pitch: xx` |
| PCA9555 | LED 流水灯效果 | `PCA9555 初始化成功` |
| SD 卡 | 蜂鸣器提示音 | 文件列表和测试结果 |
| 蜂鸣器 | 播放音乐 | 当前播放曲目名称 |

---

## 📚 使用文档

### 核心文档

| 文档 | 说明 |
|------|------|
| [LVGL使用指南.md](test/LVGL使用指南.md) | LVGL 图形库完整教程，从入门到进阶 |
| [PCA9555使用指南.md](test/PCA9555使用指南.md) | GPIO 扩展器详细使用说明和示例 |
| [ESP-NOW使用说明.md](test/ESP-NOW使用说明.md) | 无线通信配置和调试指南 |
| [HiPNUC-IMU使用说明.md](test/HiPNUC-IMU使用说明.md) | IMU 传感器通信协议和配置 |
| [软串口性能分析.md](test/软串口性能分析.md) | 串口性能测试和优化方案 |
| [软串口频率优化说明.md](test/软串口频率优化说明.md) | 高频数据采集优化指南 |

### 示例代码

所有演示程序位于 `test/` 目录：

```
test/
├── lvgl_demo.cpp                        # LVGL 完整 UI 演示
├── pca9555_spi_cs_demo.cpp             # PCA9555 基础演示
├── pca9555_spi_integration.cpp         # PCA9555 高级集成
├── sdcard_demo.cpp                      # SD 卡读写演示
├── buzzer_demo.cpp                      # 蜂鸣器音乐播放
├── espnow_communication.cpp             # ESP-NOW 通信
├── 编码器数据轮询读取备份              # Modbus 轮询模式
├── 编码器数据批量快速读取备份          # Modbus 批量模式
└── *.md                                 # 各模块详细文档
```

---

## 🎯 典型应用场景

### 1. 移动机器人控制
- **编码器**: 实时读取轮速和位置
- **IMU**: 姿态稳定和导航
- **ESP-NOW**: 远程控制指令传输
- **LVGL**: 实时状态显示和参数调节

### 2. 数据采集系统
- **多传感器**: 同时采集编码器、IMU、气压等数据
- **SD 卡**: 高速数据记录和离线分析
- **GPIO 扩展**: 连接更多传感器和执行器
- **蜂鸣器**: 采集完成或异常提示

### 3. 智能设备控制面板
- **LVGL**: 触摸屏 UI 界面
- **按键**: 物理按键交互
- **LED**: 状态指示和氛围灯
- **蜂鸣器**: 操作反馈音效

### 4. 物联网网关
- **ESP-NOW**: 本地设备组网
- **WiFi**: 云端数据上传
- **SD 卡**: 本地数据缓存
- **GPIO 扩展**: 连接多个外设

---

## 📊 性能指标

| 指标 | 数值 | 说明 |
|------|------|------|
| **IMU 采集频率** | 100 Hz | 硬件串口稳定输出 |
| **Modbus 更新率** | 100+ Hz | 4 个编码器批量读取 |
| **ESP-NOW 延迟** | <10 ms | 点对点传输 |
| **LVGL 帧率** | 30-60 FPS | 根据缓冲区配置 |
| **SD 卡写入速度** | 500+ KB/s | SdFat 库优化 |
| **GPIO 扩展** | 16 路 | PCA9555 I2C 控制 |
| **内存占用** | <200 KB | LVGL + 双缓冲 |

---

## 💡 LED 状态指示说明

WS2812B RGB LED 通过不同颜色指示系统状态：

| 颜色 | 含义 | 触发场景 |
|------|------|----------|
| 🟠 **橙色** | 系统初始化 | 启动阶段 |
| 🔵 **蓝色** | 系统就绪 | 初始化完成，等待操作 |
| 🟢 **绿色** | 通信正常 | 所有设备响应正常 |
| 🔴 **红色** | 通信异常 | 设备无响应或数据错误 |
| 🟣 **紫色** | 仅接收模式 | ESP-NOW 未配置对方 MAC |
| 🟡 **黄色** | 警告状态 | 部分功能异常 |
| ⚪ **白色** | 测试模式 | 硬件测试或演示模式 |

---

## 🛠️ 故障排查
---

## 🛠️ 故障排查

### 通用问题

#### 编译错误
```bash
# 清理缓存重新编译
platformio run --target clean
platformio run

# 更新库依赖
platformio lib update
```

#### 上传失败
1. 检查 USB 连接和驱动（CH340/CP2102）
2. 按住 BOOT 按键再上传（部分开发板需要）
3. 确认串口号正确：`platformio device list`

### 模块特定问题

#### 📡 Modbus 编码器
**现象**: LED 显示红色，无数据输出

**排查步骤**:
1. 检查 RS485 接线：A-A, B-B, GND-GND
2. 确认编码器波特率为 115200
3. 验证设备地址与代码配置一致
4. 检查终端电阻（120Ω）是否安装
5. 使用 Modbus Poll 工具单独测试编码器

**解决方案**:
```cpp
// 在代码中调整超时时间
#define MODBUS_TIMEOUT_MS 100  // 增大超时值

// 降低通信频率
if (millis() - last_output_time >= 50) {  // 改为 50ms
```

#### 📶 ESP-NOW 通信
**现象**: LED 紫色，发送失败

**排查步骤**:
1. 确认两个设备都运行 ESP-NOW 程序
2. 检查对方 MAC 地址是否正确
3. 将设备靠近测试（<5 米）
4. 确认两个设备在同一信道

**解决方案**:
```cpp
// 获取本机 MAC 地址
Serial.println(WiFi.macAddress());

// 在对方设备中填写此 MAC 地址
uint8_t peerMAC[] = {0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX};
```

📖 详见：[ESP-NOW使用说明.md](test/ESP-NOW使用说明.md)

#### 🧭 HiPNUC IMU
**现象**: FPS 为 0，无数据输出

**排查步骤**:
1. 检查 IMU 供电（5V 或 3.3V）
2. 确认 TX/RX 引脚连接：IMU TX → GPIO16, IMU RX → GPIO17
3. 验证 IMU 波特率为 115200
4. 使用 IMU 配置软件检查输出设置

**解决方案**:
```cpp
// 检查串口初始化
Serial2.begin(115200, SERIAL_8N1, 16, 17);

// 启用调试输出
#define DEBUG_IMU_RAW_DATA
```

📖 详见：[HiPNUC-IMU使用说明.md](test/HiPNUC-IMU使用说明.md)

#### 🎨 LVGL 显示
**现象**: 白屏或花屏

**排查步骤**:
1. 检查 TFT_eSPI 引脚配置
2. 确认屏幕分辨率设置正确
3. 尝试切换字节序

**解决方案**:
```cpp
// 在 lv_conf.h 中调整
#define LV_COLOR_16_SWAP 1  // 切换为 0 或 1

// 在 lvgl_demo.cpp 中调整分辨率
static const uint16_t SCREEN_WIDTH = 320;   // 你的屏幕宽度
static const uint16_t SCREEN_HEIGHT = 240;  // 你的屏幕高度
```

**现象**: FPS 低，卡顿

**解决方案**:
```cpp
// 增大缓冲区
static const uint16_t BUF_SIZE = SCREEN_WIDTH * 60;

// 降低刷新率
#define LV_REFR_PERIOD 50  // 50ms = 20 FPS
```

📖 详见：[LVGL使用指南.md](test/LVGL使用指南.md)

#### 💾 SD 卡
**现象**: 初始化失败

**排查步骤**:
1. 检查 SD 卡格式（FAT32）
2. 确认 SPI 引脚连接正确
3. 检查 SD 卡电压（需要 3.3V）
4. 尝试更换 SD 卡（部分卡不兼容）

**解决方案**:
```cpp
// 降低 SPI 速度
if (!SD.begin(SD_CS_PIN, SD_SCK_MHZ(10))) {  // 从 40 降至 10
```

#### 🔌 PCA9555
**现象**: I2C 通信失败

**排查步骤**:
1. 检查 I2C 地址（默认 0x20）
2. 确认 SDA/SCL 上拉电阻（4.7kΩ）
3. 扫描 I2C 总线查找设备

**解决方案**:
```cpp
// I2C 扫描代码
Wire.begin();
for (uint8_t addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
        Serial.printf("Found device at 0x%02X\n", addr);
    }
}
```

📖 详见：[PCA9555使用指南.md](test/PCA9555使用指南.md)

---

## 📂 项目结构

```
ESP32PicoCurrentRobotics/
├── src/
│   ├── main.cpp                          # 当前运行的主程序
│   └── hipnuc_dec.c                      # HiPNUC 协议解码库
├── include/
│   ├── hipnuc_dec.h                      # HiPNUC 协议头文件
│   ├── lv_conf.h                         # LVGL 配置文件
│   └── README                            # include 说明
├── test/                                 # 所有演示程序和文档
│   ├── lvgl_demo.cpp                     # ⭐ LVGL 完整 UI 演示
│   ├── pca9555_spi_cs_demo.cpp          # PCA9555 基础演示
│   ├── pca9555_spi_integration.cpp      # PCA9555 SPI 集成
│   ├── sdcard_demo.cpp                   # SD 卡读写演示
│   ├── buzzer_demo.cpp                   # 蜂鸣器音效演示
│   ├── espnow_communication.cpp          # ESP-NOW 通信
│   ├── 编码器数据轮询读取备份           # Modbus 轮询模式
│   ├── 编码器数据批量快速读取备份       # Modbus 批量模式
│   ├── LVGL使用指南.md                   # 📖 LVGL 完整教程
│   ├── PCA9555使用指南.md                # 📖 GPIO 扩展器指南
│   ├── ESP-NOW使用说明.md                # 📖 无线通信配置
│   ├── HiPNUC-IMU使用说明.md             # 📖 IMU 传感器说明
│   ├── 软串口性能分析.md                 # 📖 性能测试报告
│   ├── 软串口频率优化说明.md             # 📖 优化指南
│   └── README                            # test 文件夹说明
├── lib/                                  # 自定义库（当前为空）
├── platformio.ini                        # ⚙️ PlatformIO 配置
├── README.md                             # 📚 本文件
└── LICENSE                               # MIT 许可证
```

---

## 🤝 贡献指南

欢迎提交 Issue 和 Pull Request！

### 贡献方式

1. **报告问题**: 在 [Issues](https://github.com/Lijinzh/ESP32PicoCurrentRobotics/issues) 提交 Bug 或功能请求
2. **改进文档**: 完善文档、修正错别字、添加示例
3. **提交代码**: Fork 项目，创建分支，提交 PR

### 代码规范

- 使用中文注释
- 遵循现有代码风格
- 添加必要的文档说明
- 测试通过后再提交

---

## 📝 开发路线图

### 已完成 ✅
- [x] Modbus RTU 高速编码器采集
- [x] ESP-NOW 点对点无线通信
- [x] HiPNUC IMU 姿态传感器集成
- [x] LVGL 图形界面系统
- [x] PCA9555 GPIO 扩展器驱动
- [x] SD 卡高性能存储
- [x] 蜂鸣器音效控制
- [x] 完整中文文档

### 计划中 🚧
- [ ] WiFi Web 配置界面
- [ ] MQTT 物联网云端通信
- [ ] 蓝牙 BLE 控制
- [ ] 多传感器数据融合
- [ ] 机器学习边缘计算
- [ ] OTA 无线固件更新

### 欢迎建议 💡
在 [Issues](https://github.com/Lijinzh/ESP32PicoCurrentRobotics/issues) 中提出你的想法！

---

## 📜 许可证

本项目采用 **MIT License** 开源协议。

```
MIT License

Copyright (c) 2026 Lijinzh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 📧 联系方式

- **GitHub**: [Lijinzh](https://github.com/Lijinzh)
- **Issues**: [提交问题](https://github.com/Lijinzh/ESP32PicoCurrentRobotics/issues)
- **Discussions**: [参与讨论](https://github.com/Lijinzh/ESP32PicoCurrentRobotics/discussions)

---

## 🙏 致谢

感谢以下开源项目：

- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [PlatformIO](https://platformio.org/)
- [LVGL](https://lvgl.io/) - 专业嵌入式图形库
- [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)
- [FastLED](https://github.com/FastLED/FastLED)
- [SdFat](https://github.com/greiman/SdFat)
- [modbus-esp8266](https://github.com/emelianov/modbus-esp8266)

---

## ⚠️ 免责声明

本项目仅供学习和研究使用。在实际应用中，请：

- ✅ 充分测试所有功能
- ✅ 添加必要的安全保护措施
- ✅ 遵守当地法律法规
- ✅ 注意电气安全和设备保护

作者不对使用本项目造成的任何损失负责。

---

<div align="center">

**⭐ 如果这个项目对你有帮助，请给个 Star！⭐**

Made with ❤️ by [Lijinzh](https://github.com/Lijinzh)

[返回顶部](#esp32-pico-d4-机器人开发平台)

</div>
