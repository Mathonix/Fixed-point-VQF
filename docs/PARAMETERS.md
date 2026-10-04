# Default parameters / 默认参数

The 6D algorithm uses the [original VQF defaults](https://vqf.readthedocs.io/en/latest/ref_cpp_params.html). Values are converted to integer coefficients offline and quantized to the formats used by the library.

六轴算法采用原版 VQF 默认参数，离线转换为定点系数。数值会按对应格式量化。

| Parameter / 参数 | Default / 默认值 |
|---|---|
| `tauAcc` | 3.0 s |
| `motionBiasEstEnabled` | true |
| `restBiasEstEnabled` | true |
| `biasSigmaInit` | 0.5 deg/s |
| `biasForgettingTime` | 100 s |
| `biasClip` | 2.0 deg/s |
| `biasSigmaMotion` | 0.1 deg/s |
| `biasVerticalForgettingFactor` | 0.0001 |
| `biasSigmaRest` | 0.03 deg/s |
| `restMinT` | 1.5 s |
| `restFilterTau` | 0.5 s |
| `restThGyr` | 2.0 deg/s |
| `restThAcc` | 0.5 m/s² |

The current implementation does not use magnetometer parameters. The sampling rate is a separate compile-time setting (`FIXED_VQF_SAMPLE_HZ`, 1000 or 2000 Hz); it is not an algorithm tuning parameter.

当前实现不使用磁力计参数。采样率由编译宏 `FIXED_VQF_SAMPLE_HZ` 单独指定（1000 或 2000 Hz），不属于算法调参。

The coefficient generator is [tools/generate_coefficients.py](../tools/generate_coefficients.py). After changing a physical parameter in its `DEFAULTS`, regenerate the header and rerun the numerical tests:

参数定义和系数生成入口如下；修改物理参数后应重新生成头文件并执行数值测试：

```sh
python tools/generate_coefficients.py
python tools/run_tests.py
```

The covariance calculation follows VQF's `P0`, `V`, `Wmotion`, `Wvertical`, and `Wrest` equations. A one-LSB positive covariance floor and symmetric rounding are numerical safeguards for integer arithmetic, not additional physical tuning parameters.

协方差按 VQF 的 `P0`、`V`、`Wmotion`、`Wvertical`、`Wrest` 公式生成。单个定点 LSB 的正协方差下限及对称舍入属于整数运算的数值保护，不是额外物理参数。
