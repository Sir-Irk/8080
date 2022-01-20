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
print_state(cpu_8080 *s)
{
    printf("PC: %hu | SP: %hu | IE: %u\n", s->pc, s->sp, s->interruptEnabled);
    printf("A: %u | B: %u | C: %u | D: %u | E %u | H %u | L %u\n", s->a, s->b, s->c, s->d, s->e, s->h, s->l);
    printf("Z: %u | S: %u | P: %u | CY: %u | AC: %u\n\n", s->cc.z, s->cc.s, s->cc.p, s->cc.cy, s->cc.ac);
}

internal inline void
machine_out(struct cpu_8080 *state, u8 port)
{
    printf("MACHINE OUT\n");
}

internal inline u8
machine_in(struct cpu_8080 *state, u8 port)
{
    printf("MACHINE IN\n");
    return 0;
}

int main()
{
    struct game_memory gm = { 0 };
    gm.primaryBuffer = si_allocate_primary_buffer(si_megabytes(256), 0);
    si_initialize_arena(&gm.mainArena, si_megabytes(128), gm.primaryBuffer.data);

    struct read_file_result program = read_entire_file("invaders/invaders", &gm.mainArena);
    // struct read_file_result program = read_entire_file("cpudiag.bin", &gm.mainArena);
    assert(program.contents);

    struct cpu_8080 state = {};
    u16 stackSize = si_kilobytes(4);
    state.m = si_push_size(&gm.mainArena, program.contentsSize + stackSize);
    memcpy(state.m, program.contents, program.contentsSize);

    // state.sp = stackStart;
    u64 instructionsExecuted = 0;
    print_state(&state);
    printf("Program size %td\n", program.contentsSize);

    while (true) {
        print_8080_hex_instruction(&state.m[state.pc]);
        // printf("\n");
        if (instructionsExecuted >= 50000) {
            while ((getchar()) != '\n') {}
        }

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

    si_free_primary_buffer(&gm.primaryBuffer);

    return 0;
}