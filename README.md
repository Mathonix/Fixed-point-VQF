# Fixed-VQF

从 [CH32V203G6U Fixed-VQF IMU](https://github.com/Mathonix/CH32V203G6U-Fixed-VQF-IMU) 提取的 **C99 定点 Full-VQF 六轴姿态融合库**。保留陀螺仪积分、加速度计倾角校正、静止检测、静止零偏估计和运动零偏估计。融合核心只使用整数运算，无堆分配；不依赖 CH32 外设库、传感器驱动或 RTOS。

本仓库取自 **2026-10-04 的本地工作区**，包含原项目尚未提交的最新定点 VQF 修改。原项目 HEAD 与原始文件 SHA-256 见 [来源清单](docs/source_manifest.json)。这是独立算法库；板级采样、六面校准、UART/CAN、OTA、欧拉角输出和 ZARU 航向保持由应用层实现。

## 功能和参数

- 编译时选择 **1000 Hz / 2000 Hz**，默认 2000 Hz；陀螺仪和加速度计使用同一固定采样周期。
- 三档运行时参数，初始化默认 balanced；非法档号回退 balanced。
- 运行时切换保留四元数、零偏和协方差，并适配低通滤波状态。
- 当前主机 ABI 下每个 `fixed_vqf_t` 状态为 **496 字节**，实际移植应检查目标编译器的 `sizeof`。
- 附带 LSM6DSV 原始陀螺仪转换，支持 ±125 dps / ±2000 dps；其他传感器可直接提供已缩放的输入。

| ID | 档位 | 加速度时间常数 | 静止零偏 sigma | 陀螺仪低通系数对应截止频率 |
|---|---|---|---|---|
| 0 | fast | 1.0 s | 0.03 deg/s | 120 Hz |
| 1 | balanced | 2.5 s | 0.02 deg/s | 80 Hz |
| 2 | stable | 5.0 s | 0.01 deg/s | 40 Hz |

`fixed_vqf_gyro_coeffs()` 返回供应用层使用的二阶低通系数 `[b0,b1,b2,a1,a2]`。融合核心不会自动用这组系数预滤波陀螺仪输入。其余公共参数为静止低通 0.5 s、静止持续时间 1 s、静止阈值 0.6 deg/s 与 0.15 m/s²、零偏裁剪 2 deg/s。

## 数据格式

| 数据 | 格式 / 单位 |
|---|---|
| 陀螺仪输入 `q24_t[3]` | signed Q7.24，rad/s，整数除以 `2^24` 得到物理值 |
| 加速度输入 `q30_t[3]` | signed Q1.30，g；`1073741824` 表示 1 g |
| 四元数输出 `q30_t[4]` | Q1.30，顺序 `[w,x,y,z]`，将传感器坐标旋转到融合参考坐标 |
| 内部零偏 `gyro_bias_q32[3]` | `int64_t` Q31.32，rad/s |
| `fixed_vqf_get_bias_q24()` | Q7.24，rad/s |
| `fixed_vqf_get_bias_q16()` | Q16.16，rad/s |
| 内部协方差 | `int64_t`，20 位小数 |

坐标轴必须保持一致，水平静止样例为加速度 `[0,0,+1 g]`。传感器安装方向由调用方映射。六轴融合没有磁力计，yaw 为相对航向，仍可能漂移。

Q1.30 的轴范围为 `[-2,2)` g。调用方需要识别并跳过饱和加速度帧；库只自动忽略全零加速度输入。`imu_accel_raw_to_q30()` 提供带饱和标志的转换辅助函数。保持原工程缩放时，±2 g 档使用 `65536`，±4 g 档使用 `131072`；其他传感器须按自身灵敏度换算。

## 最小集成

将 `src/fixed_vqf.c`、`src/fixed_vqf_tuning.h` 与 `include/` 加入工程。所有使用本库的编译单元采用相同采样率和传感器量程宏。

```c
#include "fixed_vqf.h"

static fixed_vqf_t state;

void fusion_init(void)
{
    fixed_vqf_init(&state); /* balanced */
    fixed_vqf_set_profile(&state, 1);
}

/* 在固定周期中，针对同一时刻的新样本先更新 gyro，再更新 acc。 */
void fusion_sample(const q24_t gyro[3], const q30_t acc[3], q30_t q[4])
{
    fixed_vqf_update_gyr(&state, gyro);
    fixed_vqf_update_acc(&state, acc);
    fixed_vqf_get_q30(&state, q);
}
```

必须先初始化状态，参数指针必须有效。接口没有可变 `dt`，掉样和重复样本应由采样层处理。每个状态应串行更新。三个 `fixed_vqf_*_count` 诊断计数器是全局计数，多个实例共享它们。

## 编译与测试

需要 C99 编译器和 CMake 3.16+。融合库本身不依赖 `libm`；主机数值测试使用浮点解析参考值进行校验。

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

生成的 `vqf_stationary` 示例运行两秒水平静止样本，输出 Q30 四元数和静止标志。可执行文件位于构建目录中；多配置生成器需要在上述命令中使用 `--config Release` / `-C Release`。

选择 1 kHz 和 ±2000 dps 原始转换：

```sh
cmake -S . -B build-1k -DFIXED_VQF_SAMPLE_HZ=1000 -DFIXED_VQF_GYRO_2000DPS=ON
cmake --build build-1k
```

完整主机测试矩阵需要 Python 3 和 GCC 兼容编译器：

```sh
python tools/run_tests.py
# 编译器具备 UBSan 运行库时：
python tools/run_tests.py --sanitize
```

覆盖 2 种采样率 × 2 种陀螺仪量程 × 3 套测试：三档系数/DC 增益/极点、90 秒静止零偏收敛、档位切换状态保持、三轴解析旋转、静止进入/退出、全零加速度处理及完整 int16 原始转换边界。测试会校验生成系数与已提交头文件一致。GitHub Actions 执行同一矩阵，并验证 CMake 构建。

系数重新生成：

```sh
python tools/vqf_coefficients.py
```

Python 中的浮点计算只用于离线生成系数；目标融合核心保持定点运算。

## 嵌入式移植

默认 `sections.h` 的代码段标记为空。CH32 工程若使用原来的 `.fasttext` / `.slowtext` 链接布局，可定义 `FIXED_VQF_USE_CH32_SECTIONS=1`（CMake 对应选项设为 `ON`），并在目标链接脚本中正确映射这些段。其他平台通常保持默认即可。

采用 CMake 子项目时，可关闭主机测试和示例，然后链接 `FixedVQF::fixed_vqf`：

```cmake
set(FIXED_VQF_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FIXED_VQF_BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory(Fixed-VQF)
target_link_libraries(your_firmware PRIVATE FixedVQF::fixed_vqf)
```

本次提取的主机测试不代表在新硬件上的实时性能、采样精度或运动场景误差验证。目标平台需实测采样时序、CPU 负载和漂移。

## 文件与来源

```text
include/       公共 API、数值辅助、可选代码段配置
src/           定点融合核心与生成系数
examples/      水平静止示例
tests/         系数/零偏/运动/数值边界测试
tools/         系数生成器和主机测试入口
docs/          原工作区来源清单与验证记录
```

算法核心、公共 API、数值辅助和系数头从原工作区直接复制；提取时修改了路径、独立构建配置与代码段开关，未修改融合数值逻辑。许可信息保留原项目附带的 [MIT 许可及 Hugo Chiang 版权声明](LICENSE)，详见 [NOTICE](NOTICE.md)。
