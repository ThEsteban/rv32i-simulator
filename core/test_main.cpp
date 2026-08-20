#include "cpu.hpp"
#include <iostream>
#include <cassert> 
#include<vector>
//ai written unit test

struct TestCase {
    const char* name;
    uint32_t raw_instruction;
    InstructionType expected_type;
    uint8_t expected_rd;
    uint8_t expected_rs1;
    uint8_t expected_rs2;
    int32_t expected_imm;
    uint8_t expected_funct3;
};

int main() {
    CPU cpu;

    std::vector<TestCase> tests = {
        {
            "ADDI (Positive Imm)",
            0x00500093, // addi x1, x0, 5
            InstructionType::I, 1, 0, 0, 5, 0
        },
        {
            "ADDI (Negative Sign Extension)",
            0xFFF00093, // addi x1, x0, -1
            InstructionType::I, 1, 0, 0, -1, 0
        },
        {
            "SW (Store Word)",
            0x0020A423, // sw x2, 8(x1)
            InstructionType::S, 0, 1, 2, 8, 0x2
        },
        {
            "BEQ (Branch Backward Target)",
            0xFE000EE3, // beq x0, x0, -4
            InstructionType::B, 0, 0, 0, -4, 0
        },
        {
            "LUI (Upper Immediate)",
            0x123450B7, // lui x1, 0x12345
            InstructionType::U, 1, 0, 0, 0x12345000, 0
        },
        {
            "JAL (Jump and Link)",
            0xFFDFF06F, // jal x0, -4
            InstructionType::J, 0, 0, 0, -4, 0
        }
    };

    int failed = 0;
    for (const auto& t : tests) {
        DecodedInstruction dec = cpu.decode(t.raw_instruction); // Calls cpu.decode[cite: 3, 4]

        bool match = (dec.type   == t.expected_type &&
                      dec.rd     == t.expected_rd &&
                      dec.rs1    == t.expected_rs1 &&
                      dec.rs2    == t.expected_rs2 &&
                      dec.imm    == t.expected_imm &&
                      dec.funct3 == t.expected_funct3);

        if (match) {
            std::cout << "[PASS] " << t.name << "\n";
        } else {
            std::cerr << "[FAIL] " << t.name << "\n"
                      << "  Expected -> Type: " << static_cast<int>(t.expected_type) 
                      << ", rd: " << (int)t.expected_rd 
                      << ", imm: " << t.expected_imm << "\n"
                      << "  Actual   -> Type: " << static_cast<int>(dec.type) 
                      << ", rd: " << (int)dec.rd 
                      << ", imm: " << dec.imm << "\n\n";
            failed++;
        }
    }

    if (failed == 0) {
        std::cout << "\nAll Decoder tests passed successfully!.\n";
    } else {
        std::cerr << "\n" << failed << " test(s) failed.\n";
    }

    return failed;
}