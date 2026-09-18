#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>

typedef std::uint8_t Register;

enum class InstructionType {
    ADD,
    ADDI,
    SUB,
    AND,
    ANDI,
    OR,
    ORI,
    XOR,
    SLL,
    SRL,
    SRA,
    SLT,
    SLTI,
    LW,
    SW,
    BEQ,
    BNE,
    BLT,
    BGE,
    JAL,
    JALR,
    LUI
};

class Instruction{
    private:
        std::uint32_t bits = 0;

    public:
        Instruction(InstructionType i, Register rd, Register rs1, Register rs2);

        Instruction(InstructionType i, Register rd, Register rs, std::int16_t imm);

        Instruction(InstructionType i, Register rd, std::int32_t imm);

        std::uint32_t get_bits() const;
};

#endif