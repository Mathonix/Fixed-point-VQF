#include "fixed_vqf.h"
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static double component(q30_t x) { return (double)x / 1073741824.0; }

int main(void)
{
    fixed_vqf_t s, before;
    q30_t q[4];
    const q30_t gravity[3] = {0, 0, 1073741824};
    const q30_t zero_acc[3] = {0, 0, 0};
    const q24_t zero_gyr[3] = {0, 0, 0};
    /* One radian per second for one second, isolated on each gyro axis. */
    for (unsigned axis = 0; axis < 3; ++axis) {
        q24_t gyr[3] = {0, 0, 0};
        gyr[axis] = 16777216;
        fixed_vqf_init(&s);
        for (unsigned i = 0; i < FIXED_VQF_SAMPLE_HZ; ++i)
            fixed_vqf_update_gyr(&s, gyr);
        fixed_vqf_get_q30(&s, q);
        assert(fabs(component(q[0]) - cos(0.5)) < 0.00002);
        for (unsigned k = 0; k < 3; ++k)
            assert(fabs(component(q[k+1]) - (k == axis ? sin(0.5) : 0)) < 0.00002);
    }
    fixed_vqf_init(&s);
    before = s;
    fixed_vqf_update_acc(&s, zero_acc);
    assert(memcmp(&s, &before, sizeof(s)) == 0);
    /* Rest detection enters after one second and clears on real motion. */
    for (unsigned i = 0; i < FIXED_VQF_SAMPLE_HZ; ++i) {
        fixed_vqf_update_gyr(&s, zero_gyr);
        fixed_vqf_update_acc(&s, gravity);
        if (i+1 < FIXED_VQF_SAMPLE_HZ) assert(!fixed_vqf_get_rest_detected(&s));
    }
    assert(fixed_vqf_get_rest_detected(&s));
    fixed_vqf_get_q30(&s, q);
    assert(q[0] == 1073741824 && q[1] == 0 && q[2] == 0 && q[3] == 0);
    {
        const q24_t gyr[3] = {0, 0, 16777216};
        for (unsigned i = 0; i < FIXED_VQF_SAMPLE_HZ; ++i) {
            fixed_vqf_update_gyr(&s, gyr);
            fixed_vqf_update_acc(&s, gravity);
        }
    }
    assert(!fixed_vqf_get_rest_detected(&s));
    fixed_vqf_get_q30(&s, q);
    assert(fabs(component(q[0]) - cos(0.5)) < 0.001);
    assert(fabs(component(q[3]) - sin(0.5)) < 0.001);
    /* Raw sensor conversion obeys the selected build-time full scale. */
    {
        const double dps_per_lsb = LSM6DSV_GYRO_FS_2000DPS ? 0.070 : 0.004375;
        for (int raw = -32768; raw <= 32767; ++raw) {
            const double expected = raw * dps_per_lsb * acos(-1.0) / 180.0 * 16777216.0;
            /* Q40 coefficient quantization plus Q24 output rounding: <1 LSB. */
            assert(fabs((double)fixed_vqf_gyro_raw_to_q24((int16_t)raw) - expected) < 1.0);
        }
    }
    puts("VQF motion: analytic rotations/rest transitions/zero-input/raw conversion passed");
    return 0;
}
