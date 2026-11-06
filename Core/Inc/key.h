/**
 * @file    key.h
 * @brief   按键扫描与事件上报。
 *
 * @note    当前只有接口，KEY 引脚与有效电平为 TBD，函数为占位实现。
 *          本模块只产出事件，不直接调用业务模块。
 */

#ifndef KEY_H
#define KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 按键事件。 */
typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_ACQUIRE, /*!< 软件四：触发一次采集 */
    KEY_EVENT_SWEEP    /*!< 软件五：启动一次扫频测量 */
} key_event_t;

/**
 * @brief 按键模块初始化。
 * @note  TODO: 配置输入引脚与上下拉（TBD）。
 */
void key_init(void);

/**
 * @brief 主循环轮询，返回本次扫描产生的事件。
 * @retval KEY_EVENT_NONE 无事件
 * @note  TODO: 去抖（计数或时间窗），保证一次按下只上报一次。
 */
key_event_t key_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* KEY_H */
