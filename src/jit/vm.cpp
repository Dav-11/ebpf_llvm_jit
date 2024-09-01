//
// Created by root on 8/17/24.
//

#include "vm.h"
#include "context.h"
#include "spdlog/spdlog.h"
#include <cerrno>

using namespace ebpf_llvm_jit::jit;

vm::vm() : ext_funcs(MAX_EXT_FUNCS)
{
    this->jit_ctx = std::make_unique<context>(this);
}

std::string vm::get_error_message()
{
    return error_msg;
}

int vm::register_external_function(size_t index, const std::string &name, void *fn)
{
    if (index >= ext_funcs.size()) {
        error_msg = "Index too large";
        SPDLOG_ERROR("Index too large {} for ext func {}", index, name);
        return -E2BIG;
    }

    if (ext_funcs[index].has_value()) {
        error_msg = "Already defined";
        SPDLOG_ERROR("Index {} already occupied by {}", index, ext_funcs[index]->name);
        return -EEXIST;
    }

    ext_funcs[index] = external_function(name, fn);
    return 0;
}

int vm::load_code(const void *code, size_t code_len)
{
    if (code_len % 8 != 0) {
        error_msg = "Code len must be a multiple of 8";
        return -EINVAL;
    }
    instructions.assign((ebpf_inst *)code,
                        (ebpf_inst *)code + code_len / 8);
    return 0;
}

void vm::unload_code()
{
    instructions.clear();
}

int vm::exec(void *mem, size_t mem_len, uint64_t &bpf_return_value)
{
    if (jitted_function) {
        SPDLOG_DEBUG("llvm-jit: Called jitted function {:x}",
                     (uintptr_t)jitted_function.value());
        auto ret =
                (*jitted_function)(mem, static_cast<uint64_t>(mem_len));
        SPDLOG_DEBUG(
                "LLJIT: called from jitted function {:x} returned {}",
                (uintptr_t)jitted_function.value(), ret);
        bpf_return_value = ret;
        return 0;
    }
    auto func = compile();
    if (!func) {
        SPDLOG_ERROR("Unable to compile eBPF program");
        return -1;
    }
    SPDLOG_DEBUG("LLJIT: compiled function: {:x}", (uintptr_t)func.value());
    // after compile, run
    return exec(mem, mem_len, bpf_return_value);
}

void vm::set_lddw_helpers(uint64_t (*map_by_fd)(uint32_t),
                          uint64_t (*map_by_idx)(uint32_t),
                          uint64_t (*map_val)(uint64_t),
                          uint64_t (*var_addr)(uint32_t),
                          uint64_t (*code_addr)(uint32_t))
{
    this->map_by_fd = map_by_fd;
    this->map_by_idx = map_by_idx;
    this->map_val = map_val;
    this->var_addr = var_addr;
    this->code_addr = code_addr;
}

std::vector<uint8_t> vm::do_aot_compile(bool print_ir)
{
    return this->jit_ctx->do_aot_compile(print_ir);
}

std::optional<ebpf_llvm_jit::utils::precompiled_ebpf_function>
vm::compile()
{
    auto func = jit_ctx->compile();
    jitted_function = func;
    return func;
}

std::optional<ebpf_llvm_jit::utils::precompiled_ebpf_function>
vm::load_aot_object(const std::vector<uint8_t> &object)
{
    if (jitted_function) {
        error_msg = "Already compiled";
        return {};
    }
    this->jit_ctx->load_aot_object(object);
    jitted_function = this->jit_ctx->get_entry_address();
    return jitted_function;
}

