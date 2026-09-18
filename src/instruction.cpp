#include "../includes/instruction.h"
#include <iostream>
#include <cstdint>

namespace{
    //binary encodings for the Opcodes in the supported ISA
    namespace Opcode{
        constexpr std::uint8_t R_TYPE = 0b0110011;
        constexpr std::uint8_t I_TYPE = 0b0010011;
        constexpr std::uint8_t LOAD = 0b0000011;
        constexpr std::uint8_t JALR = 0b1100111;
        constexpr std::uint8_t STORE = 0b0100011;
        constexpr std::uint8_t BRANCH = 0b1100011;
        constexpr std::uint8_t JAL = 0b1101111;
        constexpr std::uint8_t LUI = 0b0110111;
    }

    //binary encodings for the function codes that differentiate the operations
    namespace funct3{
        constexpr std::uint8_t ADD_SUB = 0b000;
        constexpr std::uint8_t JALR = 0b000;
        constexpr std::uint8_t BEQ = 0b000;
        constexpr std::uint8_t SLL = 0b001;
        constexpr std::uint8_t SLT = 0b010;
        constexpr std::uint8_t XOR = 0b100;
        constexpr std::uint8_t SRL_SRA = 0b101;
        constexpr std::uint8_t OR = 0b110;
        constexpr std::uint8_t AND = 0b111;
        constexpr std::uint8_t BNE = 0b001;
        constexpr std::uint8_t BLT = 0b100;
        constexpr std::uint8_t BGE = 0b101;
        constexpr std::uint8_t LW = 0b010;
    }

    //binary encoding for the additional function code needed to differentiate operations that
    //have the same funct3 code
    namespace funct7{
        constexpr std::uint8_t NORMAL = 0b0000000;
        constexpr std::uint8_t SUB_SRA = 0b0100000;
    }

    std::uint32_t encode_r_type(std::uint8_t funct7, std::uint8_t rs2, std::uint8_t rs1, std::uint8_t funct3, std::uint8_t rd, std::uint8_t opcode){
        std::uint32_t bits = 0;
        bits |= (static_cast<std::uint32_t>(funct7) & 0x7f) << 25;
        bits |= (static_cast<std::uint32_t>(rs2) & 0x1f) << 20;
        bits |= (static_cast<std::uint32_t>(rs1) & 0x1f) << 15;
        bits |= (static_cast<std::uint32_t>(funct3) & 0x07) << 12;
        bits |= (static_cast<std::uint32_t>(rd) & 0x1f) << 7;
        bits |= (static_cast<std::uint32_t>(opcode) & 0x7f);
        return bits;
    }

    std::uint32_t encode_i_type(std::int16_t imm, std::uint8_t rs, std::uint8_t funct3, std::uint8_t rd, std::uint8_t opcode){
        std::uint32_t bits = 0;
        bits |= (static_cast<std::int32_t>(imm) & 0xfff) << 20;
        bits |= (static_cast<std::uint32_t>(rs) & 0x1f) << 15;
        bits |= (static_cast<std::uint32_t>(funct3) & 0x07) << 12;
        bits |= (static_cast<std::uint32_t>(rd) & 0x1f) << 7;
        bits |= (static_cast<std::uint32_t>(opcode) & 0x7f);
        return bits;
    }

    std::uint32_t encode_b_type(std::int16_t imm, std::uint8_t rs2, std::uint8_t rs1, std::uint8_t funct3, std::uint8_t opcode){
        std::uint32_t bits = 0;
        //this instruction assumes that imm[0] = 0
        bits |= ((static_cast<std::uint32_t>(imm) >> 12) & 0x1) << 31; //preserves imm[12] which is the sign bit
        bits |= ((static_cast<std::uint32_t>(imm) >> 5) & 0x3f) << 25; //preserves imm[10:5]
        bits |= (static_cast<std::uint32_t>(rs2) & 0x1f) << 20;
        bits |= (static_cast<std::uint32_t>(rs1) & 0x1f) << 15;
        bits |= (static_cast<std::uint32_t>(funct3) & 0x07) << 12;
        bits |= ((static_cast<std::uint32_t>(imm) >> 1) & 0x0f) << 8; //preserves imm[4:1]
        bits |= ((static_cast<std::uint32_t>(imm) >> 11) & 0x1) << 7; //preserves imm[11]
        bits |= (static_cast<std::uint32_t>(opcode) & 0x7f);
        return bits;
    }

    std::uint32_t encode_j_type(std::int32_t imm, std::uint8_t rd, std::uint8_t opcode){
        std::uint32_t bits = 0;
        //this instruction assumes that imm[0] = 0
        bits |= ((static_cast<std::uint32_t>(imm) >> 20) & 0x1) << 31; //preserves imm[20]
        bits |= ((static_cast<std::uint32_t>(imm) >> 1) & 0x3ff) << 21; //preserves imm[10:1]
        bits |= ((static_cast<std::uint32_t>(imm) >> 11) & 0x1) << 20; //preserves imm[11]
        bits |= ((static_cast<std::uint32_t>(imm) >> 12) & 0xff) << 12; //preserves imm[19:12]
        bits |= (static_cast<std::uint32_t>(rd) & 0x1f) << 7;
        bits |= (static_cast<std::uint32_t>(opcode) & 0x7f);
        return bits;
    }

