# 库封装范式（`lib/<name>/` + `library.json`）

> 本文只讲可复用组件怎么放进 `lib/<name>/`，以及 `library.json`、`lib_deps`、
> `build.flags` 各管什么。本仓当前**没有** `lib/` 目录，也没有内置任何外设驱动；
> 文中的 `<name>`、`src/`、`inc/` 都是占位名，不代表本仓已有这些代码。
>
> 业务代码的落点另见 [目录结构](LAYOUT.md)：既有 8 个模块仍在 `Core/Src`，新写的
> 业务代码放 `app/`。不要把业务整体搬进 `lib/`，两者的编译模型不同。

## 1. 什么时候用 `lib/`

适合放：

- 与具体工程无关的可复用组件，例如协议解析、显示驱动、纯算法；
- 需要在本仓之外复用的代码；
- 不想让 CubeMX 重生成碰到的代码（CubeMX 只生成 `Core/`，不会覆盖 `lib/`）。

不要放：

- 新写的业务逻辑，放 `app/`（见 [目录结构](LAYOUT.md)）；
- 既有的 8 个业务模块，留在 `Core/Src`，不要为了引入 `lib/` 专门迁移；
- `Drivers/`、`Middlewares/`、CubeMX 的 `startup_*.s` 或 `system_stm32g4xx.c`：
  前两类由 `framework-stm32cubeg4` 提供，后几类放进 `lib/` 会造成重复定义。

## 2. `library.json` 范式

放在 `lib/<name>/library.json`，建议的目录与字段：

```text
lib/<name>/
  library.json
  inc/<name>.h
  inc/port/...        # 子目录头文件用 #include "port/xxx.h" 引用
  src/<name>.c
  src/port/...
```

```json
{
  "name": "<name>",
  "version": "1.0.0",
  "description": "<一句话说明这个组件做什么>",
  "license": "MIT",
  "build": {
    "srcDir": "src",
    "includeDir": "inc",
    "srcFilter": [
      "+<*.c>",
      "+<port/*.c>"
    ],
    "flags": [
      "-std=c11",
      "-Wall",
      "-Wextra",
      "-Werror"
    ]
  }
}
```

| 字段 | 作用 | 注意 |
| --- | --- | --- |
| `name` | 库名，`lib_deps` 按它登记 | 不要和目录名混用；登记时用这里的值 |
| `version` / `description` / `license` | 清单元数据 | 本地库也写清楚，别留空 |
| `build.srcDir` | 源文件根目录，相对 `library.json` | 默认 `src/`（没有则库根目录）；显式写出来便于看清布局 |
| `build.includeDir` | 对外暴露的头文件目录 | 这个目录会加到依赖方的 include 路径；`srcDir` 也会被 PlatformIO 一并加入，但不要把实现目录当公共头文件目录 |
| `build.srcFilter` | 参与编译的源文件白名单 | 用 `+<...>` 逐项列出；不写时默认排除 `.git/`、`.svn/`、`example/`、`examples/`、`test/`、`tests/`，`src/` 下的 `*_test.c` 仍会被编进来 |
| `build.flags` | 这个库自己的编译选项 | 见 §2.1；`build_src_flags` 不会自动作用到 `lib/` |
| `build.unflags` | 从继承的选项里去掉指定 flag | 需要时再用 |

`library.json` 里**没有** `includeDirs` 这个字段。PlatformIO Core 6.2 的
`piolib.py` 只读 `includeDir`（以及 `srcDir` 默认加入 include 路径），所以多目录
头文件不要指望写一个列表就生效：把公共头文件放在 `inc/` 下按子目录组织，或者放到
`src/` 下再用相对路径引用。

常见变体：

- 源文件和头文件都在库根目录：`"srcDir": "."`、`"includeDir": "."`，
  `"srcFilter": ["+<foo.c>", "+<bar.c>"]`。
- 只做头文件库：省略 `srcFilter`，保留 `"includeDir": "inc"`。
- 目录里混了不该编译的源码：用 `-<...>` 排除，例如
  `"srcFilter": ["+<*.c>", "-<*_test.c>"]`。

### 2.1 `lib/` 不继承 `-Werror`

`platformio.ini` 的 `build_src_flags` 只作用于 `src_dir` 下的源码（本仓是
`Core/Src/*` 与 `app/src/*`），`lib/` 里的 `.c` 拿不到 `-std=c11 -Wall -Wextra -Werror`。
把代码从 `Core/Src` 或 `app/` 挪到 `lib/`，等于撤掉严格告警门禁。

补救办法是在 `library.json` 的 `build.flags` 里显式写同一组选项：

