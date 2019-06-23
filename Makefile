include $(DIR_ZHONGHENG)/config.mk

SRC_DIR = ./src

TGT = $(BUILD_ROOT)/elinkclient

INC_PATH = $(CURDIR)/include
INC_PATH += -I./easylogger/inc  -I./easylogger/plugins/

CFLAGS  += -I$(INC_PATH)
CFLAGS  += -I$(DIR_OPENSSL_PATH)/include
CFLAGS  += $(shell pkg-config --cflags json-c libubox)
LDFLAGS += $(shell pkg-config --libs  json-c  libubox)
LDFLAGS += -L$(SRC_DIR)
LDFLAGS += -lpthread -lcrypto

C_FILEs  = $(wildcard $(SRC_DIR)/*.c)
C_FILEs += $(wildcard easylogger/**/*.c)
C_FILEs += $(wildcard easylogger/plugins/file/*.c)


C_OBJs	= $(addprefix $(BUILD_ROOT)/,$(patsubst %.c,%.o,$(C_FILEs)))

prefix ?= $(DIR_TMP_INSTALL)


.PHONY: all romfs clean prepare build

all: build

prepare:
	@mkdir -p $(BUILD_ROOT)
	if [ -f "$(SRC_DIR)/libcrypto.so" ]; \
	then \
		rm $(SRC_DIR)/libcrypto.so; \
	fi;
	@ln -s $(DIR_OPENSSL_PATH)/libcrypto.so $(SRC_DIR)/libcrypto.so

-include $(C_OBJs:.o=.d)

$(BUILD_ROOT)/%.d: %.c
	@set -e; rm -f $@; mkdir -p $(@D); \
	$(CC) -MM $(CFLAGS) -MT "$(@:.d=.o)" $< > $@; \
	echo '	$$(CC) $$(CFLAGS) -c $$< -o $$@' >> $@

$(TGT): $(C_OBJs)
	$(CC) $(LDFLAGS) -o $@ $^

build: prepare $(TGT)
	@echo "Build finished"

clean:
	rm -rf $(BUILD_ROOT)

install:
	$(INSTALL_DIR) $(prefix)/bin
	$(INSTALL_BIN) $(TGT) $(prefix)/bin

romfs:
	$(ROMFSINST) $(TGT) /bin
