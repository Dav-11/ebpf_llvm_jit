//
// Created by root on 8/27/24.
//

#include "helper.h"
#include <utility>

ebpf_llvm_jit::helpers::helper::helper(unsigned int index, std::string name, void *fn) {

    this->fn = fn;
    this->name = std::move(name);
    this->index = index;

}

unsigned int ebpf_llvm_jit::helpers::helper::getIndex() const {
    return index;
}

const std::string &ebpf_llvm_jit::helpers::helper::getName() const {
    return name;
}

void *ebpf_llvm_jit::helpers::helper::getFn() const {
    return fn;
}