#include "types.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#define CASE_INSTRUCTION_GROUP_8(startHex, instruction, _outBuffer) \
    case (startHex + 0): printf("%s      B", instruction); break;   \
    case (startHex + 1): printf("%s      C", instruction); break;   \
    case (startHex + 2): printf("%s      D", instruction); break;   \
    case (startHex + 3): printf("%s      E", instruction); break;   \
    case (startHex + 4): printf("%s      H", instruction); break;   \
    case (startHex + 5): printf("%s      L", instruction); break;   \
    case (startHex + 6): printf("%s      M", instruction); break;   \
    case (startHex + 7): printf("%s      A", instruction); break;

// derp

#define CASE_INSTRUCTION_GROUP_8_2(startHex, instruction, reg, _outBuffer) \
    case (startHex + 0): printf("%s      %c,B", instruction, reg); break;  \
    case (startHex + 1): printf("%s      %c,C", instruction, reg); break;  \
    case (startHex + 2): printf("%s      %c,D", instruction, reg); break;  \
    case (startHex + 3): printf("%s      %c,E", instruction, reg); break;  \
    case (startHex + 4): printf("%s      %c,H", instruction, reg); break;  \
    case (startHex + 5): printf("%s      %c,L", instruction, reg); break;  \
    case (startHex + 6): printf("%s      %c,M", instruction, reg); break;  \
    case (startHex + 7): printf("%s      %c,A", instruction, reg); break;

