obj-m += pnb.o

all:
	#sudo rmmod pnb
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	sudo insmod pnb.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
