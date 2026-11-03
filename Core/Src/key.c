/**
 * @file    key.c
 * @brief   按键扫描与去抖的占位实现。
 */

#include "key.h"

/*
 * TODO(软件四): KEY 引脚与有效电平均为 TBD。
 * TODO(软件四): 去抖策略（计数 / 时间窗）待定，需保证一次按下只上报一次事件。
 */

void key_init(void)
{
    /* TODO: 配置输入引脚与上下拉。 */
}

key_event_t key_poll(void)
{
    /* TODO: 扫描 + 去抖，返回 KEY_EVENT_ACQUIRE 或 KEY_EVENT_SWEEP。 */
    return KEY_EVENT_NONE;
}
