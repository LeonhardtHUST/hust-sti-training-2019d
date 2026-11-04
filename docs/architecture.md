# 架构说明

本文件只描述**计划中的软件边界**，不描述具体外设、引脚或算法实现。
当前所有模块均为编译安全的空壳，接口可随时调整。

## 模块边界

```text
acquisition
  TIM / ADC / DMA 采集接口：启动一次采集、查询完成、交回采样帧

signal_analysis
  直流偏置 / 幅度 / 相位相关算法：输入一帧采样数据，输出纯数值结果

uart_stream
  采样数据输出：把整帧数据交给上位机（VOFA+ 等），不参与计算

ui
  LCD 呈现：初始化屏幕、显示数值与状态，不参与计算

key
  用户输入：扫描、去抖、产出事件，不直接调用业务模块

dds
  Software V 的 AD9959 接口：只负责写频率/相位/幅度，不含测量策略

measurement
  Software V 的测量编排：按状态机串起 dds → acquisition → signal_analysis
```

## 依赖方向

```text
main ──► key, acquisition, signal_analysis, ui, uart_stream
measurement ──► dds, acquisition, signal_analysis
ui ──► signal_analysis（仅依赖结果结构体）
signal_analysis ──► acquisition（仅依赖采样帧视图）
```

规则：

- 依赖单向，不出现回指。`acquisition` 不认识 `ui` / `uart_stream`。
- `signal_analysis` 是纯计算模块：不得包含任何 `HAL_*` 调用、不得访问外设寄存器，
  便于在 PC 上用录制的数据离线验证算法。
- `uart_stream` / `ui` 只消费数据，不改数据。
- `dds` 不懂"扫频"，`measurement` 不懂"寄存器"，两者边界不得交叉。

## 数据所有权

- 采样缓冲区**由 `acquisition` 独占持有**。其它模块通过 `acq_frame_t` 拿到只读视图
  （`const uint16_t *`），不得写入。
- 视图有效期：从 `acq_get_frame()` 成功返回，到下一次 `acq_start()` 为止。调用方若需
  跨采集保留数据，必须自行拷贝（`uart_stream` 的发送在视图有效期内完成即可）。
- `signal_analysis` 的输出结构由调用方分配，模块内部不保留状态。
- `measurement` 的结果数组由模块自己持有，对外只提供 `const` 指针 + 长度。

## 并发约定

- 目前**无 RTOS**，全部逻辑跑在主循环。中断服务程序只做两件事：清标志、置标志。
- 禁止在 ISR 中调用 `ui` / `uart_stream` / `dds`。
- `measurement_poll()` 与 `key` 轮询在主循环内串行执行，天然互斥，不需要锁。

## 初始化与主循环形态

```text
HAL_Init → SystemClock_Config → GPIO（CubeMX 生成）
        → key_init → acquisition_init → uart_stream_init → ui_init
        → [Software V] dds_init → measurement_init
主循环：
        轮询 key 事件 → 分发（单次采集 / 扫频）
        → measurement_poll()（Software V）
        → UI 刷新节流
        → 板载红灯（PC2）心跳
```

## 尚未确定（`TBD`）

- 采样缓冲区是单缓冲 + 忙标志，还是双缓冲乒乓。
- UART 输出采用阻塞发送、中断发送还是 DMA 发送。
- 采样帧的存储布局：两通道交错存放，还是各自独立的连续区。
- 系统时钟最终方案（当前保持 HSI 16 MHz 不变）。

以上选择都不影响模块边界，实现时再定。
