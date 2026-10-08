#include "../../includes/compiler.h"
#include <algorithm>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {
    struct TestCase {
        const char* name;
        const char* assembly_file;
        std::vector<std::uint32_t> expected;
    };

    void print_encoding(std::uint32_t encoding) {
        std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0')
                  << encoding << std::dec << std::setfill(' ');
    }

    bool run_test(const TestCase& test, const std::filesystem::path& cases_directory) {
        try {
            const auto executable = build_executable(
                (cases_directory / test.assembly_file).string(), true);
            const auto& actual = executable.instructions;
            if (actual == test.expected) {
                std::cout << "[PASS] " << test.name << '\n';
                return true;
            }

            std::cout << "[FAIL] " << test.name << ": expected "
                      << test.expected.size() << " encodings, got "
                      << actual.size() << '\n';

            const auto count = std::max(test.expected.size(), actual.size());
            for (std::size_t i = 0; i < count; ++i) {
                std::cout << "  instruction " << i + 1 << ": expected ";
                if (i < test.expected.size()) {
                    print_encoding(test.expected[i]);
                } else {
                    std::cout << "<none>";
                }
                std::cout << ", got ";
                if (i < actual.size()) {
                    print_encoding(actual[i]);
                } else {
                    std::cout << "<none>";
                }
                std::cout << '\n';
            }
            return false;
        } catch (const std::exception& error) {
            std::cout << "[FAIL] " << test.name << ": compiler threw exception: "
                      << error.what() << '\n';
            return false;
        } catch (...) {
            std::cout << "[FAIL] " << test.name
                      << ": compiler threw an unknown exception\n";
            return false;
        }
    }
}

int main(int argc, char* argv[]) {
    const std::filesystem::path cases_directory = argc > 1
        ? argv[1]
        : "tests/compiler_tests/cases";

    const std::vector<TestCase> tests = {
        {
            "R-type instruction encodings",
            "valid_r_type.asm",
            {
                0x00f583b3, 0x401101b3, 0x012b72b3, 0x01e46533, 0x01f040b3,
                0x00209fb3, 0x001fd133, 0x4195d1b3, 0x0062a233
            }
        },
        {
            "I-type, load/store, and JALR encodings",
            "valid_i_memory.asm",
            {
                0x00000013, 0x80008f93, 0x06447213, 0xfff16193, 0x7ff32293,
                0x06402183, 0xffc12f83, 0x06302223, 0xfff12e23, 0x00c100e7
            }
        },
        {
            "branch instruction encodings",
            "valid_branches.asm",
            {
                0x06838263, 0xfe2098e3, 0x020fc063, 0xfe525ee3
            }
        },
        {
            "JAL and LUI encodings",
            "valid_jump_upper.asm",
            {
                0x001000ef, 0xffdfffef, 0x02710437, 0xfffff037
            }
        },
        {
            "reject invalid operand types and extra operands",
            "invalid_operand_forms.asm",
            {
                0x00900493
            }
        },
        {
            "reject register numbers outside x0-x31",
            "invalid_registers.asm",
            {
                0x00a00513
            }
        },
        {
            "reject non-numeric immediates without throwing",
            "invalid_immediate.asm",
            {}
        }
    };

    std::size_t failures = 0;
    for (const auto& test : tests) {
        if (!run_test(test, cases_directory)) {
            ++failures;
        }
    }

    std::cout << "\n" << tests.size() - failures << " of " << tests.size()
              << " compiler tests passed.\n";
    return failures == 0 ? 0 : 1;
}
