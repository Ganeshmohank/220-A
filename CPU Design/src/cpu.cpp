// =============================================================================
//  SPARTAN-16  –  CPU state helpers
// =============================================================================
#include "cpu.h"
#include <cstdio>
#include <cstring>

void CPUState::reset() {
    std::fill(regs, regs + NUM_REGS, static_cast<u16>(0));
    regs[REG_SP] = STACK_INIT_ADDR;  // stack pointer starts at top of RAM
    pc     = RESET_VECTOR;
    flags  = 0;
    cycles = 0;
    halted = false;
}

std::string CPUState::flags_str() const {
    char buf[16];
    snprintf(buf, sizeof(buf), "[%c%c%c%c]",
             flag_z() ? 'Z' : '-',
             flag_n() ? 'N' : '-',
             flag_c() ? 'C' : '-',
             flag_v() ? 'V' : '-');
    return buf;
}

void CPUState::dump() const {
    printf("\n── Register File ──────────────────────────────────────────────\n");
    for (int i = 0; i < 8; ++i) {
        printf("  R%d  = 0x%04X  (%5u)%s",
               i, regs[i], regs[i], (i % 2 == 1) ? "\n" : "    ");
    }
    printf("  SP  = 0x%04X  (%5u)    LR  = 0x%04X  (%5u)\n",
           regs[REG_SP], regs[REG_SP], regs[REG_LR], regs[REG_LR]);
    printf("  PC  = 0x%04X            FLAGS = %s\n", pc, flags_str().c_str());
    printf("  Cycles executed: %llu\n", (unsigned long long)cycles);
    printf("───────────────────────────────────────────────────────────────\n");
}
