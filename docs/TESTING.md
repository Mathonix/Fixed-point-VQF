# Testing / 测试

Run the host matrix with Python 3 and a GCC-compatible compiler:

使用 Python 3 和兼容 GCC 的编译器执行主机测试矩阵：

```sh
python tools/run_tests.py
```

The matrix runs 3 suites at 1000 / 2000 Hz and both raw gyro ranges (12 configurations). It checks original VQF default coefficients and covariance equations, filter stability, 90-second rest bias convergence, analytical rotations, rest transitions at 1.5 seconds, all-zero acceleration handling, and int16 conversion boundaries. It also verifies the generated header against the generator.

测试覆盖 2 种采样率和 2 种原始角速度量程下的 3 套用例，共 12 个配置。检查默认参数系数与协方差公式、滤波稳定性、90 秒静止零偏收敛、解析旋转、1.5 秒静止状态转换、零加速度处理和 int16 转换边界，并核对生成系数头。

When UBSan is available:

编译器具备 UBSan 运行库时：

```sh
python tools/run_tests.py --sanitize
```

GitHub Actions executes the sanitizer matrix, CMake Release build, and CTest. Assertions remain enabled in Release tests. Host numerical tests do not measure target MCU timing or hardware sensor accuracy.

GitHub Actions 执行上述数值检查、CMake Release 构建与 CTest。Release 测试保留断言。主机测试不测量目标 MCU 实时性或传感器硬件精度。
