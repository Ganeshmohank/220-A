// =============================================================================
//  SPARTAN-16  –  Command-line entry point
// =============================================================================
//
//  Usage:
//    cpu assemble <source.asm> [-o <output.bin>]   Assemble source → binary
//    cpu run      <program.bin> [--trace] [--dump]  Run binary
//    cpu exec     <source.asm>  [--trace] [--dump]  Assemble + run in one step
//
//  Flags:
//    --trace   Print every instruction as it executes (Fetch/Decode/Execute log)
//    --dump    Hex-dump the first 256 bytes of RAM after the program halts
//
// =============================================================================
#include "assembler.h"
#include "emulator.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// ── Small helpers ─────────────────────────────────────────────────────────────
static bool has_flag(const std::vector<std::string>& args, const std::string& flag) {
    for (const auto& a : args) if (a == flag) return true;
    return false;
}

static std::string replace_ext(const std::string& path, const std::string& ext) {
    auto dot = path.rfind('.');
    return (dot == std::string::npos ? path : path.substr(0, dot)) + ext;
}

static void print_usage(const char* prog) {
    fprintf(stderr,
        "\nSPARTAN-16 CPU Emulator & Assembler\n"
        "=====================================\n"
        "Usage:\n"
        "  %s assemble <source.asm> [-o out.bin]  Assemble to binary\n"
        "  %s run      <prog.bin>  [--trace] [--dump]  Run binary\n"
        "  %s exec     <source.asm>[--trace] [--dump]  Assemble & run\n"
        "\nFlags:\n"
        "  --trace   Show Fetch/Decode/Execute trace for every instruction\n"
        "  --dump    Hex-dump the first 256 bytes of RAM after halt\n\n",
        prog, prog, prog);
}

// =============================================================================
//  ASSEMBLE sub-command
// =============================================================================
static int cmd_assemble(const std::vector<std::string>& args) {
    if (args.empty()) {
        fprintf(stderr, "Error: 'assemble' requires a source file argument.\n");
        return 1;
    }

    std::string infile  = args[0];
    std::string outfile = replace_ext(infile, ".bin");

    // Check for explicit output flag
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "-o" && i + 1 < args.size()) {
            outfile = args[i + 1];
            break;
        }
    }

    Assembler asm_;
    if (!asm_.assemble(infile, outfile)) return 1;
    return 0;
}

// =============================================================================
//  RUN sub-command
// =============================================================================
static int cmd_run(const std::vector<std::string>& args,
                   bool trace, bool dump) {
    if (args.empty()) {
        fprintf(stderr, "Error: 'run' requires a binary file argument.\n");
        return 1;
    }

    Emulator emu;
    if (!emu.load(args[0])) return 1;

    printf("\n──────────────────────────────────────────────\n");
    printf("  SPARTAN-16  Running: %s\n", args[0].c_str());
    printf("──────────────────────────────────────────────\n");

    emu.run(trace);

    if (dump) {
        emu.dump_memory(0x0000, 0x00FF);   // first 256 bytes
    }

    emu.cpu().dump();   // always show final register state
    return 0;
}

// =============================================================================
//  EXEC sub-command  (assemble + run in one step)
// =============================================================================
static int cmd_exec(const std::vector<std::string>& args,
                    bool trace, bool dump) {
    if (args.empty()) {
        fprintf(stderr, "Error: 'exec' requires a source file argument.\n");
        return 1;
    }

    std::string infile  = args[0];
    std::string outfile = replace_ext(infile, ".bin");

    // Assemble
    Assembler asm_;
    if (!asm_.assemble(infile, outfile)) return 1;

    // Run
    std::vector<std::string> run_args = {outfile};
    return cmd_run(run_args, trace, dump);
}

// =============================================================================
//  MAIN
// =============================================================================
int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Collect non-flag arguments and flags
    std::string              sub_cmd = argv[1];
    std::vector<std::string> rest;
    std::vector<std::string> all_args;

    for (int i = 2; i < argc; ++i) all_args.emplace_back(argv[i]);

    // Filter flag arguments
    bool trace = has_flag(all_args, "--trace");
    bool dump  = has_flag(all_args, "--dump");
    for (const auto& a : all_args)
        if (a != "--trace" && a != "--dump") rest.push_back(a);

    // Dispatch
    if (sub_cmd == "assemble") return cmd_assemble(rest);
    if (sub_cmd == "run")      return cmd_run(rest, trace, dump);
    if (sub_cmd == "exec")     return cmd_exec(rest, trace, dump);

    fprintf(stderr, "Error: unknown sub-command '%s'\n", sub_cmd.c_str());
    print_usage(argv[0]);
    return 1;
}
