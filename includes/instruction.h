#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <cstdint>

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
        Instruction(){};

        Instruction(InstructionType i, std::uint8_t rd, std::uint8_t rs1, std::uint8_t rs2);

        Instruction(InstructionType i, std::uint8_t rd, std::uint8_t rs, std::int16_t imm);

        Instruction(InstructionType i, std::uint8_t rd, std::int32_t imm);

        std::uint32_t get_bits() const;
};

#endif