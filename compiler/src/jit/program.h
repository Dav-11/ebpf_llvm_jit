//
// Created by Davide Collovigh on 30/09/24.
//

#ifndef EBPF_LLVM_JIT_PROGRAM_H
#define EBPF_LLVM_JIT_PROGRAM_H

#include "../ebpf_inst.h"
#include "external_function.h"

#include <llvm/ExecutionEngine/Orc/Core.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/IRBuilder.h>

typedef struct program {
    std::vector<llvm::Value *> regs;
    std::vector<ebpf_inst> insns;


    /********************************
     * Blocks
     ********************************/

    // If blockBegin[i] == true -> i is the first instruction of a block
    std::vector<bool> blockBegin;


    std::vector<llvm::BasicBlock *> allBlocks;
    std::map<uint16_t, llvm::BasicBlock *> instBlocks;

    llvm::BasicBlock *setupBlock;
    llvm::BasicBlock *exitBlock;
    llvm::BasicBlock *localRetBlock;

    // These blocks are the next instructions of the returning target of local functions
    std::map<uint16_t, llvm::BlockAddress *> localFuncRetBlks;


    /********************************
     * Main functions
     ********************************/
    llvm::Function *bpf_main;


    /********************************
     * Types
     ********************************/
    llvm::FunctionType *lddwHelperWithUint32;
    llvm::FunctionType *lddwHelperWithUint64;
    llvm::FunctionType *helperFuncTy;


    /********************************
     * External functions
     ********************************/

    std::map<std::string, llvm::Function *> lddwHelper;
    std::map<std::string, llvm::Function *> extFunc;
} program_t;


#endif //EBPF_LLVM_JIT_PROGRAM_H
