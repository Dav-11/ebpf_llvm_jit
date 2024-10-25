
all: runtime ebpf_llvm_jit

.PHONY: runtime
runtime:
	$(MAKE) -C rv64_baremetal_runtime

ebpf_llvm_jit:
	mkdir -p compiler/build
	cd compiler/build &&\
		cmake .. &&\
		make
	ln -s compiler/build/ebpf_llvm_jit ebpf_llvm_jit

.PHONY: examples
examples:
	$(MAKE) -C examples/02_qemu_riscv_no_calls
	$(MAKE) -C examples/03_qemu_riscv_local_calls
	$(MAKE) -C examples/04_qemu_riscv_printk_simple
	$(MAKE) -C examples/05_qemu_riscv_xdp_load_pkt
	$(MAKE) -C examples/06_qemu_riscv_xdp_multiple_pkts
	$(MAKE) -C examples/07_qemu_riscv_printk_multisection