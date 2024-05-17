# --------------------------------------------------------------
# File          : Makefile
# Authors       : 西安电子科技大学:网络与信息安全国家级实验教学中心
# Created on    : <2024-02-01>
# Last modified : <2024-03-01>
# LICENSE       : 由实验教学中心拥有，分发需申请并征得同意
# -------------------------------------------------------------*/

all:
	@gcc ./main.c    -o a.out
	@gcc ./qjumper.c -o b.out
	@echo Compile done.

runa:
	@sudo taskset -c 0 ./a.out

runb:
	@sudo taskset -c 0 ./b.out

trace:
	@sudo bpftrace ./trace.bt
