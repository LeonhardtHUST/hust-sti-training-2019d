# 目录结构与代码归属

> 本文说明「新代码该写在哪里」以及 `platformio.ini` 为什么这样配。
> 模块边界与依赖方向另见 [架构说明](architecture.md)，工程硬约束见
> [工程规约](../AGENTS.md)，CubeMX 生成坑见 [工具实测](TOOLING-cubemx.md)。

## 1. 谁拥有哪些目录

| 路径 | 归属 | 放什么 | 参与编译的方式 |
| --- | --- | --- | --- |
| `.ioc` | 人 + CubeMX | 引脚 / 时钟 / 外设配置，**唯一事实源** | — |
| `Core/` | CubeMX + 人 | 生成物（`main.c`、`*_it.c`、`*_msp.c` 等）+ 既有 8 个业务模块 | `Core/Src/*.c` 全编 |
| `Core/Inc/xpulse.h` | 人 | 板级「信号 → 引脚」映射 | 头文件，CubeMX 不生成 |
| `app/` | 人 | **新写的业务代码** | `app/src/**` 全编，继承 `-Werror` |
| `lib/<name>/` | 人 | 可复用 / 第三方组件 | 仅被 `#include` 链拉入时编译 |
| `boards/` `docs/` `scripts/` | 人 | 板卡定义、文档、脚本 | 不参与 |

**本次只建立 `app/` 落点，没有迁移既有模块。** 8 个模块（`acquisition`、
`signal_analysis`、`signal_math`、`dds`、`ui`、`key`、`measurement`、
`uart_stream`）仍在 `Core/Src`，依赖方向以 [架构说明](architecture.md) 为准。
这是刻意的折中：模板已把业务代码统一放进 `app/`，但本仓库正在进行采集功能开发，
整批迁移会与未完成的模块实现纠缠在一起，留作后续独立重构。

## 2. 为什么新代码放 `app/` 而不是 `lib/` 或 `Core/`

`app/` 在 `src_dir` 内，因此三件事自动成立：源文件**无条件编译**（写坏了立刻报错，
不会静默躺着）、继承 `build_src_flags` 的 `-std=c11 -Wall -Wextra -Werror`、
与 `Core/Inc` 共用一套 include 路径。`lib/` 的按需编译是 PlatformIO 的设计意图，
适合第三方组件，但用来自写业务代码会踩两个静默盲区：

- 未被 `#include` 链拉入的库**根本不参与编译**，写坏了直到某天引用才炸；
- 库源码**不继承** `build_src_flags` 的 `-Werror`，要在 `library.json` 的
  `build.flags` 里重复写一遍；想强制全量编译可用 `lib_deps` 登记，代价是
  该库恒被编译、失去按需编译的收益。

在 `Core/` 的 USER CODE 块内写业务代码**本来是安全的**（`.ioc` 里
`ProjectManager.KeepUserCode=true`，重生成会保留块内内容）。把新逻辑放进 `app/`
不是消除覆盖风险，而是避免逻辑与生成物混在一起、在十几个 USER CODE 块之间反复
找位置；`main.c` 里的三处胶水（若将来引入 `app_init()` / `app_loop()`）是
CubeMX 生成文件里唯一需要长期维护的用户内容。

## 3. `src_dir = .` 的三条硬约束

`platformio.ini` 把 `src_dir` 设为项目根，`app/` 才能与 `Core/Src` 共用
`build_src_flags`。代价是下面三条**不能省**：

1. **`build_src_filter` 必须写 `+<...>` 白名单**。用 `+<*>` 会把项目树深处
   （`docs/XPulse/例程/`、`node_modules/` 等）的 `.c` 一起拉进编译——上游模板
   已用故意写错的探针文件实测复现。本仓 2026-11-06 全量构建复核：99 个编译对象，
   这两处误编行数均为 0。
2. **排除项要写全相对路径**：`-<Core/Src/system_stm32g4xx.c>`。沿用旧的
   `-<system_stm32g4xx.c>` 在 `src_dir = .` 下不生效，该文件会与
   `framework-stm32cubeg4` 的 CMSIS 重复定义符号（[工程规约](../AGENTS.md) §2.5）。
3. **头文件路径用相对写法** `-Iapp/inc`。写成 `-I${PROJECT_DIR}/app/inc`
   会被拼成畸形路径（上游模板实测，报找不到头文件）。编译命令里显示为
   `-Iapp\inc`（Windows 分隔符），属正常表现。

`+<app/src/*>` 会递归收进子目录，所以在 `app/src/` 下新增文件**不需要**改
`platformio.ini`。

## 4. 硬件浮点（同批同步）

G474 是 Cortex-M4F（单精度 FPv4-SP）。`build_flags` 里的
`-mfpu=fpv4-sp-d16` / `-mfloat-abi=hard` 必须成对出现，且只能进 CCFLAGS；
`scripts/pio-hard-fpu.py` 负责把同一对 flag 补进 `ASFLAGS` / `LINKFLAGS`，
否则链接期报 `uses VFP register arguments`。

- `double` **仍是软件模拟**：FPv4-SP 只有单精度硬件单元，不要以为开了硬浮点
  双精度就变快。
- 需要 `printf("%f")` 时另加 `-Wl,-u,_printf_float`；newlib-nano 默认不含
  浮点格式化。本仓当前业务代码没有用到。

## 5. 改了目录之后

- **必须重生成 `compile_commands.json`**（`pio run -t compiledb` 或
  `./scripts/init-clangd.sh`）。该文件被 `.gitignore` 忽略、不进提交，但
  `.clangd` 指向它；不重生成的话 VS Code 里 `app/` 源码没有补全与跳转。
- 新增 `app/` 源文件后跑一次 `pio run`（L3）；影响运行时行为的改动按
  [工程规约](../AGENTS.md) §3 还需 **L5 实板确认**，构建通过不等于能用。
- 若改用 `lib/<name>/` 存放组件，记得配 `library.json` 的 `build.flags`，
  未被引用的库不会自动编译。

## 6. 本仓同步记录（2026-11-06）

| 阶段 | `--print-memory-usage` | PlatformIO size |
| --- | --- | --- |
| WIP 基线（软浮点，`src_dir = Core/Src`） | RAM 8264 B / FLASH 9968 B | 6728 B |
| 补硬浮点（`fix(pio)`） | RAM 8264 B / FLASH 9988 B | 6728 B |
| 建 `app/` 落点（`build(app)`） | RAM 8264 B / FLASH 9988 B | 6728 B |

+20 B 来自硬浮点；建 `app/` 后体积不变，因为当时 `app/` 下没有 `.c`。
`arm-none-eabi-readelf -A firmware.elf` 得 `Tag_FP_arch: VFPv4-D16`、
`Tag_ABI_VFP_args: VFP registers`，HAL 侧 `stm32g4xx_hal_gpio.o` 同样带该属性。

**验证等级：L3**（三次 `pio run -t clean && pio run` 全量构建，均 SUCCESS、
`-Werror` 下零告警）。**未做 L5 实板**：本批改动未改变运行时逻辑，但采集帧、
DMA 波形与浮点运行结果仍需在板上确认，不能引用模板仓库的实板结论。
