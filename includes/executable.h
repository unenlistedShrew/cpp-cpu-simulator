#ifndef EXECUTABLE_H
#define EXECUTABLE_H

#include <vector>
#include <cstddef>
#include "instruction.h"

struct Executable{
    std::vector<std::uint32_t> instructions = {};
    std::uint32_t location;

    Executable(){}
};

#endif