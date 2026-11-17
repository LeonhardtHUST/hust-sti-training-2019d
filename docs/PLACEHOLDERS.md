# 模板占位符与初始条件（PLACEHOLDERS）

本文件列出**复制本模板后必须人工核对的占位符**，以及无法靠代码消除的「初始条件」。
新建工程时请逐条过一遍；agent 拿到本模板后也应以本文件为清单先做核对。

> 约定：本模板中**没有任何**指向原作者机器的绝对路径是「可用的」——凡是绝对路径都属于占位符，
> 换机器必须重新确认，不要照抄。

---

## A. 工程名相关（上游模板有脚本，本仓库没有）

上游模板用 `scripts/init-project.mjs`（`vpr init` / `pnpm run init`）一次性替换工程名；
**本仓库是从模板派生后手工落盘的，没有该脚本**，工程名相关的引用只能人工同步。
以后若要改名，下列位置必须一起改，漏一处就会出现指向不存在文件的引用：

| 位置 | 当前值 |
| ---- | ------ |
| 根目录 `.ioc` 文件名 | `template-mcu-minimal-STM32G474VET6-XPulse.ioc` |
| `.ioc` 内 | `ProjectManager.ProjectName` / `ProjectManager.ProjectFileName` |
| `scripts/cubemx-generate.mx` | 第一行 `config load <当前 .ioc>` |
| `README.md` | 标题与正文里的工程名 |
| `package.json` | `name`（slug） |

目录名、`git remote`、探针、板级参数、CubeMX 安装路径、工具版本不在上表，见 §B。

---

## B. 必须人工核对（脚本未覆盖）

| # | 位置 | 当前值 / 占位符 | 应改成 | 说明 |
| - | ---- | -------------- | ------ | ---- |
| B1 | `platformio.ini` 的 `upload_protocol` / `debug_tool` | `stlink` / `stlink` | 换 CMSIS-DAP 时两项都改成 `cmsis-dap` | 默认探针是 ST-Link + SWD。本模板**刻意不入库** `launch.json`（已被 `.gitignore` 忽略），换探针时要自己配调试器的 `configFiles`，不要指望仓库里有现成文件。 |
| B2 | `boards/genericSTM32G474VE.json` 的 `mcu` / `maximum_size` / `maximum_ram_size` | `stm32g474vet6` / 524288 / 131072 | 换芯片型号时三处一起改 | `mcu` 同时决定启动文件与链接脚本的自动匹配，只改容量会烧错。还要同步 `.ioc` 的 `Mcu.CPN`、`Mcu.Name`、`Mcu.Package`、`ProjectManager.DeviceId` 并重新生成。 |
| B3 | `boards/*.json` 的 `f_cpu` | `170000000L`（与 `.ioc` 的 `RCC.SYSCLKFreq_VALUE=170000000` 一致） | 换板或改主频时与 `.ioc` 一起改 | 本仓已对齐到 170 MHz。`scripts/check-clock.py` 在每次 `pio run` 前读 `.ioc` 的 `RCC.SYSCLKFreq_VALUE` 与 `boards/<BOARD>.json` 的 `build.f_cpu`，两者不等就让构建失败；改 `.ioc` 时钟后忘了同步这里会被它拦下。`f_cpu` 只决定 `F_CPU` 等工具链宏，不改变真实时钟，真实主频以 `.ioc` 的 RCC 为准。 |
| B4 | `.vscode/tasks.json` 的 `windows.command` / `linux.command` | `STM32CubeMX.exe` / `STM32CubeMX`（依赖 PATH） | 本机 CubeMX 不在 PATH、或安装到非默认目录时改成绝对路径 | 本机实测装在 `C:/ST/STM32CubeMX/STM32CubeMX.exe`（**不是**官方安装器默认的 `C:/Program Files/STMicroelectronics/...`）；这是本机值，换机器要重新确认。定位方法见 [CubeMX 工具实测](TOOLING-cubemx.md)。 |
| B5 | `platformio.ini` 的版本锁定 | `ststm32@20.0.0`、`framework-stm32cubeg4@1.6.3`、`toolchain-gccarmnoneeabi@1.70201.0`、`tool-ldscripts-ststm32@0.3.0`、`tool-openocd@3.1200.7` | 升级时要一起升级 | 与 CubeMX 的 FW_G4 组成验证组合，升级须**同时**验证「生成代码 + `pio run`」，不要单独升 HAL。 |
| B6 | `.ioc` 与 CubeMX 版本 | `.ioc` 写回 `MxCube.Version=6.18.1`、`MxDb.Version=DB.6.0.181` | 以 exe 属性 / 运行日志为准 | 版本戳可能是历史继承值；迁移后审阅工具写回并验证生成 + 构建。 |
| B7 | 仓库自身 | `origin` = `hust-sti-training-2019d` | 保持指向本仓库 | `git remote -v` 若仍指向模板仓库，提交会推到模板去；换机克隆后再确认一次。 |
| B8 | `README.md` 第 1 行 | 仍是模板名 `template-mcu-minimal-STM32G474VET6-XPulse` | 本工程名 | 本仓库没有 `vpr init`，改名时手工同步这一行与 §A 表格里的各处。 |

