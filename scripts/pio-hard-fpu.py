# PlatformIO 预处理脚本：把 Cortex-M4F（FPv4-SP）硬件浮点参数补进汇编与链接阶段。
#
# 为什么需要它：
#   `platformio.ini` 的 build_flags 里已写 `-mfpu=fpv4-sp-d16` 与 `-mfloat-abi=hard`，
#   但 PlatformIO 用 SCons 的 ParseFlags 解析 build_flags，只有以 `-Wl,` 开头的项
#   会进 LINKFLAGS，其余（含这两个）只进 CCFLAGS。于是所有 .o 都是硬浮点，链接
#   命令行却仍是软浮点，ld 报 "xxx.o uses VFP register arguments, firmware.elf
#   does not" 并去链软浮点版 libgcc / newlib。这里补齐 ASFLAGS 与 LINKFLAGS，
#   让编译、汇编、链接三个 scope 使用同一套 ABI。
#
# 必需的是「把同一对 flag 补进 ASFLAGS / LINKFLAGS」这件事本身；`pre:` 前缀只是
# 书写习惯，不是链接成功的前提。做法参照 PlatformIO 官方论坛 maxgerhardt 关于
# "uses VFP register arguments" 的答复。
Import("env")

_FPU_FLAGS = ["-mfpu=fpv4-sp-d16", "-mfloat-abi=hard"]

env.AppendUnique(ASFLAGS=_FPU_FLAGS, LINKFLAGS=_FPU_FLAGS)
