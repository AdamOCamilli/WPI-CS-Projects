obj-m := new_sys_calls.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: 
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
