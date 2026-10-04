#include "fixed_vqf.h"
#include "fixed_vqf_coefficients.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void check_filter(const q30_t b[3], const q30_t a[2], double tau)
{
    const double c = tan(1.0 / FIXED_VQF_SAMPLE_HZ / (sqrt(2.0)*tau));
    const double d = 1 + sqrt(2.0)*c + c*c;
    assert(llabs((long long)b[0] - llround(c*c/d*1073741824.0)) <= 1);
    assert(llabs((long long)a[0] - llround(2*(c*c-1)/d*1073741824.0)) <= 1);
    assert(llabs((long long)a[1] - llround((1-sqrt(2.0)*c+c*c)/d*1073741824.0)) <= 1);
    assert((int64_t)b[0]+b[1]+b[2] == 1073741824LL+a[0]+a[1]);
    assert(a[1] > -1073741824 && a[1] < 1073741824);
    assert(1073741824LL+a[0]+a[1] > 0);
    assert(1073741824LL-a[0]+a[1] > 0);
}

int main(void)
{
    fixed_vqf_t s;
    const q24_t gyr[3] = {-225473, 70276, 125912};
    const q30_t acc[3] = {0, 0, 1073741824};
    q24_t bias[3];
    const double v = 1.0 / FIXED_VQF_SAMPLE_HZ;
    const double q20 = 1048576.0;
    const double pi = acos(-1.0);
    check_filter(ACC_B_Q30, ACC_A_Q30, 3.0);
    check_filter(REST_B_Q30, REST_A_Q30, 0.5);
    assert(REST_MIN_SAMPLES == 3U*FIXED_VQF_SAMPLE_HZ/2U);
    assert(REST_GYR_TH_Q24 == llround(2.0*pi/180.0*16777216.0));
    assert(REST_ACC_TH_Q30 == llround(0.5/9.81*1073741824.0));
    assert(BIAS_V_Q20 == llround(v*q20));
    assert(BIAS_W_MOTION_Q20 == llround((10000.0/v+100)*q20));
    assert(BIAS_W_VERTICAL_Q20 == llround((10000.0/v+100)/0.0001*q20));
    assert(BIAS_W_REST_Q20 == llround((81.0/v+9)*q20));
    assert(BIAS_P0_Q20 == 2500LL*1048576);
    fixed_vqf_init(&s);
    for (unsigned i = 0; i < 90U*FIXED_VQF_SAMPLE_HZ; ++i) {
        fixed_vqf_update_gyr(&s, gyr);
        fixed_vqf_update_acc(&s, acc);
    }
    assert(fixed_vqf_get_rest_detected(&s));
    fixed_vqf_get_bias_q24(&s, bias);
    for (unsigned k = 0; k < 3; ++k) {
        printf("axis %u residual Q24 %ld\n", k, (long)(bias[k]-gyr[k]));
        assert(llabs((long long)bias[k]-gyr[k]) < 600);
    }
    puts("VQF defaults: original parameters/coefficient stability/rest bias convergence passed");
    return 0;
}
