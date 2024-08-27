#ifndef EBPF_LLVM_JIT_VM_H
#define EBPF_LLVM_JIT_VM_H

#include <memory>
#include <optional>
#include <vector>

#include "../ebpf_inst.h"
#include "external_function.h"
#include "context.h"
#include "../utils/bo.h"

#ifndef MAX_EXT_FUNCS
#define MAX_EXT_FUNCS 8192
#endif

namespace ebpf_llvm_jit::jit {

    class vm {
    public:

        vm();

        std::string get_error_message();
        int register_external_function(size_t index, const std::string &name, void *fn);
        int load_code(const void *code, size_t code_len) ;
        void unload_code() ;
        int exec(void *mem, size_t mem_len, uint64_t &bpf_return_value);
        std::vector<uint8_t> do_aot_compile(bool print_ir = false);
        std::optional<utils::precompiled_ebpf_function>
        load_aot_object(const std::vector<uint8_t> &object);
        std::optional<utils::precompiled_ebpf_function> compile();
        void set_lddw_helpers(uint64_t (*map_by_fd)(uint32_t),
                              uint64_t (*map_by_idx)(uint32_t),
                              uint64_t (*map_val)(uint64_t),
                              uint64_t (*var_addr)(uint32_t),
                              uint64_t (*code_addr)(uint32_t));

        class context *get_jit_context()
        {
            return jit_ctx.get();
        }

    private:
        uint64_t (*map_by_fd)(uint32_t) = nullptr;
        uint64_t (*map_by_idx)(uint32_t) = nullptr;
        uint64_t (*map_val)(uint64_t) = nullptr;
        uint64_t (*var_addr)(uint32_t) = nullptr;
        uint64_t (*code_addr)(uint32_t) = nullptr;
        std::vector<ebpf_inst> instructions;
        std::vector<std::optional<external_function>> ext_funcs;

        std::unique_ptr<context> jit_ctx;
        friend class context;

        std::string error_msg;
        std::optional<utils::precompiled_ebpf_function> jitted_function;
    };

}


#endif //EBPF_LLVM_JIT_VM_H
