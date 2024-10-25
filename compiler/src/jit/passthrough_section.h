//
// Created by Davide Collovigh on 18/10/24.
//

#ifndef EBPF_LLVM_JIT_PASSTHROUGH_SECTION_H
#define EBPF_LLVM_JIT_PASSTHROUGH_SECTION_H

#include<string>

namespace ebpf_llvm_jit::jit {

    typedef struct passthrough_section {
        std::string name;
        std::string str_content;
    } passthrough_section;

}

#endif //EBPF_LLVM_JIT_PASSTHROUGH_SECTION_H
