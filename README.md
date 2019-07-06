# MonitorAgent
Linux下CPU/内存/磁盘/网络监测软件(与异常检测服务端Monitor-Server配合使用更佳)

包括3种不同的实现方式：

desktop：桌面版的客户端，有操作界面

server：后台运行的客户端，没有操作界面，数据通过socket发送给服务端

localfile：后台运行的客户端，数据直接保存在本地文件中

采集的数据类型包括：
1.系统指标：
  1.1CPU使用率（User、System、Irq、SoftIrq、Idle等各种CPU态下的使用率）
  1.2内存使用率（Buffer、Cache、Actual等使用率）
  1.3磁盘IO（读写速率、IOPS）
  1.4网络IO（进站、出站流量）
  1.5进程信息
2.硬件指标：
  2.1CPU温度
  2.2GPU负载
  2.3CPU风扇
3.安全日志：
  3.1用户操作日志
  3.2用户登陆失败/成功等安全事件
  3.3crontab任务调度等安全事件


