//
// Created by root on 8/27/24.
//

#include "helper.h"
#include <utility>

using namespace ebpf_llvm_jit::helpers;

helper::helper(unsigned int index, std::string name, void *fn) {

    this->fn = fn;
    this->name = std::move(name);
    this->index = index;

}

unsigned int helper::getIndex() const {
    return index;
}

const std::string &helper::getName() const {
    return name;
}

void *helper::getFn() const {
    return fn;
}

void helper::set_index(const unsigned int index)
{
    this->index = index;
}

void helper::set_name(const std::string& name)
{
    this->name = name;
}

void helper::set_fn(void* fn)
{
    this->fn = fn;
}