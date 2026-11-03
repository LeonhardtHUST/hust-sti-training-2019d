/**
 * @file    signal_analysis.h
 * @brief   直流偏置 / 峰峰值 / 相位差（积化和差·正交法）的纯计算接口。
 *
 * @note    本模块不包含任何 HAL 调用，也不访问外设寄存器，
 *          以便用录制数据在 PC 上离线验证算法。
 *          当前所有函数均为占位实现，不含算法。
 */

#ifndef SIGNAL_ANALYSIS_H
#define SIGNAL_ANALYSIS_H

#include <stdint.h>

#include "acquisition.h"

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
 * @param[in]  frame  采样帧视图
 * @param[out] result 分析结果
 * @note  TODO: 码值→电压换算、去直流、峰峰值、正交法相位差。
 */
void sa_analyze(const acq_frame_t *frame, sa_result_t *result);

/**
 * @brief 计算一段采样数据的直流偏置（电压）。
 * @param[in] samples 采样码值
 * @param[in] count   点数
 * @note  TODO: 换算系数（VREF、前端衰减）待标定。
 */
float sa_dc_offset(const uint16_t *samples, uint32_t count);

/**
 * @brief 计算一段采样数据的峰峰值（电压）。
 * @note  TODO: 是否先去直流、是否做极值滤波待定。
 */
float sa_peak_to_peak(const uint16_t *samples, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* SIGNAL_ANALYSIS_H */