```json
"build": {
  "flags": ["-std=c11", "-Wall", "-Wextra", "-Werror"],
  "srcDir": "src",
  "includeDir": "inc"
}
```

这样写坏的库源码会在编译期报错，而不是只留一条告警。自写业务代码放 `app/` 则自动
继承 `build_src_flags`，不需要重复这段配置。

### 2.2 未被 `#include` 的库不参与编译

PlatformIO 默认的 LDF 模式只把被源码 `#include` 链拉进来的库交给编译器。往
`lib/<name>/` 放一份没有被任何文件引用的 `.c`，`pio run` 不会编译它，语法错误也
不会暴露。这不影响不用的库，但写坏的库会一直躺着，直到某天被引用。

需要强制全量编译时，在 `platformio.ini` 现有的 `[env:genericSTM32G474VE]` 段里登记：

```ini
lib_deps =
  <name>
```

`<name>` 取 `library.json` 的 `name`。登记后这个库会参与编译并生成
`lib<name>.a`；函数是否进入最终固件仍受 `-ffunction-sections` 与
`-Wl,--gc-sections` 影响，但编译门禁会完整走一遍。

注意两件事：

- `lib_deps` 管「编不编」，`build.flags` 管「门禁严不严」，两者正交。要「全量且
  严格」得同时配。
- 不要用 `lib_ldf_mode = off` 来指望全量编译本地 `lib/`。它会关闭 include 扫描，
  只保留 `lib_deps` 指定的依赖；本仓没有复测这条路径，不要拿它替代 `lib_deps`。

## 3. 接入流程

1. 外设在 `.ioc` 里配好，按 [工程规约](../AGENTS.md) §2.3 做 L4 生成核验
   （日志无 `Cannot map`、`main.c` 有对应 `MX_xxx_Init()`）。
2. 把可复用源码放进 `lib/<name>/src/`，公共头文件放进 `inc/`。`MX_xxx_Init()`、
   中断入口和 `stm32g4xx_hal_msp.c` 留给 CubeMX 生成，不要在库里重复初始化外设。
3. 写 `library.json`，按 §2 显式声明 `srcDir`、`includeDir`、`srcFilter`、`flags`。
4. 在 `app/` 或 `Core/Src` 的业务代码里 `#include` 库头文件；需要全量编译门禁时
   在 `platformio.ini` 的 `lib_deps` 登记 `name`。
5. 跑 `pio run`（L3）。纯 C、不依赖 HAL 的部分可以用主机 GCC 单独编译做 L2；
   影响运行时行为的改动按 [工程规约](../AGENTS.md) §3 做 L5 实板确认，构建通过
   不等于能用。
6. 改过 `lib/` 后重新生成 `compile_commands.json`（`pio run -t compiledb` 或
   `./scripts/init-clangd.sh`），否则 VS Code 里没有补全和跳转。

## 4. 与 `app/`、`Core/Src` 的边界

| 位置 | 放什么 | 编译与告警 |
| --- | --- | --- |
| `app/src/` | 新写的业务代码 | 无条件参与编译，继承 `-Werror` |
| `Core/Src/` | 既有 8 个业务模块 + CubeMX 生成物 | 全编，继承 `-Werror`；生成文件只改 USER CODE 块 |
| `lib/<name>/` | 可复用 / 第三方组件 | 按需编译；严格告警要在 `build.flags` 里自己写 |

本仓的既有 8 个模块仍在 `Core/Src`，这是刻意保留的例外，[目录结构](LAYOUT.md) §1
有说明。引入 `lib/` 不改变这条约定；新业务代码照旧放 `app/`，不要因为某个功能看起来
「可复用」就把整块业务塞进 `lib/`。

## 5. 常见坑

- 把 `startup_stm32g474xx.s`、`system_stm32g4xx.c`、`Drivers/` 拷进 `lib/`：与
  framework 里的同名对象重复定义，链接期报错。
- `srcFilter` 不写：`src/` 下除 `test/`、`tests/` 等默认排除目录外的源文件都会被收进来，
  测试文件、临时备份也可能一起编译。
- 公共头文件放 `src/port/` 却只暴露 `includeDir = inc`：依赖方找不到头文件。要么把
  头文件放 `inc/` 的子目录、用 `#include "port/xxx.h"`，要么把 `src/` 作为
  `includeDir`（不推荐，会把实现目录也暴露出去）。
- 库没有被 `#include` 却指望它参与编译：默认不会，登记 `lib_deps` 才会。
- 只配 `lib_deps` 不配 `build.flags`：库会编译，但仍不带 `-Werror`。
