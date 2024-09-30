//
// Created by root on 8/17/24.
//

#ifndef EBPF_LLVM_JIT_COMPILER_XDP_H
#define EBPF_LLVM_JIT_COMPILER_XDP_H

#include <optional>
#include <memory>

#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/IRBuilder.h>

#include "../utils/bo.h"
#include "../ebpf_inst.h"
#include "external_function.h"
#include "program.h"

#ifndef MAX_EXT_FUNCS
#define MAX_EXT_FUNCS 8192
#endif


#define IS_ALIGNED(x, a) (((uintptr_t)(x) & ((a)-1)) == 0)

namespace ebpf_llvm_jit::jit {

    const static char *LDDW_HELPER_MAP_BY_FD = "__lddw_helper_map_by_fd";
    const static char *LDDW_HELPER_MAP_BY_IDX = "__lddw_helper_map_by_idx";
    const static char *LDDW_HELPER_MAP_VAL = "__lddw_helper_map_val";
    const static char *LDDW_HELPER_VAR_ADDR = "__lddw_helper_var_addr";
    const static char *LDDW_HELPER_CODE_ADDR = "__lddw_helper_code_addr";

    class CompilerXDP {
        //class vm *vm;

        std::optional<std::unique_ptr<llvm::orc::LLJIT> > jit;

        std::unique_ptr<pthread_spinlock_t> compiling;

        // lddwHelpers (????)
        uint64_t (*map_by_fd)(uint32_t) = nullptr;
        uint64_t (*map_by_idx)(uint32_t) = nullptr;
        uint64_t (*map_val)(uint64_t) = nullptr;
        uint64_t (*var_addr)(uint32_t) = nullptr;
        uint64_t (*code_addr)(uint32_t) = nullptr;

        std::string error_msg;
        std::vector<ebpf_inst> insts;

        std::vector<std::optional<external_function>> ext_funcs;


        static void loadLddwHelpers(program_t *p, std::unique_ptr<llvm::LLVMContext> &ctx, std::unique_ptr<llvm::Module> &module, const std::vector<std::string> &lddwHelpers);
        static void loadExtFuncs(program_t *p, std::unique_ptr<llvm::LLVMContext> &ctx, std::unique_ptr<llvm::Module> &module, const std::vector<std::string> &extFuncNames);
        static void split_blocks(program_t *p);

        llvm::Expected<llvm::orc::ThreadSafeModule>
        generateModule(const std::vector<std::string> &extFuncNames,
                       const std::vector<std::string> &lddwHelpers,
                       bool patch_map_val_at_compile_time);

//        llvm::Expected<int>
//        HandleInstruction(llvm::IRBuilder<> &builder, uint16_t pc, std::vector<llvm::Value *> regs, bool patch_map_val_at_compile_time,llvm::Value *callStack, llvm::Value *callItemCnt);


    public:
        CompilerXDP();

        std::string get_error_message();

        virtual ~CompilerXDP();

        int load_code(const void *code, size_t code_len);
        int register_external_function(size_t index, const std::string &name, void *fn);

        std::vector<uint8_t> do_aot_compile(bool print_ir = false);
    };
}

#endif //EBPF_LLVM_JIT_COMPILER_XDP_H
