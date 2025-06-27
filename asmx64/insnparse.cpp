
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "asmx64.h"

using namespace std;

OpcodeInfos::OpcodeInfos()
{
    reset();
}

void OpcodeInfos::reset()
{
    this->opcodesSize = 0;
    this->numberOfOperand = 0;
    this->op0 = MatchType::None;
    this->op1 = MatchType::None;
    this->op2 = MatchType::None;
    this->op3 = MatchType::None;
    this->opcodeFlag = OpcodeFlag_None;
    this->forbidInfo = MatchForbid_None;
    this->digit = 0;
}

InsnProcessFlag BaseInsn::getInsnFlag()
{
	return InsnProcessFlag::None;
}

OpcodeInfos BaseInsn::noOperand;
uint8_t BaseInsn::finalCode[32];           // 全体code
uint8_t BaseInsn::prefixCode[32];          // 前缀部分的code
uint8_t BaseInsn::insCode[32];             // 指令部分的code
uint8_t BaseInsn::addrCode[32];            // 内存寻址部分的code
uint8_t BaseInsn::immCode[32];             // 立即数部分的code

int BaseInsn::processCpuIns(X64Assembler & assembler, const std::string & insnStr, SourceLine & sourceLine, int pass, const std::list<OpcodeInfos> & opcodeInfos)
{
    printf("process %s. \n", insnStr.c_str());

    if (sourceLine.hasLabel) {
        if (pass == 1) {
            Symbol * sym = assembler.globalSymbolList.getSymbol(sourceLine.labelStr);
            if (sym == nullptr) {
                Symbol symbol;
                symbol.symbolName = sourceLine.labelStr;
                symbol.sizeType = SymbolSizeType::Proc;
                symbol.sectionIndex = sourceLine.sectionIndex;
                symbol.offsetValue = assembler.getCurrOffset(sourceLine.sectionIndex);
                symbol.visibType = SymbolVisibilityType::None;
                assembler.globalSymbolList.addSymbol(symbol);
            }
            else {
                //// 报错，重复定义符号
                return 0;
            }
        }
        else {

        }
    }

    int insTotalSize = 0;
    std::list<OpcodeInfos>::const_iterator info = opcodeInfos.cend();
    const OpcodeInfos * matchedInfo = nullptr;

    int expressionsCount = (int) sourceLine.expressions.size();       // 当前指令的表达式的个数 
    auto currentNode = opcodeInfos.cbegin();
    while (currentNode != opcodeInfos.cend()) {
        info = currentNode;
        if (info->numberOfOperand == expressionsCount) {
            if (info->numberOfOperand == 0) {
                matchedInfo = & noOperand;
                break;                                              // 不需要操作数的指令直接匹配
            }
            //
            if (expressionsCount == 1) {
                if (checkOperandMatch(&(sourceLine.expressions[0].operand), info->op0)) {
                    matchedInfo = &*info;
                    break;
                }
            }
            else if (expressionsCount == 2) {
                if (checkOperandMatch(&(sourceLine.expressions[0].operand), info->op0) &&
                    checkOperandMatch(&(sourceLine.expressions[1].operand), info->op1)) {
                    matchedInfo = &*info;
                    break;
                }
            }
            else if (expressionsCount == 3) {
                if (checkOperandMatch(&(sourceLine.expressions[0].operand), info->op0) &&
                    checkOperandMatch(&(sourceLine.expressions[1].operand), info->op1) &&
                    checkOperandMatch(&(sourceLine.expressions[2].operand), info->op2)) {
                    matchedInfo = &*info;
                    break;
                }
            }
        }
        currentNode++;
    }

    // matchedInfo非空则表示匹配
    if (matchedInfo != nullptr) {
        insTotalSize = matchedInfo->opcodesSize;
        MemoryAddressResult * mem = nullptr;
        X64RegValue regFieldInModRM = X64RegValue::None;
        X64RegValue rmFieldInModRM = X64RegValue::None;
        int prefixCodeSize = 0;
        int insCodeSize = 0;
        int addrCodeSize = 0;
        int immCodeSize = 0;
        int totalCodeSize = 0;
        bool flagRexE = false;
        bool flagRexW = false;
        bool flagRexR = false;
        bool flagRexX = false;
        bool flagRexB = false;
        bool addr32bitPrefix = false;
        int bitSize = 0;
        shared_ptr<RelocInfo> addrRelocInfo = nullptr;         // 寻址代码中的重定位信息
        shared_ptr<RelocInfo> immRelocInfo = nullptr;          // 立即数代码中的重定位信息

        switch (expressionsCount) {
        case 0: {

        }
            break;
        case 1: {

        }
            break;
        case 2: {
            bitSize = getBaseInsnOpSize2(sourceLine, pass, opcodeInfos);       // 得到指令的操作数的位数大小，返回8/16/32/64
            if (bitSize == 0) {
                //// 报错, 无法决定操作数类型
                return 0;
            }
            if (u32HasFlag(matchedInfo->opcodeFlag, OpcodeFlag_ModRM_R) || u32HasFlag(matchedInfo->opcodeFlag, OpcodeFlag_ModRM_Digit)) {
                // 存在 ModRM 字段
                if (matchedInfo->op0 == MatchType::rm) {                        // 如果 op0 匹配了 R/M 域
                    auto & operand0 = sourceLine.expressions[0].operand;
                    auto & operand1 = sourceLine.expressions[1].operand;
                    if (operand0.type == X64OperandType::MemoryAddress) {
                        mem = & operand0.addressRes;
                    }
                    else if (operand0.type == X64OperandType::Register) {
                        rmFieldInModRM = operand0.regValue;
                    }
                    else if (operand0.type == X64OperandType::Symbol) {
                        // to-do
                    }
                    //
                    if (operand1.type == X64OperandType::Register) {
                        regFieldInModRM = operand1.regValue;
                    }
                }
                else if (matchedInfo->op1 == MatchType::rm) {                     // 如果 op1 匹配了 R/M 域
                    auto & operand0 = sourceLine.expressions[0].operand;
                    auto & operand1 = sourceLine.expressions[1].operand;
                    if (operand1.type == X64OperandType::MemoryAddress) {
                        mem = & operand1.addressRes;
                    }
                    else if (operand1.type == X64OperandType::Register) {
                        rmFieldInModRM = operand1.regValue;
                    }
                    else if (operand1.type == X64OperandType::Symbol) {
                        // to-do
                    }
                    //
                    if (operand0.type == X64OperandType::Register) {
                        regFieldInModRM = operand0.regValue;
                    }
                }
                else {
                    //// 报错
                    return 0;
                }
                //
                memcpy(insCode, matchedInfo->opcodes, matchedInfo->opcodesSize);
                insCodeSize = matchedInfo->opcodesSize;

                memcpy(addrCode, mem->code, mem->codeSize);
                addrCodeSize = mem->codeSize;
                
                //
                if (u64HasFlag((uint64_t)mem->type, (uint64_t)MemoryAddressType::withRex_X))
                    flagRexX = true;
                if (u64HasFlag((uint64_t)mem->type, (uint64_t)MemoryAddressType::withRex_B))
                    flagRexB = true;
                if (u64HasFlag((uint64_t)mem->type, (uint64_t)MemoryAddressType::with32bitRegAddr))
                    addr32bitPrefix = true;
                if (u64HasFlag((uint64_t)mem->type, (uint64_t)MemoryAddressType::hasSymbol)) {
                    addrRelocInfo = make_shared<RelocInfo>();
                    addrRelocInfo->offset = (uint32_t)mem->relocOffset;
                    addrRelocInfo->type = mem->relocType;
                    addrRelocInfo->name = mem->symName;
                }
                //
                if (u32HasFlag(matchedInfo->opcodeFlag, OpcodeFlag_ModRM_Digit)) {
                    // 把操作码插入到 ModRM 中的 reg 域
                    addrCode[0] &= 0xC7;
                    addrCode[0] |= (uint8_t)((matchedInfo->digit & 0x07) << 3);
                }
                else { //HasFlag(OpcodeFlag.ModRM_R)
                    // 将寄存器插入到 ModRM 中的 reg 域
                    if (regFieldInModRM != X64RegValue::None) {
                        uint64_t regValue = (uint64_t)regFieldInModRM;
                        addrCode[0] &= 0xC7;
                        addrCode[0] |= (uint8_t)((regValue & 0x07) << 3);
                    }
                    if (X64RegUtil::isRexExtensionReg(regFieldInModRM))
                        flagRexR = true;
                    if (X64RegUtil::isRexPrefixReg(regFieldInModRM))
                        flagRexE = true;
                }
                //
                if (u32HasFlag(matchedInfo->opcodeFlag, OpcodeFlag_bit0Size)) {
                    if (bitSize != 8) {
                        insCode[insCodeSize - 1] |= 0x01;
                    }
                }
                if (u32HasFlag(matchedInfo->opcodeFlag, OpcodeFlag_bit3Size)) {
                    if (bitSize != 8) {
                        insCode[insCodeSize - 1] |= 0x08;
                    }
                }
                if (u32HasFlag(matchedInfo->opcodeFlag, OpcodeFlag_withImm)) {
                    int bytesize = bitSize / 8;
                    //// to-do
                }
                if (bitSize == 64)
                    flagRexW = true;
                // 下面获得前缀操作码
                getPrefixCode(prefixCodeSize, sourceLine, *matchedInfo, addr32bitPrefix, bitSize, flagRexE, flagRexW, flagRexR, flagRexX, flagRexB);
                // 处理imm
                if (u32HasFlag(matchedInfo->opcodeFlag, OpcodeFlag_withImm)) {

                }
                // 复制指令码
                int newCodeSize = combineCode(prefixCodeSize, insCodeSize, addrCodeSize, immCodeSize);
                if (newCodeSize != 0) {
                    if (pass > 1) {
                        int oldCodeSize = (int) sourceLine.code.size();
                        if (oldCodeSize != newCodeSize) {
                            assembler.setNeedRescan(true);                // 代码大小发生变化了，需要重新扫描
                        }
                    }
                    std::vector<uint8_t> newCode;
                    for (int i = 0; i < newCodeSize; i++) {
                        newCode.push_back(finalCode[i]);
                    }
                    sourceLine.code = newCode;
                    vector<RelocInfo> relocs = combineRelocs(addrRelocInfo, immRelocInfo, prefixCodeSize, insCodeSize, addrCodeSize);
                    sourceLine.relocInfos = relocs;
                    totalCodeSize = newCodeSize;
                }
                if (totalCodeSize > 0)
                    assembler.addCodeSize((uint32_t)totalCodeSize, sourceLine, pass);
                return totalCodeSize;
            }
        }
                break;
        case 3: {

        }
                break;
        default: {
            //// 报错，没有超过3个操作数的指令
        }
                 break;
        }

    }
    else {
        //// 报错，找不到指令匹配
        return 0;
    }

    /*
     public enum OpcodeFlag: uint
{
    None = 0,
    ModRM_R = 0x01,                 //  /r :     带 ModRM 并且 其中代表两个值，reg 和 r/m
    ModRM_Digit = 0x02,             //  /digit:  带 ModRM 并且 r/m 域代表r/m, reg 域代表 3bit 的额外 opcode
    bit0Size = 0x04,                //  指令码的 bit0 代表操作数大小, bit0==0是为8bit, bit0==1是为 16/32/64 bit,
    bit3Size = 0x08,                //  指令码的 bit3 代表操作数大小, bit3==0是为8bit, bit3==1是为 16/32/64 bit
    opcodeWithReg = 0x10,           //  将寄存器插入到 opcode 的 bit2-bit0 位置
    withImm = 0x20,                 //  指令码最后带立即数作为操作数
    withM64 = 0x40,
    RMInRight = 0x80,               //  该bit为1时表示匹配的 R/M 域放置于第2个操作数，为0则R/M 域放置于第1个操作数
}
         */


    return 0;
}

