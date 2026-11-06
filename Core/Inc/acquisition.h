/**
 * @file    acquisition.h
 * @brief   软件四：TIM 触发 / ADC 双通道规则组 / DMA 的采集接口。
 *
 * @note    当前只有接口和参数常量，采集链路尚未配置。所有函数均为占位实现。
 *          外设实例、通道、引脚见 docs/software4.md 中标为 TBD 的条目。
 */

#ifndef ACQUISITION_H
#define ACQUISITION_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 采样参数（推导过程见 docs/software4.md 第 2 节） ---- */
#define ACQ_SIGNAL_FREQ_HZ       1000u                                            /* 1 kHz */
#define ACQ_SAMPLES_PER_PERIOD   400u                                             /* 400 点/周期 */
#define ACQ_PERIODS              2u                                               /* 2 个周期 */
#define ACQ_CHANNEL_COUNT        2u                                               /* CH1 / CH2 */
#define ACQ_SAMPLES_PER_CHANNEL  (ACQ_SAMPLES_PER_PERIOD * ACQ_PERIODS)            /* 800 */
#define ACQ_TOTAL_SAMPLES        (ACQ_SAMPLES_PER_CHANNEL * ACQ_CHANNEL_COUNT)     /* 1600 */
#define ACQ_TRIGGER_RATE_HZ      (ACQ_SIGNAL_FREQ_HZ * ACQ_SAMPLES_PER_PERIOD)     /* 400 kHz */

/** @brief 采集接口返回码。 */
typedef enum {
    ACQ_OK = 0,
    ACQ_BUSY,
    ACQ_ERROR
} acq_status_t;

/**
 * @brief 一帧采样数据的只读视图。
 *
 * 缓冲区由 acquisition 模块独占持有，调用方不得写入；
 * 视图在下一帧采集启动前有效。
 */
typedef struct {
    const uint16_t *ch1; /*!< CH1 采样码值，长度 ACQ_SAMPLES_PER_CHANNEL */
    const uint16_t *ch2; /*!< CH2 采样码值，长度 ACQ_SAMPLES_PER_CHANNEL */
    uint32_t        count; /*!< 每个通道的有效点数 */
} acq_frame_t;

/* ---- 接口 ---- */

/**
 * @brief 采集模块初始化。
 * @note  TODO: 配置 TIM 触发、ADC 双通道规则组、DMA 通道与采样缓冲区。
 */
void acq_init(void);

/**
 * @brief 请求一次完整采集（通常由按键触发）。
 * @retval ACQ_OK    请求已被接受
 * @retval ACQ_BUSY  上一帧尚未取走
 */
acq_status_t acq_start(void);

/**
 * @brief 查询当前帧是否采集完成。
 * @note  TODO: 由 DMA 传输完成回调置位。
 */
bool acq_is_done(void);

/**
 * @brief 取回已完成帧的只读视图。
 * @param[out] frame 输出视图，由本函数填充
 * @retval ACQ_OK    成功
 * @retval ACQ_BUSY  尚未采集完成
 * @retval ACQ_ERROR 参数错误
 */
acq_status_t acq_get_frame(acq_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif /* ACQUISITION_H */
