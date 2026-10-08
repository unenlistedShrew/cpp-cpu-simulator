#ifndef COMPILER_H
#define COMPILER_H

#include <string>
#include <string_view>
#include <optional>
#include "instruction.h"
#include "executable.h"

class Tokenizer{
    private:
        std::string_view buffer;
        std::string_view delimeters;
        void skip_delims();
    public:
        explicit Tokenizer(std::string_view b, std::string_view d = " ,\n");
        std::string_view next_token();
        bool has_more() const;
        bool contains(char c) const;
};

Executable build_executable(const std::string& filename, bool testing = false);

#endif