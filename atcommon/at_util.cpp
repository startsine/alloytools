#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "at_util.h"

int get_number_digits(uint64_t num)
{
    int digits = 0;
    uint64_t curr = num;
    do {
        curr /= 10;
        digits++;
    } while (curr != 0);
    return digits;
}

int get_number_digits(int64_t num)
{
    int digits = 0;
    int64_t curr = num;
    do {
        curr /= 10;
        digits++;
    } while (curr != 0);
    return digits;
}