---

## C. 环境前提（不是文件，但缺了就跑不起来）

| # | 前提 | 检查方式 | 缺失后果 |
| - | ---- | -------- | -------- |
| C1 | STM32CubeMX 6.x 已安装 | 看 `tasks.json` 里的命令能否启动；本机在 `C:/ST/STM32CubeMX/STM32CubeMX.exe` | 无法改 `.ioc`、无法生成 `Core/` |
| C2 | 已安装 FW_G4 包（V1.6.3，与 PlatformIO 包同版本） | CubeMX → Help → Manage embedded software packages；本机通常在 `<用户目录>/STM32Cube/Repository/STM32Cube_FW_G4_V1.6.3` | `project generate` 失败 |
| C3 | PlatformIO Core | `pio --version` | 无法构建 / 烧录 |
| C4 | 主机 GCC | `gcc --version` | L2 主机回归跑不了；`pio run` 不受影响 |
| C5 | Node.js ≥ 20 + pnpm / npm / `vpr` 之一 | `node --version`、`pnpm --version` | `scripts/` 里的初始化、clangd、删除示例命令与 commitlint 校验跑不了；**不参与固件编译** |
| C6 | 默认 ST-Link 走 SWD；目标板正确供电、共地，VTref 接线按探针定义 | 看探针型号与接线 | 探针与固件不匹配或目标无电会连接失败 |
| C7 | 首次 `pio run` 需联网下载 ststm32 平台与 CubeG4 包 | — | 离线首次构建失败 |

---

## D. 已知陷阱（不改也会踩）

