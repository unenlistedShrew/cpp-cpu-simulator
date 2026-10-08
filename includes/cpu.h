#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <cmath>
#include <vector>
#include "instruction.h"
#include "executable.h"

const std::size_t MEMORY_SIZE = 1024ULL * 1024ULL * 1024ULL;

using Byte = std::uint8_t;

class CPU{
    private:
        Byte* ram = new Byte[MEMORY_SIZE]{0};
        std::uint32_t gprs[32]; //x0 to x31
        std::uint32_t pc = 0;

        struct DecodedInstruction{
            InstructionType type;
            std::uint8_t rd;
            std::uint8_t rs1;
            std::uint8_t rs2;
            std::int32_t imm;
        };

    public:
        CPU();
        ~CPU();
        void load_program(const Executable& e);
        //general cpu pipeline functions
        std::uint32_t instruction_fetch();
        DecodedInstruction instruction_decode(std::uint32_t bits);
        void execute(const DecodedInstruction& instruction);
        void memory_access();
        void write_back();
};

#endif