    std::uint32_t encode_u_type(std::int32_t imm, std::uint8_t rd, std::uint8_t opcode){
        std::uint32_t bits = 0;
        bits |= (static_cast<std::uint32_t>(imm) & 0xFFFFF) << 12;
        bits |= (static_cast<std::uint32_t>(rd) & 0x1f) << 7;
        bits |= (static_cast<std::uint32_t>(opcode) & 0x7f);
        return bits;
    }
}


Instruction::Instruction(InstructionType i, Register rd, Register rs1, Register rs2){
    switch(i){
        case InstructionType::ADD:
            bits = encode_r_type(funct7::NORMAL, rs2, rs1, funct3::ADD_SUB, rd, Opcode::R_TYPE);
            break;
        case InstructionType::AND:
            bits = encode_r_type(funct7::NORMAL, rs2, rs1, funct3::AND, rd, Opcode::R_TYPE);
            break;
        case InstructionType::SUB:
            bits = encode_r_type(funct7::SUB_SRA, rs2, rs1, funct3::ADD_SUB, rd, Opcode::R_TYPE);
            break;
        case InstructionType::OR:
            bits = encode_r_type(funct7::NORMAL, rs2, rs1, funct3::OR, rd, Opcode::R_TYPE);
            break;
        case InstructionType::XOR:
            bits = encode_r_type(funct7::NORMAL, rs2, rs1, funct3::XOR, rd, Opcode::R_TYPE);
            break;
        case InstructionType::SLL:
            bits = encode_r_type(funct7::NORMAL, rs2, rs1, funct3::SLL, rd, Opcode::R_TYPE);
            break;
        case InstructionType::SRL:
            bits = encode_r_type(funct7::NORMAL, rs2, rs1, funct3::SRL_SRA, rd, Opcode::R_TYPE);
            break;
        case InstructionType::SRA:
            bits = encode_r_type(funct7::SUB_SRA, rs2, rs1, funct3::SRL_SRA, rd, Opcode::R_TYPE);
            break;
        case InstructionType::SLT:
            bits = encode_r_type(funct7::NORMAL, rs2, rs1, funct3::SLT, rd, Opcode::R_TYPE);
            break;
        default:
            std::cout<<"ERR::UNKNOWN_INSTRUCTION"<<std::endl;
    }
}

Instruction::Instruction(InstructionType i, Register rd, Register rs, std::int16_t imm){
    switch(i){
        case InstructionType::ADDI:
            bits = encode_i_type(imm, rs, funct3::ADD_SUB, rd, Opcode::I_TYPE);
            break;
        case InstructionType::ANDI:
            bits = encode_i_type(imm, rs, funct3::AND, rd, Opcode::I_TYPE);
            break;
        case InstructionType::ORI:
            bits = encode_i_type(imm, rs, funct3::OR, rd, Opcode::I_TYPE);
            break;
        case InstructionType::SLTI:
            bits = encode_i_type(imm, rs, funct3::SLT, rd, Opcode::I_TYPE);
            break;
        case InstructionType::LW:
            bits = encode_i_type(imm, rs, funct3::LW, rd, Opcode::LOAD);
            break;
        case InstructionType::JALR:
            bits = encode_i_type(imm, rs, funct3::JALR, rd, Opcode::JALR);
            break;
        case InstructionType::BEQ:
            bits = encode_b_type(imm, rd, rs, funct3::BEQ, Opcode::BRANCH);
            break;
        case InstructionType::BGE:
            bits = encode_b_type(imm, rd, rs, funct3::BGE, Opcode::BRANCH);
            break;
        case InstructionType::BLT:
            bits = encode_b_type(imm, rd, rs, funct3::BLT, Opcode::BRANCH);
            break;
        case InstructionType::BNE:
            bits = encode_b_type(imm, rd, rs, funct3::BNE, Opcode::BRANCH);
            break;
        default:
            std::cout<<"ERR::UNKNOWN_INSTRUCTION"<<std::endl;
    }
}

Instruction::Instruction(InstructionType i, Register rd, std::int32_t imm){
    switch(i){
        case InstructionType::JAL:
            bits = encode_j_type(imm, rd, Opcode::JAL);
            break;
        case InstructionType::LUI:
            bits = encode_u_type(imm, rd, Opcode::LUI);
            break;
        default:
            std::cout<<"ERR::UNKNOWN_INSTRUCTION"<<std::endl;
    }
}

std::uint32_t Instruction::get_bits() const{
    return bits;
}
