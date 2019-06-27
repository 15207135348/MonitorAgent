# MonitorAgent
云平台主动防御系统（客户端部分）

包括3种不同的实现方式：

desktop：桌面版的客户端，有操作界面

server：后台运行的客户端，没有操作界面，数据通过socket发送给服务端

localfile：后台运行的客户端，数据直接保存在本地文件中

采集的数据类型包括：
系统指标：
  CPU使用率（User、System、Irq、SoftIrq、Idle等各种CPU态下的使用率）
  内存使用率（Buffer、Cache、Actual等使用率）
  磁盘IO（读写速率、IOPS）
  网络IO（进站、出站流量）
  进程信息
硬件指标：
  CPU温度
  GPU负载
  CPU风扇
安全日志：
  用户操作日志
  用户登陆失败/成功等安全事件
  crontab任务调度等安全事件


