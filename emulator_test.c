#include <stdio.h>

#define SI_MEMORY_IMPLEMENTATION
#include "si_memory.h"

#include "types.h"
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include "8080.c"
#include "decompiler_debug.c"
#include "read_file.c"

typedef struct game_memory {
    si_primary_buffer primaryBuffer;
    si_memory_arena mainArena;
} game_memory;

internal void
print_state(state_8080 *s)
{
    printf("PC: %hu | SP: %hu | IE: %u\n", s->pc, s->sp, s->interruptEnabled);
    printf("A: %u | B: %u | C: %u | D: %u | E %u | H %u | L %u\n", s->a, s->b, s->c, s->d, s->e, s->h, s->l);
    printf("Z: %u | S: %u | P: %u | CY: %u | AC: %u\n\n", s->cc.z, s->cc.s, s->cc.p, s->cc.cy, s->cc.ac);
}

internal inline void
machine_out(struct state_8080 *state, u8 port)
{
    printf("MACHINE OUT\n");
}

internal inline u8
machine_in(struct state_8080 *state, u8 port)
{
    printf("MACHINE IN\n");
    return 0;
}

int main()
{
    struct game_memory gm = { 0 };
    gm.primaryBuffer = si_allocate_primary_buffer(si_megabytes(256), 0);
    si_initialize_arena(&gm.mainArena, si_megabytes(128), gm.primaryBuffer.data);

    // struct read_file_result program = read_entire_file("invaders/invaders", &gm.mainArena);
    struct read_file_result program = read_entire_file("cpudiag.bin", &gm.mainArena);
    assert(program.contents);

    struct state_8080 state = {};
    u16 stackSize = si_kilobytes(4);
    u16 stackStart = program.contentsSize + stackSize;
    state.m = si_push_size(&gm.mainArena, program.contentsSize + stackSize);
    // memcpy(state.m + 0x100, program.contents, program.contentsSize);
    memcpy(state.m, program.contents, program.contentsSize);

// state.pc = (256-171);
#if 0
    state.pc = 0x100;

    // Fix the first instruction to be JMP 0x100
    state.m[0] = 0xc3;
    state.m[1] = 0x65;
    state.m[2] = 0x00;

    // Fix the stack pointer from 0x6ad to 0x7ad
    //  this 0x06 byte 112 in the code, which is
    //  byte 112 + 0x100 = 368 in memory
    state.m[368] = 0x7;
    // Skip DAA test
    state.m[0x59c] = 0xc3; // JMP
    state.m[0x59d] = 0xc2; // addr byte 1
    state.m[0x59e] = 0x05; // addr byte 2
#endif;

#if 0
    printf("Loaded program: ");
    for (int i = 0; i < program.contentsSize; ++i) {
        printf("%02x ", ((u8 *)program.contents)[i]);
    }
    printf("\n");
#endif

#if 1
    // state.sp = stackStart;
    u64 instructionsExecuted = 0;
    print_state(&state);
    // printf("%td", program.contentsSize);
    //  while (state.pc < program.contentsSize && state.pc >= 0) {

    while (true) {
        print_8080_hex_instruction(&state.m[state.pc]);
        // printf("\n");
        while ((getchar()) != '\n') {}

        u8 *opcode = &state.m[state.pc];
        if (opcode[0] == 0xdb) { // IN
            u8 port = opcode[1];
            state.a = machine_in(&state, port);
            state.pc += 2;
        } else if (opcode[0] == 0xd3) { // OUT
            u8 port = opcode[1];
            machine_out(&state, port);
            state.pc += 2;
        } else {
            emulate_8080(&state);
        }

        // if (instructionsExecuted++ >= 1000000) break;
        instructionsExecuted++;
        printf("Instuctions executed: %" PRIu64 "\n", instructionsExecuted);
        print_state(&state);
        //    assert(state.sp >= 0);
        //     assert(state.sp <= stackStart);
    }

    print_state(&state);
    printf("Instuctions executed: %" PRIu64 "\n", instructionsExecuted);
#endif

    si_free_primary_buffer(&gm.primaryBuffer);

    return 0;
}