/**
 * @file buzzer_demo.cpp
 * @brief ezBuzzer蜂鸣器库演示 - 简单易用的蜂鸣器控制
 * @note 支持单音、旋律播放、非阻塞模式
 *
 * 硬件连接：
 * - 蜂鸣器正极 -> ESP32 GPIO 2
 * - 蜂鸣器负极 -> GND
 */

#include <Arduino.h>
#include <ezBuzzer.h>

// ==================== 引脚配置 ====================
#define BUZZER_PIN 2

// ==================== 全局对象 ====================
ezBuzzer buzzer(BUZZER_PIN);

// ==================== 音符频率定义 ====================
// 基于标准音高，频率单位Hz
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_G5 784
#define NOTE_A5 880
#define NOTE_B5 988
#define NOTE_C6 1047

#define REST 0 // 休止符

// ==================== 旋律定义 ====================

// 超级玛丽主题曲（简化版）
int melody_mario[] = {
    NOTE_E5, NOTE_E5, REST, NOTE_E5, REST, NOTE_C5, NOTE_E5, REST,
    NOTE_G5, REST, REST, REST, NOTE_G4, REST, REST, REST};
int noteDurations_mario[] = {
    8, 8, 8, 8, 8, 8, 8, 8,
    4, 8, 8, 8, 4, 8, 8, 8};

// 生日快乐歌
int melody_birthday[] = {
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,
    NOTE_C4, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
    NOTE_C4, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_E4, NOTE_D4,
    NOTE_B4, NOTE_B4, NOTE_A4, NOTE_F4, NOTE_G4, NOTE_F4};
int noteDurations_birthday[] = {
    8, 8, 4, 4, 4, 2,
    8, 8, 4, 4, 4, 2,
    8, 8, 4, 4, 4, 4, 4,
    8, 8, 4, 4, 4, 2};

// 两只老虎
int melody_tiger[] = {
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4,
    NOTE_C4, NOTE_D4, NOTE_E4, NOTE_C4,
    NOTE_E4, NOTE_F4, NOTE_G4,
    NOTE_E4, NOTE_F4, NOTE_G4};
int noteDurations_tiger[] = {
    4, 4, 4, 4,
    4, 4, 4, 4,
    4, 4, 2,
    4, 4, 2};

// 小星星
int melody_twinkle[] = {
    NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4,
    NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4};
int noteDurations_twinkle[] = {
    4, 4, 4, 4, 4, 4, 2,
    4, 4, 4, 4, 4, 4, 2};

// ==================== 提示音效 ====================

// 开机提示音
void playStartupSound()
{
    int melody[] = {NOTE_C5, NOTE_E5, NOTE_G5};
    for (int i = 0; i < 3; i++)
    {
        buzzer.beep(melody[i], 100);
        delay(150);
    }
}

// 成功提示音
void playSuccessSound()
{
    buzzer.beep(NOTE_G5, 100);
    delay(100);
    buzzer.beep(NOTE_C6, 200);
}

// 错误提示音
void playErrorSound()
{
    buzzer.beep(NOTE_A4, 100);
    delay(100);
    buzzer.beep(NOTE_F4, 100);
    delay(100);
    buzzer.beep(NOTE_C4, 200);
}

// 按键提示音
void playClickSound()
{
    buzzer.beep(NOTE_C5, 50);
}

// 警报音
void playAlarmSound()
{
    for (int i = 0; i < 3; i++)
    {
        buzzer.beep(1000, 200);
        delay(200);
        buzzer.beep(800, 200);
        delay(200);
    }
}

// SOS求救信号 (摩斯码: ... --- ...)
void playSOSSound()
{
    // S: 三短音
    for (int i = 0; i < 3; i++)
    {
        buzzer.beep(1000, 100);
        delay(200);
    }
    delay(300);

    // O: 三长音
    for (int i = 0; i < 3; i++)
    {
        buzzer.beep(1000, 300);
        delay(200);
    }
    delay(300);

    // S: 三短音
    for (int i = 0; i < 3; i++)
    {
        buzzer.beep(1000, 100);
        delay(200);
    }
}

// ==================== 旋律播放函数 ====================

