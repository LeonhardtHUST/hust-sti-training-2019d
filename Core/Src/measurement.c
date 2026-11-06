/**
 * @file    measurement.c
 * @brief   软件五测量编排的占位实现（状态机尚未运行）。
 */

#include <stddef.h>

#include "measurement.h"

/*
 * TODO(软件五): 按 docs/software5.md 第 4 节实现状态机：
 *   IDLE -> SET_FREQ -> SETTLE -> ACQUIRE -> ANALYZE -> STORE -> (NEXT_POINT | REPORT)
 * TODO(软件五): 对数频点生成、容量检查、单频点超时保护。
 * 全部在主循环内非阻塞推进，不在中断里调用 dds / uart_stream / ui。
 */

static measurement_state_t s_state = MEASUREMENT_IDLE;
static measurement_point_t s_points[MEASUREMENT_MAX_POINTS];
static uint32_t            s_count;

void measurement_init(void)
{
    s_state = MEASUREMENT_IDLE;
    s_count = 0u;
}

bool measurement_start(void)
{
    if (s_state != MEASUREMENT_IDLE) {
        return false;
    }

    /* TODO: 生成对数频点、清空结果、迁移到 MEASUREMENT_SET_FREQ。 */
    return false;
}

void measurement_abort(void)
{
    /* TODO: 停止采集与 DDS 输出，保留已完成的数据点。 */
    s_state = MEASUREMENT_IDLE;
}

void measurement_poll(void)
{
    /* TODO: 推进一步状态机。 */
}

measurement_state_t measurement_state(void)
{
    return s_state;
}

uint32_t measurement_point_count(void)
{
    return s_count;
}

const measurement_point_t *measurement_points(void)
{
    if (s_count == 0u) {
        return NULL;
    }
    return s_points;
}
