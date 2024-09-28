//
// Created by root on 8/17/24.
//

#ifndef EBPF_LLVM_JIT_CONTEXT_H
#define EBPF_LLVM_JIT_CONTEXT_H

#include <optional>
#include <memory>

#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include "vm.h"
#include "../utils/bo.h"

#define IS_ALIGNED(x, a) (((uintptr_t)(x) & ((a)-1)) == 0)

namespace ebpf_llvm_jit::jit {

    const static char *LDDW_HELPER_MAP_BY_FD = "__lddw_helper_map_by_fd";
    const static char *LDDW_HELPER_MAP_BY_IDX = "__lddw_helper_map_by_idx";
    const static char *LDDW_HELPER_MAP_VAL = "__lddw_helper_map_val";
    const static char *LDDW_HELPER_VAR_ADDR = "__lddw_helper_var_addr";
    const static char *LDDW_HELPER_CODE_ADDR = "__lddw_helper_code_addr";

    class context {
        class vm *vm;
        std::optional<std::unique_ptr<llvm::orc::LLJIT> > jit;
        std::unique_ptr<pthread_spinlock_t> compiling;
        llvm::Expected<llvm::orc::ThreadSafeModule>
        generateModule(const std::vector<std::string> &extFuncNames,
                       const std::vector<std::string> &lddwHelpers,
                       bool patch_map_val_at_compile_time);
        std::vector<uint8_t>
        do_aot_compile(const std::vector<std::string> &extFuncNames,
                       const std::vector<std::string> &lddwHelpers,
                       bool print_ir,
                       std::string cpu,
                       std::string cpu_features,
                       std::string target_triple);
        // (JIT, extFuncs, definedLddwSymbols)
        std::tuple<std::unique_ptr<llvm::orc::LLJIT>, std::vector<std::string>,
        std::vector<std::string> >
        create_and_initialize_lljit_instance();

    public:
        void do_jit_compile();
        context(class vm *vm);
        virtual ~context();
        ebpf_llvm_jit::utils::precompiled_ebpf_function compile();
        ebpf_llvm_jit::utils::precompiled_ebpf_function get_entry_address();
        std::vector<uint8_t> do_aot_compile(bool print_ir = false, std::string cpu = "generic", std::string cpu_features="", std::string target_triple = "");
        void load_aot_object(const std::vector<uint8_t> &buf);
    };
}

#endif //EBPF_LLVM_JIT_CONTEXT_H
