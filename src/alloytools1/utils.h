#pragma once

#include <stdint.h>
#include <stdio.h>

FILE* fopen_utf8_filename(const char* path, const char* mode);


void file_seek(FILE* fp, int64_t offset, int32_t origin);
uint16_t fread_u16(FILE* fp);
uint32_t fread_u32(FILE* fp);
uint64_t fread_u64(FILE* fp);
size_t fwrite_u16(FILE* fp, uint16_t value);
size_t fwrite_u32(FILE* fp, uint32_t value);
size_t fwrite_u64(FILE* fp, uint64_t value);

