# Output directory
OUTPUT := .output

OUT_FILES := $(OUTPUT)/start.o \
	$(OUTPUT)/qemu_rv_uart.o \
	$(OUTPUT)/load_pkt_from_mem.o \
	$(OUTPUT)/bpf_printk.o

all: $(OUT_FILES)

# create folders
$(OUTPUT):
	$(call msg,MKDIR,$@)
	$(Q)mkdir -p $@

$(OUTPUT)/start.o: $(OUTPUT) start.S
	$(call msg,AS,$@)
	$(Q) riscv64-unknown-elf-as -march=rv64g -mabi=lp64 -o "$@" start.S

$(OUTPUT)/qemu_rv_uart.o: $(OUTPUT) qemu_rv_uart.c qemu_rv_uart.h memory.h
	$(call msg,CC,$@)
	$(Q) riscv64-unknown-elf-gcc -c -g -O0 -ffreestanding -mcmodel=medany -march=rv64g -mabi=lp64 -o "$@" qemu_rv_uart.c

$(OUTPUT)/load_pkt_from_mem.o: $(OUTPUT) load_pkt_from_mem.c load_pkt_from_mem.h qemu_rv_uart.h bpf_helpers.h
	$(call msg,CC,$@)
	$(Q) riscv64-unknown-elf-gcc -c -g -O0 -ffreestanding -mcmodel=medany -march=rv64g -mabi=lp64 -o "$@" load_pkt_from_mem.c

$(OUTPUT)/bpf_printk.o: $(OUTPUT) bpf_printk.c bpf_helpers.h memory.h
	$(call msg,CC,$@)
	$(Q) riscv64-unknown-elf-gcc -c -g -O0 -ffreestanding -mcmodel=medany -march=rv64g -mabi=lp64 -o "$@" bpf_printk.c

.PHONY: clean
clean:
	rm -f $(OUT_FILES)