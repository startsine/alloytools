#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

MemoryAddressResult::MemoryAddressResult(): relocType(RelocType::None)
{
}

X64Operand::X64Operand()
{
    type = X64OperandType::Unknown;
}

X64Operand::X64Operand(X64RegValue newValue)
{
    type = X64OperandType::Register;
    regValue = newValue;
}

X64Operand::X64Operand(uint64_t newValue)
{
    type = X64OperandType::ImmediateValue;
    ulongValue = newValue;
}

X64Operand::X64Operand(const MemoryAddressInfo & info)
{

}

bool X64RegUtil::is32Or64BitReg(X64RegValue reg)
{
    return enum64HasFlag((uint64_t)reg, (uint64_t)X64RegValue::Common) && 
        (enum64HasFlag((uint64_t)reg, (uint64_t)X64RegValue::REG_64bit) || enum64HasFlag((uint64_t)reg, (uint64_t)X64RegValue::REG_32bit));
}



