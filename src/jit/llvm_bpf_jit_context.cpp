//
// Created by root on 8/17/24.
//

#include "llvm_bpf_jit_context.h"

using namespace ebpf_llvm_jit::jit;

std::vector<uint8_t> llvm_bpf_jit_context::do_aot_compile(bool print_ir)
{
    std::vector<std::string> extNames, lddwNames;

    for (uint32_t i = 0; i < std::size(vm->ext_funcs); i++) {

        if (vm->ext_funcs[i].has_value()) {
#if LLVM_VERSION_MAJOR >= 16
            extNames.emplace_back(ext_func_sym(i));
#else
            extNames.push_back(ext_func_sym(i));
#endif
        }
    }

    const auto tryDefineLddwHelper = [&](const char *name, void *func) {
        if (func) {
#if LLVM_VERSION_MAJOR >= 16
            lddwNames.emplace_back(name);
#else
            lddwNames.push_back(name);
#endif
        }
    };
    // Only map_val will have a chance to be called at runtime
    tryDefineLddwHelper(LDDW_HELPER_MAP_VAL, (void *)vm->map_val);
    // These symbols won't be used at runtime
    // tryDefineLddwHelper(LDDW_HELPER_MAP_BY_FD, (void *)vm->map_by_fd);
    // tryDefineLddwHelper(LDDW_HELPER_MAP_BY_IDX, (void *)vm->map_by_idx);
    // tryDefineLddwHelper(LDDW_HELPER_CODE_ADDR, (void *)vm->code_addr);
    // tryDefineLddwHelper(LDDW_HELPER_VAR_ADDR, (void *)vm->var_addr);
    return this->do_aot_compile(extNames, lddwNames, print_ir);
}
