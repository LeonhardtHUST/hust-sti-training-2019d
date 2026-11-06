/**
 * @file    key.c
 * @brief   KEY1（PB8 / BOOT0）扫描与去抖。
 *
 * 采样周期 10 ms，连续 3 次采样一致才认账（判决窗口 30 ms）；
 * 只在「松开 → 按下」的稳定边沿上报一次事件，一次按压只产生一个事件。
 * 全程非阻塞：没有 HAL_Delay，靠 HAL_GetTick() 控制节拍。
 */

#include <stdint.h>

#include "key.h"
#include "xpulse.h"     /* 板级引脚映射 + stm32g4xx_hal.h */

#define KEY_SCAN_MS    10u   /* 采样周期 */
#define KEY_STABLE_N    3u   /* 连续一致次数 */

/* 同一颗 KEY1 在软件四 / 软件五 里语义不同，编译时二选一：
 * 软件五可用 -DKEY_PRESS_EVENT=KEY_EVENT_SWEEP 覆盖。 */
#ifndef KEY_PRESS_EVENT
#define KEY_PRESS_EVENT KEY_EVENT_ACQUIRE
#endif

static uint8_t  key_status_prev;        /*!< 上一次采样值 */
static uint8_t  consistency_counter;    /*!< 连续一致计数 */
static uint8_t  stable_state;           /*!< 去抖后的稳定状态：1 = 按下 */
static uint32_t last_sample_time;       /*!< 上次采样时刻 */

/** @brief 读原始电平（板上有 10 kΩ 下拉，按下为高）。 */
static uint8_t key_read_raw(void)
{
    return (HAL_GPIO_ReadPin(XPULSE_KEY_BOOT_GPIO_Port, XPULSE_KEY_BOOT_Pin)
            == XPULSE_KEY_BOOT_ACTIVE) ? 1u : 0u;
}

/** @brief 初始化按键扫描状态。 */
void key_init(void)
{
    key_status_prev  = key_read_raw();
    consistency_counter     = 0u;
    stable_state    = key_status_prev;
    last_sample_time = HAL_GetTick();
}

/** @brief 轮询按键状态。
 *
 * @return 按键事件，如果没有事件则返回 KEY_EVENT_NONE。
 */
key_event_t key_poll(void)
{
    uint32_t    now;
    uint8_t     raw;
    key_event_t event = KEY_EVENT_NONE;

    now = HAL_GetTick();
    if ((now - last_sample_time) < KEY_SCAN_MS) {
        return KEY_EVENT_NONE;      /* 未到采样时刻：直接返回 */
    }
    last_sample_time = now;

    raw = key_read_raw();

    if (raw != key_status_prev) {   /* 采样跳变 → 仍在抖动，重新计数 */
        key_status_prev = raw;
        consistency_counter = 0u;
        return KEY_EVENT_NONE;
    }

    if (consistency_counter < KEY_STABLE_N) {
        consistency_counter++;
    }

    if ((consistency_counter == KEY_STABLE_N) && (raw != stable_state)) {
        stable_state = raw;         /* 通过多次轮询，裁定：稳定状态真正翻转 */
        if (raw) {
            event = (key_event_t)KEY_PRESS_EVENT;
        }
    }

    return event;
}
