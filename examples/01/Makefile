
SHELL := /bin/bash
ARCH := $(shell uname -m | sed 's/x86_64/x86/' | sed 's/aarch64/arm64/' | sed 's/ppc64le/powerpc/' | sed 's/mips.*/mips/')
LLVM_STRIP ?= llvm-strip

EBPF_LLVM_JIT := ../../cmake-build-debug/ebpf_llvm_jit
CC_RV := riscv64-unknown-linux-gnu-gcc

# Source directories
LIBBPF_SRC := $(abspath ../third_party/bpftool/libbpf/src)
BPFTOOL_SRC := $(abspath ../third_party/bpftool/src)

# Output directory
OUTPUT := .output
LIBBPF_OBJ := $(abspath $(OUTPUT)/libbpf.a)
LIBBPF_PKGCONFIG := $(abspath $(OUTPUT)/pkgconfig)
BPFTOOL_OUTPUT ?= $(abspath $(OUTPUT)/bpftool)
BPFTOOL ?= $(BPFTOOL_OUTPUT)/bootstrap/bpftool
VMLINUX_H := $(abspath $(OUTPUT)/vmlinux.h)

# Compiler and linker options
INCLUDES := -I$(OUTPUT) -I../libs/libbpf/include/uapi
CFLAGS := -g -Wall -DLOG_USE_COLOR
ALL_LDFLAGS := $(LDFLAGS) $(EXTRA_LDFLAGS)
ALL_LDFLAGS += -lrt -ldl -lpthread -lm

# hide output unless V=1
ifeq ($(V),1)
	Q =
	msg =
else
	Q = @
	msg = @printf '  %-8s %s%s\n'					\
		      "$(1)"						\
		      "$(patsubst $(abspath $(OUTPUT))/%,%,$(2))"	\
		      "$(if $(3), $(3))";
	MAKEFLAGS += --no-print-directory
endif

APPS := $(OUTPUT)/main
MAIN_FILES := main.c \
	../../src/include/helpers_impl.h \
	$(OUTPUT)/hello_world.host.o

RV_FILES := main.c \
	../../src/include/helpers_impl.h \
	$(OUTPUT)/hello_world.rv64.o


####
# TARGETS
####

all: $(APPS)

# create folders
$(OUTPUT) $(OUTPUT)/libbpf $(BPFTOOL_OUTPUT):
	$(call msg,MKDIR,$@)
	$(Q)mkdir -p $@

# Build libbpf
$(LIBBPF_OBJ):
	$(call msg,LIB,$@)
	$(Q)$(MAKE) -C $(LIBBPF_SRC) BUILD_STATIC_ONLY=1	\
		OBJDIR=$(dir $@)/libbpf DESTDIR=$(dir $@)		\
		INCLUDEDIR= LIBDIR= UAPIDIR=					\
		install

# Build bpftool
$(BPFTOOL): | $(BPFTOOL_OUTPUT)
	$(call msg,BPFTOOL,$@)
	$(Q)$(MAKE) ARCH= CROSS_COMPILE= OUTPUT=$(BPFTOOL_OUTPUT)/ -C $(BPFTOOL_SRC) bootstrap

$(VMLINUX_H): $(BPFTOOL)
	$(call msg,VMLINUX,$@)
	$(Q) $(BPFTOOL) btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

deps: $(LIBBPF_OBJ) $(BPFTOOL) $(VMLINUX_H)

# Build BPF code
$(OUTPUT)/%.bpf.o: %.bpf.c $(LIBBPF_OBJ) $(wildcard %.h) $(VMLINUX_H) | $(OUTPUT)
	$(call msg,BPF,$@)
	$(Q) clang -g -O2 -target bpf -D__TARGET_ARCH_$(ARCH) $(INCLUDES) $(CLANG_BPF_SYS_INCLUDES) -c $(filter %.c,$^) -o $@
	$(Q) $(LLVM_STRIP) -g $@ # strip useless DWARF info

# build local
$(OUTPUT)/hello_world.host.o: $(OUTPUT)/main.bpf.o
	$(call msg,BPF,$@)
	$(Q) $(EBPF_LLVM_JIT) build $(OUTPUT)/main.bpf.o -o $(OUTPUT)
	$(Q) mv $(OUTPUT)/hello_world.o $@

$(OUTPUT)/main: main.c $(OUTPUT)/hello_world.host.o
	$(call msg,CLANG,$@)
	clang -Xlinker --export-dynamic -O2 -flto $(MAIN_FILES)
	mv a.out $@

# xbuild RV64
$(OUTPUT)/hello_world.rv64.o: $(OUTPUT)/main.bpf.o
	$(call msg,BPF,$@)
	$(Q) $(EBPF_LLVM_JIT) buildrv $(OUTPUT)/main.bpf.o -o $(OUTPUT)
	$(Q) mv $(OUTPUT)/hello_world.o $@

$(OUTPUT)/main_rv: main.c $(OUTPUT)/hello_world.host.o
	$(call msg,CLANG,$@)
	$(CC_RV) -march=rv64g -mabi=lp64 -Xlinker --export-dynamic -O2 -flto $(RV_FILES)
	mv a.out $@

clean-apps:
	$(call msg,RM, $(OUTPUT)/hello_world.rv64.o)
	$(Q) rm -f $(OUTPUT)/hello_world.rv64.o
	$(call msg,RM, $(OUTPUT)/main_rv)
	$(Q) rm -f $(OUTPUT)/main_rv
	$(call msg,RM, $(OUTPUT)/hello_world.host.o)
	$(Q) rm -f $(OUTPUT)/hello_world.host.o
	$(call msg,RM, $(OUTPUT)/main)
	$(Q) rm -f $(OUTPUT)/main
	$(call msg,RM, $(OUTPUT)/main.bpf.o)
	$(Q) rm -f $(OUTPUT)/main.bpf.o

clean:
	$(call msg,RM, $(OUTPUT))
	$(Q) rm -f $(OUTPUT)
