/**
 * @file lvgl_demo.cpp
 * @brief LVGL 完整功能演示程序 - ESP32-PICO-D4 + TFT_eSPI
 *
 * 功能特性:
 * - 按钮点击交互 (计数器)
 * - 滑块控制 LED 亮度
 * - 开关控制蜂鸣器
 * - 进度条动画展示
 * - 实时数据图表
 * - 标签动态更新
 * - 流畅的 UI 动画
 *
 * 硬件连接:
 * - TFT 显示屏: 按照 TFT_eSPI 配置连接
 * - LED: GPIO2 (可选)
 * - 蜂鸣器: 按照已有配置
 *
 * 使用方法:
 * 1. 将 src/main.cpp 内容替换为本文件
 * 2. 或在 main.cpp 中调用 lvgl_demo_setup() 和 lvgl_demo_loop()
 * 3. platformio run -t upload
 */

#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

/* ===========================
 *  全局变量
 * =========================== */

TFT_eSPI tft = TFT_eSPI(); // TFT 实例

// LVGL 显示缓冲区
static const uint16_t SCREEN_WIDTH = 320;           // 根据你的屏幕调整
static const uint16_t SCREEN_HEIGHT = 240;          // 根据你的屏幕调整
static const uint16_t BUF_SIZE = SCREEN_WIDTH * 40; // 缓冲区大小

static lv_color_t buf1[BUF_SIZE];
static lv_color_t buf2[BUF_SIZE];

// UI 对象指针
lv_obj_t *btn_counter;
lv_obj_t *label_counter;
lv_obj_t *slider_led;
lv_obj_t *label_slider;
lv_obj_t *switch_buzzer;
lv_obj_t *bar_progress;
lv_obj_t *chart_data;
lv_obj_t *label_status;

// 状态变量
uint32_t click_count = 0;
uint32_t progress_value = 0;
lv_chart_series_t *chart_series;

/* ===========================
 *  LVGL 显示驱动回调
 * =========================== */

/**
 * @brief LVGL 显示刷新回调函数
 */
void lvgl_display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t w = lv_area_get_width(area);
    uint32_t h = lv_area_get_height(area);

    // 设置显示窗口
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);

    // 推送像素数据
    tft.pushColors((uint16_t *)px_map, w * h, true);
    tft.endWrite();

    // 通知 LVGL 刷新完成
    lv_display_flush_ready(disp);
}

/**
 * @brief LVGL 触摸输入读取 (可选,如果有触摸屏)
 */
void lvgl_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    // 如果有触摸屏,在这里读取触摸坐标
    // uint16_t touchX, touchY;
    // bool touched = tft.getTouch(&touchX, &touchY);

    // data->state = touched ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    // data->point.x = touchX;
    // data->point.y = touchY;

    data->state = LV_INDEV_STATE_RELEASED; // 当前未使用触摸
}

/* ===========================
 *  UI 事件回调函数
 * =========================== */

/**
 * @brief 按钮点击事件
 */
void btn_counter_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        click_count++;
        lv_label_set_text_fmt(label_counter, "Clicked: %d times", click_count);

        // 按钮点击动画
        lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);

        // 创建按压效果
        static lv_anim_t anim;
        lv_anim_init(&anim);
        lv_anim_set_var(&anim, btn);
        lv_anim_set_exec_cb(&anim, [](void *var, int32_t value)
                            {
            lv_obj_t *obj = (lv_obj_t *)var;
            lv_obj_set_style_transform_scale(obj, value, 0); });
        lv_anim_set_values(&anim, 250, 256); // 缩小到 97% 再恢复
        lv_anim_set_duration(&anim, 150);
        lv_anim_set_playback_duration(&anim, 150);
        lv_anim_start(&anim);

        Serial.printf("Button clicked! Total: %d\n", click_count);
    }
}

/**
 * @brief 滑块值变化事件
 */
