//
// Created by root on 8/17/24.
//

#include "compiler_xdp.h"
#include "spdlog/spdlog.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/ExecutionEngine/JITSymbol.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Error.h>
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

CompilerXDP::CompilerXDP() : ext_funcs(MAX_EXT_FUNCS)
{
    using namespace llvm;
    int zero = 0;
    if (__atomic_compare_exchange_n(&llvm_initialized, &zero, 1, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {

        SPDLOG_INFO("Initializing llvm...");
        LLVMInitializeAllTargetInfos();
        LLVMInitializeAllTargets();
        LLVMInitializeAllTargetMCs();
        LLVMInitializeAllAsmParsers();
        LLVMInitializeAllAsmPrinters();
        SPDLOG_INFO("done");
    }

    compiling = std::make_unique<pthread_spinlock_t>();
    pthread_spin_init(compiling.get(), PTHREAD_PROCESS_PRIVATE);
}
CompilerXDP::~CompilerXDP()
{
    pthread_spin_destroy(compiling.get());
}
std::string CompilerXDP::get_error_message()
{
    return error_msg;
}
int CompilerXDP::load_code(const void *code, size_t code_len)
{
    if (code_len % 8 != 0) {
        error_msg = "Code len must be a multiple of 8";
        return -EINVAL;
    }

    insts.assign((ebpf_inst *)code,(ebpf_inst *)code + code_len / 8);
    return 0;
}
std::vector<uint8_t> CompilerXDP::do_aot_compile(bool print_ir, const std::string &roData)
{

    std::vector<std::string> extFuncNames, lddwHelpers;

    for (uint32_t i = 0; i < std::size(ext_funcs); i++) {

        if (ext_funcs[i].has_value()) {

            SPDLOG_INFO("Found ext_fn non empty: {} -> {}", i, ext_funcs[i]->name);

#if LLVM_VERSION_MAJOR >= 16
            extFuncNames.emplace_back(ext_func_sym(i));
#else
            extFuncNames.push_back(utils::ext_func_sym(i));
#endif
        }
    }

    const auto tryDefineLddwHelper = [&](const char *name, void *func) {
        if (func) {
#if LLVM_VERSION_MAJOR >= 16
            lddwHelpers.emplace_back(name);
#else
            lddwHelpers.emplace_back(name);
#endif
        }
    };

    // Only map_val will have a chance to be called at runtime
    tryDefineLddwHelper(LDDW_HELPER_MAP_VAL, (void *)map_val);

    SPDLOG_INFO("AOT: start");
    if (auto module = generateModule(extFuncNames, lddwHelpers, false); module) {

        // Create LLVM context and IR builder
        llvm::LLVMContext *context = module->getContext().getContext(); //->getContext();
        llvm::IRBuilder<> builder(*context);

        // Convert roData string to an LLVM constant array
        llvm::Constant *roDataConstant = llvm::ConstantDataArray::getString(*context, roData, true);

        // Create a global variable for roData in the module
        llvm::GlobalVariable roDataGV(
            *module,
            roDataConstant->getType(),
            true,
            llvm::GlobalValue::ExternalLinkage,
            roDataConstant,
            //"rodata",
            //nullptr,
            //ThreadLocalMode = NotThreadLocal,
            //Optional<unsigned> AddressSpace = None,
            //bool isExternallyInitialized = false
        );

        // Set the section for the global variable to .rodata
        roDataGV.setSection(".rodata");

        // Optionally align the data (e.g., 1-byte alignment)
        roDataGV.setAlignment(llvm::Align(1));

        auto targetTriple = "riscv64-unknown-elf";
        auto cpu = "generic"; // or a specific RISC-V CPU like 'rocket'
        auto features = ""; //"+f,+d";

        SPDLOG_INFO("AOT: target triple: {}", targetTriple);
        return module->withModuleDo([&](auto &module) -> std::vector<uint8_t> {
            if (print_ir) {
                module.print(llvm::errs(), nullptr);
            }

            // TODO: disabled otherwise it would eliminate calls to helpers
            //optimizeModule(module);

            // Set the target triple for the module
            module.setTargetTriple(targetTriple);

            std::string error;
            auto target = llvm::TargetRegistry::lookupTarget(
                    targetTriple, error);

            if (!target) {
                SPDLOG_ERROR("AOT: Failed to get local target: {}", error);
                throw std::runtime_error("Unable to get local target");
            }

            // Create target machine with hardware floating-point support (RV64G with FD extensions)
            llvm::TargetMachine *targetMachine = target->createTargetMachine(
                    targetTriple, cpu, features, llvm::TargetOptions(), llvm::Reloc::PIC_);

            SPDLOG_INFO("Creating LLVM target machine using [target_machine={}, cpu={}, features={}]", targetTriple, cpu, features);

            if (!targetMachine) {
                SPDLOG_ERROR("Unable to create target machine");
                throw std::runtime_error("Unable to create target machine");
            }
            module.setDataLayout(targetMachine->createDataLayout());
            llvm::SmallVector<char, 0> objStream;
            std::unique_ptr<llvm::raw_svector_ostream> BOS =std::make_unique<llvm::raw_svector_ostream>(
                            objStream);

            llvm::legacy::PassManager pass;
// auto FileType = CGFT_ObjectFile;
#if LLVM_VERSION_MAJOR >= 18
            if (targetMachine->addPassesToEmitFile(
				    pass, *BOS, nullptr,
				    CodeGenFileType::ObjectFile)) {
#elif LLVM_VERSION_MAJOR >= 10
            if (targetMachine->addPassesToEmitFile(pass, *BOS, nullptr, llvm::CGFT_ObjectFile)) {
#elif LLVM_VERSION_MAJOR >= 8
                if (targetMachine->addPassesToEmitFile(
				    pass, *BOS, nullptr,
				    TargetMachine::CGFT_ObjectFile)) {
#else
			if (targetMachine->addPassesToEmitFile(
				    pass, *BOS, TargetMachine::CGFT_ObjectFile,
				    true)) {
#endif
                SPDLOG_ERROR("Unable to emit module for target machine");
                throw std::runtime_error("Unable to emit module for target machine");
            }

            pass.run(module);
            SPDLOG_INFO("AOT: done, received {} bytes",objStream.size());

            std::vector<uint8_t> result(objStream.begin(),objStream.end());
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
int CompilerXDP::register_external_function(size_t index, const std::string &name, void *fn)
{
    if (index >= ext_funcs.size()) {
        error_msg = "Index too large";
        SPDLOG_ERROR("Index too large {} for ext func {}", index, name);
        return -E2BIG;
    }

    if (ext_funcs[index].has_value()) {
        error_msg = "Already defined";
        SPDLOG_ERROR("Index {} already occupied by {}", index, ext_funcs[index]->name);
        return -EEXIST;
    }

    ext_funcs[index] = external_function(name, fn);
    return 0;
}


