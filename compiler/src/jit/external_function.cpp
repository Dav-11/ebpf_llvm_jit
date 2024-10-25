//
// Created by root on 8/18/24.
//

#include "external_function.h"

ebpf_llvm_jit::jit::external_function::external_function(std::string name, void *fn) {

    this->fn = fn;
    this->name = std::move(name);
}
