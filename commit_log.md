# 2025.5.11
[first commit] 移植SC7到RVfpga,支持线程切换
1. 采用了SC7 master分支的ff89dcf6fb33a0e9ae2036d679f827fb3686f52f提交，大幅度删减代码，以适配RV32
2. 使用Serial Monitor监视对应串口，输出应该为：

#### Serial Monitor
```
---- 已打开串行端口 COM8 ----
AtBtCtDtEtFtGtHtItJtKtLtMtNtOtPtQtRtStTtUtVtWtXtYtZt
xn6_start_kernel at :0x0000091a
proc初始化完成
switch to process 0
process init here!
process 0 is not RUNNABLE, skip.
switch to process 1
process1 here!
......

```

# 2025.5.12
[refactor] 大幅度删减，精简到7个文件
1. test.c中的函数可以正常使用，printf支持颜色打印。

# 2025.5.14
[feat] 增加figlet.c，可以输出艺术字

# 2025.5.15
[feat] 增加了sc7_create_process函数

# 2025.5.15
[feat] 修正了figlet的部分格式，增加process_figlet例程

# 2025.5.15
[docs] 添加SC7-RVfpga.docx

# 2025.5.15
[feat] 增加控制switch,led,segdig的例程和函数

# 2025.5.21
[feat] 实现开关切换用户程序，实现sleep、wakeup、delay函数

# 2025.5.23
[fix] 修复因pid不从0开始导致的偏移量问题

# 2025.5.23
[feat] 移植priority_queue,重整代码格式

# 2025.5.23
[feat] 添加priority_queue例程,修改进程调度算法为优先级调度,添加sc7_start_process启动函数


