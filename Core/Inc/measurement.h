/**
 * @file    measurement.h
 * @brief   软件五：测量编排（状态机 + 扫频结果存储）。
 *
 * @note    本模块是软件五唯一的编排者，不含 DDS 寄存器操作，
 *          也不含信号处理算法。当前为占位实现。
 */

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MEASUREMENT_MAX_POINTS 128u /*!< 扫频结果容量，实际点数待定 */

/** @brief 单个频点的测量结果。 */
typedef struct {
    uint32_t freq_hz;       /*!< 该点频率 */
    float    amplitude_vpp; /*!< 被测电路输出峰峰值，单位 V */
    float    gain_db;       /*!< 相对输入幅度的增益，单位 dB */
} measurement_point_t;

/** @brief 测量状态机状态，迁移关系见 docs/software5.md 第 4 节。 */
typedef enum {
    MEASUREMENT_IDLE = 0,
    MEASUREMENT_SET_FREQ, /*!< 设置 DDS 频点 */
    MEASUREMENT_SETTLE,   /*!< 等待输出与电路建立 */
    MEASUREMENT_ACQUIRE,  /*!< 启动并等待一次采集 */
    MEASUREMENT_ANALYZE,  /*!< 提取输入/输出幅度 */
    MEASUREMENT_STORE,    /*!< 写入结果数组 */
    MEASUREMENT_REPORT,   /*!< 全部频点完成，输出结果 */
    MEASUREMENT_ERROR
} measurement_state_t;

/* ---- 接口 ---- */

/** @brief 初始化测量模块（清空结果、回到 IDLE）。 */
void measurement_init(void);

/**
 * @brief 启动一次扫频测量。
 * @retval false 未就绪 / 已在运行
 * @note  TODO: 对数频点生成、容量检查。
 */
bool measurement_start(void);

/** @brief 中止当前测量，保留已完成的数据点。 */
void measurement_abort(void);

/**
 * @brief 由主循环周期调用，推进一步状态机。
 * @note  TODO: 非阻塞推进，只置标志不阻塞等待。
 */
void measurement_poll(void);

/** @brief 当前状态。 */
measurement_state_t measurement_state(void);

/** @brief 已完成的频点数。 */
uint32_t measurement_point_count(void);

/**
 * @brief 结果数组只读视图。
 * @retval NULL 尚无数据
 */
const measurement_point_t *measurement_points(void);

#ifdef __cplusplus
}
#endif

#endif /* MEASUREMENT_H */
