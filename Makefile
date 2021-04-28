obj-m += the_syscalladder.o
the_syscalladder-objs += syscalladder.o ./lib/vtpmo.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

