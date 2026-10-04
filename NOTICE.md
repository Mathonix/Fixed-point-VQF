# 来源与许可说明

本仓库的定点融合代码提取自 Mathonix 的 CH32V203G6U Fixed-VQF IMU 工程，取用日期 2026-10-04。

原工程包含 Hugo Chiang 的 VQF-C 参考实现，其文件标注 `Copyright (c) 2024 Hugo Chiang` 与 `SPDX-License-Identifier: MIT`，并附带 `User/VQF-C-LICENSE.txt`。本仓库保留该许可全文及版权声明于 `LICENSE`。

定点实现由原项目在 VQF 算法与 C 参考实现基础上开发。本次提取保留工作区中的定点实现，不附带原工程的浮点参考实现、WCH 外设库、启动代码或板级固件。

原项目 HEAD 只标识提取时的已提交版本；本次实际来源包含本地未提交修改，各原始文件的 SHA-256 列于 `docs/source_manifest.json`，不能将这些源文件全部视为该 HEAD 中的内容。
