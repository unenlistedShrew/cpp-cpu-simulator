#include "../includes/compiler.h"
#include "../includes/executable.h"
#include <string>
#include <string_view>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

namespace{
    std::ifstream get_file_stream(const std::string& filename){
        std::ifstream inputfile(filename);
        if(!inputfile.is_open()){
            return std::ifstream{};
        }
        return inputfile;
    }

    bool check_immediate(const std::string& imm){
        return std::any_of(imm.begin(), imm.end(), [](unsigned char c){
            return std::isalpha(c);
        });
    }

    std::optional<Instruction> register_register(InstructionType it, Tokenizer& t){ //instructions of the type <opcode> xd, xs1, xs2
        std::string rd(t.next_token());
        std::string rs1(t.next_token());
        std::string rs2(t.next_token());
        if(t.has_more()){
            return std::nullopt;
        }
        if(rd[0] != 'x' || rs1[0] != 'x' || rs2[0] != 'x'){
            return std::nullopt;
        }
        std::uint32_t d = static_cast<std::uint32_t>(std::stoi(rd.substr(1)));
        std::uint32_t s1 = static_cast<std::uint32_t>(std::stoi(rs1.substr(1)));
        std::uint32_t s2 = static_cast<std::uint32_t>(std::stoi(rs2.substr(1)));
        if(d > 31 || s1 > 31 || s2 > 31){
            return std::nullopt;
        }
        return Instruction(it, static_cast<std::uint8_t>(d), static_cast<std::uint8_t>(s1), static_cast<std::uint8_t>(s2));
    }

    std::optional<Instruction> register_immediate(InstructionType it, Tokenizer& t){ //instruction of the type <opcode> xd, xs1, imm
        std::string rd(t.next_token());
        std::string rs1(t.next_token());
        std::string imm(t.next_token());
        if(t.has_more()){
            return std::nullopt;
        }
        if(rd[0] != 'x' || rs1[0] != 'x' || check_immediate(imm)){
            return std::nullopt;
        }
        std::uint32_t d = static_cast<std::uint32_t>(std::stoi(rd.substr(1)));
        std::uint32_t s1 = static_cast<std::uint32_t>(std::stoi(rs1.substr(1)));
        if(d > 31 || s1 > 31){
            return std::nullopt;
        }
        int16_t i = static_cast<std::int16_t>(std::stoi(imm));
        return Instruction(it, static_cast<std::uint8_t>(d), static_cast<std::uint8_t>(s1), i);
    }

    std::optional<Instruction> register_immediate_offset(InstructionType it, Tokenizer& t){ //instruction of the type <opcode> xd, n(xs2)
        std::string rds(t.next_token());
        std::string_view addr = t.next_token();
        if(t.has_more()){
            return std::nullopt;
        }
        std::size_t open_paran = addr.find('(');
        std::size_t close_paran = addr.find(')');
        if(open_paran == std::string_view::npos || close_paran == std::string_view::npos){
            return std::nullopt;
        }
        std::string imm(addr.substr(0, open_paran));
        std::string rb(addr.substr(open_paran + 1, close_paran - open_paran - 1));
        if(rds[0] != 'x' || rb[0] != 'x' || check_immediate(imm)){
            return std::nullopt;
        }
        std::uint32_t ds = static_cast<std::uint32_t>(std::stoi(rds.substr(1)));
        std::uint32_t b = static_cast<std::uint32_t>(std::stoi(rb.substr(1)));
        if(ds > 31 || b > 31){
            return std::nullopt;
        }
        std::int16_t i = static_cast<std::int16_t>(std::stoi(imm));
        return Instruction(it, static_cast<std::uint8_t>(ds), static_cast<std::uint8_t>(b), i);
    }

    std::optional<Instruction> branch(InstructionType it, Tokenizer& t){ //instruction of the branching type
        std::string rs1(t.next_token());
        std::string rs2(t.next_token());
        std::string imm(t.next_token());
        if(t.has_more()){
            return std::nullopt;
        }
        if(rs1[0] != 'x' || rs2[0] != 'x' || check_immediate(imm)){
            return std::nullopt;
        }
        std::uint32_t s1 = static_cast<std::uint32_t>(std::stoi(rs1.substr(1)));
        std::uint32_t s2 = static_cast<std::uint32_t>(std::stoi(rs2.substr(1)));
        if(s1 > 31 || s2 > 31){
            return std::nullopt;
        }
        int16_t i = static_cast<int16_t>(std::stoi(imm));
        return Instruction(it, static_cast<std::uint8_t>(s2), static_cast<std::uint8_t>(s1), i);
    }

