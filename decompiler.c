#include "types.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define CASE_INSTRUCTION_GROUP_8(startHex, instruction, _outBuffer)                         \
    case (startHex + 0): _outBuffer += sprintf(_outBuffer, "%s     B", instruction); break; \
    case (startHex + 1): _outBuffer += sprintf(_outBuffer, "%s     C", instruction); break; \
    case (startHex + 2): _outBuffer += sprintf(_outBuffer, "%s     D", instruction); break; \
    case (startHex + 3): _outBuffer += sprintf(_outBuffer, "%s     E", instruction); break; \
    case (startHex + 4): _outBuffer += sprintf(_outBuffer, "%s     H", instruction); break; \
    case (startHex + 5): _outBuffer += sprintf(_outBuffer, "%s     L", instruction); break; \
    case (startHex + 6): _outBuffer += sprintf(_outBuffer, "%s     M", instruction); break; \
    case (startHex + 7): _outBuffer += sprintf(_outBuffer, "%s     A", instruction); break;

// derp

#define CASE_INSTRUCTION_GROUP_8_2(startHex, instruction, reg, _outBuffer)                          \
    case (startHex + 0): _outBuffer += sprintf(_outBuffer, "%s     %c,B", instruction, reg); break; \
    case (startHex + 1): _outBuffer += sprintf(_outBuffer, "%s     %c,C", instruction, reg); break; \
    case (startHex + 2): _outBuffer += sprintf(_outBuffer, "%s     %c,D", instruction, reg); break; \
    case (startHex + 3): _outBuffer += sprintf(_outBuffer, "%s     %c,E", instruction, reg); break; \
    case (startHex + 4): _outBuffer += sprintf(_outBuffer, "%s     %c,H", instruction, reg); break; \
    case (startHex + 5): _outBuffer += sprintf(_outBuffer, "%s     %c,L", instruction, reg); break; \
    case (startHex + 6): _outBuffer += sprintf(_outBuffer, "%s     %c,M", instruction, reg); break; \
    case (startHex + 7): _outBuffer += sprintf(_outBuffer, "%s     %c,A", instruction, reg); break;

