obj-m := echo.o

all:
	make -C /usr/lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /usr/lib/modules/$(shell uname -r)/build M=$(PWD) clean
