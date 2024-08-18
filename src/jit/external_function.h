//
// Created by root on 8/18/24.
//

#ifndef EBPF_LLVM_JIT_EXTERNAL_FUNCTION_H
#define EBPF_LLVM_JIT_EXTERNAL_FUNCTION_H

#include <string>

namespace ebpf_llvm_jit::jit {
    class external_function {
    public:
        external_function(std::string name, void *fn);

        std::string name;
        void *fn;
    };
}


#endif //EBPF_LLVM_JIT_EXTERNAL_FUNCTION_H
