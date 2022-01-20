#include <stdio.h>
#include <stdlib.h>

#define SI_MEMORY_IMPLEMENTATION
#include "si_memory.h"

#include "types.h"
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>

#include "read_file.c"

typedef struct game_memory {
    si_primary_buffer primaryBuffer;
    si_memory_arena mainArena;
} game_memory;

internal inline char *
skip_white_space(char *str)
{
    while (*str == ' ') {
        ++str;
    }
    return str;
}

const char validRegisterNames[] = { 'A', 'B', 'C', 'D', 'E', 'H', 'L', 'M' };

internal b32
valid_register_name(char r)
{
    for (int i = 0; i < si_array_count(validRegisterNames); ++i) {
        if (r == validRegisterNames[i]) {
            return true;
        }
    }
    return false;
}

internal void
validate_register_name(char reg)
{
    if (!valid_register_name(reg)) {
        fprintf(stderr, "Invalid register name \"%c\"\n", reg);
        assert(false);
        // exit(1);
    }
}

internal inline u8
instruction_register_pair(u8 baseInstruction, u8 mul, u8 reg)
{
    u8 instruction = 0;
    switch (reg) {
        case 'B': instruction = baseInstruction + 0 * mul; break;
        case 'D': instruction = baseInstruction + 1 * mul; break;
        case 'H': instruction = baseInstruction + 1 * mul; break;
        // TODO handle SP
        default: {
            fprintf(stderr, "Error: invalid register name for instruction LXI: %c\n", (char)reg);
            assert(false);
        } break;
    }
    return instruction;
}

internal inline u8
instruction_offset(u8 baseInstruction, u8 mul, u8 reg)
{
    u8 instruction = baseInstruction;
    switch (reg) {
        case 'B': instruction = baseInstruction + 0 * mul; break;
        case 'C': instruction = baseInstruction + 1 * mul; break;
        case 'D': instruction = baseInstruction + 2 * mul; break;
        case 'E': instruction = baseInstruction + 3 * mul; break;
        case 'H': instruction = baseInstruction + 4 * mul; break;
        case 'L': instruction = baseInstruction + 5 * mul; break;
        case 'M': instruction = baseInstruction + 6 * mul; break;
        case 'A': instruction = baseInstruction + 7 * mul; break;
        default: {
            assert(false);
        } break;
    }
    return instruction;
}

internal inline char *
extract_register_name(char *code, u8 *out)
{
    code = skip_white_space(code);
    *out = *code++;
    valid_register_name(*out);
    return code;
}
internal inline char *
extract_immediate_u16(char *code, u16 *out)
{
    code = skip_white_space(code);
    char buf[6];
    int i = 0;
    while (true) {
        char c = *code;
        if (c == '\n' || c == '\0') {
            break;
        }
        assert((i + 1) <= 5);
        assert(isdigit(c));
        buf[i++] = c;
        code++;
    }
    buf[5] = '\0';
    u32 result = (u32)strtol(buf, NULL, 10);
    if (result > 0xffff) {
        fprintf(stderr, "Error: value %d is too large for 16-bits\n", result);
        assert(false);
    }
    *out = (u16)result;
    return code;
}

