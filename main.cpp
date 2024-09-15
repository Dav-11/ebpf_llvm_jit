#include <filesystem>
#include <cstdint>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>

#include <bpf/libbpf.h>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "argparse/argparse.hpp"
#include "src/jit/vm.h"
#include "src/jit/context.h"
#include "src/helpers/helper.h"
#include "src/include/helpers_impl.h"

static int add_helpers(ebpf_llvm_jit::jit::vm *vm) {
    int err;

    ebpf_llvm_jit::helpers::helper bpf_printk = ebpf_llvm_jit::helpers::helper(6, "_bpf_helper_ext_0006", (void *) _bpf_helper_ext_0006);
    err = bpf_printk.addToVm(vm);
    if (err) {
        return err;
    }

    SPDLOG_INFO("added helper {}", "_bpf_helper_ext_0006 (bpf_printk)");

    return 0;
}

static int build_ebpf_program(const std::string &ebpf_elf,
                              const std::filesystem::path &output, bool to_riscv)
{
    bpf_object *obj = bpf_object__open(ebpf_elf.c_str());
    if (!obj) {
        SPDLOG_CRITICAL("Unable to open BPF elf: {}", errno);
        return 1;
    }
    std::unique_ptr<bpf_object, decltype(&bpf_object__close)> elf(
            obj, bpf_object__close);
    bpf_program *prog;

    bpf_object__for_each_program(prog, elf.get())
    {
        auto name = bpf_program__name(prog);
        SPDLOG_INFO("Processing program {}", name);

        ebpf_llvm_jit::jit::vm vm;

        if (add_helpers(&vm)) {
            SPDLOG_ERROR("error while loading helpers");
            return 1;
        }

        if (vm.load_code(
            (const void *)bpf_program__insns(prog),
            (uint32_t)bpf_program__insn_cnt(prog) * 8) < 0
        ) {
            SPDLOG_ERROR(
                    "Unable to load instruction of program {}: ",
                    name, vm.get_error_message());
            return 1;
        }

        ebpf_llvm_jit::jit::context ctx(&vm);

        // write result to file
        auto result = ctx.do_aot_compile(true, to_riscv);
        auto out_path = output / (std::string(name) + ".o");
        std::ofstream ofs(out_path, std::ios::binary);
        ofs.write((const char *)result.data(), result.size());

        SPDLOG_INFO("Program {} written to {}", name, out_path.c_str());
    }

    return 0;
}


int main(int argc, const char **argv)
{

    // set global level to debug:
    // export SPDLOG_LEVEL=debug
    // spdlog::cfg::load_env_levels();
    spdlog::set_level(spdlog::level::trace);


//    ????????
//    InitializeNativeTarget();
//    InitializeNativeTargetAsmPrinter();

    argparse::ArgumentParser program(argv[0]);
    argparse::ArgumentParser build_command("build");
    argparse::ArgumentParser buildrv_command("buildrv");

    build_command.add_description(
            "Build native ELF(s) from eBPF ELF. Each program in the eBPF ELF will be built into a single native ELF");
    build_command.add_argument("-o", "--output")
            .default_value(".")
            .help("Output directory (There might be multiple output files for a single input)");
    build_command.add_argument("EBPF_ELF")
            .help("Path to an eBPF ELF executable");

    buildrv_command.add_description(
            "Build native ELF(s) from eBPF ELF. Each program in the eBPF ELF will be built into a single native ELF");
    buildrv_command.add_argument("-o", "--output")
            .default_value(".")
            .help("Output directory (There might be multiple output files for a single input)");
    buildrv_command.add_argument("EBPF_ELF")
            .help("Path to an eBPF ELF executable");

//    argparse::ArgumentParser run_command("run");
//    run_command.add_description("Run an native eBPF program");
//    run_command.add_argument("PATH").help("Path to the ELF file");
//    run_command.add_argument("MEMORY")
//            .help("Path to the memory file")
//            .nargs(0, 1);

    program.add_subparser(build_command);
    program.add_subparser(buildrv_command);
//    program.add_subparser(run_command);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }
    if (!program) {
        std::cerr << program;
        std::exit(1);
    }

    if (program.is_subcommand_used(build_command)) {
        return build_ebpf_program(
                build_command.get<std::string>("EBPF_ELF"),
                build_command.get<std::string>("output"),
                false);
    } else if (program.is_subcommand_used(buildrv_command)) {
        return build_ebpf_program(
                buildrv_command.get<std::string>("EBPF_ELF"),
                buildrv_command.get<std::string>("output"),
                true);
    }


//    else if (program.is_subcommand_used(run_command)) {
//        if (run_command.is_used("MEMORY")) {
//            return run_ebpf_program(
//                    run_command.get<std::string>("PATH"),
//                    run_command.get<std::string>("MEMORY"));
//        } else {
//            return run_ebpf_program(
//                    run_command.get<std::string>("PATH"), {});
//        }
//    }
    return 0;
}