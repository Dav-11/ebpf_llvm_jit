#include <filesystem>
#include <cstdint>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <array>

#include <bpf/libbpf.h>

#include <llvm/Object/ObjectFile.h>
#include <llvm/Object/ELF.h>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "argparse/argparse.hpp"
#include "../src/jit/compiler_xdp.h"
#include "../src/helpers/helper.h"
#include "../src/include/helpers_impl.h"
#include "../src/jit/passthrough_section.h"

#define XDP_SECT "xdp"

using namespace llvm::object;
using namespace llvm;

// bpf_printk
uint64_t _bpf_helper_ext_0006(const void *fmt, uint64_t fmt_size, ...)
{
    const char *fmt_str = (const char *)fmt;
    va_list args;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#pragma GCC diagnostic ignored "-Wvarargs"
    va_start(args, fmt_str);
    long ret = vprintf(fmt_str, args);
#pragma GCC diagnostic pop
    va_end(args);
    return 0;
}

// Assume this is called to load and access the eBPF object file
std::string loadSection(const std::string &sourceFile, const std::string& sectionNameString) {

    // Load the source object file
    Expected<OwningBinary<ObjectFile>> binaryOrErr = ObjectFile::createObjectFile(sourceFile);
    if (!binaryOrErr) {
        llvm::errs() << "Failed to open source file\n";
        SPDLOG_ERROR("Failed to open source file {}\n", sourceFile);
        return "";
    }
    if (auto E = binaryOrErr.takeError()) {
        SPDLOG_ERROR("Failed to open source file error: {}\n", toString(std::move(E)));
        return "";
    }

    ObjectFile &srcObj = *binaryOrErr->getBinary();

    // Find the .rodata section
    for (const SectionRef &section : srcObj.sections()) {
        auto sectionName = section.getName();

        if (auto E = sectionName.takeError()) {
            SPDLOG_ERROR("Failed to open source file error: {}\n", toString(std::move(E)));
            return "";
        }

        SPDLOG_INFO("Found Section {}", sectionName->str());

        if (sectionName->str() == sectionNameString) {
            // Retrieve the contents of the .rodata section (string literals)
            auto content = section.getContents();

            if (auto E = content.takeError()) {
                SPDLOG_ERROR("Failed to get content of {} {}\n", sectionNameString, toString(std::move(E)));
                return "";
            }

            return content->str(); // Return the rodata content as a string
        }
    }

    SPDLOG_INFO(".rodata section not found\n");
    return "";
}
static int add_helpers(ebpf_llvm_jit::jit::CompilerXDP *ctx) {
    int err;

    ebpf_llvm_jit::helpers::helper bpf_printk = ebpf_llvm_jit::helpers::helper(
        6,
        "_bpf_helper_ext_0006",
        (void *) _bpf_helper_ext_0006
    );

    err = ctx->register_external_function(bpf_printk.getIndex(), bpf_printk.getName(), bpf_printk.getFn());
    if (err) {
        return err;
    }

    SPDLOG_INFO("added helper {}", "_bpf_helper_ext_0006 (bpf_printk)");

    return 0;
}
static int build_xdp(bpf_program *prog, const char *name, const std::filesystem::path &output, std::vector<ebpf_llvm_jit::jit::passthrough_section> &sections)
{
    ebpf_llvm_jit::jit::CompilerXDP ctx;

    if (add_helpers(&ctx)) {
        SPDLOG_ERROR("error while loading helpers");
        return 1;
    }

    if (ctx.load_code(
            (const void *)bpf_program__insns(prog),
            (uint32_t)bpf_program__insn_cnt(prog) * 8) < 0
            ) {
        SPDLOG_ERROR(
                "Unable to load instruction of program {}: ",
                name, ctx.get_error_message());
        return 1;
    }

    // write result to file
    auto result = ctx.do_aot_compile(true, sections);

    auto out_path = output / (std::string(name) + ".o");
    std::ofstream ofs(out_path, std::ios::binary);
    ofs.write((const char *)result.data(), result.size());

    SPDLOG_INFO("Program {} written to {}", name, out_path.c_str());

    return 0;
}
static int build_ebpf_program(const std::string &ebpf_elf, const std::filesystem::path &output)
{
    bpf_object *obj = bpf_object__open(ebpf_elf.c_str());
    if (!obj) {
        SPDLOG_CRITICAL("Unable to open BPF elf: {}", errno);
        return 1;
    }
    std::unique_ptr<bpf_object, decltype(&bpf_object__close)> elf(obj, bpf_object__close);
    bpf_program *prog;

    std::vector<ebpf_llvm_jit::jit::passthrough_section> sections = {
            {
                    ".rodata.bpf",
                    loadSection(ebpf_elf, ".rodata")
            },
            {
                    ".rodata.str1.1",
                    loadSection(ebpf_elf, ".rodata.str1.1")
            }
    };

    bpf_object__for_each_program(prog, elf.get())
    {
        auto name = bpf_program__name(prog);
        auto sect = bpf_program__section_name(prog);
        SPDLOG_INFO("Processing program {}", name);

        int err = 0;

        if (strcmp(sect, "xdp") == 0) {
            err = build_xdp(prog, name, output, sections);
        } else {
            SPDLOG_ERROR("BPF section type \"{}\" is unsupported", sect);
        }

        if (err != 0) {
            SPDLOG_CRITICAL("Failed to compile {}, err: {}", name, err);
            return err;
        }
    }

    return 0;
}


int main(int argc, const char **argv)
{

    // set global level to debug:
    // export SPDLOG_LEVEL=debug
    // spdlog::cfg::load_env_levels();
    spdlog::set_level(spdlog::level::trace);


    argparse::ArgumentParser program(argv[0]);
    argparse::ArgumentParser build_command("build");

    build_command.add_description(
            "Build native ELF(s) from eBPF ELF. Each program in the eBPF ELF will be built into a single native ELF");
    build_command.add_argument("-o", "--output")
            .default_value(".")
            .help("Output directory (There might be multiple output files for a single input)");
    build_command.add_argument("EBPF_ELF")
            .help("Path to an eBPF ELF executable");

    program.add_subparser(build_command);

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
            build_command.get<std::string>("output")
        );
    }

    return 0;
}