| # | 陷阱 | 现状 | 建议 |
| - | ---- | ---- | ---- |
| D1 | 保存时重排 `Core/Src/main.c` | 本仓 `.vscode/settings.json` 目前**没有**开 `editor.formatOnSave`，但用户级 / 其它工作区设置或 clang-format 插件仍可能开启；一旦开启，保存生成文件会产生与 CubeMX 无关的巨大 diff | 处理 `Core/**` 前确认 formatOnSave 已关，或加 `.clang-format` + `// clang-format off` 保护，只保留与任务相关的 diff |
| D2 | `.ioc` 行尾 | CubeMX 写 **CRLF**；本仓 `.gitattributes` 是 `* text=auto eol=lf`，Git 入库 / checkout 与编辑器统一 LF，`text` 属性在 add / 比对时归一化 | 不必手工转回 CRLF；不要提交纯行尾变化，异常时用 `git ls-files --eol` 核对。混用 LF / CRLF 会让 `.ioc` 键解析错乱（见 [CubeMX 工具实测](TOOLING-cubemx.md) §3.9） |
| D3 | `.ioc` 内的参数白名单 | 每个外设一行 `<IP>.IPParameters=a,b,c`，每个引脚一行 `<Pin>.GPIOParameters=...`；只有列在白名单里的 key 才被识别 | 手改 `.ioc` 加参数时**必须同步补进对应白名单行**，否则 CubeMX 静默忽略。例如初始电平要 `PC2.GPIOParameters=PinState` + `PC2.PinState=GPIO_PIN_SET` 才生效。外设 IP 名以 CubeMX 重写后的写法为准（QSPI 是 `QUADSPI1`） |
| D4 | 信号名是 `S_` 前缀的枚举 | PA0 上 TIM2 通道 1 的正确写法是 `S_TIM2_CH1_ETR`，写成 `TIM2_CH1_ETR` 会失败 | 信号名一律以本机 CubeMX 器件数据库（`db/mcu/STM32G474V(B-C-E)Tx.xml` 等）为准，先查库再写 |
| D5 | CubeMX 对 `.ioc` 的错误是**非致命**的 | 信号名写错只在日志里留一行 `(OptionalMessage_ERROR) Cannot map signal ...`，`project generate` 依然返回成功，外设被静默丢弃 | 改过 `.ioc` 后**必须**核验：① 日志无 `Cannot map`；② 生成的 `main.c` 里有对应的 `MX_xxx_Init`。见 [工程规约](../AGENTS.md) §2.3 |
| D6 | `Drivers/` 被 git 忽略 | `.gitignore` 一行 `Drivers/` 排除整个目录；HAL / CMSIS 由 PlatformIO framework 提供 | 新克隆的仓库没有 `Drivers/` 是**预期**的。注意 git 不能 re-include 被排除目录下的文件，若确需加占位目录要同时放行目录本身；本模板不需要 |
| D7 | `ProjectManager.AskForMigrate` 被写回 `true` | 本仓 `.ioc` 当前实际是 `ProjectManager.AskForMigrate=true`（2026-11-17 核对，不是上游模板说的 `false`）；CubeMX 每次生成 / 保存也会写回 `true` | 无头生成若卡在 `Begin LoadConfig()` 后不再输出，多半是模态迁移框在等人点；先把它改成 `false` 再跑，生成完再核对一次。见 [CubeMX 工具实测](TOOLING-cubemx.md) §3.4 |
| D8 | G474 工程会有一条 `VP_RIF_VS_RIF1` 诊断 | 即使只保留 NVIC / RCC / SYS，`project generate` 也可能打印 `(OptionalMessage_ERROR) Pin<N> (VP_RIF_VS_RIF1) cannot be retrieved for this MCU`；`N` 随配置浮动，上游模板 2026-11-05 记 `Pin5`、2026-11-06 记 `Pin4`（属模板记录），本仓已配 ADC1 / TIM6 / DMA1 等，编号未必相同 | 与本工程引脚无关，属 CubeMX G4 数据库噪音；**编号不是判据**，只要没有 `Cannot map` 且 `MX_xxx_Init` 齐全即可，不要为它删外设去「修」 |
| D9 | 重复生成**可能**把 `syscalls.c` / `sysmem.c` 写成叠加路径 | 出现时日志报 `FileNotFoundException: <工程目录>\D:\...\Core\Src\syscalls.c`，这两个文件不会更新；上游模板 2026-11-06 的一次无头重生成未复现（属模板记录，本仓未单独复测），不是每次必然出现 | 它们是标准 newlib 桩，改动很少；是否发生以日志为准，需要更新时从 FW_G4 模板工程里取，或先删除再生成 |
| D10 | 重新生成后 `Core/Src/main.c` 漂移 | 本仓这份 `main.c` 含用户代码，不是空白生成物：8 个业务模块仍在 `Core/Src`（见 [目录结构](LAYOUT.md)），`main.c` 的 USER CODE 块里也有采集 / 按键 / 心跳实现；2026-11-17 核对为 **379 行 / 25 个 `USER CODE BEGIN`**。上游模板记录的 120 行 / 19 个标记 / 209 行都是**模板仓库的实测值，不能当本仓判据或实证**（本仓外设更多，标记数自然不同） | `main.c` 的块外内容由 CubeMX 全权生成，**不要手工精简块外**；本仓不把既有 8 个业务模块搬去 `app/`，新代码才放 `app/`。验漂移用 `git diff`，或用 `grep -c "USER CODE BEGIN"` 比较生成前后，不要拿固定行数 / 固定标记数当标准；这类重生成 diff 单独成一次提交（[工程规约](../AGENTS.md) §1.3）；不要为缩小 diff 删注释 |
| D11 | 重生成 `Core/` 的副作用（**上游模板实测，本仓未复测**） | ① `.ioc` 被回写约 46 行元数据（`AskForMigrate`、空 `ProjectManager.*` 键、`RCC.*Freq_Value` 推导值），无引脚 / IP / 时钟实质变化；② CubeMX 写 CRLF，`.gitattributes` 归一为 LF 后 `git status` 先报 6 个 `M`、`git diff` 却为空；③ 上游模板的 `.ioc` 只有 `PC2.GPIOParameters=GPIO_Label`，没有有效电平键，`MX_GPIO_Init` 因此把 PC2 初值生成 `GPIO_PIN_RESET`（灯亮），与它 `xpulse.h` 的 `XPULSE_LED_OFF=GPIO_PIN_SET` 相反。**③ 不适用于本仓**：本仓 `.ioc` 已写 `PC0/PC1/PC2.GPIOParameters=PinState` 和 `PinState=GPIO_PIN_SET`，当前 `MX_GPIO_Init()` 写的初值是 `GPIO_PIN_SET`（灯灭），与 `XPULSE_LED_OFF` 一致；不要照抄上游「没有有效电平键、PinState 无效、上电亮」的推断 | ① 用 `git checkout -- <当前 .ioc>` 还原元数据，不提交；② 行尾归一属正常表现，`git add` 一次即归零；③ 本仓以生成后的 `MX_GPIO_Init()` 实际内容核对，不要为「上电灭」再加补丁。三项都是上游记录，本仓若重生成需自己核对 |

