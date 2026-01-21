/**
 * @file main.cpp
 * @brief Multi-encoder communication - Serial Studio compatible output with frequency monitoring
 * @note Serial Studio Frame Format: /*$1,angle1,angle2,angle3,angle4*\/
 */

#include <Arduino.h>
#include <ModbusRTU.h>
#include <FastLED.h>

// ================= Pin Configuration =================
#define RS485_RX_PIN 32     // Module RO
#define RS485_TX_PIN 33     // Module DI
#define RS485_DE_RE_PIN 25  // Module DE/RE Control

#define WS2812_PIN 26       // WS2812 Data Pin
#define NUM_LEDS 1          // Number of LEDs
#define BUZZER_PIN 2        // Passive Buzzer Pin

// ================= Encoder Configuration =================
#define NUM_ENCODERS 4      // 编码器数量（根据实际情况修改）
const uint8_t ENCODER_IDS[NUM_ENCODERS] = {1, 2, 3, 5}; // 各编码器的Modbus从站ID

// ================= Serial Studio Configuration =================
#define FRAME_START "/*"    // 帧起始标记
#define FRAME_END "*/"      // 帧结束标记
#define OUTPUT_INTERVAL 10  // 数据输出间隔(ms)

// ================= Frequency Monitoring =================
unsigned long last_cycle_time = 0;      // 上次完成一轮采集的时间
unsigned long cycle_count = 0;          // 完成的采集轮数
unsigned long freq_calc_start = 0;      // 频率计算开始时间
const uint32_t FREQ_REPORT_INTERVAL = 1000; // 每5秒报告一次频率

// ================= Object Instantiation =================
ModbusRTU mb;
CRGB leds[NUM_LEDS];

// 存储每个编码器的数据
uint16_t encoder_raw_values[NUM_ENCODERS];
float encoder_angles[NUM_ENCODERS];
bool encoder_status[NUM_ENCODERS];

uint8_t current_encoder_index = 0;
unsigned long last_request_time = 0;
unsigned long last_output_time = 0;
unsigned long last_freq_report_time = 0;
const uint32_t POLL_INTERVAL = 1; // Modbus轮询间隔(ms)

bool request_pending = false;
bool data_ready = false; // 一轮数据采集完成标志

// ================= Buzzer Functions =================
void playStartupSound() {
    int melody[] = {1000, 1500, 2000};
    for (int i = 0; i < 3; i++) {
        tone(BUZZER_PIN, melody[i]);
        delay(100);
        noTone(BUZZER_PIN);
        delay(50);
    }
}

// ================= Frequency Calculation =================
void reportFrequency() {
    if (freq_calc_start > 0 && cycle_count > 0) {
        unsigned long elapsed_time = millis() - freq_calc_start;
        float frequency = (float)cycle_count * 1000.0 / elapsed_time; // Hz

        // 输出频率信息到Serial（注释格式，不影响CSV数据）
        Serial.print("# Update Frequency: ");
        Serial.print(frequency, 2);
        Serial.print(" Hz (");
        Serial.print(cycle_count);
        Serial.print(" cycles in ");
        Serial.print(elapsed_time);
        Serial.println(" ms)");

        // 重置计数器
        cycle_count = 0;
        freq_calc_start = millis();
    }
}

// ================= Serial Studio 数据输出 =================
void outputSerialStudioFrame() {
    // 格式: /*$1,angle1,angle2,angle3,angle4*/
    // $1 表示使用第1个数据帧映射
    Serial.print(FRAME_START);
    Serial.print("$1");

    for (int i = 0; i < NUM_ENCODERS; i++) {
        Serial.print(",");
        if (encoder_status[i]) {
            Serial.print(encoder_angles[i], 2); // 保留2位小数
        } else {
            Serial.print("0.00"); // 通信失败时输出0
        }
    }

    // 可选：添加原始值用于调试
    // for (int i = 0; i < NUM_ENCODERS; i++) {
    //     Serial.print(",");
    //     Serial.print(encoder_raw_values[i]);
    // }

    Serial.println(FRAME_END);
}

// 输出简化CSV格式（无帧标记，用于简单绘图）
void outputSimpleCSV() {
    for (int i = 0; i < NUM_ENCODERS; i++) {
        if (i > 0) Serial.print(",");
        Serial.print(encoder_angles[i], 2);
    }
    Serial.println();
}

// ================= Modbus Callback =================
bool onModbusFinished(Modbus::ResultCode event, uint16_t transactionId, void *data) {
    uint8_t idx = current_encoder_index;

    if (event == Modbus::EX_SUCCESS) {
        encoder_status[idx] = true;
        encoder_angles[idx] = (encoder_raw_values[idx] * 360.0) / 65536.0;
    } else {
        encoder_status[idx] = false;
        encoder_angles[idx] = 0.0;
    }

    // 切换到下一个编码器
    current_encoder_index++;
    if (current_encoder_index >= NUM_ENCODERS) {
        current_encoder_index = 0;
        data_ready = true; // 一轮采集完成

        // 频率统计
        unsigned long current_time = millis();
        if (last_cycle_time > 0) {
            cycle_count++;
            if (freq_calc_start == 0) {
                freq_calc_start = current_time;
            }
        }
        last_cycle_time = current_time;
    }

    // 更新LED状态
    bool all_ok = true;
    for (int i = 0; i < NUM_ENCODERS; i++) {
        if (!encoder_status[i]) all_ok = false;
    }
    leds[0] = all_ok ? CRGB::Green : CRGB::Red;
    FastLED.show();

    request_pending = false;
    return true;
}

void setup() {
    delay(500);
    Serial.begin(115200);

    // Hardware Initialization
    pinMode(BUZZER_PIN, OUTPUT);
    FastLED.addLeds<WS2812B, WS2812_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(50);

    playStartupSound();

    // Initialize encoder data
    for (int i = 0; i < NUM_ENCODERS; i++) {
        encoder_raw_values[i] = 0;
        encoder_angles[i] = 0.0;
        encoder_status[i] = false;
    }

    // Initialize RS-485
    Serial2.begin(115200, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    mb.begin(&Serial2, RS485_DE_RE_PIN);
    mb.master();

    leds[0] = CRGB::Blue;
    FastLED.show();

    // 输出Serial Studio JSON配置提示
    Serial.println("# Serial Studio Ready - Frequency monitoring enabled");

    // 初始化频率计算
    freq_calc_start = 0;
    cycle_count = 0;
    last_freq_report_time = millis();
}

void loop() {
    mb.task();

    // Modbus轮询
    if (!request_pending && (millis() - last_request_time > POLL_INTERVAL)) {
        last_request_time = millis();

        if (!mb.slave()) {
            uint8_t slave_id = ENCODER_IDS[current_encoder_index];
            request_pending = true;
            mb.readHreg(slave_id, 1, &encoder_raw_values[current_encoder_index], 1, onModbusFinished);
        }
    }

    // 定时输出数据到Serial Studio
    if (millis() - last_output_time >= OUTPUT_INTERVAL) {
        last_output_time = millis();
        // outputSerialStudioFrame(); // 使用帧格式
        outputSimpleCSV(); // 或使用简单CSV格式
    }

    // 定时报告频率
    if (millis() - last_freq_report_time >= FREQ_REPORT_INTERVAL) {
        last_freq_report_time = millis();
        reportFrequency();
    }
}