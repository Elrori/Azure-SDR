This directory contains 8051 firmware for the Cypress Semiconductor EZ-USB FX2 
chip.

The purpose of this code is to demonstrate how to utilize EZUSB FX2 Slave Sync Mode
(in a back to back application - FX2 in SLAVE FIFO Sync).

The code is written in C and uses both the EZ-USB FX library and the FrameWorks.

It configures FX2 as follows:
01).  EP2 512 4x BULK OUT - 16-bit sync AUTO mode
02).  EP6 512 4x BULK IN - 16-bit sync AUTO mode

.....from the slave (in this case is FX2 in Slave FIFO mode)
01).  512 byte buffer for EP2 OUT (master) -> EP6 IN data (slave)
02).  512 byte buffer for EP6 IN (master) -> EP2 OUT data (slave)
04).  peripheral interface functions in 16-bit sync mode

.....from "the user":
01).  EP2 512 4x BULK OUT data is sent to EP6 512 4x BULK IN
02).  EP6 512 4x BULK IN data is received from EP2 512 4x BULK OUT

NOTE: we'll initially test using 16-bit mode so the host application/driver
  doesn't need to pad odd data sizes, say 8191 bytes... etc.

The "slave_sync.hex" file loads into internal memory.
...issue "build -i" at the command prompt...

This example is for illustrative purpose(s) and unless you have an ext. slave
that emulates the testing environment this example won't actually produce 
expected results when downloaded via Control Panel.  The external slave in this
case is EZUSB FX2 running in Slave FIFO mode


In this implementation the master to slave pin assignments are as follows:

slave(FX Slave FIFO SYNC mode)   master(FX GPIF SYNC mode)      
====================             =========================       
SLRD          <----              CTL0
SLWR          <----              CTL1                         
SLOE          <----              CTL2


FIFOADR0      <----              PA6
FIFOADR1      <----              PA7

FLAGA_PF      ---->              PA4
FLAGB_FF      ---->              RDY1
FLAGC_EF      ---->              RDY0
 
PA0           ---->              INT0#  

IFCLK         <--->              IFCLK


The Control Panel Application may be used to drive this example as described in 
the tutorials.

