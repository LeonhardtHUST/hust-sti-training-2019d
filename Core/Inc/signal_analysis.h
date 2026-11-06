/**
 * @file    signal_analysis.h
 * @brief   直流偏置 / 峰峰值 / 相位差（积化和差·正交法）的纯计算接口。
 *
 * @note    本模块不包含任何 HAL 调用，也不访问外设寄存器，
 *          以便用录制数据在 PC 上离线验证算法。
 *          合法输入由 acquisition.h 的采样宏唯一锁定：count 必须等于
 *          ACQ_SAMPLES_PER_CHANNEL，部分帧一律拒绝。
 */

#ifndef SIGNAL_ANALYSIS_H
#define SIGNAL_ANALYSIS_H

#include <float.h>
#include <stdint.h>
#include "signal_math.h"
#include "acquisition.h"

#define SA_VOLTS_PER_COUNT  (3.0f / 4096.0f)

/** 非法输入的统一返回标记；合法的电压（>= 0）/ 角度结果不会达到该量级。 */
#define SA_INVALID_RESULT   (-FLT_MAX)

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 单个通道的分析结果。 */
typedef struct {
    float vpp;       /*!< 峰峰值，单位 V */
    float dc_offset; /*!< 直流偏置，单位 V */
} sa_channel_result_t;

/** @brief 一帧数据的完整分析结果。 */
typedef struct {
    sa_channel_result_t channel[ACQ_CHANNEL_COUNT]; /*!< channel[0] = CH1, channel[1] = CH2 */
    float               phase_deg;                  /*!< CH1 相对 CH2 的相位差，单位度 */
} sa_result_t;

/* ---- 接口 ---- */

/**
 * @brief 分析一帧采样数据，填满结果结构。
 * @param[in]  frame  采样帧视图，count 必须等于 ACQ_SAMPLES_PER_CHANNEL
 * @param[out] result 分析结果
 * @note 非法输入时 result 各字段填 SA_INVALID_RESULT。
 */
void sa_channels_analyse(const acq_frame_t *frame, sa_result_t *result);

/**
 * @brief 计算一段采样数据的直流偏置（电压）。
 * @param[in] samples 采样码值
 * @param[in] count   点数，仅接受 ACQ_SAMPLES_PER_CHANNEL
 * @return 直流偏置；非法输入返回 SA_INVALID_RESULT
 * @note  换算系数（VREF、前端衰减）待标定。
 */
float sa_dc_offset(const uint16_t *samples, uint32_t count);

/**
 * @brief 计算一段采样数据的峰峰值（去直流后最大值 - 最小值）。
 * @param[in] samples 采样码值
 * @param[in] count   点数，仅接受 ACQ_SAMPLES_PER_CHANNEL
 * @return Vpp；非法输入返回 SA_INVALID_RESULT
 */
float sa_vpp(const uint16_t *samples, uint32_t count);

/** @brief 计算相位差。
 *  @param frame 采样帧数据指针，count 必须等于 ACQ_SAMPLES_PER_CHANNEL
 *  @param result 分析结果指针，须已填好两通道的 dc_offset（只复用不重算）
 *  @return 相位差（度）；非法输入返回 SA_INVALID_RESULT
 */
float sa_phase_difference(const acq_frame_t *frame, sa_result_t *result);

#ifdef __cplusplus
}
#endif

#endif /* SIGNAL_ANALYSIS_H */