    std::optional<Instruction> jal_lui(InstructionType it, Tokenizer& t){
        std::string rd(t.next_token());
        std::string imm(t.next_token());
        if(t.has_more()){
            return std::nullopt;
        }
        if(rd[0] != 'x' || check_immediate(imm)){
            return std::nullopt;
        }
        std::uint32_t d = static_cast<std::uint32_t>(std::stoi(rd.substr(1)));
        if(d > 31){
            return std::nullopt;
        }
        std::int32_t i = static_cast<std::int32_t>(std::stoi(imm));
        return Instruction(it, static_cast<std::uint8_t>(d), i);
    }

    std::optional<Instruction> parse_line(std::string_view line){
        Tokenizer tokenizer(line);
        auto temp = tokenizer.next_token();
        if(temp[0] == '#' || temp.empty()){
            return std::nullopt;
        }
        std::string opcode(temp);
        if(opcode == "ADD"){
            return register_register(InstructionType::ADD, tokenizer);
        }
        if(opcode == "ADDI"){
            return register_immediate(InstructionType::ADDI, tokenizer);
        }
        if(opcode == "SUB"){
            return register_register(InstructionType::SUB, tokenizer);
        }
        if(opcode == "AND"){
            return register_register(InstructionType::AND, tokenizer);
        }
        if(opcode == "ANDI"){
            return register_immediate(InstructionType::ANDI, tokenizer);
        }
        if(opcode == "OR"){
            return register_register(InstructionType::OR, tokenizer);
        }
        if(opcode == "ORI"){
            return register_immediate(InstructionType::ORI, tokenizer);
        }
        if(opcode == "XOR"){
            return register_register(InstructionType::XOR, tokenizer);
        }
        if(opcode == "SLL"){
            return register_register(InstructionType::SLL, tokenizer);
        }
        if(opcode == "SRL"){
            return register_register(InstructionType::SRL, tokenizer);
        }
        if(opcode == "SRA"){
            return register_register(InstructionType::SRA, tokenizer);
        }
        if(opcode == "SLT"){
            return register_register(InstructionType::SLT, tokenizer);
        }
        if(opcode == "SLTI"){
            return register_immediate(InstructionType::SLTI, tokenizer);
        }
        if(opcode == "LW"){
            if(tokenizer.contains('(')){
                return register_immediate_offset(InstructionType::LW, tokenizer);
            }
            return std::nullopt;
        }
        if(opcode == "SW"){
            if(tokenizer.contains('(')){
                return register_immediate_offset(InstructionType::SW, tokenizer);
            }
            return std::nullopt;
        }
        if(opcode == "BEQ"){
            return branch(InstructionType::BEQ, tokenizer);
        }
        if(opcode == "BNE"){
            return branch(InstructionType::BNE, tokenizer);
        }
        if(opcode == "BLT"){
            return branch(InstructionType::BLT, tokenizer);
        }
        if(opcode == "BGE"){
            return branch(InstructionType::BGE, tokenizer);
        }
        if(opcode == "JAL"){
            return jal_lui(InstructionType::JAL, tokenizer);
        }
        if(opcode == "JALR"){
            return register_immediate(InstructionType::JALR, tokenizer);
        }
        if(opcode == "LUI"){
            return jal_lui(InstructionType::LUI, tokenizer);
        }
        return std::nullopt;
    }
}

void Tokenizer::skip_delims(){
    size_t first_valid = buffer.find_first_not_of(delimeters);
    if(first_valid != std::string_view::npos){
        buffer.remove_prefix(first_valid);
    }else{
        buffer = {};
    }
}

Tokenizer::Tokenizer(std::string_view b, std::string_view d) :
    buffer(b), delimeters(d) {
        skip_delims();
    }

std::string_view Tokenizer::next_token(){
    if(buffer.empty()){
        return {};
    }

    size_t token_end = buffer.find_first_of(delimeters);
    if(token_end == std::string_view::npos){
        std::string_view token = buffer;
        buffer = {};
        return token;
    }

    std::string_view token = buffer.substr(0, token_end);
    buffer.remove_prefix(token_end);
    skip_delims();
    return token;
}

bool Tokenizer::has_more() const{
    return !buffer.empty();
}

bool Tokenizer::contains(char c) const{
    return (buffer.find(c) == std::string_view::npos) ? false : true;
}

Executable build_executable(const std::string& filename, bool testing){
    std::ifstream inputstream = get_file_stream(filename);
    if(!inputstream.is_open()){
        std::cerr<<"ERR::FILE_NOT_OPENED"<<std::endl;
        return Executable();
    }
    Executable exe;
    std::string line;
    while(std::getline(inputstream, line)){
        Tokenizer tokenizer(line);
        if(!tokenizer.has_more()){
            continue;
        }
        std::optional<Instruction> i = parse_line(line);
        if(i.has_value()){
            exe.instructions.push_back(i->get_bits());
        }else{
            if(!testing){std::cerr<<"ERR::INVALID_INSTRUCTION::TRACEBACK: "<<tokenizer.next_token()<<std::endl;}
        }
    }
    return exe;
}