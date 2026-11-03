/**
 * @file    acquisition.c
 * @brief   TIM 触发 / ADC 双通道规则组 / DMA 采集接口的占位实现。
 */

#include <stddef.h>

#include "acquisition.h"

/*
 * TODO(软件四): 以下外设均未配置，实例、通道与引脚待定（见 docs/software4.md）：
 *   - TIM : TBD，更新事件作为 ADC 外部触发，400 kHz
 *   - ADC : TBD，规则组序列 CH1 -> CH2，12 位
 *   - DMA : TBD，ADC 请求 -> 内存，半字宽度，ACQ_TOTAL_SAMPLES 次传输
 *   - GPIO: TBD，两路模拟输入
 */

static bool s_busy; /*!< 一帧正在采集 */
static bool s_done; /*!< 一帧已完成、尚未被取走 */

void acq_init(void)
{
    /* TODO: 配置 TIM / ADC / DMA，准备采样缓冲区。 */
    s_busy = false;
    s_done = false;
}

acq_status_t acq_start(void)
{
    if (s_busy) {
        return ACQ_BUSY;
    }

    /* TODO: 复位 DMA 传输计数、清完成标志、使能 TIM 触发。 */
    s_busy = true;
    s_done = false;
    return ACQ_OK;
}

bool acq_is_done(void)
{
    /* TODO: s_done 由 DMA 传输完成回调置位，同时清 s_busy。 */
    return s_done;
}

acq_status_t acq_get_frame(acq_frame_t *frame)
{
    if (frame == NULL) {
        return ACQ_ERROR;
    }
    if (!s_done) {
        return ACQ_BUSY;
    }

    /* TODO: 交回 DMA 缓冲区的只读视图。
     * 存储布局（两通道交错 or 各自连续）尚未确定。 */
    frame->ch1 = NULL;
    frame->ch2 = NULL;
    frame->count = 0u;
    return ACQ_OK;
}
