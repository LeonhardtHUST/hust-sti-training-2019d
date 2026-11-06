/**
 * @file    dds.h
 * @brief   AD9959 DDS 接口（软件五）。
 *
 * @note    边界：本模块只负责把频率/幅度写进芯片，不含任何测量策略。
 *          SPI 实例、控制引脚、参考时钟均为 TBD，函数为占位实现。
 */

#ifndef DDS_H
#define DDS_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DDS_CHANNEL_COUNT 4u /*!< AD9959 通道数 */

/**
 * @brief DDS 初始化（控制引脚、SPI、上电复位序列）。
 * @retval true  初始化完成
 * @retval false 未实现 / 通信失败
 * @note  TODO: IO_UPDATE / RESET / CS / SCLK / SDIO 引脚分配（TBD）。
 */
bool dds_init(void);

/**
 * @brief 设置指定通道的输出频率。
 * @param[in] channel 通道号 0..DDS_CHANNEL_COUNT-1
 * @param[in] freq_hz 目标频率，单位 Hz
 * @retval true 已写入
 * @note  TODO: FTW = round(f_out * 2^32 / f_sysclk)，并产生 IO_UPDATE 脉冲。
 */
bool dds_set_frequency(uint8_t channel, uint32_t freq_hz);

/**
 * @brief 设置指定通道的输出幅度。
 * @param[in] channel   通道号 0..DDS_CHANNEL_COUNT-1
 * @param[in] amplitude 幅度标度，具体含义（ASF 码值 / mVpp）待定
 * @retval true 已写入
 */
bool dds_set_amplitude(uint8_t channel, uint16_t amplitude);

/**
 * @brief 查询 DDS 是否可用。
 */
bool dds_is_ready(void);

#ifdef __cplusplus
}
#endif

#endif /* DDS_H */
