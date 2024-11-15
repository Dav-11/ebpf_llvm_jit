SHELL := /bin/bash
LLVM_STRIP ?= llvm-strip
ARCH := $(shell uname -m | sed 's/x86_64/x86/' | sed 's/aarch64/arm64/' | sed 's/ppc64le/powerpc/' | sed 's/mips.*/mips/')
EBPF_LLVM_JIT := ../../ebpf_llvm_jit

# Source directories
LIBBPF_SRC := $(abspath ../third_party/bpftool/libbpf/src)
BPFTOOL_SRC := $(abspath ../third_party/bpftool/src)

# Output directory
OUTPUT := .output
RNT_BASE := ../../rv64_baremetal_runtime
OUT_RNT := $(RNT_BASE)/.output
LIBBPF_OBJ := $(abspath $(OUTPUT)/libbpf.a)
LIBBPF_PKGCONFIG := $(abspath $(OUTPUT)/pkgconfig)
BPFTOOL_OUTPUT ?= $(abspath $(OUTPUT)/bpftool)
BPFTOOL ?= $(BPFTOOL_OUTPUT)/bootstrap/bpftool

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

RUNTIME_HDR := $(RNT_BASE)/bpf_helpers.h \
	$(RNT_BASE)/load_pkt_from_mem.h \
	$(RNT_BASE)/memory.h \
	$(RNT_BASE)/qemu_rv_uart.h

RUNTIME_BIN := $(OUT_RNT)/start.o \
	$(OUT_RNT)/load_pkt_from_mem.o \
	$(OUT_RNT)/qemu_rv_uart.o \
	$(OUT_RNT)/bpf_printk.o

####
# TARGETS
####

all: $(OUTPUT)/hello.elf

$(RUNTIME_BIN):
	$(MAKE) -C $(RNT_BASE) all

# create folders
$(OUTPUT) $(OUTPUT)/libbpf $(BPFTOOL_OUTPUT):
	$(call msg,MKDIR,$@)
	$(Q)mkdir -p $@

# Build libbpf
$(LIBBPF_OBJ):
	$(call msg,LIB,$@)
	$(Q)$(MAKE) -C $(LIBBPF_SRC) BUILD_STATIC_ONLY=1	\
		OBJDIR=$(dir $@)libbpf DESTDIR=$(dir $@)		\
		INCLUDEDIR= LIBDIR= UAPIDIR=					\
		install

# Build bpftool
$(BPFTOOL): | $(BPFTOOL_OUTPUT)
	$(call msg,BPFTOOL,$@)
	$(Q)$(MAKE) ARCH= CROSS_COMPILE= OUTPUT=$(BPFTOOL_OUTPUT)/ -C $(BPFTOOL_SRC) bootstrap

deps: $(LIBBPF_OBJ) $(BPFTOOL) $(RUNTIME_BIN)

$(OUTPUT)/main.bpf.o: main.bpf.c $(LIBBPF_OBJ) $(wildcard %.h) | $(OUTPUT)
	$(call msg,BPF,$@)
	$(Q) clang -g -O2 -target bpf -D__TARGET_ARCH_$(ARCH) $(INCLUDES) $(CLANG_BPF_SYS_INCLUDES) -c $(filter %.c,$^) -o $@
	$(Q) $(LLVM_STRIP) -g $@ # strip useless DWARF info

$(OUTPUT)/hello_world.rv64.o: $(OUTPUT)/main.bpf.o
	$(call msg,BPF,$@)
	$(Q) $(EBPF_LLVM_JIT) build $(OUTPUT)/main.bpf.o -o $(OUTPUT)
	$(Q) mv $(OUTPUT)/hello_world.o $@

$(OUTPUT)/main.o: main.c $(RUNTIME_HDR)
	$(call msg,CC,$@)
	$(Q) riscv64-unknown-elf-gcc -c -g -O0 -ffreestanding -mcmodel=medany -march=rv64g -mabi=lp64 -o "$@" main.c

$(OUTPUT)/hello.elf: $(OUTPUT) $(OUTPUT)/main.o $(OUTPUT)/hello_world.rv64.o $(RUNTIME_BIN)
	$(call msg,LD,$@)
	$(Q) riscv64-unknown-elf-ld -T $(RNT_BASE)/baremetal.ld -m elf64lriscv -o "$@" $(OUTPUT)/main.o $(OUTPUT)/hello_world.rv64.o $(RUNTIME_BIN)

hello.dis.s: $(OUTPUT)/hello.elf
	$(call msg,DISASM,$@)
	$(Q) riscv64-unknown-elf-objdump -d $(OUTPUT)/hello.elf > "$@"

run: $(OUTPUT)/hello.elf
	@echo "Ctrl-A C for QEMU console, then quit to exit"
	qemu-system-riscv64 -nographic -serial mon:stdio -machine virt -bios $(OUTPUT)/hello.elf

clean:
	rm -f $(OUTPUT)/main.o $(OUTPUT)/start.o $(OUTPUT)/hello.elf $(OUTPUT)/qemu_rv_uart.o $(OUTPUT)/hello_world.rv64.o $(OUTPUT)/main.bpf.o $(OUTPUT)/bpf_printk.o

clean-apps:
	$(MAKE) -C $(RNT_BASE) clean
	rm -rf $(OUTPUT)
