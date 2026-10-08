ADDI x0, x0, 0
ADDI x31, x1, -2048
ANDI x4, x8, 100
ORI x3, x2, -1
SLTI x5, x6, 2047
LW x3, 100(x0)
LW x31, -4(x2)
SW x3, 100(x0)
SW x31, -4(x2)
JALR x1, x2, 12
