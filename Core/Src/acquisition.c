/**
 * @file    acquisition.c
 * @brief   采集模块实现：ADC + DMA + TIM6 触发，提供非阻塞采集接口。
 */

#include <stddef.h>
#include "main.h"
#include "acquisition.h"

/* CubeMX 把外设句柄定义在 main.c（非 static），main.h 里没有对应声明，
 * 用到的地方各自 extern。 */
extern ADC_HandleTypeDef hadc1;

/*
 * TODO(软件四): 外设已在 CubeMX 中配置，本文件尚未接上：
 *   - TIM6 TRGO 每 2.5 µs 触发一次（400 kHz）
 *   - ADC1 规则组序列 CH1 -> CH2，12 位，PA0/PA1 单端
 *   - DMA1_Channel1，ADC 请求 -> 内存，半字宽度，ACQ_TOTAL_SAMPLES 次传输
 */

/** @brief 采集状态。任意时刻只能处于其中一种，互斥关系由类型保证。 */
typedef enum {
    ACQ_STATE_IDLE = 0, /*!< 空闲：无有效帧，可启动新采集 */
    ACQ_STATE_BUSY,     /*!< 采集中：DMA 尚未传输完成 */
    ACQ_STATE_DONE,     /*!< 已完成：帧有效，等待取走（下次 acq_start 前一直有效） */
    ACQ_STATE_ERROR     /*!< 错误：采集失败，需重新初始化 */
} acq_state_t;

/* ISR 中会写、主循环中会读，必须是 volatile；
 * 单次 32 位写入在 Cortex-M4 上是原子的，无需关中断保护。 */
static volatile acq_state_t s_state = ACQ_STATE_IDLE;

static uint16_t s_buffer[ACQ_TOTAL_SAMPLES]; /*!< DMA 采集缓冲区，存储两通道交错数据。 */
static uint16_t s_buffer_ch1[ACQ_SAMPLES_PER_CHANNEL]; /*!< 采集帧视图：通道 1。 */
static uint16_t s_buffer_ch2[ACQ_SAMPLES_PER_CHANNEL]; /*!< 采集帧视图：通道 2。 */

void acq_init(void)
{
    s_state = ACQ_STATE_IDLE;
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
        s_state = ACQ_STATE_ERROR;
        Error_Handler();
    }
}

acq_status_t acq_start(void)
{
    if (s_state == ACQ_STATE_BUSY) {
        return ACQ_BUSY;
    }
    if (s_state == ACQ_STATE_ERROR) {
        return ACQ_ERROR;
    }
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)s_buffer, ACQ_TOTAL_SAMPLES) != HAL_OK) {
        s_state = ACQ_STATE_ERROR;
        // Error_Handler(); // shall not call Error_Handler() in non-blocking function, just return error code
        return ACQ_ERROR;
    }

    s_state = ACQ_STATE_BUSY;
    return ACQ_OK;
}

bool acq_is_done(void)
{
    return s_state == ACQ_STATE_DONE;
}

acq_status_t acq_get_frame(acq_frame_t *frame)
{
    if (frame == NULL) {
        return ACQ_ERROR;
    }
    if (s_state != ACQ_STATE_DONE) {
        return ACQ_BUSY;
    }

    /* ----- ACQ_STATE_DONE ----- */

    if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK) {
        s_state = ACQ_STATE_ERROR;
        return ACQ_ERROR;
    }
    __DMB();  // Data Memory Barrier

    for (uint32_t i = 0u; i < ACQ_SAMPLES_PER_CHANNEL; i++) {
        s_buffer_ch1[i] = s_buffer[i * 2u];
        s_buffer_ch2[i] = s_buffer[i * 2u + 1u];
    }
    s_state = ACQ_STATE_IDLE;
    frame->ch1 = s_buffer_ch1;
    frame->ch2 = s_buffer_ch2;
    frame->count = ACQ_SAMPLES_PER_CHANNEL;
    return ACQ_OK;
}

/**
 * @brief  ADC 转换完成回调函数。
 * @param hadc ADC 句柄
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1) {
        if (s_state == ACQ_STATE_BUSY) {
            s_state = ACQ_STATE_DONE;
        }
    }
}