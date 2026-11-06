/**
 * @file    signal_math.h
 * @brief   信号分析数学基础。
 */

#ifndef SIGNAL_MATH_H
#define SIGNAL_MATH_H

#include <math.h>

#define SIG_PI 3.14159265358979323846f
#define SIG_RAD_OVER_DEG (SIG_PI / 180.0f)

#ifdef __cplusplus
extern "C" {
#endif

/** @brief 相量 */
typedef struct {
    float magnitude; /*!< 幅值，单位 V */
    float phase;     /*!< 相位，单位 rad */
} phasor_t;

/** @brief 复数 */
typedef struct {
    float real; /*!< 实部 */
    float imag; /*!< 虚部 */
} complex_t;

complex_t phasor_to_complex(phasor_t *p);
phasor_t complex_to_phasor(complex_t *c);

/** @brief 复数取模。 */
static inline float complex_norm(complex_t *c)
{
    return sqrtf(c->real * c->real + c->imag * c->imag);
}

/** @brief 复数取辐角（rad）。 */
static inline float complex_arg(complex_t *c)
{
    return atan2f(c->imag, c->real);
}

#ifdef __cplusplus
}
#endif

#endif /* SIGNAL_MATH_H */
