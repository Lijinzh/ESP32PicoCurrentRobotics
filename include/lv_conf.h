/**
 * @file lv_conf.h
 * LVGL 配置文件 for ESP32-PICO-D4
 * 针对 240MHz ESP32 + TFT_eSPI 优化
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   颜色设置
 *====================*/

/* 颜色深度: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888) */
#define LV_COLOR_DEPTH 16

/* 交换 RGB565 的高低字节 (部分显示器需要) */
#define LV_COLOR_16_SWAP 0

/* 启用屏幕透明度 (需要额外内存) */
#define LV_COLOR_SCREEN_TRANSP 0

/*=========================
   内存设置
 *=========================*/

/* 内存管理器: LV_MEM_CUSTOM=0 使用内置内存管理, 1=自定义 */
#define LV_MEM_CUSTOM 0

/* LVGL 使用的内存大小 (字节) - ESP32 有足够的 RAM */
#define LV_MEM_SIZE (64U * 1024U) /* 64KB */

/* 内存池地址 (0: malloc 分配) */
#define LV_MEM_ADR 0

/* 内存对齐, 如果使用 DMA 设置为 4 或 8 */
#define LV_MEM_BUF_MAX_NUM 16

/*====================
   HAL 设置
 *====================*/

/*=======================
   加速和优化
 *=======================*/

/* 禁用 ARM 特定加速 (ESP32 使用 Xtensa 架构) */
#define LV_USE_DRAW_SW_ASM LV_DRAW_SW_ASM_NONE

/* 默认显示刷新周期 (ms) */
#define LV_DISP_DEF_REFR_PERIOD 30

/* 输入设备读取周期 (ms) */
#define LV_INDEV_DEF_READ_PERIOD 30

/* 使用自定义 tick 源 (我们使用 millis()) */
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())
#endif

/*=================
   特性配置
 *=================*/

/* 启用阴影绘制 (消耗 RAM) */
#define LV_USE_SHADOW 1
#if LV_USE_SHADOW
#define LV_SHADOW_CACHE_SIZE 0
#endif

/* 启用图像旋转 */
#define LV_USE_IMG_TRANSFORM 1

/* 启用抗锯齿 */
#define LV_ANTIALIAS 1

/* 启用动画 */
#define LV_USE_ANIMATION 1
#if LV_USE_ANIMATION
#define LV_ANIM_PLAYTIME_LIMIT 5000 /* 动画最长持续时间 (ms) */
#endif

/* 启用 GPU 加速 (ESP32 不支持, 设为 0) */
#define LV_USE_GPU 0

/* 启用文件系统 */
#define LV_USE_FS_STDIO 1
#define LV_FS_STDIO_LETTER 'S'

/*==================
   字体
 *==================*/

/* Montserrat 字体 */
#define LV_FONT_MONTSERRAT_8 0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* 默认字体 */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* 启用内置图标字体 */
#define LV_USE_FONT_COMPRESSED 1
#define LV_USE_FONT_SUBPX 0

/*===================
   文本设置
 *===================*/

/* 文本选择 */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/* 支持双向文本 (阿拉伯语、希伯来语等) */
#define LV_USE_BIDI 0

/* 支持阿拉伯-波斯语字符 */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*=================
   组件
 *=================*/

/* 启用所有常用组件 */
#define LV_USE_BTN 1
#define LV_USE_LABEL 1
#define LV_USE_IMG 1
#define LV_USE_LINE 1
#define LV_USE_ARC 1
#define LV_USE_BAR 1
#define LV_USE_SLIDER 1
#define LV_USE_SWITCH 1
#define LV_USE_CHECKBOX 1
#define LV_USE_DROPDOWN 1
#define LV_USE_ROLLER 1
#define LV_USE_TEXTAREA 1
#define LV_USE_TABLE 1
#define LV_USE_CALENDAR 1
#define LV_USE_CHART 1
#define LV_USE_COLORWHEEL 1
#define LV_USE_IMGBTN 1
#define LV_USE_KEYBOARD 1
#define LV_USE_LED 1
#define LV_USE_LIST 1
#define LV_USE_MENU 1
#define LV_USE_METER 1
#define LV_USE_MSGBOX 1
#define LV_USE_SPAN 1
#define LV_USE_SPINBOX 1
#define LV_USE_SPINNER 1
#define LV_USE_TABVIEW 1
#define LV_USE_TILEVIEW 1
#define LV_USE_WIN 1

/*==================
   主题
 *==================*/

#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
#define LV_THEME_DEFAULT_DARK 0
#define LV_THEME_DEFAULT_GROW 1
#define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif

#define LV_USE_THEME_BASIC 1
#define LV_USE_THEME_MONO 1

/*==================
   布局
 *==================*/

#define LV_USE_FLEX 1
#define LV_USE_GRID 1

/*==================
   其他
 *==================*/

/* 日志等级: TRACE, INFO, WARN, ERROR, USER, NONE */
#define LV_USE_LOG 1
#if LV_USE_LOG
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 0
#endif

/* 启用断言检查 (调试用) */
#define LV_USE_ASSERT_NULL 1
#define LV_USE_ASSERT_MALLOC 1
#define LV_USE_ASSERT_STYLE 0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ 0

/* 性能监控 */
#define LV_USE_PERF_MONITOR 1
#if LV_USE_PERF_MONITOR
#define LV_USE_PERF_MONITOR_POS LV_ALIGN_TOP_RIGHT
#endif

/* 内存监控 */
#define LV_USE_MEM_MONITOR 1
#if LV_USE_MEM_MONITOR
#define LV_USE_MEM_MONITOR_POS LV_ALIGN_TOP_LEFT
#endif

/* 帧率限制 */
#define LV_REFR_PERIOD 30 /* 30ms = ~33 FPS */

/* 启用 demo */
#define LV_USE_DEMO_WIDGETS 0
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
#define LV_USE_DEMO_BENCHMARK 0
#define LV_USE_DEMO_STRESS 0
#define LV_USE_DEMO_MUSIC 0

/*==================
   编译器设置
 *==================*/

#define LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TASK_HANDLER
#define LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_LARGE_CONST

/* C++ 兼容 */
#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* LV_CONF_H */