### D10 / D11 的 L4 实测记录（来源：上游模板仓库，不是本仓实证）

上游模板仓库在 2026-11-06 用下面这条命令做过一次无头重生成，记录留在这里方便对照检查项。
**数字和结论都属于上游的 16 MHz 最小骨架，不是本仓的实证**：

```bash
"C:/ST/STM32CubeMX/jre/bin/java.exe" -jar "C:/ST/STM32CubeMX/STM32CubeMX.exe" \
  -q scripts/cubemx-generate.mx
```

| 核验项 | 上游记录 |
| --- | --- |
| 退出码 / 日志 | rc=0；`Cannot map` 0 条；唯一的 `OptionalMessage_ERROR` 是 D8 的 RIF 诊断（本次 `Pin4`）；`FileNotFoundException` 0 条 |
| `Core/Src/main.c` | 209 行 / **19 个 `USER CODE BEGIN`**；三处 `app/` 胶水保留；`MX_GPIO_Init()` 齐全 |
| 生成物字节 | 6 个 `Core/` 生成文件与生成前逐字节一致；`.vscode/extensions.json` 未变 |
| `.ioc` | 回写约 46 行元数据，按 D11 ① 还原后 `git diff` 为空 |
| L3 | `pio run -t clean && pio run` → SUCCESS；RAM 1584 B / FLASH 3776 B，PlatformIO size 44 B，93 个编译对象 |

本仓当前配置与上游不同：170 MHz 时钟树、8 个业务模块在 `Core/Src`、全量构建 **99 个
编译对象 / RAM 8264 B / FLASH 9988 B**（见 [目录结构](LAYOUT.md) §6，2026-11-06 的 L3
记录）。上游的 1584 B / 3776 B / 93 对象 / 19 标记都不能拿来当本仓的构建结果或判据。
本仓**尚无自己的 L4 重生成记录**；若在本仓执行，按本文 §E 的流程另记日志与 `git diff`，
不能引用上表数字。`KeepUserCode=true` 时 USER CODE 块内的胶水受重生成保护这一点，上游
已经实测；本仓要引用同样结论，仍需自己跑一次 L4。

---

## E. 推荐的「改配置」流程（含校验）

按优先级：

1. **首选**：用 CubeMX GUI 改 `.ioc`，保存后 `project generate`。
2. **脚本化 / headless**：用 `java -jar STM32CubeMX.exe -q <script>.mx` 加载已有 `.ioc`
   再生成，工作目录必须是项目根目录（命令格式见 [CubeMX 工具实测](TOOLING-cubemx.md)）。
3. **手改 `.ioc`**：仅限小改动，且**必须**做完 D5 的两项核验；改完先在临时副本生成验证，
   别直接信「编辑成功」。
4. **生成后检查 `git status`**：CubeMX 可能覆盖 `.vscode/extensions.json`、
   写出 CRLF 的生成物、或把 `launch.json` 换成自动生成版本（后者本仓已忽略）。

任何 `.ioc` 改动都按 [工程规约](../AGENTS.md) 的 **L4 生成核验** 处理；影响编译的改动再补
L3 `pio run`，影响运行时行为的改动最终要 L5 实板确认。