internal void
decompile_8080_program_from_hex(const u8 *program, ptrdiff_t byteCount, char *outBuffer)
{
    // clang-format off
    assert(outBuffer);
    const u8* code = program;
    for(i32 i = 0; i < byteCount; code = &program[i]) {
        int opbytes = 1;
        outBuffer += sprintf(outBuffer, "%d ", i);
        switch (*code) {
            case 0x00: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0x10: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0x08: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0x18: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0x20: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0x28: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0x30: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0x38: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0xcb: outBuffer += sprintf(outBuffer, "NOP"); break;
            case 0xd9: outBuffer += sprintf(outBuffer,"NOP"); break;
            case 0xdd: outBuffer += sprintf(outBuffer,"NOP"); break;
            case 0xed: outBuffer += sprintf(outBuffer,"NOP"); break;
            case 0xfd: outBuffer += sprintf(outBuffer,"NOP"); break;
            case 0x07: outBuffer += sprintf(outBuffer,"RLC      "); break;
            case 0x17: outBuffer += sprintf(outBuffer,"RAL      "); break;
            case 0x27: outBuffer += sprintf(outBuffer,"DAA      "); break;
            case 0x0a: outBuffer += sprintf(outBuffer,"LDAX    B"); break;
            case 0x1a: outBuffer += sprintf(outBuffer,"LDAX    D"); break;
            case 0x02: outBuffer += sprintf(outBuffer,"STAX    B"); break;
            case 0x12: outBuffer += sprintf(outBuffer,"STAX    D"); break;
            case 0x0f: outBuffer += sprintf(outBuffer,"RRC      "); break;
            case 0x1f: outBuffer += sprintf(outBuffer,"RAR      "); break;
            case 0x2f: outBuffer += sprintf(outBuffer,"CMA      "); break;
            case 0x0b: outBuffer += sprintf(outBuffer,"DCX     B" ); break;
            case 0x1b: outBuffer += sprintf(outBuffer,"DCX     D" ); break;
            case 0x2b: outBuffer += sprintf(outBuffer,"DCX     H" ); break;
            case 0x3b: outBuffer += sprintf(outBuffer,"DCX     SP"); break;
            case 0x09: outBuffer += sprintf(outBuffer,"DAD     B"); break;
            case 0x19: outBuffer += sprintf(outBuffer,"DAD     D"); break;
            case 0x29: outBuffer += sprintf(outBuffer,"DAD     H"); break;
            case 0x39: outBuffer += sprintf(outBuffer,"DAD     SP"); break;
            case 0x04: outBuffer += sprintf(outBuffer,"INR     B"); break;
            case 0x0c: outBuffer += sprintf(outBuffer,"INR     C"); break;
            case 0x14: outBuffer += sprintf(outBuffer,"INR     D"); break;
            case 0x1c: outBuffer += sprintf(outBuffer,"INR     E"); break;
            case 0x24: outBuffer += sprintf(outBuffer,"INR     H"); break;
            case 0x2c: outBuffer += sprintf(outBuffer,"INR     L"); break;
            case 0x34: outBuffer += sprintf(outBuffer,"INR     M"); break;
            case 0x3c: outBuffer += sprintf(outBuffer,"INR     A"); break;
            case 0x05: outBuffer += sprintf(outBuffer,"DCR     B"); break;
            case 0x0d: outBuffer += sprintf(outBuffer,"DCR     C"); break;
            case 0x15: outBuffer += sprintf(outBuffer,"DCR     D"); break;
            case 0x1d: outBuffer += sprintf(outBuffer,"DCR     E"); break;
            case 0x25: outBuffer += sprintf(outBuffer,"DCR     H"); break;
            case 0x2d: outBuffer += sprintf(outBuffer,"DCR     L"); break;
            case 0x35: outBuffer += sprintf(outBuffer,"DCR     M"); break;
            case 0x3d: outBuffer += sprintf(outBuffer,"DCR     A"); break;
            case 0x03: outBuffer += sprintf(outBuffer,"INX     B" ); break;
            case 0x13: outBuffer += sprintf(outBuffer,"INX     D" ); break;
            case 0x23: outBuffer += sprintf(outBuffer,"INX     H" ); break;
            case 0x33: outBuffer += sprintf(outBuffer,"INX     SP"); break;
            case 0x37: outBuffer += sprintf(outBuffer,"STC      "); break;
            case 0x3f: outBuffer += sprintf(outBuffer,"CMC      "); break;
            CASE_INSTRUCTION_GROUP_8_2(0x40, "MOV", 'B', outBuffer);
            CASE_INSTRUCTION_GROUP_8_2(0x48, "MOV", 'C', outBuffer);
            CASE_INSTRUCTION_GROUP_8_2(0x50, "MOV", 'D', outBuffer);
            CASE_INSTRUCTION_GROUP_8_2(0x58, "MOV", 'E', outBuffer);
            CASE_INSTRUCTION_GROUP_8_2(0x60, "MOV", 'H', outBuffer);
            CASE_INSTRUCTION_GROUP_8_2(0x68, "MOV", 'L', outBuffer);

            case 0x70: outBuffer += sprintf(outBuffer,"MOV     M,B"); break;
            case 0x71: outBuffer += sprintf(outBuffer,"MOV     M,C"); break;
            case 0x72: outBuffer += sprintf(outBuffer,"MOV     M,D"); break;
            case 0x73: outBuffer += sprintf(outBuffer,"MOV     M,E"); break;
            case 0x74: outBuffer += sprintf(outBuffer,"MOV     M,H"); break;
            case 0x75: outBuffer += sprintf(outBuffer,"MOV     M,L"); break;
            case 0x76: outBuffer += sprintf(outBuffer,"HLT        "); break;
            case 0x77: outBuffer += sprintf(outBuffer,"MOV     M,A"); break;

            CASE_INSTRUCTION_GROUP_8_2(0x78, "MOV", 'A', outBuffer);
            
            CASE_INSTRUCTION_GROUP_8(0x80, "ADD", outBuffer);
            CASE_INSTRUCTION_GROUP_8(0x88, "ADC", outBuffer);
            CASE_INSTRUCTION_GROUP_8(0x90, "SUB", outBuffer);
            CASE_INSTRUCTION_GROUP_8(0x98, "SBB", outBuffer);
            CASE_INSTRUCTION_GROUP_8(0xa0, "ANA", outBuffer);
            CASE_INSTRUCTION_GROUP_8(0xa8, "XRA", outBuffer);
            CASE_INSTRUCTION_GROUP_8(0xb0, "ORA", outBuffer);
            CASE_INSTRUCTION_GROUP_8(0xb8, "CMP", outBuffer);

            case 0xc0: outBuffer += sprintf(outBuffer,"RNZ      "); break;
            case 0xc1: outBuffer += sprintf(outBuffer,"POP     B"); break;
            case 0xd1: outBuffer += sprintf(outBuffer,"POP     D"); break;
            case 0xe1: outBuffer += sprintf(outBuffer,"POP     H"); break;
            case 0xf1: outBuffer += sprintf(outBuffer,"POP     PSW"); break;
            case 0xc5: outBuffer += sprintf(outBuffer,"PUSH    B"); break;
            case 0xd5: outBuffer += sprintf(outBuffer,"PUSH    D"); break;
            case 0xe5: outBuffer += sprintf(outBuffer,"PUSH    H"); break;
            case 0xf5: outBuffer += sprintf(outBuffer,"PUSH    PSW"); break;

            case 0x06: outBuffer += sprintf(outBuffer,"MVI     B,#$%02x", code[1]); opbytes = 2; break;
            case 0x0e: outBuffer += sprintf(outBuffer,"MVI     C,#$%02x", code[1]); opbytes = 2; break;
            case 0x16: outBuffer += sprintf(outBuffer,"MVI     D,#$%02x", code[1]); opbytes = 2; break;
            case 0x1e: outBuffer += sprintf(outBuffer,"MVI     E,#$%02x", code[1]); opbytes = 2; break;
            case 0x26: outBuffer += sprintf(outBuffer,"MVI     H,#$%02x", code[1]); opbytes = 2; break;
            case 0x2e: outBuffer += sprintf(outBuffer,"MVI     L,#$%02x", code[1]); opbytes = 2; break;
            case 0x36: outBuffer += sprintf(outBuffer,"MVI     M,#$%02x", code[1]); opbytes = 2; break;
            case 0x3e: outBuffer += sprintf(outBuffer,"MVI     A,#$%02x", code[1]); opbytes = 2; break;

            case 0x01: outBuffer += sprintf(outBuffer,"LXI     B,#$%02x%02x",  code[2], code[1]); opbytes = 3; break;
            case 0x11: outBuffer += sprintf(outBuffer,"LXI     D,#$%02x%02x",  code[2], code[1]); opbytes = 3; break;
            case 0x21: outBuffer += sprintf(outBuffer,"LXI     H,#$%02x%02x",  code[2], code[1]); opbytes = 3; break;
            case 0x31: outBuffer += sprintf(outBuffer,"LXI     SP,#$%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0x22: outBuffer += sprintf(outBuffer,"SHLD    $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0x2a: outBuffer += sprintf(outBuffer,"LHLD    $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0x32: outBuffer += sprintf(outBuffer,"STA     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0x3a: outBuffer += sprintf(outBuffer,"LDA     #$%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xc2: outBuffer += sprintf(outBuffer,"JNZ     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xc3: outBuffer += sprintf(outBuffer,"JMP     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xc4: outBuffer += sprintf(outBuffer,"CNZ     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xca: outBuffer += sprintf(outBuffer,"JZ      $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xcc: outBuffer += sprintf(outBuffer,"CZ      $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xcd: outBuffer += sprintf(outBuffer,"CALL    $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xd2: outBuffer += sprintf(outBuffer,"JNC     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xd4: outBuffer += sprintf(outBuffer,"CNC     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xda: outBuffer += sprintf(outBuffer,"JC      $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xdc: outBuffer += sprintf(outBuffer,"CC      $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xe2: outBuffer += sprintf(outBuffer,"JPO     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xe4: outBuffer += sprintf(outBuffer,"CPO     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xea: outBuffer += sprintf(outBuffer,"JPE     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xec: outBuffer += sprintf(outBuffer,"CPE     $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xf2: outBuffer += sprintf(outBuffer,"JP      $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xf4: outBuffer += sprintf(outBuffer,"CP      $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xfa: outBuffer += sprintf(outBuffer,"JM      $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xfc: outBuffer += sprintf(outBuffer,"CM      $%02x%02x", code[2], code[1]); opbytes = 3; break;
            case 0xc6: outBuffer += sprintf(outBuffer,"ADI     $%02x", code[1]); opbytes = 2; break;
            case 0xce: outBuffer += sprintf(outBuffer,"ACI     $%02x", code[1]); opbytes = 2; break;
            case 0xd3: outBuffer += sprintf(outBuffer,"OUT     $%02x", code[1]); opbytes = 2; break;
            case 0xd6: outBuffer += sprintf(outBuffer,"SUI     $%02x", code[1]); opbytes = 2; break;
            case 0xdb: outBuffer += sprintf(outBuffer,"IN      $%02x", code[1]); opbytes = 2; break;
            case 0xde: outBuffer += sprintf(outBuffer,"SBI     $%02x", code[1]); opbytes = 2; break;
            case 0xe6: outBuffer += sprintf(outBuffer,"ANI     $%02x", code[1]); opbytes = 2; break;
            case 0xee: outBuffer += sprintf(outBuffer,"XRI     $%02x", code[1]); opbytes = 2; break;
            case 0xf6: outBuffer += sprintf(outBuffer,"ORI     $%02x", code[1]); opbytes = 2; break;
            case 0xfe: outBuffer += sprintf(outBuffer,"CPI     $%02x", code[1]); opbytes = 2; break;
            case 0xc7: outBuffer += sprintf(outBuffer,"RST     0"); break;
            case 0xcf: outBuffer += sprintf(outBuffer,"RST     1"); break;
            case 0xd7: outBuffer += sprintf(outBuffer,"RST     2"); break;
            case 0xdf: outBuffer += sprintf(outBuffer,"RST     3"); break;
            case 0xe7: outBuffer += sprintf(outBuffer,"RST     4"); break;
            case 0xef: outBuffer += sprintf(outBuffer,"RST     5"); break;
            case 0xf7: outBuffer += sprintf(outBuffer,"RST     6"); break;
            case 0xff: outBuffer += sprintf(outBuffer,"RST     7"); break;
            case 0xc8: outBuffer += sprintf(outBuffer,"RZ       "); break;
            case 0xc9: outBuffer += sprintf(outBuffer,"RET      "); break;
            case 0xd0: outBuffer += sprintf(outBuffer,"RNC      "); break;
            case 0xd8: outBuffer += sprintf(outBuffer,"RC       "); break;
            case 0xe0: outBuffer += sprintf(outBuffer,"RPO      "); break;
            case 0xe3: outBuffer += sprintf(outBuffer,"XHTL     "); break;
            case 0xe8: outBuffer += sprintf(outBuffer,"RPE      "); break;
            case 0xe9: outBuffer += sprintf(outBuffer,"PCHL     "); break;
            case 0xeb: outBuffer += sprintf(outBuffer,"XCHG     "); break;
            case 0xf0: outBuffer += sprintf(outBuffer,"RP       "); break;
            case 0xf3: outBuffer += sprintf(outBuffer,"DI       "); break;
            case 0xfb: outBuffer += sprintf(outBuffer,"EI       "); break;
            case 0xf8: outBuffer += sprintf(outBuffer,"RM       "); break;
            case 0xf9: outBuffer += sprintf(outBuffer,"SPHL     "); break;

            default: {
                fprintf(stderr, "\nUncaught hexcode: %x\n", *code);
                assert(0);
            } break;
        }

        assert(outBuffer >= 0);

        outBuffer += sprintf(outBuffer, "\n");
        i += opbytes;
    }
    outBuffer[1] = '\0';
    // clang-format on
}