internal u8
print_8080_hex_instruction(u8 *code)
{
    // clang-format off
    int opbytes = 1;
    switch (*code) {
        case 0x00: printf("NOP: %02x", *code); break; // NOP
        case 0x08: printf("NOP: %02x", *code); break; // NOP
        case 0x10: printf("NOP: %02x", *code); break; // NOP
        case 0x18: printf("NOP: %02x", *code); break; // NOP
        case 0x20: printf("NOP: %02x", *code); break; // NOP
        case 0x28: printf("NOP: %02x", *code); break; // NOP
        case 0x30: printf("NOP: %02x", *code); break; // NOP
        case 0x38: printf("NOP: %02x", *code); break; // NOP
        case 0xcb: printf("NOP: %02x", *code); break; // NOP
        case 0xd9: printf("NOP: %02x", *code); break; // NOP
        case 0xdd: printf("NOP: %02x", *code); break; // NOP
        case 0xed: printf("NOP: %02x", *code); break; // NOP
        case 0xfd: printf("NOP: %02x", *code); break; // NOP
        case 0x07: printf("RLC      "); break;
        case 0x17: printf("RAL      "); break;
        case 0x27: printf("DAA      "); break;
        case 0x0a: printf("LDAX    B"); break;
        case 0x1a: printf("LDAX    D"); break;
        case 0x02: printf("STAX    B"); break;
        case 0x12: printf("STAX    D"); break;
        case 0x0f: printf("RRC      "); break;
        case 0x1f: printf("RAR      "); break;
        case 0x2f: printf("CMA      "); break;
        case 0x0b: printf("DCX     B" ); break;
        case 0x1b: printf("DCX     D" ); break;
        case 0x2b: printf("DCX     H" ); break;
        case 0x3b: printf("DCX     SP"); break;
        case 0x09: printf("DAD     B"); break;
        case 0x19: printf("DAD     D"); break;
        case 0x29: printf("DAD     H"); break;
        case 0x39: printf("DAD     SP"); break;
        case 0x04: printf("INR     B"); break;
        case 0x0c: printf("INR     C"); break;
        case 0x14: printf("INR     D"); break;
        case 0x1c: printf("INR     E"); break;
        case 0x24: printf("INR     H"); break;
        case 0x2c: printf("INR     L"); break;
        case 0x34: printf("INR     M"); break;
        case 0x3c: printf("INR     A"); break;
        case 0x05: printf("DCR     B"); break;
        case 0x0d: printf("DCR     C"); break;
        case 0x15: printf("DCR     D"); break;
        case 0x1d: printf("DCR     E"); break;
        case 0x25: printf("DCR     H"); break;
        case 0x2d: printf("DCR     L"); break;
        case 0x35: printf("DCR     M"); break;
        case 0x3d: printf("DCR     A"); break;
        case 0x03: printf("INX     B" ); break;
        case 0x13: printf("INX     D" ); break;
        case 0x23: printf("INX     H" ); break;
        case 0x33: printf("INX     SP"); break;
        case 0x37: printf("STC      "); break;
        case 0x3f: printf("CMC      "); break;
        CASE_INSTRUCTION_GROUP_8_2(0x40, "MOV", 'B', outBuffer);
        CASE_INSTRUCTION_GROUP_8_2(0x48, "MOV", 'C', outBuffer);
        CASE_INSTRUCTION_GROUP_8_2(0x50, "MOV", 'D', outBuffer);
        CASE_INSTRUCTION_GROUP_8_2(0x58, "MOV", 'E', outBuffer);
        CASE_INSTRUCTION_GROUP_8_2(0x60, "MOV", 'H', outBuffer);
        CASE_INSTRUCTION_GROUP_8_2(0x68, "MOV", 'L', outBuffer);

        case 0x70: printf("MOV     M,B"); break;
        case 0x71: printf("MOV     M,C"); break;
        case 0x72: printf("MOV     M,D"); break;
        case 0x73: printf("MOV     M,E"); break;
        case 0x74: printf("MOV     M,H"); break;
        case 0x75: printf("MOV     M,L"); break;
        case 0x76: printf("HLT        "); break;
        case 0x77: printf("MOV     M,A"); break;

        CASE_INSTRUCTION_GROUP_8_2(0x78, "MOV", 'A', outBuffer);
        
        CASE_INSTRUCTION_GROUP_8(0x80, "ADD", outBuffer);
        CASE_INSTRUCTION_GROUP_8(0x88, "ADC", outBuffer);
        CASE_INSTRUCTION_GROUP_8(0x90, "SUB", outBuffer);
        CASE_INSTRUCTION_GROUP_8(0x98, "SBB", outBuffer);
        CASE_INSTRUCTION_GROUP_8(0xa0, "ANA", outBuffer);
        CASE_INSTRUCTION_GROUP_8(0xa8, "XRA", outBuffer);
        CASE_INSTRUCTION_GROUP_8(0xb0, "ORA", outBuffer);
        CASE_INSTRUCTION_GROUP_8(0xb8, "CMP", outBuffer);

        case 0xc0: printf("RNZ      "); break;
        case 0xc1: printf("POP     B"); break;
        case 0xd1: printf("POP     D"); break;
        case 0xe1: printf("POP     H"); break;
        case 0xf1: printf("POP     PSW"); break;
        case 0xc5: printf("PUSH    B"); break;
        case 0xd5: printf("PUSH    D"); break;
        case 0xe5: printf("PUSH    H"); break;
        case 0xf5: printf("PUSH    PSW"); break;

        case 0x06: printf("MVI     B,#$%02x", code[1]); opbytes = 2; break;
        case 0x0e: printf("MVI     C,#$%02x", code[1]); opbytes = 2; break;
        case 0x16: printf("MVI     D,#$%02x", code[1]); opbytes = 2; break;
        case 0x1e: printf("MVI     E,#$%02x", code[1]); opbytes = 2; break;
        case 0x26: printf("MVI     H,#$%02x", code[1]); opbytes = 2; break;
        case 0x2e: printf("MVI     L,#$%02x", code[1]); opbytes = 2; break;
        case 0x36: printf("MVI     M,#$%02x", code[1]); opbytes = 2; break;
        case 0x3e: printf("MVI     A,#$%02x", code[1]); opbytes = 2; break;

        case 0x01: printf("LXI     B,#$%02x%02x",  code[2], code[1]); opbytes = 3; break;
        case 0x11: printf("LXI     D,#$%02x%02x",  code[2], code[1]); opbytes = 3; break;
        case 0x21: printf("LXI     H,#$%02x%02x",  code[2], code[1]); opbytes = 3; break;
        case 0x31: printf("LXI     SP,#$%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x22: printf("SHLD    $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x2a: printf("LHLD    $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x32: printf("STA     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0x3a: printf("LDA     #$%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xc2: printf("JNZ     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xc3: printf("JMP     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xc4: printf("CNZ     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xca: printf("JZ      $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xcc: printf("CZ      $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xcd: printf("CALL    $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xd2: printf("JNC     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xd4: printf("CNC     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xda: printf("JC      $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xdc: printf("CC      $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xe2: printf("JPO     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xe4: printf("CPO     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xea: printf("JPE     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xec: printf("CPE     $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xf2: printf("JP      $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xf4: printf("CP      $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xfa: printf("JM      $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xfc: printf("CM      $%02x%02x", code[2], code[1]); opbytes = 3; break;
        case 0xc6: printf("ADI     $%02x", code[1]); opbytes = 2; break;
        case 0xce: printf("ACI     $%02x", code[1]); opbytes = 2; break;
        case 0xd3: printf("OUT     $%02x", code[1]); opbytes = 2; break;
        case 0xd6: printf("SUI     $%02x", code[1]); opbytes = 2; break;
        case 0xdb: printf("IN      $%02x", code[1]); opbytes = 2; break;
        case 0xde: printf("SBI     $%02x", code[1]); opbytes = 2; break;
        case 0xe6: printf("ANI     $%02x", code[1]); opbytes = 2; break;
        case 0xee: printf("XRI     $%02x", code[1]); opbytes = 2; break;
        case 0xf6: printf("ORI     $%02x", code[1]); opbytes = 2; break;
        case 0xfe: printf("CPI     $%02x", code[1]); opbytes = 2; break;
        case 0xc7: printf("RST     0"); break;
        case 0xcf: printf("RST     1"); break;
        case 0xd7: printf("RST     2"); break;
        case 0xdf: printf("RST     3"); break;
        case 0xe7: printf("RST     4"); break;
        case 0xef: printf("RST     5"); break;
        case 0xf7: printf("RST     6"); break;
        case 0xff: printf("RST     7"); break;
        case 0xc8: printf("RZ       "); break;
        case 0xc9: printf("RET      "); break;
        case 0xd0: printf("RNC      "); break;
        case 0xd8: printf("RC       "); break;
        case 0xe0: printf("RPO      "); break;
        case 0xe3: printf("XHTL     "); break;
        case 0xe8: printf("RPE      "); break;
        case 0xe9: printf("PCHL     "); break;
        case 0xeb: printf("XCHG     "); break;
        case 0xf0: printf("RP       "); break;
        case 0xf3: printf("DI       "); break;
        case 0xfb: printf("EI       "); break;
        case 0xf8: printf("RM       "); break;
        case 0xf9: printf("SPHL     "); break;

        default: {
            fprintf(stderr, "\nUncaught hexcode: %x\n", *code);
            assert(0);
        } break;
    }
    //printf("\n");
    //  clang-format on
    return opbytes;
}