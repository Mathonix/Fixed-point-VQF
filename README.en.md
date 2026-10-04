# Fixed-point VQF

[中文](README.md) | [English](README.en.md)

Fixed-point VQF is a C99 fixed-point implementation of VQF (Versatile Quaternion-based Filter) for embedded IMU orientation estimation. The fusion core uses integer arithmetic without dynamic memory allocation, making it suitable for processors without a floating-point unit.

This implementation provides **6D sensor fusion without a magnetometer**, including gyroscope integration, accelerometer inclination correction, gyroscope bias estimation, and rest detection. It outputs a quaternion in `[w, x, y, z]` order. The default algorithm parameters use the original VQF 6D settings, converted to fixed-point coefficients.

VQF algorithm: D. Laidig and T. Seel, Information Fusion, 2023. [Paper](https://doi.org/10.1016/j.inffus.2022.10.014) · [Original VQF](https://github.com/dlaidig/vqf).

## Documentation

- [API and fixed-point formats](docs/API.md)
- [Default parameters](docs/PARAMETERS.md)
- [Testing](docs/TESTING.md)

## Build

Requires a C99 compiler and CMake 3.16 or later:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

The default sampling rate is 2000 Hz. Use `-DFIXED_VQF_SAMPLE_HZ=1000` to build for 1000 Hz. The sampling rate determines the integration step and discrete filter coefficients; the physical meaning of the algorithm parameters stays the same.

Alternatively, add `vqf/c/fixed_vqf.c` to your project and add `vqf/c/` to its include paths. The library itself has no dependency on `libm`, an operating system, or an MCU peripheral library.

## Usage

```c
#include "fixed_vqf.h"

fixed_vqf_t filter;
q24_t gyro[3] = {0, 0, 0};          /* rad/s, Q7.24 */
q30_t acc[3] = {0, 0, 1073741824};  /* g, Q1.30 */
q30_t quat[4];

fixed_vqf_init(&filter);

/* Process one set of new samples at each fixed sampling interval. */
fixed_vqf_update_gyr(&filter, gyro);
fixed_vqf_update_acc(&filter, acc);
fixed_vqf_get_q30(&filter, quat);
```

`fixed_vqf_get_bias_q24()` returns the gyroscope bias, and `fixed_vqf_get_rest_detected()` returns the rest status. See the [documentation](docs/API.md) for the full API.

## Implementation

```text
vqf/c/           C99 fixed-point implementation, coefficients, and example
docs/            API, parameter, and testing documentation
tests/           Host numerical tests
tools/           Offline coefficient generation and test tools
```

Coefficients are generated offline by `tools/generate_coefficients.py`; target runtime calculations remain fixed-point. The current API uses one shared fixed sampling period and provides 6D fusion. Without a magnetometer, yaw is a relative heading.

## License

Licensed under the [MIT License](LICENSE). See [NOTICE](NOTICE.md) for copyright and license notices.