// 判断操作数是否与指定的类型是否匹配
bool BaseInsn::checkOperandMatch(const X64Operand * operand, MatchType matchType)
{
    if (operand == nullptr) {
        return false;
    }
    switch (matchType) {
    case MatchType::reg: {   // 判断操作数是否为一个通用寄存器
        if (operand->type == X64OperandType::Register) {
            if (X64RegUtil::isCommonReg(operand->regValue)) {
                return true;
            }
        }
    }
                         break;
    case MatchType::rm: {
        if (operand->type == X64OperandType::Register) {
            if (X64RegUtil::isCommonReg(operand->regValue)) {
                return true;
            }
        }
        if (operand->type == X64OperandType::MemoryAddress) {
            return true;
        }
        if (operand->type == X64OperandType::Symbol) {
            // TO-DO, 这里要判断 不加 offset 修饰的符号当作内存寻址
        }
    }
                        break;
    case MatchType::acc: {
        if (operand->type == X64OperandType::Register) {
            if (operand->regValue == X64RegValue::AL || operand->regValue == X64RegValue::AX
                || operand->regValue == X64RegValue::EAX || operand->regValue == X64RegValue::RAX) {
                return true;
            }
        }
    }
                         break;            
    case MatchType::imm: {
        //if (operand.type) {

        //}
    }
                         break;
                        /*

                imm,                            // 操作数是立即数
                rm,                             // 操作数是寄存器或者内存寻址
                //moffset32,                      // 操作数是内存寻址，用[imm]寻址的
                moffset64,                      // 操作数是内存寻址，用[imm64]寻址的
                segReg,                         // 操作数是段寄存器
                debugReg,                       // 操作数是DR0-DR15
                ctrlReg,                        // 操作数是CR0-CR15
                         */
    }
    return false;
}

