# API and fixed-point formats / 接口与定点格式

Include `fixed_vqf.h` from `vqf/c/`. Initialize every `fixed_vqf_t` instance before use. Update gyroscope first, then acceleration, at the configured fixed sampling interval. Use valid pointers and serialize access to each state.

引用 `vqf/c/fixed_vqf.h`，每个实例使用前先初始化。按照固定采样周期，先更新陀螺仪，再更新加速度计。指针必须有效，同一实例应串行访问。

| API | Purpose / 用途 |
|---|---|
| `fixed_vqf_init(s)` | Initialize or reset a state / 初始化或复位 |
| `fixed_vqf_update_gyr(s, gyr)` | Integrate angular velocity / 角速度更新 |
| `fixed_vqf_update_acc(s, acc)` | Inclination correction and bias estimation / 倾角校正与零偏估计 |
| `fixed_vqf_get_q30(s, q)` | Get 6D quaternion `[w,x,y,z]` / 获取六轴四元数 |
| `fixed_vqf_get_bias_q24(s, bias)` | Get bias in Q7.24 rad/s / 获取零偏 |
| `fixed_vqf_get_bias_q16(s, bias)` | Get bias in Q16.16 rad/s / 获取零偏 |
| `fixed_vqf_get_rest_detected(s)` | Get rest flag / 获取静止标志 |

| Value / 数据 | Format and unit / 格式与单位 |
|---|---|
| Gyroscope / 角速度 | `int32_t`, Q7.24 rad/s; physical value = integer / `2^24` |
| Acceleration / 加速度 | `int32_t`, Q1.30 g; physical value = integer / `2^30` |
| Quaternion / 四元数 | `int32_t[4]`, Q1.30, `[w,x,y,z]` |
| Internal bias / 内部零偏 | `int64_t[3]`, Q31.32 rad/s |
| Covariance / 协方差 | `int64_t[9]`, 20 fractional bits / 20 位小数 |

The quaternion rotates the sensor frame into the fusion reference frame. Use consistent axes for gyro and acceleration; a level stationary sensor has acceleration `[0,0,+1 g]`. Each acceleration axis is limited to `[-2,2)` g. The caller must reject saturated samples. All-zero acceleration inputs are ignored.

四元数将传感器坐标旋转到融合参考坐标。角速度和加速度必须采用一致的坐标轴；水平静止样例加速度为 `[0,0,+1 g]`。每个加速度轴的范围为 `[-2,2)` g，调用方需剔除饱和样本；全零加速度输入被忽略。

The API has no variable `dt`. Sampling code must handle missing or duplicate samples. Relative yaw can drift without an absolute heading reference. Global diagnostic counters (`fixed_vqf_acc_reject_count`, `fixed_vqf_kalman_update_count`, `fixed_vqf_kalman_reject_count`) are shared across instances.

接口不支持可变 `dt`，采样层应处理丢样或重复样本。没有绝对航向参考时，相对 yaw 可能漂移。三个全局诊断计数器由所有实例共享。

Optional sensor helpers: `fixed_vqf_gyro_raw_to_q24()` converts LSM6DSV raw gyro readings; `LSM6DSV_GYRO_FS_2000DPS=0` selects ±125 dps, and `1` selects ±2000 dps. CMake exposes `FIXED_VQF_GYRO_2000DPS`. `imu_accel_raw_to_q30()` accepts a scale factor and returns a saturation flag. Other sensors can directly supply inputs in the documented units.

可选传感器辅助函数：`fixed_vqf_gyro_raw_to_q24()` 转换 LSM6DSV 原始角速度；上述宏 `0` 对应 ±125 dps，`1` 对应 ±2000 dps。CMake 使用 `FIXED_VQF_GYRO_2000DPS` 选项。`imu_accel_raw_to_q30()` 接收缩放系数并提供饱和标志。其他传感器可直接提供符合上述单位的输入。

For CMake integration, disable host executables and link the library:

作为 CMake 子项目使用时，可关闭主机程序并链接库：

```cmake
set(FIXED_VQF_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FIXED_VQF_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(Fixed-VQF)
target_link_libraries(your_firmware PRIVATE FixedVQF::fixed_vqf)
```

Use the same sample-rate macros in the library and its callers. C++ callers are supported through C linkage.

库和调用方应使用一致的采样率宏。头文件提供 C 链接声明，支持 C++ 工程调用。
