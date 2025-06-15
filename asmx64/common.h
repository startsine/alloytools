
#ifndef ASMX64_COMMON_H
#define ASMX64_COMMON_H 1

#include <stddef.h>
#include <stdint.h>

inline bool enum64HasFlag(uint64_t var1, uint64_t flag)
{
    return (var1 & flag) != 0;
}

inline bool enum32HasFlag(uint32_t var1, uint32_t flag)
{
    return (var1 & flag) != 0;
}

inline void enum64SetFlag(uint64_t * var1, uint64_t flag)
{
    (*var1) |= flag;
}

inline bool enum32SetFlag(uint32_t * var1, uint32_t flag)
{
    (*var1) |= flag;
}

inline void enum64ClearFlag(uint64_t * var1, uint64_t flag)
{
    (*var1) &= (~flag);
}

inline bool enum32ClearFlag(uint32_t * var1, uint32_t flag)
{
    (*var1) &= (~flag);
}

#endif // ASMX64_COMMON_H


