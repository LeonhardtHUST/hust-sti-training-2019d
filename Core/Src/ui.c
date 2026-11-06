/**
 * @file    ui.c
 * @brief   LCD 显示的占位实现。
 */

#include "ui.h"

/*
 * TODO(软件四): LCD 型号、总线（SPI / I2C / 并口）与引脚均为 TBD。
 * TODO(软件四): 显示六项数值的排版、单位与刷新节流待定。
 */

void ui_init(void)
{
    /* TODO: 初始化 LCD、清屏、设置字体。 */
}

void ui_show_result(const sa_result_t *result)
{
    (void)result;

    /* TODO:
     *   CH1 Vpp / CH2 Vpp
     *   CH1 DC  / CH2 DC
     *   相位差 (deg)
     */
}

void ui_show_text(const char *text)
{
    (void)text;

    /* TODO: 显示一行状态文本。 */
}
