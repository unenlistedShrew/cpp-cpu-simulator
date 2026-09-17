# cpp-cpu-simulator
Basic cpu simulator implemented in cpp.
**Architecture:**
- 32-bit RISC-V
- Little-endian byte ordering
- 32 general purpose registers

**Instruction Set Supported:**
- add/addi
- sub
- and/andi/or/ori/xor
- sll/srl/sra (logical and arthimetic left or right bit shifting)
- slt/slti (set less than)
- lw/sw
- beq/bne
- blt/bgt
- jal/jalr
- lui

**Specifications:**
- 1 GB of addressable memory
- Byte-addressable
- Word-aligned access