internal inline char *
extract_immediate_u8(char *code, u8 *out)
{
    code = skip_white_space(code);
    char buf[4];
    int i = 0;
    while (true) {
        char c = *code;
        if (c == '\n' || c == '\0') {
            break;
        }
        assert((i + 1) <= 3);
        assert(isdigit(c));
        buf[i++] = c;
        code++;
    }
    buf[3] = '\0';
    u16 result = (u16)strtol(buf, NULL, 10);
    if (result > 0xff) {
        fprintf(stderr, "Error: value %d is too large for 8-bits\n", result);
        assert(false);
    }
    *out = (u8)result;
    return code;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Error: Need a file name\n");
        return -1;
    }
    struct game_memory gm = { 0 };
    gm.primaryBuffer = si_allocate_primary_buffer(si_megabytes(8), 0);
    si_initialize_arena(&gm.mainArena, si_megabytes(4), gm.primaryBuffer.data);

    struct read_file_result program = read_entire_file(argv[1], &gm.mainArena);
    assert(program.contents);

    char *code = (char *)program.contents;
    printf("%s\n", code);

    si_size outSize = si_kilobytes(1);
    u8 *out = si_push_size(&gm.mainArena, outSize);

    i32 outIdx = 0;
    i32 lineCounter = 1;

    while (1) {
        code = skip_white_space(code);
        if (*code == '\0')
            break;

        i32 byteCount = 1;
        u8 instruction[3] = { 0 }; // 0 = NOP

        if (strncmp(code, "NOP", 3) == 0) {
            code += 3;
        } else if (strncmp(code, "MVI", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            if (strncmp(code, ",", 1) == 0) {
                u8 immediate;
                code = extract_immediate_u8(code + 1, &immediate);
                instruction[1] = immediate;
                instruction[0] = instruction_offset(0x06, 8, reg0);
                byteCount = 2;
            } else {
                assert(!"IVALID SYNTAX");
            }
        } else if (strncmp(code, "MOV", 3) == 0) {
            u8 reg0, reg1;
            code = extract_register_name(code + 3, &reg0);
            if (strncmp(code, ",", 1) == 0) {
                code = extract_register_name(code + 1, &reg1);
                switch (reg0) {
                    case 'B': instruction[0] = instruction_offset(0x40, 1, reg1); break;
                    case 'C': instruction[0] = instruction_offset(0x48, 1, reg1); break;
                    case 'D': instruction[0] = instruction_offset(0x50, 1, reg1); break;
                    case 'E': instruction[0] = instruction_offset(0x58, 1, reg1); break;
                    case 'H': instruction[0] = instruction_offset(0x60, 1, reg1); break;
                    case 'L': instruction[0] = instruction_offset(0x68, 1, reg1); break;
                    case 'M': instruction[0] = instruction_offset(0x70, 1, reg1); break;
                    case 'A': instruction[0] = instruction_offset(0x78, 1, reg1); break;
                }
            } else {
                assert(!"IVALID SYNTAX");
            }
        } else if (strncmp(code, "LXI", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            if (strncmp(code, ",", 1) == 0) {
                u16 imm0;
                code = extract_immediate_u16(code + 1, &imm0);
                instruction[1] = (u8)(imm0 >> 0);
                instruction[2] = (u8)(imm0 >> 8);
                instruction[0] = instruction_register_pair(0x01, 16, reg0);
                byteCount = 3;
            }
        } else if (strncmp(code, "CMA", 3) == 0) {
            instruction[0] = 0x2f;
            code = skip_white_space(code + 3);
        } else if (strncmp(code, "CMC", 3) == 0) {
            instruction[0] = 0x3f;
            code = skip_white_space(code + 3);
        } else if (strncmp(code, "RLC", 3) == 0) {
            instruction[0] = 0x07;
            code = skip_white_space(code + 3);
        } else if (strncmp(code, "RRC", 3) == 0) {
            instruction[0] = 0x0f;
            code = skip_white_space(code + 3);
        } else if (strncmp(code, "RAL", 3) == 0) {
            instruction[0] = 0x17;
            code = skip_white_space(code + 3);
        } else if (strncmp(code, "RAR", 3) == 0) {
            instruction[0] = 0x1f;
            code = skip_white_space(code + 3);
        } else if (strncmp(code, "INX", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            instruction[0] = instruction_register_pair(0x03, 16, reg0);
        } else if (strncmp(code, "INR", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            instruction[0] = instruction_offset(0x04, 8, reg0);
        } else if (strncmp(code, "DCR", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            instruction[0] = instruction_offset(0x05, 8, reg0);
        } else if (strncmp(code, "ADD", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            instruction[0] = instruction_offset(0x80, 1, reg0);
        } else if (strncmp(code, "ADC", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            instruction[0] = instruction_offset(0x88, 1, reg0);
        } else if (strncmp(code, "ADI", 3) == 0) {
            u8 imm0;
            code = extract_immediate_u8(code + 3, &imm0);
            instruction[0] = 0xc6;
            instruction[1] = imm0;
            byteCount = 2;
        } else if (strncmp(code, "DAD", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            instruction[0] = instruction_register_pair(0x09, 16, reg0);
        } else if (strncmp(code, "SUB", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            instruction[0] = instruction_offset(0x90, 1, reg0);
        } else if (strncmp(code, "SBB", 3) == 0) {
            u8 reg0;
            code = extract_register_name(code + 3, &reg0);
            instruction[0] = instruction_offset(0x98, 1, reg0);
        } else {
            char *ptr = code;
            printf("Error: Invalid instruction(line %d): \"", lineCounter);
            while (*ptr != '\n' && *ptr != '\0') {
                printf("%c", *ptr++);
            }
            printf("\"\n");
            assert(false);
        }
        // code = skip_white_space(code + 1);

        for (int i = 0; i < byteCount; ++i) {
            // printf("0x%x ", instruction[i]);
            out[outIdx++] = instruction[i];
        }
        // printf("\n");

        lineCounter++;
        assert(code[0] == '\n' || code[0] == '\0');
        if (*code == '\n') {
            ++code;
        } else if (*code == '\0') {
            break;
        }
    }
    out[outIdx] = '\0';
    for (int i = 0; i < outIdx; ++i) {
        printf("%02x ", out[i]);
    }
    printf("\n");

    FILE *file = fopen("test.o", "wb+");
    assert(file);
    fwrite(out, 1, outIdx, file);
    fclose(file);

    printf("\n%d\n", (14 << 8) | 16);

    si_free_primary_buffer(&gm.primaryBuffer);

    return 0;
}