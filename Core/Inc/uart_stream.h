/**
 * @file    uart_stream.h
 * @brief   把整帧采样数据输出给上位机（VOFA+ 等）。
 *
 * @note    当前只有接口，UART 实例、波特率、帧格式均未确定，
 *          函数为占位实现。
 */

#ifndef UART_STREAM_H
#define UART_STREAM_H

#include <stdbool.h>

#include "acquisition.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 输出通道初始化。
 * @note  TODO: 绑定 UART 实例与波特率（TBD）。
 */
void uart_stream_init(void);

/**
 * @brief 输出一帧完整采样数据。
 * @param[in] frame 采样帧视图
 * @retval true  已送出（或已完整入队）
 * @retval false 未实现 / 通道忙
 * @note  TODO: 帧头帧尾、通道分离、按上位机可解析格式定长输出。
 *        发送需在本帧视图有效期内完成。
 */
bool uart_stream_send_frame(const acq_frame_t *frame);

/**
 * @brief 输出一行调试文本。
 * @note  TODO: 行尾、换行约定待定。
 */
void uart_stream_send_text(const char *text);

#ifdef __cplusplus
}
#endif

#endif /* UART_STREAM_H */
