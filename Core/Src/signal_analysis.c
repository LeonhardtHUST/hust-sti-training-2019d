/**
 * @file    signal_analysis.c
 * @brief   直流偏置 / 峰峰值 / 相位差的纯计算实现。
 */

#include <math.h>
#include <float.h>
#include <stddef.h>
#include "signal_math.h"
#include "signal_analysis.h"

/*
 * TODO(软件四):
 *   1. 码值 -> 电压换算系数（VREF、前端衰减）待标定，当前按 3.0V / 4096 档换算。
 */

/*
 * 采样参数的唯一事实来源是 acquisition.h，本模块不自带数值副本：
 * 只有与采集宏完全一致的整帧输入才是合法输入；count 运行时传参
 * 仅用于入口核验，不参与任何计算。
 * 周期关系在编译期钉死：
 *   - 周期点数能被 4 整除，才存在精确的 90 度移相采样点；
 *   - 帧长是周期整数倍，整周期抵消（直流 / 倍频项归零）才成立。
 */
_Static_assert(ACQ_SAMPLES_PER_PERIOD % 4u == 0u,
               "周期点数必须能被 4 整除，否则不存在精确的 90 度移相采样点");
_Static_assert(ACQ_SAMPLES_PER_CHANNEL % ACQ_SAMPLES_PER_PERIOD == 0u,
               "帧长必须是信号周期的整数倍，整周期抵消假设才成立");

/** 90 度移相量：四分之一个信号周期的采样点数。 */
#define SA_QUARTER_PERIOD_SHIFT (ACQ_SAMPLES_PER_PERIOD / 4u)

/** @brief 信号参数分析。
 *  @param frame 采样帧数据指针，count 必须等于 ACQ_SAMPLES_PER_CHANNEL
 *  @param result 分析结果存储指针
 *  @note 非法输入（指针为空、count 不符）时 result 各字段填 SA_INVALID_RESULT。
 */
void sa_channels_analyse(const acq_frame_t *frame, sa_result_t *result)
{
    if (result == NULL) {
        return;
    }
    if ((frame == NULL) || (frame->ch1 == NULL) || (frame->ch2 == NULL) ||
        (frame->count != ACQ_SAMPLES_PER_CHANNEL)) {
        result->channel[0].dc_offset = SA_INVALID_RESULT;
        result->channel[0].vpp = SA_INVALID_RESULT;
        result->channel[1].dc_offset = SA_INVALID_RESULT;
        result->channel[1].vpp = SA_INVALID_RESULT;
        result->phase_deg = SA_INVALID_RESULT;
        return;
    }

    const uint16_t *samples[ACQ_CHANNEL_COUNT] = { frame->ch1, frame->ch2 };

    result->channel[0].dc_offset = sa_dc_offset(samples[0], ACQ_SAMPLES_PER_CHANNEL);
    result->channel[0].vpp = sa_vpp(samples[0], ACQ_SAMPLES_PER_CHANNEL);
    result->channel[1].dc_offset = sa_dc_offset(samples[1], ACQ_SAMPLES_PER_CHANNEL);
    result->channel[1].vpp = sa_vpp(samples[1], ACQ_SAMPLES_PER_CHANNEL);
    result->phase_deg = sa_phase_difference(frame, result);
}

/** @brief 计算直流偏置。
 *  @param samples 采样数据指针
 *  @param count 采样数据点数，仅接受 ACQ_SAMPLES_PER_CHANNEL
 *  @return 直流偏置电压值；非法输入返回 SA_INVALID_RESULT
 */
float sa_dc_offset(const uint16_t *samples, uint32_t count)
{
    if ((samples == NULL) || (count != ACQ_SAMPLES_PER_CHANNEL)) {
        return SA_INVALID_RESULT;
    }

    float sum_of_samples = 0.0f;
    for (uint32_t i = 0u; i < ACQ_SAMPLES_PER_CHANNEL; i++) {
        sum_of_samples += (float)samples[i];
    }
    return sum_of_samples * SA_VOLTS_PER_COUNT / (float)ACQ_SAMPLES_PER_CHANNEL;
}

/** @brief 计算峰峰值（去直流后的最大值 - 最小值）。
 *  @param samples 采样数据指针
 *  @param count 采样数据点数，仅接受 ACQ_SAMPLES_PER_CHANNEL
 *  @return Vpp 电压值；非法输入返回 SA_INVALID_RESULT
 */
float sa_vpp(const uint16_t *samples, uint32_t count)
{
    if ((samples == NULL) || (count != ACQ_SAMPLES_PER_CHANNEL)) {
        return SA_INVALID_RESULT;
    }
    float dc = sa_dc_offset(samples, ACQ_SAMPLES_PER_CHANNEL);
    float max = -FLT_MAX;
    float min = FLT_MAX;
    for (uint32_t i = 0u; i < ACQ_SAMPLES_PER_CHANNEL; i++) {
        float v = (float)samples[i] * SA_VOLTS_PER_COUNT - dc;
        if (v > max) {
            max = v;
        }
        if (v < min) {
            min = v;
        }
    }
    return max - min;
}

/** @brief 计算相位差。
 *  @param frame 采样帧数据指针，count 必须等于 ACQ_SAMPLES_PER_CHANNEL
 *  @param result 分析结果指针，须已填好两通道的 dc_offset（本函数只复用不重算）
 *  @return 相位差（度）；非法输入返回 SA_INVALID_RESULT
 */
float sa_phase_difference(const acq_frame_t *frame, sa_result_t *result)
{
    if ((frame == NULL) || (result == NULL) || (frame->ch1 == NULL) ||
        (frame->ch2 == NULL) || (frame->count != ACQ_SAMPLES_PER_CHANNEL)) {
        return SA_INVALID_RESULT;
    }

    /*
     * 求和长度与移相量解耦：
     *   - 求和取整一个信号周期（400 点）：积化和差产生的 2 倍频项
     *     在整周期上恰好归零，I/Q 才只剩相位差信息；
     *   - 移相取四分之一周期（100 点 = 90 度）。
     * ch2 移相后最大下标 399 + 100 = 499 < ACQ_SAMPLES_PER_CHANNEL，不越界。
     */
    const uint32_t L = ACQ_SAMPLES_PER_PERIOD;
    const uint32_t h = SA_QUARTER_PERIOD_SHIFT;
    float I = 0.0f, Q = 0.0f;

    for (uint32_t n = 0u; n < L; n++) {
        float x1 = (float)frame->ch1[n] * SA_VOLTS_PER_COUNT - result->channel[0].dc_offset;
        float x2 = (float)frame->ch2[n] * SA_VOLTS_PER_COUNT - result->channel[1].dc_offset;
        float x2s = (float)frame->ch2[n + h] * SA_VOLTS_PER_COUNT - result->channel[1].dc_offset;
        I += x1 * x2;
        Q += x1 * x2s;
    }
    return atan2f(Q, I) / SIG_RAD_OVER_DEG;
}
