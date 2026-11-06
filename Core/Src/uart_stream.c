/**
 * @file    uart_stream.c
 * @brief   采样数据串口输出的占位实现。
 */

#include <stddef.h>

#include "uart_stream.h"

/*
 * TODO(软件四): UART 实例、波特率、TX 引脚均为 TBD。
 * TODO(软件四): 帧格式（帧头/帧尾、通道分离、定长或文本）待定，
 *               目标是被 VOFA+ 等上位机直接绘出两路波形。
 * TODO(软件四): 发送方式（阻塞 / 中断 / DMA）待定，不得阻塞下一次采集触发。
 */

void uart_stream_init(void)
{
    /* TODO: 绑定 UART 实例并配置波特率。 */
}

bool uart_stream_send_frame(const acq_frame_t *frame)
{
    if (frame == NULL) {
        return false;
    }

    /* TODO: 输出 ACQ_TOTAL_SAMPLES 个采样点，需在本帧视图有效期内完成。 */
    return false;
}

void uart_stream_send_text(const char *text)
{
    (void)text;

    /* TODO: 输出调试文本。 */
}
