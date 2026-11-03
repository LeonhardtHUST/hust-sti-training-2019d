/**
 * @file    signal_analysis.c
 * @brief   信号参数计算的占位实现（暂无算法）。
 */

#include <stddef.h>

#include "signal_analysis.h"

/*
 * TODO(软件四):
 *   1. 码值 -> 电压换算：V = code / 4095 * VREF+（VREF 与前端系数待标定）。
 *   2. 直流偏置：整帧均值。
 *   3. 峰峰值：最大值 - 最小值。
 *   4. 相位差：积化和差 / 正交分解法，形如
 *        I = sum( x1[n] * x2[n] )
 *        Q = sum( x1[n] * x2_shift90[n] )
 *        phase = atan2(Q, I)
 *      其中 90 度移相数据的构造方式与取点范围待定。
 */

void sa_analyze(const acq_frame_t *frame, sa_result_t *result)
{
    uint32_t i;

    if ((frame == NULL) || (result == NULL)) {
        return;
    }

    for (i = 0u; i < ACQ_CHANNEL_COUNT; i++) {
        result->channel[i].vpp = 0.0f;
        result->channel[i].dc_offset = 0.0f;
    }
    result->phase_deg = 0.0f;

    /* TODO: 依次调用 sa_dc_offset() / sa_peak_to_peak()，再计算相位差。 */
}

float sa_dc_offset(const uint16_t *samples, uint32_t count)
{
    (void)samples;
    (void)count;

    /* TODO: 累加求均值，换算为电压。 */
    return 0.0f;
}

float sa_peak_to_peak(const uint16_t *samples, uint32_t count)
{
    (void)samples;
    (void)count;

    /* TODO: 求最大值与最小值，换算为电压。 */
    return 0.0f;
}
