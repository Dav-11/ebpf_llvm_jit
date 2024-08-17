//
// Created by root on 8/17/24.
//

#include "bpftime_llvm_jit_vm.h"
#include <cerrno>

using namespace ebpf_llvm_jit::jit;

std::string bpftime_llvm_jit_vm::get_error_message()
{
    return error_msg;
}

int bpftime_llvm_jit_vm::load_code(const void *code, size_t code_len)
{
    if (code_len % 8 != 0) {
        error_msg = "Code len must be a multiple of 8";
        return -EINVAL;
    }
    instructions.assign((ebpf_inst *)code,
                        (ebpf_inst *)code + code_len / 8);
    return 0;
}