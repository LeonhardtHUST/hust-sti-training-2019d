/**
 * @file    ui.h
 * @brief   LCD 呈现：初始化与数值/状态显示。
 *
 * @note    当前只有接口，LCD 型号、总线与引脚均为 TBD，函数为占位实现。
 */

#ifndef UI_H
#define UI_H

#include "signal_analysis.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 显示层初始化。
 * @note  TODO: LCD 驱动、背光、清屏、字体（TBD）。
 */
void ui_init(void);

/**
 * @brief 显示一次测量结果：两路峰峰值、两路直流偏置、相位差。
 * @param[in] result 分析结果
 * @note  TODO: 排版、单位、刷新节流。
 */
void ui_show_result(const sa_result_t *result);

/**
 * @brief 显示一行状态文本（如 "ACQ..." / "ERROR"）。
 */
void ui_show_text(const char *text);

#ifdef __cplusplus
}
#endif

#endif /* UI_H */