void slider_led_event_cb(lv_event_t *e)
{
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);

    lv_label_set_text_fmt(label_slider, "LED: %d%%", value);

    // 控制 LED 亮度 (使用 PWM)
    // analogWrite(LED_PIN, map(value, 0, 100, 0, 255));

    Serial.printf("Slider value: %d%%\n", value);
}

/**
 * @brief 开关切换事件
 */
void switch_buzzer_event_cb(lv_event_t *e)
{
    lv_obj_t *sw = (lv_obj_t *)lv_event_get_target(e);
    bool state = lv_obj_has_state(sw, LV_STATE_CHECKED);

    if (state)
    {
        lv_label_set_text(label_status, "Status: Buzzer ON");
        // 蜂鸣器开启逻辑
        Serial.println("Buzzer ON");
    }
    else
    {
        lv_label_set_text(label_status, "Status: Buzzer OFF");
        // 蜂鸣器关闭逻辑
        Serial.println("Buzzer OFF");
    }
}

/* ===========================
 *  UI 创建函数
 * =========================== */

/**
 * @brief 创建主界面
 */
void create_demo_ui(void)
{
    // 设置默认主题
    lv_theme_t *theme = lv_theme_default_init(
        lv_display_get_default(),
        lv_palette_main(LV_PALETTE_BLUE),
        lv_palette_main(LV_PALETTE_RED),
        false, // 日间模式
        LV_FONT_DEFAULT);
    lv_display_set_theme(lv_display_get_default(), theme);

    /* =====================
     * 1. 标题栏
     * ===================== */
    lv_obj_t *label_title = lv_label_create(lv_screen_active());
    lv_label_set_text(label_title, "LVGL Demo - ESP32-PICO");
    lv_obj_set_style_text_font(label_title, &lv_font_montserrat_20, 0);
    lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 5);

    /* =====================
     * 2. 点击计数按钮
     * ===================== */
    btn_counter = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn_counter, 150, 50);
    lv_obj_align(btn_counter, LV_ALIGN_TOP_LEFT, 10, 40);
    lv_obj_add_event_cb(btn_counter, btn_counter_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(btn_counter);
    lv_label_set_text(btn_label, "Click Me!");
    lv_obj_center(btn_label);

    label_counter = lv_label_create(lv_screen_active());
    lv_label_set_text(label_counter, "Clicked: 0 times");
    lv_obj_align(label_counter, LV_ALIGN_TOP_LEFT, 10, 100);

    /* =====================
     * 3. LED 亮度滑块
     * ===================== */
    lv_obj_t *slider_label_title = lv_label_create(lv_screen_active());
    lv_label_set_text(slider_label_title, "LED Brightness:");
    lv_obj_align(slider_label_title, LV_ALIGN_TOP_RIGHT, -10, 40);

    slider_led = lv_slider_create(lv_screen_active());
    lv_obj_set_width(slider_led, 140);
    lv_obj_align(slider_led, LV_ALIGN_TOP_RIGHT, -10, 65);
    lv_slider_set_range(slider_led, 0, 100);
    lv_slider_set_value(slider_led, 50, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_led, slider_led_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    label_slider = lv_label_create(lv_screen_active());
    lv_label_set_text(label_slider, "LED: 50%");
    lv_obj_align(label_slider, LV_ALIGN_TOP_RIGHT, -10, 100);

    /* =====================
     * 4. 蜂鸣器开关
     * ===================== */
    lv_obj_t *switch_label_title = lv_label_create(lv_screen_active());
    lv_label_set_text(switch_label_title, "Buzzer Control:");
    lv_obj_align(switch_label_title, LV_ALIGN_LEFT_MID, 10, -30);

    switch_buzzer = lv_switch_create(lv_screen_active());
    lv_obj_align(switch_buzzer, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_event_cb(switch_buzzer, switch_buzzer_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* =====================
     * 5. 进度条
     * ===================== */
    lv_obj_t *bar_label_title = lv_label_create(lv_screen_active());
    lv_label_set_text(bar_label_title, "Auto Progress:");
    lv_obj_align(bar_label_title, LV_ALIGN_LEFT_MID, 10, 30);

    bar_progress = lv_bar_create(lv_screen_active());
    lv_obj_set_size(bar_progress, 140, 20);
    lv_obj_align(bar_progress, LV_ALIGN_LEFT_MID, 10, 55);
    lv_bar_set_value(bar_progress, 0, LV_ANIM_OFF);

    /* =====================
     * 6. 实时数据图表
     * ===================== */
    chart_data = lv_chart_create(lv_screen_active());
    lv_obj_set_size(chart_data, 140, 80);
    lv_obj_align(chart_data, LV_ALIGN_BOTTOM_RIGHT, -10, -10);
    lv_chart_set_type(chart_data, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart_data, 20);
    lv_chart_set_range(chart_data, LV_CHART_AXIS_PRIMARY_Y, 0, 100);

    chart_series = lv_chart_add_series(chart_data,
                                       lv_palette_main(LV_PALETTE_GREEN),
                                       LV_CHART_AXIS_PRIMARY_Y);

    // 初始化图表数据
    for (int i = 0; i < 20; i++)
    {
        lv_chart_set_next_value(chart_data, chart_series, random(0, 100));
    }

    lv_obj_t *chart_label = lv_label_create(lv_screen_active());
    lv_label_set_text(chart_label, "Live Data");
    lv_obj_align(chart_label, LV_ALIGN_BOTTOM_RIGHT, -60, -95);

    /* =====================
     * 7. 状态标签
     * ===================== */
    label_status = lv_label_create(lv_screen_active());
    lv_label_set_text(label_status, "Status: Ready");
    lv_obj_set_style_text_color(label_status, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_align(label_status, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    Serial.println("UI Created Successfully!");
}

/* ===========================
 *  初始化和主循环
 * =========================== */

/**
 * @brief LVGL + TFT_eSPI 初始化
 */
void lvgl_demo_setup()
{
    Serial.begin(115200);
    delay(100);
    Serial.println("\n=== LVGL Demo Starting ===");

    // 初始化 TFT 显示屏
    tft.begin();
    tft.setRotation(1); // 横屏模式
    tft.fillScreen(TFT_BLACK);

    // 初始化 LVGL
    lv_init();

    // 创建显示驱动
    lv_display_t *disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_flush_cb(disp, lvgl_display_flush);
    lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // 创建输入设备 (触摸或按键)
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, lvgl_touchpad_read);

    // 创建 UI
    create_demo_ui();

    Serial.println("LVGL Initialization Complete!");
    Serial.printf("Screen: %dx%d, Buffer: %d bytes\n", SCREEN_WIDTH, SCREEN_HEIGHT, BUF_SIZE);
}

/**
 * @brief 主循环 - 定时调用
 */
void lvgl_demo_loop()
{
    static uint32_t last_tick = 0;
    static uint32_t last_chart_update = 0;
    static uint32_t last_progress_update = 0;

    uint32_t now = millis();

    // LVGL 任务处理 (每 5ms)
    if (now - last_tick >= 5)
    {
        lv_task_handler();
        last_tick = now;
    }

    // 更新进度条 (每 50ms)
    if (now - last_progress_update >= 50)
    {
        progress_value = (progress_value + 1) % 101;
        lv_bar_set_value(bar_progress, progress_value, LV_ANIM_ON);
        last_progress_update = now;
    }

    // 更新图表 (每 500ms)
    if (now - last_chart_update >= 500)
    {
        lv_chart_set_next_value(chart_data, chart_series, random(0, 100));
        lv_chart_refresh(chart_data);
        last_chart_update = now;
    }

    delay(1); // 避免看门狗超时
}

/* ===========================
 *  Arduino 标准入口
 * =========================== */

void setup()
{
    lvgl_demo_setup();
}

void loop()
{
    lvgl_demo_loop();
}
