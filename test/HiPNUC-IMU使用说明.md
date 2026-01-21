# 超核电子 IMU 通信使用说明

## 📝 简介

本代码用于ESP32与超核电子IMU传感器进行串口通信，支持HiPNUC协议的多种数据包格式。

## 🔌 硬件连接

| IMU引脚 | ESP32引脚 | 说明 |
|---------|-----------|------|
| TX | GPIO 16 (RX) | IMU数据输出 |
| RX | GPIO 17 (TX) | IMU配置输入（可选）|
| GND | GND | 地线 |
| VCC | 3.3V/5V | 电源（根据IMU型号选择）|

## 📦 支持的数据包类型

### 1. 0x91 包 - IMU数据
包含：
- 加速度计数据（m/s²）
- 陀螺仪数据（°/s）
- 磁力计数据（μT）
- 姿态角（Roll/Pitch/Yaw，度）
- 四元数
- 气压、温度

### 2. 0x81 包 - INS数据
包含：
- GPS位置（经纬度、海拔）
- 速度（ENU坐标系）
- 姿态角（度）
- GNSS状态信息
- UTC时间

### 3. 0x83 包 - 灵活数据包
根据data_bitmap动态包含不同数据字段

## 🚀 使用方法

### 1. 上传代码

```bash
# 方法1：直接替换main.cpp
cp test/hipnuc_imu_reader.cpp src/main.cpp
pio run --target upload

# 方法2：查看串口输出
pio device monitor -b 115200
```

### 2. 串口命令

打开串口监视器（115200波特率），可使用以下命令：

| 命令 | 功能 |
|------|------|
| `d` | 显示详细数据（JSON格式）|
| `i` | 显示系统信息 |
| `s` | 显示统计信息（FPS、运行时间等）|
| `r` | 重启ESP32 |
| `h` | 显示帮助信息 |

## 📊 输出格式

### 紧凑格式（默认，10Hz更新）

```
[50.2 Hz | 12.3s] IMU: Roll= 10.23° Pitch=  5.67° Yaw=180.45° | Acc=[ -0.12,  0.05,  9.81]m/s² | Gyr=[  0.5,  1.2, -0.3]°/s
```

### 详细格式（输入命令'd'查看）

```json
{
  "type": "HI91",
  "main_status": [0x0],
  "system_time": 12345,
  "acc": [0.012, -0.005, 1.001],
  "gyr": [0.5, 1.2, -0.3],
  "mag": [12.3, -5.6, 45.2],
  "pitch": 5.67,
  "roll": 10.23,
  "yaw": 180.45,
  "quat": [0.998, 0.012, -0.005, 0.050],
  "air_pressure": 101325.0
}
```

## 💡 LED状态指示

| 颜色 | 状态 |
|------|------|
| 🟠 橙色 | 系统初始化中 |
| 🔵 蓝色 | 等待IMU数据 |
| 🟢 绿色 | 正在接收IMU数据(0x91) |
| 🔵 青色 | 正在接收INS数据(0x81) |
| 🔴 红色 | 错误/无数据 |

## 🔧 参数配置

### 修改串口引脚

在代码中修改：
```cpp
#define IMU_RX_PIN 16      // 改为你的RX引脚
#define IMU_TX_PIN 17      // 改为你的TX引脚
```

### 修改波特率

```cpp
#define IMU_BAUDRATE 115200  // 改为IMU的实际波特率
```

### 修改显示频率

```cpp
const int DISPLAY_INTERVAL = 100;  // 100ms = 10Hz，可改为200(5Hz)或50(20Hz)
```

## 🛠️ 故障排查

### 问题1: 无数据输出（LED显示红色）

**可能原因：**
- IMU未上电或未正确连接
- 波特率不匹配
- TX/RX接线反了
- IMU未配置为输出模式

**解决方法：**
1. 检查硬件连接
2. 确认IMU波特率为115200
3. 交换TX/RX连接试试
4. 使用IMU配置软件确认输出已开启

### 问题2: FPS很低（<10Hz）

**可能原因：**
- IMU输出频率设置过低
- 串口波特率不够
- 数据包过大

**解决方法：**
1. 使用IMU配置软件提高输出频率
2. 检查IMU配置的数据包类型

### 问题3: 数据解析错误

**可能原因：**
- CRC校验失败
- 数据包损坏

**解决方法：**
1. 检查接线是否松动
2. 缩短连接线长度
3. 添加电源滤波电容

## 📚 HiPNUC协议说明

### 数据帧格式

```
[0x5A][0xA5][LEN_L][LEN_H][CRC_L][CRC_H][PAYLOAD...]
```

- 同步头: 0x5A 0xA5
- 长度: 2字节（Payload长度）
- CRC: 2字节校验和
- Payload: 实际数据内容

### CRC计算

使用CRC-16/CCITT算法，多项式：0x1021

## 🔗 相关文件

- `src/hipnuc_dec.c` - HiPNUC协议解码库
- `include/hipnuc_dec.h` - 协议头文件
- `test/hipnuc_imu_reader.cpp` - ESP32通信代码

## 💡 进阶应用

### 结合ESP-NOW无线传输

可以将IMU数据通过ESP-NOW发送到另一个ESP32：

```cpp
// 在接收到IMU数据后
if (hipnuc_input(&hipnuc_raw, data) > 0) {
    // 将姿态角数据打包
    sendData.sensor_data[0] = hipnuc_raw.hi91.roll;
    sendData.sensor_data[1] = hipnuc_raw.hi91.pitch;
    sendData.sensor_data[2] = hipnuc_raw.hi91.yaw;
    
    // 通过ESP-NOW发送
    esp_now_send(peerMAC, (uint8_t*)&sendData, sizeof(sendData));
}
```

### 结合编码器数据

可以同时读取编码器和IMU数据，实现完整的机器人状态监测。

## 📧 技术支持

- HiPNUC官网: https://www.hipnuc.com
- 产品手册: 参考IMU产品说明书
- GitHub Issues: 本项目问题反馈

---

**注意**: 确保IMU已正确配置输出模式和波特率，否则无法接收数据。
