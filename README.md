
Azure-SDR 是使用Altera FPGA,CY7C68013A USB2.0和一块高速ADC的短波直采样SDR接收方案.

Azure-SDR 不能单独工作,需要配合HDSDR软件.

Azure-SDR 是通过模块拼接的,目前没有整合的PCB.

Azure-SDR 除了DDS使用了Altera IP NCO外其他算法使用纯verilog HDL描述,可综合代码在目录:\qii-17.1\rtl

信号处理流程：

40Msps直接复混频 -> CIC 20倍抽取滤波(40/20=2Msps)-> FIR匹配滤波 2倍抽取(2/2=1Msps,1Msps x 64bits = 8MByte/s) -> USB2.0

**目录结构：**

```ExtIO```      

vs2019工程，用于编译产生ExtIO.dll(已经事先编译),放置到HDSDR目录 确保该目录只有一个dll,打开HDSDR

```cy7c68013a```

USB2.0芯片固件、固件源码、固件下载方法、驱动

```pcb```   

AD9235-40模块PCB,拼版。ADC工作在40MSPS,PCB上使用了AD8138驱动，但是实际使用没有焊接AD8138,按照AD9235技术手册改成了单端输入。

```qii-17.1```

Quartus 17.1 FPGA工程

```rtl_sim```

iverilog仿真

**如何复现设计：**

1 需要40M 12bits ADC模块,Altera FPGA板子,cy7c68013a模块，以及与FPGA正确的连接

2 需要修改qii-17.1中引脚约束,重新编译下载FPGA

3 通过cypress control center 下载.iic固件到cy7c68013a,并安装cy7c68013a PC驱动

4 将事先编译好的ExtIO_Example.dll放置到HDSDR目录

5 打开HDSDR会自动运行

测试：https://www.bilibili.com/video/BV1x741127pv/

![Image](https://github.com/Elrori/Azure-SDR/blob/master/IMG_5121.JPG)

**感谢：**

https://github.com/marsohod4you/FPGA_SDR

