#rules.mk

#USE_PC=1
ifneq ($(CROSS_COMPILE),)
	CC=$(CROSS_COMPILE)-gcc
	CXX=$(CROSS_COMPILE)-g++
	AR=$(CROSS_COMPILE)-ar
	LD=$(CROSS_COMPILE)-ld
else
	CC  = mipsel-openwrt-linux-gcc
	CXX = mipsel-openwrt-linux-g++
	AR  = mipsel-openwrt-linux-ar
	LD  = mipsel-openwrt-linux-ld
endif


MAKE = make

CFLAGS = -Wall -O2 -I$(WORK_DIR)
DFLAGS = 

DEPS := $(patsubst %.o, %.c.dep, $(OBJS))
TEMPDEPS := $(addsuffix ., $(dir $(DEPS)))
DEPS := $(join $(TEMPDEPS), $(notdir $(DEPS)))

IGNORE=$(wildcard .*.c.dep)
-include $(IGNORE)

%.o: %.c
	@echo "  CC  " $< "-o" $@; \
	$(CC) $(CFLAGS) $(DFLAGS) -c $< -o $@ -Wp,-MMD,$(dir $<).$(notdir $<).dep

