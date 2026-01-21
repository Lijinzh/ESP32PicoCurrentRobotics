# ESP32 Pico 机器人通信系统

基于ESP32-PICO-D4的多功能机器人通信系统，包含高性能Modbus RTU多编码器数据采集和ESP-NOW无线通信功能。

## 📋 项目简介

本项目提供了两种核心功能模块：

### 1. Modbus RTU 编码器数据采集
通过RS485总线与多个Modbus编码器进行高速通信，优化了数据采集流程，绕过了传统库的开销，通过紧凑的发送-接收循环实现了最高频率的数据更新。

### 2. ESP-NOW 无线通信
实现两个ESP32设备之间的点对点无线通信，无需路由器即可实现低延迟、高可靠的数据传输，适用于机器人编队协作等场景。

## ✨ 主要特性

### Modbus 编码器采集
- 🚀 **高速通信**: 115200波特率，支持100+Hz的数据更新频率
- 📡 **多编码器支持**: 同时读取多个Modbus RTU编码器（当前配置4个）
- 🎯 **批量处理模式**: 优化的批量读取流程，最小化通信延迟
- 📊 **CSV数据输出**: 实时输出角度数据，便于数据分析

### ESP-NOW 无线通信
- 📶 **点对点通信**: 无需WiFi路由器，直接设备间通信
- ⚡ **低延迟**: 典型延迟<10ms，适合实时控制
- 🔄 **双向传输**: 支持双向数据收发
- 📦 **灵活数据包**: 支持传输文本、传感器数据、时间戳等

### 通用特性
- 💡 **状态指示**: WS2812 LED实时显示运行状态
- 🔊 **声音反馈**: 蜂鸣器提供操作提示音
- 🛠️ **易于配置**: 通过宏定义快速切换功能模式

## 🔧 硬件配置

### 开发板
- **主控芯片**: ESP32-PICO-D4
- **CPU频率**: 240MHz
- **Flash频率**: 80MHz

### 引脚分配

| 功能 | 引脚 | 说明 |
|------|------|------|
| RS485 RX | GPIO 32 | 接收数据 |
| RS485 TX | GPIO 33 | 发送数据 |
| RS485 DE/RE | GPIO 25 | 收发控制 |
| WS2812 LED | GPIO 26 | 状态指示灯 |
| 蜂鸣器 | GPIO 2 | 声音提示 |

### 编码器配置
- **通信协议**: Modbus RTU
- **波特率**: 115200
- **数据位**: 8N1
- **设备地址**: 1, 2, 3, 5（可在代码中修改）
- **读取寄存器**: 地址 0x0001（角度数据）

## 📦 依赖库

```ini
- emelianov/modbus-esp8266 @ ^4.1.0
- fastled/FastLED @ ^3.10.3
```

## 🚀 快速开始

### 环境准备

1. 安装 [PlatformIO](https://platformio.org/)
2. 克隆本仓库：
   ```bash
   git clone https://github.com/Lijinzh/ESP32PicoCurrentRobotics.git
   cd ESP32PicoCurrentRobotics
   ```

### 选择功能模式

本项目包含多个功能模块，使用前需选择对应的代码：

#### 模式1: Modbus 编码器采集（默认）
```bash
# 恢复编码器批量快速读取代码
cp test/encoder_fast_batch_read_backup.cpp src/main.cpp
pio run --target upload
```

#### 模式2: ESP-NOW 无线通信
```bash
# 使用ESP-NOW通信代码
cp test/espnow_communication.cpp src/main.cpp
# 修改 DEVICE_1 宏定义和对方MAC地址后上传
pio run --target upload
```

详细的ESP-NOW配置说明请查看：[test/ESP-NOW使用说明.md](test/ESP-NOW使用说明.md)

### 编译与上传

```bash
# 编译项目
pio run

# 上传到开发板
pio run --target upload

# 打开串口监视器（115200波特率）
pio device monitor
```

### 配置说明

#### Modbus 编码器配置

在 `test/encoder_fast_batch_read_backup.cpp` 中可以修改以下参数：

```cpp
// 编码器数量
#define NUM_ENCODERS 4

// 编码器Modbus地址
const uint8_t ENCODER_IDS[NUM_ENCODERS] = {1, 2, 3, 5};

// 数据输出间隔（毫秒）
// 当前为10ms，即100Hz输出频率
if (millis() - last_output_time >= 10) {
    // ...
}
```

#### ESP-NOW 配置

在 `test/espnow_communication.cpp` 中修改设备角色和对方MAC地址：

```cpp
// 设备角色：设备1设为1，设备2设为0
#define DEVICE_1  1

// 对方设备的MAC地址
uint8_t peerMAC[] = {0xF0, 0x24, 0xF9, 0xB5, 0x90, 0x08};
```

## 📊 数据格式

### 串口输出

程序通过串口输出CSV格式的角度数据：

```
180.25,90.13,270.87,45.62
181.30,90.15,271.00,45.70
...
```

每行包含所有编码器的角度值（单位：度），范围 0-360°。

### LED状态指示

- 🟠 **橙色**: 系统初始化中
- 🔵 **蓝色**: 系统就绪
- 🟢 **绿色**: 所有编码器通信正常 / ESP-NOW发送成功
- 🔴 **红色**: 至少一个编码器通信异常 / ESP-NOW发送失败
- 🟣 **紫色**: ESP-NOW仅接收模式（对方MAC未配置）

## ⚡ 性能优化

本项目采用了多项优化措施以实现最高通信频率：

1. **预生成Modbus帧**: 启动时生成所有请求帧，避免运行时开销
2. **阻塞式读取**: 使用紧凑的发送-接收循环，消除异步等待延迟
3. **最小化超时**: Serial超时设置为5ms，快速检测通信失败
4. **硬件加速**: CPU 240MHz + QIO Flash模式
5. **高速烧录**: 2Mbps上传速度，快速迭代开发

## 📁 项目结构

```
ESP32PicoCurrentRobotics/
├── src/
│   └── main.cpp                              # 当前运行的主程序
├── test/
│   ├── encoder_fast_batch_read_backup.cpp    # 编码器批量快速读取模式
│   ├── encoder_polling_read_backup.cpp       # 编码器轮询读取模式
│   ├── espnow_communication.cpp              # ESP-NOW双向通信代码
│   ├── ESP-NOW使用说明.md                     # ESP-NOW详细使用指南
│   └── README                                # test文件夹说明
├── platformio.ini                            # PlatformIO配置文件
├── README.md                                 # 本文件
└── LICENSE                                   # MIT许可证

```

## 🛠️ 故障排查

### ESP-NOW 通信问题

#### 通信失败（LED显示红色）
- 解决：按照[ESP-NOW使用说明](test/ESP-NOW使用说明.md)正确配置MAC地址

## 🛠️ 故障排查

### Modbus 编码器问题

### 通信失败（LED显示红色）

1. 检查RS485接线是否正确
2. 确认编码器波特率为115200
3. 验证编码器Modbus地址与代码中配置一致
4. 检查RS485总线终端电阻

### 数据更新频率低

1. 减少编码器数量
2. 调整数据输出间隔
3. 检查串口是否有大量输出（降低Serial输出频率）

## 📝 许可证

本项目采用 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

## 📧 联系方式

如有问题或建议，请通过 GitHub Issues 联系。

---

**注意**: 本项目为机器人控制系统的一部分，请确保在安全环境下测试和使用。
