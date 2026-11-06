/**
 * @file    dds.c
 * @brief   AD9959 接口的占位实现（无通信、无寄存器操作）。
 */

#include "dds.h"

/*
 * TODO(软件五): SPI 实例与 CS / SCLK / SDIO0..3 / IO_UPDATE / RESET 引脚均为 TBD。
 * TODO(软件五): 参考时钟来源（板载晶振或外部）待确认，影响 FTW 换算。
 * 本模块只写频率/幅度，不含扫频策略。
 */

static bool s_ready;

bool dds_init(void)
{
    /* TODO: 控制引脚与 SPI 初始化、上电复位序列、写-读校验。 */
    s_ready = false;
    return false;
}

bool dds_set_frequency(uint8_t channel, uint32_t freq_hz)
{
    if (!s_ready || (channel >= DDS_CHANNEL_COUNT) || (freq_hz == 0u)) {
        return false;
    }

    /* TODO: FTW = round(f_out * 2^32 / f_sysclk)，写通道频字后发 IO_UPDATE。 */
    return false;
}

bool dds_set_amplitude(uint8_t channel, uint16_t amplitude)
{
    if (!s_ready || (channel >= DDS_CHANNEL_COUNT)) {
        return false;
    }

    /* TODO: 幅度标度的含义（ASF 码值还是 mVpp）待定。 */
    (void)amplitude;
    return false;
}

bool dds_is_ready(void)
{
    return s_ready;
}
