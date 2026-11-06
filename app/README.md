# app/：新业务代码的落点

本目录由 2026-11-06 的模板结构同步建立，目前**没有参与编译的源文件**，
`src/` 与 `inc/` 里的 `.gitkeep` 只用于让空目录能入库。

- `src/`：放新写的业务 `.c`。`platformio.ini` 的 `+<app/src/*>` 会递归收进子目录，
  新增文件不需要改构建配置；这些文件**无条件参与编译**，并继承
  `build_src_flags` 的 `-std=c11 -Wall -Wextra -Werror`。
- `inc/`：放对应头文件，已通过 `-Iapp/inc` 加入全局 include 路径。

**既有模块暂不迁移**：`Core/Src/*.c` 下的 9 个业务模块（`acquisition`、
`signal_analysis`、`dds`、`ui`、`key`、`measurement`、`uart_stream` 等）仍在原位，
模块边界见 [架构说明](../docs/architecture.md)。新写的代码放这里，不要继续往
CubeMX 生成文件里加逻辑；在 `Core/` 的 USER CODE 块内写本来是安全的
（`KeepUserCode=true`），只是要反复在生成物里找位置、长期维护成本更高。

目录职责、`src_dir = .` 的三条硬约束与验证记录见 [目录结构](../docs/LAYOUT.md)。

> 放在 `lib/` 里的组件是另一套编译模型：未被 `#include` 链拉入的库不参与编译，
> 且库源码不继承 `-Werror`。业务代码不要放 `lib/`。
