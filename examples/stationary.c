#include "fixed_vqf.h"
#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    fixed_vqf_t state;
    const q24_t gyro[3] = {0, 0, 0};
    const q30_t acc[3] = {0, 0, 1073741824}; /* +1 g on sensor Z */
    q30_t q[4];
    fixed_vqf_init(&state);
    for (unsigned i = 0; i < 2 * FIXED_VQF_SAMPLE_HZ; ++i) {
        fixed_vqf_update_gyr(&state, gyro);
        fixed_vqf_update_acc(&state, acc);
    }
    fixed_vqf_get_q30(&state, q);
    printf("quaternion [w,x,y,z] Q30: %" PRId32 " %" PRId32
           " %" PRId32 " %" PRId32 "\n", q[0], q[1], q[2], q[3]);
    printf("rest detected: %d\n", fixed_vqf_get_rest_detected(&state));
    return 0;
}