int BaseInsn::getOpSize(const X64Operand * op)
{
    if (op == nullptr) {
        return 0;
    }
    if (op->type == X64OperandType::Register) {
        if (X64RegUtil::is8BitReg(op->regValue)) {
            return 8;
        }
        if (X64RegUtil::is16BitReg(op->regValue)) {
            return 16;
        }
        if (X64RegUtil::is32BitReg(op->regValue)) {
            return 32;
        }
        if (X64RegUtil::is64BitReg(op->regValue)) {
            return 64;
        }
        if (X64RegUtil::isSegReg(op->regValue)) {
            return 16;
        }
        if (X64RegUtil::isDebugReg(op->regValue)) {
            return 64;
        }
        if (X64RegUtil::isCtrlReg(op->regValue)) {
            return 64;
        }
    }
    return 0;
}

int BaseInsn::getBaseInsnOpSize2(const SourceLine & sourceLine, int pass, const std::list<OpcodeInfos> & opcodeInfos)
{
    int opSize0, opSize1;
    int ret = 0;
    opSize0 = getOpSize(& sourceLine.expressions[0].operand);
    opSize1 = getOpSize(& sourceLine.expressions[1].operand);
    if (opSize0 == 0 && opSize1 == 0) {
        //// 报错
        return 0;
    }
    if ((opSize0 != 0 && opSize1 != 0) && opSize0 != opSize1) {
        //// 报错
        return 0;
    }
    if (opSize0 != 0)
        ret = opSize0;
    else if (opSize1 != 0)
        ret = opSize1;

    return ret;
}

int BaseInsn::getPrefixCode(int & prefixCodeSize, const SourceLine & sourceLine, const OpcodeInfos & matchedInfo,
    bool addr32bit, int bitSize, bool flagRexE, bool flagRexW, bool flagRexR, bool flagRexX, bool flagRexB)
{
    return 2;
}

int BaseInsn::combineCode(int prefixCodeSize, int insCodeSize, int addrCodeSize, int immCodeSize)
{
    return 15;
}

std::vector<RelocInfo> BaseInsn::combineRelocs(std::shared_ptr<RelocInfo> addrRelocInfo, std::shared_ptr<RelocInfo> immRelocInfo, int prefixCodeSize, int insCodeSize, int addrCodeSize)
{
    std::vector<RelocInfo> a;
    return a;
}

