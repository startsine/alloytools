#ifndef ASMX64_SYMBOL_H
#define ASMX64_SYMBOL_H 1

#include <stdint.h>
#include <vector>
#include <string>

// 重定位类型
enum class RelocType
{
    None = 0,
    ADDR64,                                 // 64位绝对地址
    ADDR32,                                 // 32位绝对地址
    REL32,                                  // 相对下一条指令的相对PC寻址
};

class RelocInfo
{
public:
    std::string name;
    RelocType type = RelocType::None;
    uint32_t offset = 0;
};


#endif // ASMX64_SYMBOL_H

