//
// Created by root on 8/17/24.
//

#include "context.h"
#include "spdlog/spdlog.h"

#include "llvm/IR/Module.h"
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Error.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Support/Host.h>
#include <llvm/MC/TargetRegistry.h>

using namespace ebpf_llvm_jit::jit;

static int llvm_initialized = 0;

struct spin_lock_guard {
    pthread_spinlock_t *spin;
    spin_lock_guard(pthread_spinlock_t *spin) : spin(spin)
    {
        pthread_spin_lock(spin);
    }
    ~spin_lock_guard()
    {
        pthread_spin_unlock(spin);
    }
};

static llvm::ExitOnError ExitOnErr;

static void optimizeModule(llvm::Module &M)
{
    // std::cout << "LLVM_VERSION_MAJOR: " << LLVM_VERSION_MAJOR <<
    // std::endl;
#if LLVM_VERSION_MAJOR >= 17
    // =====================
	// Create the analysis managers.
	// These must be declared in this order so that they are destroyed in
	// the correct order due to inter-analysis-manager references.
	LoopAnalysisManager LAM;
	FunctionAnalysisManager FAM;
	CGSCCAnalysisManager CGAM;
	ModuleAnalysisManager MAM;

	// Create the new pass manager builder.
	// Take a look at the PassBuilder constructor parameters for more
	// customization, e.g. specifying a TargetMachine or various debugging
	// options.
	PassBuilder PB;

	// Register all the basic analyses with the managers.
	PB.registerModuleAnalyses(MAM);
	PB.registerCGSCCAnalyses(CGAM);
	PB.registerFunctionAnalyses(FAM);
	PB.registerLoopAnalyses(LAM);
	PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

	// Create the pass manager.
	// This one corresponds to a typical -O2 optimization pipeline.
	ModulePassManager MPM =
		PB.buildPerModuleDefaultPipeline(OptimizationLevel::O3);

	// Optimize the IR!
	MPM.run(M, MAM);
	// =====================================
#else
    llvm::legacy::PassManager PM;

    llvm::PassManagerBuilder PMB;
    PMB.OptLevel = 3;
    PMB.populateModulePassManager(PM);

    PM.run(M);
#endif
}

#if defined(__arm__) || defined(_M_ARM)
extern "C" void __aeabi_unwind_cpp_pr1();
#endif



