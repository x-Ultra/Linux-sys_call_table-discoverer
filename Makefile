obj-m += syscalladderv2.o
syscalladderv2-objs += syscalladder.o ./lib/vtpmo.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