void playMelody(int *melody, int *durations, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (melody[i] == REST)
        {
            delay(1000 / durations[i]);
        }
        else
        {
            int duration = 1000 / durations[i];
            buzzer.beep(melody[i], duration * 0.9); // 0.9是为了让音符之间有短暂间隔
            delay(duration);
        }
    }
}

// ==================== 菜单函数 ====================

void printMenu()
{
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║      ezBuzzer 蜂鸣器演示程序          ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println("\n【提示音效】");
    Serial.println("  1 - 开机提示音");
    Serial.println("  2 - 成功提示音");
    Serial.println("  3 - 错误提示音");
    Serial.println("  4 - 按键提示音");
    Serial.println("  5 - 警报音");
    Serial.println("  6 - SOS求救信号");
    Serial.println("\n【旋律演奏】");
    Serial.println("  a - 超级玛丽主题曲");
    Serial.println("  b - 生日快乐歌");
    Serial.println("  c - 两只老虎");
    Serial.println("  d - 小星星");
    Serial.println("\n【音阶测试】");
    Serial.println("  s - 播放音阶 (C4-C5)");
    Serial.println("  t - 频率扫描 (100Hz-2000Hz)");
    Serial.println("\n【其他】");
    Serial.println("  h - 显示此帮助");
    Serial.println("  x - 停止播放");
    Serial.println("========================================");
}

void playScale()
{
    Serial.println("播放音阶: C4 D4 E4 F4 G4 A4 B4 C5");
    int scale[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5};
    for (int i = 0; i < 8; i++)
    {
        buzzer.beep(scale[i], 300);
        delay(400);
    }
}

void frequencySweep()
{
    Serial.println("频率扫描: 100Hz -> 2000Hz");
    for (int freq = 100; freq <= 2000; freq += 50)
    {
        buzzer.beep(freq, 50);
        delay(60);
    }
}

// ==================== Setup ====================
void setup()
{
    delay(500);
    Serial.begin(115200);
    Serial.println("\n\n");

    // ezBuzzer库不需要特殊初始化，创建对象时已完成

    playStartupSound();

    Serial.println("✓ 蜂鸣器初始化完成");
    Serial.println("\n提示：输入命令字符后按Enter");
    printMenu();
}

// ==================== Loop ====================
void loop()
{
    // 必须调用loop()以支持非阻塞播放
    buzzer.loop();

    if (Serial.available())
    {
        char cmd = Serial.read();
        // 清空缓冲区
        while (Serial.available())
            Serial.read();

        Serial.printf("\n>>> 执行命令: %c\n", cmd);

        switch (cmd)
        {
        case '1':
            Serial.println("▶ 开机提示音");
            playStartupSound();
            break;

        case '2':
            Serial.println("▶ 成功提示音");
            playSuccessSound();
            break;

        case '3':
            Serial.println("▶ 错误提示音");
            playErrorSound();
            break;

        case '4':
            Serial.println("▶ 按键提示音");
            playClickSound();
            break;

        case '5':
            Serial.println("▶ 警报音");
            playAlarmSound();
            break;

        case '6':
            Serial.println("▶ SOS求救信号");
            playSOSSound();
            break;

        case 'a':
        case 'A':
            Serial.println("♪ 超级玛丽主题曲");
            playMelody(melody_mario, noteDurations_mario,
                       sizeof(melody_mario) / sizeof(melody_mario[0]));
            break;

        case 'b':
        case 'B':
            Serial.println("♪ 生日快乐歌");
            playMelody(melody_birthday, noteDurations_birthday,
                       sizeof(melody_birthday) / sizeof(melody_birthday[0]));
            break;

        case 'c':
        case 'C':
            Serial.println("♪ 两只老虎");
            playMelody(melody_tiger, noteDurations_tiger,
                       sizeof(melody_tiger) / sizeof(melody_tiger[0]));
            break;

        case 'd':
        case 'D':
            Serial.println("♪ 小星星");
            playMelody(melody_twinkle, noteDurations_twinkle,
                       sizeof(melody_twinkle) / sizeof(melody_twinkle[0]));
            break;

        case 's':
        case 'S':
            playScale();
            break;

        case 't':
        case 'T':
            frequencySweep();
            break;

        case 'x':
        case 'X':
            Serial.println("⏹ 停止播放");
            buzzer.stop();
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
