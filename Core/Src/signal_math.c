/**
 * @file    signal_math.c
 * @brief   信号分析数学基础
 */

#include <stddef.h>
#include <math.h>
#include "signal_math.h"

complex_t phasor_to_complex(phasor_t *p) {
    complex_t c;
    c.real = p->magnitude * cosf(p->phase);
    c.imag = p->magnitude * sinf(p->phase);
    return c;
}
phasor_t complex_to_phasor(complex_t *c) {
    phasor_t p;
    p.magnitude = complex_norm(c);
    p.phase = complex_arg(c);
    return p;
}