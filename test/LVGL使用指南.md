# LVGL 图形库使用指南 - ESP32-PICO-D4 专用

> **完整的嵌入式 GUI 解决方案**  
> LVGL (Light and Versatile Graphics Library) 是一个免费开源的嵌入式图形库  
> 项目地址: https://github.com/lvgl/lvgl (⭐16.7k)

---

## 📋 目录

1. [LVGL 简介](#1-lvgl-简介)
2. [快速开始](#2-快速开始)
3. [配置说明](#3-配置说明)
4. [基础使用](#4-基础使用)
5. [常用组件](#5-常用组件)
6. [高级功能](#6-高级功能)
7. [性能优化](#7-性能优化)
8. [常见问题](#8-常见问题)

---

## 1. LVGL 简介

### 1.1 主要特性

✅ **丰富的组件库**
- 40+ 内置组件 (按钮、滑块、图表、列表等)
- 支持自定义组件扩展

✅ **流畅动画**
- 内置动画引擎
- 60 FPS 流畅渲染
- 缓动函数支持

✅ **多种输入方式**
- 触摸屏
- 鼠标/键盘
- 编码器/按键

✅ **主题和样式**
- 可定制的主题系统
- CSS-like 样式控制
- 深色/浅色模式切换

✅ **内存优化**
- 仅需 64KB RAM (可配置)
- 支持部分刷新
- 双缓冲减少闪烁

### 1.2 系统架构

```
┌─────────────────────────────────┐
│     Your Application Code        │ ← lvgl_demo.cpp
├─────────────────────────────────┤
│       LVGL Core Engine           │ ← lv_* functions
│   (Widgets, Animations, Events)  │
├─────────────────────────────────┤
│     Display & Input Drivers      │ ← TFT_eSPI integration
├─────────────────────────────────┤
│         TFT_eSPI Library         │
├─────────────────────────────────┤
│           ESP32 HAL              │
└─────────────────────────────────┘
```

---

## 2. 快速开始

### 2.1 安装步骤

已自动完成以下配置:

```ini
# platformio.ini
lib_deps = 
    bodmer/TFT_eSPI @ ^2.5.43    # 显示驱动
    lvgl/lvgl @ ^9.2.0           # LVGL 核心库
```

### 2.2 运行 Demo

#### 方法 1: 直接运行 Demo

```bash
# 1. 将 test/lvgl_demo.cpp 复制到 src/main.cpp
cp test/lvgl_demo.cpp src/main.cpp

# 2. 编译上传
platformio run -t upload

# 3. 查看串口输出
platformio device monitor
```

#### 方法 2: 集成到现有项目

在 `src/main.cpp` 中:

```cpp
#include <Arduino.h>

// 声明 LVGL Demo 函数
extern void lvgl_demo_setup();
extern void lvgl_demo_loop();

void setup() {
    lvgl_demo_setup();
}

void loop() {
    lvgl_demo_loop();
}
```

### 2.3 预期效果

Demo 包含以下交互组件:

| 组件 | 功能 | 位置 |
|------|------|------|
| 🔘 按钮 | 点击计数器 | 左上 |
| 🎚️ 滑块 | LED 亮度控制 (0-100%) | 右上 |
| 🔀 开关 | 蜂鸣器开/关 | 左中 |
| 📊 进度条 | 自动循环进度 | 左中下 |
| 📈 图表 | 实时数据可视化 | 右下 |
| 📝 状态栏 | 显示当前状态 | 底部 |

---

## 3. 配置说明

### 3.1 lv_conf.h 核心配置

位置: `include/lv_conf.h`

```c
// ===== 显示配置 =====
#define LV_COLOR_DEPTH 16        // RGB565 (2 bytes/pixel)
#define LV_COLOR_16_SWAP 0       // 字节序 (根据屏幕调整)

// ===== 内存配置 =====
#define LV_MEM_SIZE (64U * 1024U)  // 64KB (ESP32 足够)

// ===== 性能配置 =====
#define LV_REFR_PERIOD 30        // 30ms = ~33 FPS
#define LV_USE_PERF_MONITOR 1    // 显示 FPS

// ===== 调试配置 =====
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
```

### 3.2 屏幕分辨率调整

在 `lvgl_demo.cpp` 中修改:

```cpp
// 根据你的 TFT 屏幕调整
static const uint16_t SCREEN_WIDTH = 320;   // 宽度
static const uint16_t SCREEN_HEIGHT = 240;  // 高度
static const uint16_t BUF_SIZE = SCREEN_WIDTH * 40;  // 缓冲区
```

常见分辨率:
- 240×320 (2.4" TFT)
- 320×240 (2.8" TFT)
- 480×320 (3.5" TFT)
- 128×160 (1.8" TFT)

### 3.3 TFT_eSPI 配置

确保 `TFT_eSPI/User_Setup.h` 中正确配置:

```cpp
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
```

---

## 4. 基础使用

### 4.1 创建简单界面

```cpp
#include <lvgl.h>

void create_ui() {
    // 创建标签
    lv_obj_t *label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello LVGL!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    
    // 创建按钮
    lv_obj_t *btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    
    // 按钮标签
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click Me");
    lv_obj_center(btn_label);
}
```

### 4.2 事件处理

```cpp
void button_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    
    if (code == LV_EVENT_CLICKED) {
        Serial.println("Button clicked!");
        
        // 获取按钮对象
        lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
        
        // 修改标签文本
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        lv_label_set_text(label, "Pressed!");
    }
}

// 绑定事件
lv_obj_add_event_cb(btn, button_event_handler, LV_EVENT_ALL, NULL);
```

### 4.3 对齐和布局

```cpp
// ===== 对齐方式 =====
lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);          // 居中
lv_obj_align(obj, LV_ALIGN_TOP_LEFT, 10, 10);     // 左上角
lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, -10, -10); // 右下角

// ===== Flex 布局 =====
lv_obj_t *cont = lv_obj_create(lv_screen_active());
lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);      // 水平排列
lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER,  // 主轴居中
                            LV_FLEX_ALIGN_CENTER,  // 交叉轴居中
                            LV_FLEX_ALIGN_CENTER); // 内容居中

// ===== Grid 布局 =====
static lv_coord_t col_dsc[] = {100, 100, 100, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {50, 50, 50, LV_GRID_TEMPLATE_LAST};
lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
```

---

## 5. 常用组件

### 5.1 按钮 (Button)

```cpp
// 创建按钮
lv_obj_t *btn = lv_button_create(lv_screen_active());
lv_obj_set_size(btn, 120, 50);

// 添加标签
lv_obj_t *label = lv_label_create(btn);
lv_label_set_text(label, "Press");
lv_obj_center(label);

// 事件回调
lv_obj_add_event_cb(btn, [](lv_event_t *e) {
    Serial.println("Button pressed!");
}, LV_EVENT_CLICKED, NULL);
```

### 5.2 滑块 (Slider)

```cpp
// 创建滑块
lv_obj_t *slider = lv_slider_create(lv_screen_active());
lv_obj_set_width(slider, 200);
lv_slider_set_range(slider, 0, 100);
lv_slider_set_value(slider, 50, LV_ANIM_OFF);

// 值变化回调
lv_obj_add_event_cb(slider, [](lv_event_t *e) {
    lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
    int value = lv_slider_get_value(slider);
    Serial.printf("Slider: %d\n", value);
}, LV_EVENT_VALUE_CHANGED, NULL);
```

### 5.3 开关 (Switch)

```cpp
lv_obj_t *sw = lv_switch_create(lv_screen_active());

lv_obj_add_event_cb(sw, [](lv_event_t *e) {
    lv_obj_t *sw = (lv_obj_t *)lv_event_get_target(e);
    bool state = lv_obj_has_state(sw, LV_STATE_CHECKED);
    Serial.printf("Switch: %s\n", state ? "ON" : "OFF");
}, LV_EVENT_VALUE_CHANGED, NULL);
```

### 5.4 进度条 (Bar)

```cpp
lv_obj_t *bar = lv_bar_create(lv_screen_active());
lv_obj_set_size(bar, 200, 20);
lv_bar_set_value(bar, 75, LV_ANIM_ON);  // 75% 带动画
```

### 5.5 图表 (Chart)

```cpp
// 创建图表
lv_obj_t *chart = lv_chart_create(lv_screen_active());
lv_obj_set_size(chart, 200, 100);
lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
lv_chart_set_point_count(chart, 10);

// 添加数据系列
lv_chart_series_t *ser1 = lv_chart_add_series(chart, 
    lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

// 设置数据
lv_chart_set_next_value(chart, ser1, 10);
lv_chart_set_next_value(chart, ser1, 20);
lv_chart_set_next_value(chart, ser1, 30);
```

### 5.6 下拉框 (Dropdown)

```cpp
lv_obj_t *dd = lv_dropdown_create(lv_screen_active());
lv_dropdown_set_options(dd, "Option 1\nOption 2\nOption 3");

lv_obj_add_event_cb(dd, [](lv_event_t *e) {
    lv_obj_t *dd = (lv_obj_t *)lv_event_get_target(e);
    uint16_t sel = lv_dropdown_get_selected(dd);
    Serial.printf("Selected: %d\n", sel);
}, LV_EVENT_VALUE_CHANGED, NULL);
```

### 5.7 文本框 (Textarea)

```cpp
lv_obj_t *ta = lv_textarea_create(lv_screen_active());
lv_obj_set_size(ta, 200, 100);
lv_textarea_set_placeholder_text(ta, "Enter text...");
lv_textarea_set_max_length(ta, 50);
```

---

## 6. 高级功能

### 6.1 动画

```cpp
// 创建动画
lv_anim_t anim;
lv_anim_init(&anim);
lv_anim_set_var(&anim, obj);  // 动画目标对象

// 设置动画回调 (X 坐标移动)
lv_anim_set_exec_cb(&anim, [](void *var, int32_t value) {
    lv_obj_set_x((lv_obj_t *)var, value);
});

lv_anim_set_values(&anim, 0, 200);      // 从 0 到 200
lv_anim_set_duration(&anim, 1000);      // 1 秒
lv_anim_set_repeat_count(&anim, LV_ANIM_REPEAT_INFINITE);  // 无限循环
lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out);      // 缓动函数

lv_anim_start(&anim);
```

### 6.2 样式定制

```cpp
// 创建样式
static lv_style_t style_btn;
lv_style_init(&style_btn);

// 设置样式属性
lv_style_set_bg_color(&style_btn, lv_palette_main(LV_PALETTE_BLUE));
lv_style_set_bg_grad_color(&style_btn, lv_palette_darken(LV_PALETTE_BLUE, 2));
lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);
lv_style_set_border_width(&style_btn, 2);
lv_style_set_border_color(&style_btn, lv_palette_main(LV_PALETTE_WHITE));
lv_style_set_radius(&style_btn, 10);

// 应用样式
lv_obj_add_style(btn, &style_btn, 0);
```

### 6.3 自定义主题

```cpp
lv_theme_t *theme = lv_theme_default_init(
    lv_display_get_default(),
    lv_palette_main(LV_PALETTE_BLUE),    // 主色调
    lv_palette_main(LV_PALETTE_RED),     // 次要色
    true,                                // 深色模式
    LV_FONT_DEFAULT
);

lv_display_set_theme(lv_display_get_default(), theme);
```

### 6.4 中文字体支持

```cpp
// 1. 使用在线工具生成字体: https://lvgl.io/tools/fontconverter
// 2. 设置 Unicode 范围: 0x4e00-0x9fff (中文常用字)
// 3. 导出 .c 文件到 src/fonts/

// 4. 在代码中引用
LV_FONT_DECLARE(my_chinese_font_16);

lv_obj_t *label = lv_label_create(lv_screen_active());
lv_obj_set_style_text_font(label, &my_chinese_font_16, 0);
lv_label_set_text(label, "你好 LVGL!");
```

---

## 7. 性能优化

### 7.1 内存优化

```cpp
// ===== 调整缓冲区大小 =====
// 较小: 节省 RAM,刷新慢
// 较大: 消耗 RAM,刷新快
static const uint16_t BUF_SIZE = SCREEN_WIDTH * 20;  // 20 行

// ===== 使用部分刷新 =====
lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), 
                       LV_DISPLAY_RENDER_MODE_PARTIAL);

// ===== 禁用不需要的组件 =====
// 在 lv_conf.h 中:
#define LV_USE_CALENDAR 0   // 禁用日历
#define LV_USE_COLORWHEEL 0 // 禁用色轮
```

### 7.2 渲染优化

```cpp
// ===== 批量更新 =====
lv_obj_invalidate(obj);  // 标记为需要重绘,但不立即绘制

// ===== 避免频繁重绘 =====
lv_refr_now(NULL);  // 强制立即刷新 (慎用)

// ===== 使用缓存 =====
lv_obj_set_style_transform_scale(obj, 256, 0);  // 硬件加速缩放
```

### 7.3 FPS 监控

```cpp
// 启用性能监视器 (已在 lv_conf.h 配置)
#define LV_USE_PERF_MONITOR 1

// 运行时会在右上角显示 FPS
```

---

## 8. 常见问题

### 8.1 编译错误

**问题**: `lv_conf.h: No such file or directory`

**解决**:
```bash
# 确认文件存在
ls include/lv_conf.h

# 清理缓存重新编译
platformio run -t clean
platformio run
```

---

**问题**: `undefined reference to 'lv_*'`

**解决**:
```ini
# 确认 platformio.ini 包含
lib_deps = 
    lvgl/lvgl @ ^9.2.0
```

### 8.2 显示问题

**问题**: 屏幕白屏或花屏

**解决**:
1. 检查 TFT_eSPI 配置
2. 尝试调整 `LV_COLOR_16_SWAP`
3. 检查 SPI 引脚连接

```cpp
// 在 lv_conf.h 中尝试:
#define LV_COLOR_16_SWAP 1  // 切换字节序
```

---

**问题**: 刷新闪烁

**解决**:
```cpp
// 使用双缓冲
lv_display_set_buffers(disp, buf1, buf2, sizeof(buf1), 
                       LV_DISPLAY_RENDER_MODE_DIRECT);
```

### 8.3 性能问题

**问题**: FPS 太低

**解决**:
1. 增大缓冲区
2. 减少刷新率
3. 禁用复杂动画

```cpp
// 增大缓冲区
static const uint16_t BUF_SIZE = SCREEN_WIDTH * 60;

// 降低刷新率
#define LV_REFR_PERIOD 50  // 50ms = 20 FPS
```

### 8.4 内存溢出

**问题**: `Guru Meditation Error` 或重启

**解决**:
```cpp
// 减少 LVGL 内存
#define LV_MEM_SIZE (32U * 1024U)  // 降至 32KB

// 减少缓冲区
static const uint16_t BUF_SIZE = SCREEN_WIDTH * 10;
```

---

## 9. 资源链接

### 9.1 官方资源

- 📚 [LVGL 官方文档](https://docs.lvgl.io/)
- 🎨 [UI 设计器 SquareLine Studio](https://squareline.io/)
- 🔧 [字体转换工具](https://lvgl.io/tools/fontconverter)
- 🖼️ [图片转换工具](https://lvgl.io/tools/imageconverter)

### 9.2 示例项目

- [LVGL Examples](https://github.com/lvgl/lv_examples)
- [ESP32 LVGL 项目](https://github.com/lvgl/lv_port_esp32)

### 9.3 社区

- [LVGL Forum](https://forum.lvgl.io/)
- [GitHub Issues](https://github.com/lvgl/lvgl/issues)

---

## 10. 下一步

🎯 **建议学习路径**:

1. ✅ 运行本 Demo 熟悉基础组件
2. 📝 修改 Demo 代码,添加自定义功能
3. 🎨 尝试创建自己的 UI 界面
4. 📊 集成传感器数据到图表
5. 🖼️ 添加自定义图标和字体
6. 🚀 优化性能,打造流畅 UI

---

**祝你使用 LVGL 创建出色的嵌入式 UI! 🎉**
