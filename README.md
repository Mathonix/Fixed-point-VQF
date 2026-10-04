# Fixed-VQF：定点四元数姿态估计

[中文](README.md) | [English](README.en.md)

Fixed-VQF 是 VQF（Versatile Quaternion-based Filter）的 C99 定点实现，用于嵌入式 IMU 姿态估计。融合计算使用整数运算，无动态内存分配，适合不带浮点单元的处理器。

本实现提供无磁力计的 **6D 传感器融合**，包括陀螺仪积分、加速度计倾角校正、陀螺仪零偏估计和静止检测，输出 `[w, x, y, z]` 四元数。默认算法参数采用原版 VQF 的 6D 设置，转换为定点系数。

VQF 算法：D. Laidig 与 T. Seel，Information Fusion，2023。[论文](https://doi.org/10.1016/j.inffus.2022.10.014) · [原版 VQF](https://github.com/dlaidig/vqf)。

## 文档

- [接口与定点格式](docs/API.md)
- [默认参数](docs/PARAMETERS.md)
- [测试说明](docs/TESTING.md)

## 编译

使用 C99 编译器及 CMake 3.16 或更高版本：

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

默认采样率为 2000 Hz；使用 `-DFIXED_VQF_SAMPLE_HZ=1000` 可编译为 1000 Hz。采样率决定积分步长和离散滤波系数，算法参数的物理含义保持一致。

也可将 `vqf/c/fixed_vqf.c` 加入工程，并将 `vqf/c/` 加入头文件搜索路径。库本身不依赖 `libm`、操作系统或 MCU 外设库。

## 使用

```c
#include "fixed_vqf.h"

fixed_vqf_t filter;
q24_t gyro[3] = {0, 0, 0};          /* rad/s，Q7.24 */
q30_t acc[3] = {0, 0, 1073741824};  /* g，Q1.30 */
q30_t quat[4];

fixed_vqf_init(&filter);

/* 每个固定采样周期处理一组新样本。 */
fixed_vqf_update_gyr(&filter, gyro);
fixed_vqf_update_acc(&filter, acc);
fixed_vqf_get_q30(&filter, quat);
```

`fixed_vqf_get_bias_q24()` 返回陀螺仪零偏，`fixed_vqf_get_rest_detected()` 返回静止状态。完整接口见[文档](docs/API.md)。

## 实现

```text
vqf/c/           C99 定点实现、系数和使用示例
docs/            接口、参数和测试文档
tests/           主机数值测试
tools/           离线系数生成与测试工具
```

系数由 `tools/generate_coefficients.py` 离线生成；目标运行时保持定点运算。当前接口使用统一的固定采样周期，提供 6D 融合。无磁力计时，yaw 为相对航向。

## 许可证

采用 [MIT 许可证](LICENSE)。版权与许可声明见 [NOTICE](NOTICE.md)。
