# IMU软串口版本使用说明

## 📋 版本说明

本项目现在包含两个IMU通信版本：

### 1. **硬件串口版本** 
- 文件：`test/hipnuc_imu_hardware_serial.cpp`
- 使用：ESP32 硬件串口2 (Serial2)
- 引脚：GPIO16(RX), GPIO17(TX)
- 限制：只能连接1个IMU（ESP32硬件串口有限）
- 优点：稳定性更好，速度更快

### 2. **软串口版本** (当前)
- 文件：`src/main.cpp`
- 使用：EspSoftwareSerial 库
- 引脚：GPIO16(RX), GPIO17(TX) 
- 优势：**可扩展到4个IMU同时接入**
- 依赖：`plerup/EspSoftwareSerial @ ^8.2.0`

---

## 🔌 软串口版本硬件连接

### 单个IMU连接
```
IMU #1:
  TX  -> ESP32 GPIO16 (软串口RX)
  RX  -> ESP32 GPIO17 (软串口TX)
  GND -> ESP32 GND
  VCC -> ESP32 3.3V/5V
```

### 多IMU扩展方案 (预留)
后续可扩展到4个IMU，每个使用不同的GPIO对：
```
IMU #1: GPIO16/17 (当前)
IMU #2: GPIO25/26 (待添加)
IMU #3: GPIO32/33 (待添加)
IMU #4: GPIO13/14 (待添加)
```

---

## 📝 主要代码变化

### 1. 添加软串口库依赖
```ini
lib_deps = 
    plerup/EspSoftwareSerial @ ^8.2.0
```

### 2. 引入头文件
```cpp
#include <SoftwareSerial.h>
```

### 3. 创建软串口对象
```cpp
SoftwareSerial imuSerial(IMU_RX_PIN, IMU_TX_PIN);
```

### 4. 初始化软串口
```cpp
// setup()中
imuSerial.begin(IMU_BAUDRATE);
```

### 5. 读取数据
```cpp
// loop()中
while (imuSerial.available()) {
    uint8_t data = imuSerial.read();
    // 处理数据...
}
```

---

## 🚀 测试步骤

### 1. 上传代码
```bash
pio run --target upload
```

### 2. 打开串口监视器
```bash
pio device monitor
```

### 3. 观察输出
应该看到类似输出：
```
软串口初始化: RX=16, TX=17, Baud=115200
╔════════════════════════════════════════╗
║    超核电子 IMU 数据采集系统          ║
╚════════════════════════════════════════╝

启动倒计时:
  3...
  2...
  1...

[50.0 Hz | 5.2s] IMU: Roll=-1.23° Pitch=2.45° Yaw=180.00° ...
```

---

## ⚙️ 性能对比

| 特性 | 硬件串口 | 软串口 |
|------|---------|--------|
| 最高波特率 | 921600 | ~115200 |
| 稳定性 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ |
| CPU占用 | 低 | 稍高 |
| 支持IMU数量 | 1个 | 最多4个 |
| 数据丢失率 | 极低 | 低 |

---

## 🔧 切换版本方法

### 切换到硬件串口版本
```bash
cp test/hipnuc_imu_hardware_serial.cpp src/main.cpp
pio run --target upload
```

### 切换到软串口版本
```bash
# 当前已经是软串口版本，无需操作
```

---

## 💡 注意事项

### 软串口使用建议
1. **波特率限制**：建议使用115200，不要超过115200
2. **引脚选择**：避免使用GPIO6-11（连接到Flash）
3. **中断影响**：WiFi等操作可能影响软串口稳定性
4. **数据监控**：观察FPS，应稳定在50Hz左右

### 故障排查
- **FPS为0**：检查接线，确认IMU电源正常
- **数据不稳定**：降低波特率到9600测试
- **乱码**：检查波特率设置是否一致
- **频繁断连**：检查是否有WiFi等高优先级任务干扰

---

## 📚 后续扩展

### 添加第二个IMU
1. 定义新引脚：
```cpp
#define IMU2_RX_PIN 25
#define IMU2_TX_PIN 26
```

2. 创建第二个软串口：
```cpp
SoftwareSerial imuSerial2(IMU2_RX_PIN, IMU2_TX_PIN);
```

3. 初始化和读取：
```cpp
imuSerial2.begin(115200);
// 轮询读取两个串口...
```

---

**版本**: v1.0 - 软串口版本  
**日期**: 2026-01-22  
**测试状态**: ✅ 编译通过，待硬件测试
