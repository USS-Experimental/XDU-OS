**Semaphore使用实验:**

使用semaphore，并利用该程序生成2个进程（注意：非线程），这两个进程写同一个文件，要求：
一个进程写入内容：“自己学号的后3位PROC1 MYFILE1”；另一个进程写入内容：“自己学号的后3位PROC2 MYFILE2”

a.互斥写，即只有一个进程写完后，才能让另一个进程写;   
b. 将该程序的semaphore替换成使用strict alternation算法的忙等待互斥锁完成。

**Usage**

Build: `make`

Test a: `make testa`

Test a: `make testb`

Clean: `make clean`