SHELL := /bin/bash
LLVM_STRIP ?= llvm-strip
ARCH := $(shell uname -m | sed 's/x86_64/x86/' | sed 's/aarch64/arm64/' | sed 's/ppc64le/powerpc/' | sed 's/mips.*/mips/')
EBPF_LLVM_JIT := ../../ebpf_llvm_jit

# Source directories
LIBBPF_SRC := $(abspath ../third_party/bpftool/libbpf/src)
BPFTOOL_SRC := $(abspath ../third_party/bpftool/src)

# Output directory
OUTPUT := .output
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

####
# TARGETS
####

all: $(OUTPUT)/hello.elf

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

deps: $(LIBBPF_OBJ) $(BPFTOOL)

$(OUTPUT)/main.bpf.o: main.bpf.c $(LIBBPF_OBJ) $(wildcard %.h) | $(OUTPUT)
	$(call msg,BPF,$@)
	$(Q) clang -g -O2 -target bpf -D__TARGET_ARCH_$(ARCH) $(INCLUDES) $(CLANG_BPF_SYS_INCLUDES) -c $(filter %.c,$^) -o $@
	$(Q) $(LLVM_STRIP) -g $@ # strip useless DWARF info

$(OUTPUT)/hello_world.rv64.o: $(OUTPUT)/main.bpf.o
	$(call msg,BPF,$@)
	$(Q) $(EBPF_LLVM_JIT) build $(OUTPUT)/main.bpf.o -o $(OUTPUT)
	$(Q) mv $(OUTPUT)/hello_world.o $@

$(OUTPUT)/main.o: main.c ../99_baremetal_runtime/qemu_rv_uart.h
	$(call msg,GCC,$@)
	$(Q) riscv64-unknown-elf-gcc -c -g -O0 -ffreestanding -mcmodel=medany -march=rv64g -mabi=lp64 -o "$@" main.c

$(OUTPUT)/start.o: start.S
	$(call msg,AS,$@)
	$(Q) riscv64-unknown-elf-as -march=rv64g -mabi=lp64 -o "$@" start.S

$(OUTPUT)/qemu_rv_uart.o: ../99_baremetal_runtime/qemu_rv_uart.c ../99_baremetal_runtime/qemu_rv_uart.h
	$(call msg,GCC,$@)
	$(Q) riscv64-unknown-elf-gcc -c -g -O0 -ffreestanding -mcmodel=medany -march=rv64g -mabi=lp64 -o "$@" ../99_baremetal_runtime/qemu_rv_uart.c

$(OUTPUT)/bpf_helpers.o: ../99_baremetal_runtime/bpf_helpers.c ../99_baremetal_runtime/bpf_helpers.h
	$(call msg,GCC,$@)
	$(Q) riscv64-unknown-elf-gcc -c -g -O0 -ffreestanding -mcmodel=medany -march=rv64g -mabi=lp64 -o "$@" ../99_baremetal_runtime/bpf_helpers.c

$(OUTPUT)/hello.elf: $(OUTPUT) $(OUTPUT)/main.o $(OUTPUT)/start.o $(OUTPUT)/qemu_rv_uart.o $(OUTPUT)/bpf_helpers.o ../99_baremetal_runtime/baremetal.ld $(OUTPUT)/hello_world.rv64.o
	$(call msg,LD,$@)
	$(Q) riscv64-unknown-elf-ld -T ../99_baremetal_runtime/baremetal.ld -m elf64lriscv -o "$@" $(OUTPUT)/main.o $(OUTPUT)/start.o $(OUTPUT)/qemu_rv_uart.o $(OUTPUT)/bpf_helpers.o $(OUTPUT)/hello_world.rv64.o

run: $(OUTPUT)/hello.elf
	@echo "Ctrl-A C for QEMU console, then quit to exit"
	qemu-system-riscv64 -nographic -serial mon:stdio -machine virt -bios $(OUTPUT)/hello.elf

clean:
	rm -f $(OUTPUT)/main.o $(OUTPUT)/start.o $(OUTPUT)/hello.elf $(OUTPUT)/qemu_rv_uart.o $(OUTPUT)/hello_world.rv64.o $(OUTPUT)/main.bpf.o $(OUTPUT)/bpf_helpers.o
