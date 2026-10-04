# 提取验证记录

验证日期：2026-10-04。

- 原工作区 `fixed_vqf.c`、`fixed_vqf.h`、`fixed_vqf_tuning.h`、`imu_numeric.h` 与提取副本逐字节一致。Git 按 `.gitattributes` 统一文本换行，来源清单保留原始字节哈希。
- Windows / Cygwin GCC 15.0.1：`python tools/run_tests.py` 的全部 **12 个配置通过**，编译开启 `-Wall -Wextra -Werror`。
- 离线生成器输出与已提交系数头一致。
- CMake 4.2.2 / Ninja / GCC，Release 静态库与示例构建成功，CTest **3/3 通过**；Release 测试保持断言启用。
- 水平静止示例输出四元数 `[1073741824,0,0,0]`（Q30），静止标志为 `1`。
- 本机 GCC 缺少 `libubsan`，本地未执行 UBSan。GitHub Actions 中使用 Ubuntu GCC 执行 `--sanitize` 矩阵；结果以仓库 Actions 中对应提交为准。

本次没有刷写板卡或执行新的硬件测试，原固件工作区不受修改。主机数值测试不等同于目标 MCU 的实时性或实测精度验证。