context::context(class vm *vm): vm(vm)
{
    using namespace llvm;
    int zero = 0;
    if (__atomic_compare_exchange_n(&llvm_initialized, &zero, 1, false,
                                    __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
        SPDLOG_INFO("Initializing llvm");
        LLVMInitializeNativeTarget();
        LLVMInitializeNativeAsmPrinter();
    }
    compiling = std::make_unique<pthread_spinlock_t>();
    pthread_spin_init(compiling.get(), PTHREAD_PROCESS_PRIVATE);
}

void context::do_jit_compile()
{
    auto [jit, extFuncNames, definedLddwHelpers] =
            create_and_initialize_lljit_instance();
    auto bpfModule = ExitOnErr(
            generateModule(extFuncNames, definedLddwHelpers, true));
    bpfModule.withModuleDo([](auto &M) { optimizeModule(M); });
    ExitOnErr(jit->addIRModule(std::move(bpfModule)));
    this->jit = std::move(jit);
}

ebpf_llvm_jit::utils::precompiled_ebpf_function context::compile() {

    spin_lock_guard guard(compiling.get());
    if (!this->jit.has_value()) {
        do_jit_compile();
    } else {
        SPDLOG_DEBUG("LLVM-JIT: already compiled");
    }

    return this->get_entry_address();
}

context::~context()
{
    pthread_spin_destroy(compiling.get());
}

std::vector<uint8_t> context::do_aot_compile(
        const std::vector<std::string> &extFuncNames,
        const std::vector<std::string> &lddwHelpers,
        bool print_ir,
        bool riscv)
{
    SPDLOG_INFO("AOT: start");
    if (auto module = generateModule(extFuncNames, lddwHelpers, false);
            module) {

        LLVMInitializeAllTargetInfos();
        LLVMInitializeAllTargets();
        LLVMInitializeAllTargetMCs();
        LLVMInitializeAllAsmParsers();
        LLVMInitializeAllAsmPrinters();

        auto defaultTargetTriple = llvm::sys::getDefaultTargetTriple();
        // Define CPU and features with hardware FPU
        std::string cpu = "generic";
        std::string features;

        if (riscv) {
            defaultTargetTriple = "riscv64-unknown-linux-gnu";
            cpu = "generic"; // or a specific RISC-V CPU like 'rocket'
            features = "+f,+d";
        }

        SPDLOG_INFO("AOT: target triple: {}", defaultTargetTriple);
        return module->withModuleDo([&](auto &module)
                                            -> std::vector<uint8_t> {
            if (print_ir) {
                module.print(llvm::errs(), nullptr);
            }

            //optimizeModule(module);

            // Set the target triple for the module
            module.setTargetTriple(defaultTargetTriple);

            std::string error;
            auto target = llvm::TargetRegistry::lookupTarget(
                    defaultTargetTriple, error);

            if (!target) {
                SPDLOG_ERROR(
                        "AOT: Failed to get local target: {}",
                        error);
                throw std::runtime_error(
                        "Unable to get local target");
            }

            // Create target machine with hardware floating-point support (RV64G with FD extensions)
            llvm::TargetMachine *targetMachine = target->createTargetMachine(
                    defaultTargetTriple, cpu, features, llvm::TargetOptions(), llvm::Reloc::PIC_);

            if (!targetMachine) {
                SPDLOG_ERROR("Unable to create target machine");
                throw std::runtime_error(
                        "Unable to create target machine");
            }
            module.setDataLayout(targetMachine->createDataLayout());
            llvm::SmallVector<char, 0> objStream;
            std::unique_ptr<llvm::raw_svector_ostream> BOS =
                    std::make_unique<llvm::raw_svector_ostream>(
                            objStream);

            llvm::legacy::PassManager pass;
// auto FileType = CGFT_ObjectFile;
#if LLVM_VERSION_MAJOR >= 18
            if (targetMachine->addPassesToEmitFile(
				    pass, *BOS, nullptr,
				    CodeGenFileType::ObjectFile)) {
#elif LLVM_VERSION_MAJOR >= 10
            if (targetMachine->addPassesToEmitFile(
                    pass, *BOS, nullptr, llvm::CGFT_ObjectFile)) {
#elif LLVM_VERSION_MAJOR >= 8
                if (targetMachine->addPassesToEmitFile(
				    pass, *BOS, nullptr,
				    TargetMachine::CGFT_ObjectFile)) {
#else
			if (targetMachine->addPassesToEmitFile(
				    pass, *BOS, TargetMachine::CGFT_ObjectFile,
				    true)) {
#endif
                SPDLOG_ERROR(
                        "Unable to emit module for target machine");
                throw std::runtime_error(
                        "Unable to emit module for target machine");
            }

            pass.run(module);
            SPDLOG_INFO("AOT: done, received {} bytes",
                        objStream.size());

            std::vector<uint8_t> result(objStream.begin(),
                                        objStream.end());
            return result;
        });
    } else {
        std::string buf;
        llvm::raw_string_ostream os(buf);
        os << module.takeError();
        SPDLOG_ERROR("Unable to generate module: {}", buf);
        throw std::runtime_error("Unable to generate llvm module");
    }
}

std::vector<uint8_t> context::do_aot_compile(bool print_ir, bool riscv)
{
    std::vector<std::string> extNames, lddwNames;

    for (uint32_t i = 0; i < std::size(vm->ext_funcs); i++) {

        if (vm->ext_funcs[i].has_value()) {

            SPDLOG_INFO("Found ext_fn non empty: {} -> {}", i, vm->ext_funcs[i]->name);

#if LLVM_VERSION_MAJOR >= 16
            extNames.emplace_back(ext_func_sym(i));
#else
            extNames.push_back(utils::ext_func_sym(i));
#endif
        }
    }

    const auto tryDefineLddwHelper = [&](const char *name, void *func) {
        if (func) {
#if LLVM_VERSION_MAJOR >= 16
            lddwNames.emplace_back(name);
#else
            lddwNames.emplace_back(name);
#endif
        }
    };
    // Only map_val will have a chance to be called at runtime
    tryDefineLddwHelper(LDDW_HELPER_MAP_VAL, (void *)vm->map_val);
    // These symbols won't be used at runtime
    // tryDefineLddwHelper(LDDW_HELPER_MAP_BY_FD, (void *)vm->map_by_fd);
    // tryDefineLddwHelper(LDDW_HELPER_MAP_BY_IDX, (void *)vm->map_by_idx);
    // tryDefineLddwHelper(LDDW_HELPER_CODE_ADDR, (void *)vm->code_addr);
    // tryDefineLddwHelper(LDDW_HELPER_VAR_ADDR, (void *)vm->var_addr);
    return this->do_aot_compile(extNames, lddwNames, print_ir, riscv);
}

void context::load_aot_object(const std::vector<uint8_t> &buf)
{
    SPDLOG_INFO("LLVM-JIT: Loading aot object");
    if (jit.has_value()) {
        SPDLOG_ERROR("Unable to load aot object: already compiled");
        throw std::runtime_error(
                "Unable to load aot object: already compiled");
    }
    auto buffer = llvm::MemoryBuffer::getMemBuffer(
            llvm::StringRef((const char *)buf.data(), buf.size()));
    auto [jit, extFuncNames, definedLddwHelpers] =
            create_and_initialize_lljit_instance();
    if (auto err = jit->addObjectFile(std::move(buffer)); err) {
        std::string buf;
        llvm::raw_string_ostream os(buf);
        os << err;
        SPDLOG_CRITICAL("Unable to add object file: {}", buf);
        throw std::runtime_error("Failed to load AOT object");
    }
    this->jit = std::move(jit);
    // Test getting entry function
    this->get_entry_address();
}

std::tuple<std::unique_ptr<llvm::orc::LLJIT>, std::vector<std::string>,
        std::vector<std::string> >
context::create_and_initialize_lljit_instance()
{
    // Create a JIT builder
    SPDLOG_DEBUG("LLVM-JIT: Creating LLJIT instance");
    auto jit = ExitOnErr(llvm::orc::LLJITBuilder().create());

    auto &mainDylib = jit->getMainJITDylib();
    std::vector<std::string> extFuncNames;
    // insert the helper functions
    llvm::orc::SymbolMap extSymbols;
    for (uint32_t i = 0; i < std::size(vm->ext_funcs); i++) {
        if (vm->ext_funcs[i].has_value()) {
            auto sym = llvm::JITEvaluatedSymbol::fromPointer(
                    vm->ext_funcs[i]->fn);
            auto symName = jit->getExecutionSession().intern(
                    utils::ext_func_sym(i));
            sym.setFlags(llvm::JITSymbolFlags::Callable |
                                 llvm::JITSymbolFlags::Exported);

#if LLVM_VERSION_MAJOR < 17
            extSymbols.try_emplace(symName, sym);
            extFuncNames.push_back(utils::ext_func_sym(i));
#else
            auto symbol = ::llvm::orc::ExecutorSymbolDef(
				::llvm::orc::ExecutorAddr(sym.getAddress()),
				sym.getFlags());
			extSymbols.try_emplace(symName, symbol);
			extFuncNames.emplace_back(ext_func_sym(i));
#endif
        }
    }
#if defined(__arm__) || defined(_M_ARM)
    SPDLOG_INFO("Defining __aeabi_unwind_cpp_pr1 on arm32");
	extSymbols.try_emplace(
		jit->getExecutionSession().intern("__aeabi_unwind_cpp_pr1"),
		JITEvaluatedSymbol::fromPointer(__aeabi_unwind_cpp_pr1));
#endif
    ExitOnErr(mainDylib.define(absoluteSymbols(extSymbols)));
    // Define lddw helpers
    llvm::orc::SymbolMap lddwSyms;
    std::vector<std::string> definedLddwHelpers;
    const auto tryDefineLddwHelper = [&](const char *name, void *func) {
        if (func) {
            SPDLOG_DEBUG("Defining LDDW helper {} with addr {:x}",
                         name, (uintptr_t)func);
            auto sym = llvm::JITEvaluatedSymbol::fromPointer(func);
            // printf("The type of sym %s\n", typeid(sym).name());
            sym.setFlags(llvm::JITSymbolFlags::Callable |
                                 llvm::JITSymbolFlags::Exported);

#if LLVM_VERSION_MAJOR < 17
            lddwSyms.try_emplace(
                    jit->getExecutionSession().intern(name), sym);
            definedLddwHelpers.emplace_back(name);
#else
            auto symbol = ::llvm::orc::ExecutorSymbolDef(
				::llvm::orc::ExecutorAddr(sym.getAddress()),
				sym.getFlags());
			lddwSyms.try_emplace(
				jit->getExecutionSession().intern(name),
				symbol);
			definedLddwHelpers.emplace_back(name);
#endif
        }
    };
    // Only map_val will have a chance to be called at runtime, so it's the
    // only symbol to be defined
    tryDefineLddwHelper(LDDW_HELPER_MAP_VAL, (void *)vm->map_val);
    // These symbols won't be used at runtime
    // tryDefineLddwHelper(LDDW_HELPER_MAP_BY_FD, (void *)vm->map_by_fd);
    // tryDefineLddwHelper(LDDW_HELPER_MAP_BY_IDX, (void *)vm->map_by_idx);
    // tryDefineLddwHelper(LDDW_HELPER_CODE_ADDR, (void *)vm->code_addr);
    // tryDefineLddwHelper(LDDW_HELPER_VAR_ADDR, (void *)vm->var_addr);
    ExitOnErr(mainDylib.define(absoluteSymbols(lddwSyms)));
    return { std::move(jit), extFuncNames, definedLddwHelpers };
}

ebpf_llvm_jit::utils::precompiled_ebpf_function
context::get_entry_address()
{
    if (!this->jit.has_value()) {
        SPDLOG_CRITICAL(
                "Not compiled yet. Unable to get entry func address");
        throw std::runtime_error("Not compiled yet");
    }
    if (auto err = (*jit)->lookup("bpf_main"); !err) {
        std::string buf;
        llvm::raw_string_ostream os(buf);
        os << err.takeError();
        SPDLOG_CRITICAL("Unable to find symbol `bpf_main`: {}", buf);
        throw std::runtime_error("Unable to link symbol `bpf_main`");
    } else {
        auto addr = err->toPtr<ebpf_llvm_jit::utils::precompiled_ebpf_function>();
        SPDLOG_DEBUG("LLVM-JIT: Entry func is {:x}", (uintptr_t)addr);
        return addr;
    }
}


