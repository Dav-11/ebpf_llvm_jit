//
// Created by root on 8/27/24.
//

#ifndef EBPF_LLVM_JIT_HELPER_H
#define EBPF_LLVM_JIT_HELPER_H


#include <string>

namespace ebpf_llvm_jit::helpers{

    class helper {
    public:

        helper(unsigned int index, std::string name, void *fn);

        [[nodiscard]] unsigned int getIndex() const;
        [[nodiscard]] const std::string &getName() const;
        [[nodiscard]] void *getFn() const;

    private:
        unsigned int index;
        std::string name;
        void *fn;

    };
}



#endif //EBPF_LLVM_JIT_HELPER_H
