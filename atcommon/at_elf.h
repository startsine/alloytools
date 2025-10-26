#pragma once

#include <stdint.h>

typedef struct
{
    uint32_t section_name;          // Section name 
    uint32_t section_type;          // Section type 
    uint64_t section_flags;         // Section attributes 
    uint64_t section_addr;          // Virtual address in memory 
    uint64_t section_offset;        // Offset in file 
    uint64_t section_size;          // Size of section 
    uint32_t section_link;          // Link to other section 
    uint32_t section_info;          // Miscellaneous information 
    uint64_t section_addralign;     // Address alignment boundary 
    uint64_t section_entsize;       // Size of entries, if section has table 
} Elf64SectionEntry;

#define ELF_SECTION_FLAG_WRITE          (1 << 0)
#define ELF_SECTION_FLAG_ALLOC          (1 << 1)
#define ELF_SECTION_FLAG_EXEC           (1 << 2)
#define ELF_SECTION_FLAG_MERGE          (1 << 4)
#define ELF_SECTION_FLAG_STRING         (1 << 5)
#define ELF_SECTION_FLAG_INFO_LINK      (1 << 6)
#define ELF_SECTION_FLAG_LINK_ORDER     (1 << 7)
#define ELF_SECTION_OS_SPECIFIC         (1 << 8)
#define ELF_SECTION_GROUP               (1 << 9)
#define ELF_SECTION_TLS                 (1 << 10)
#define ELF_SECTION_COMPRESSED          (1 << 11)
//#define ELF_SECTION_EXCLUDE             (1 << 18)
#define ELF_SECTION_GNU_RETAIN          (1 << 21)
#define ELF_SECTION_GNU_MBIND           (1 << 24)

#define ELF_SECTION_TYPE_NULL	0		// Section header table entry unused 
#define ELF_SECTION_TYPE_PROGBITS	1		// Program specific (private) data 
#define ELF_SECTION_TYPE_SYMTAB	2		// Link editing symbol table 
#define ELF_SECTION_TYPE_STRTAB	3		// A string table
#define ELF_SECTION_TYPE_RELA	4		// Relocation entries with addends 
#define ELF_SECTION_TYPE_HASH	5		// A symbol hash table 
#define ELF_SECTION_TYPE_DYNAMIC	6		// Information for dynamic linking 
#define ELF_SECTION_TYPE_NOTE	7		// Information that marks file 
#define ELF_SECTION_TYPE_NOBITS	8		// Section occupies no space in file 
#define ELF_SECTION_TYPE_REL		9		// Relocation entries, no addends 
#define ELF_SECTION_TYPE_SHLIB	10		// Reserved, unspecified semantics 
#define ELF_SECTION_TYPE_DYNSYM	11		// Dynamic linking symbol table 

#define ELF_SECTION_TYPE_INIT_ARRAY	  14		// Array of ptrs to init functions 
#define ELF_SECTION_TYPE_FINI_ARRAY	  15		// Array of ptrs to finish functions 
#define ELF_SECTION_TYPE_PREINIT_ARRAY 16		// Array of ptrs to pre-init funcs 
#define ELF_SECTION_TYPE_GROUP	  17		// Section contains a section group 
#define ELF_SECTION_TYPE_SYMTAB_SHNDX  18		// Indices for SHN_XINDEX entries 
#define ELF_SECTION_TYPE_RELR	  19		// RELR relative relocations 


typedef struct
{
    uint32_t pro_type;              // Type of segment 
    uint32_t pro_flags;             // Segment attributes 
    uint64_t pro_offset;            // Offset in file 
    uint64_t pro_vaddr;             // Virtual address in memory 
    uint64_t pro_paddr;             // Reserved 
    uint64_t pro_filesz;            // Size of segment in file 
    uint64_t pro_memsz;             // Size of segment in memory 
    uint64_t pro_align;             // Alignment of segment 
} Elf64ProgramEntry;

#define ELF_SEGMENT_FLAG_EXECUTABLE     (1 << 0)
#define ELF_SEGMENT_FLAG_WRITABLE       (1 << 1)
#define ELF_SEGMENT_FLAG_READABLE       (1 << 2)

#define ELF_SEGMENT_TYPE_NULL 0
#define ELF_SEGMENT_TYPE_LOAD 1
#define ELF_SEGMENT_TYPE_DYNAMIC 2
#define ELF_SEGMENT_TYPE_INTERP 3
#define ELF_SEGMENT_TYPE_NOTE 4
#define ELF_SEGMENT_TYPE_SHLIB 5
#define ELF_SEGMENT_TYPE_PHDR 6
#define ELF_SEGMENT_TYPE_TLS 7
#define ELF_SEGMENT_TYPE_NUM 8
#define ELF_SEGMENT_TYPE_LOOS		0x60000000	// OS-specific start id
// sun
#define ELF_SEGMENT_TYPE_SUNW_UNWIND  (ELF_SEGMENT_TYPE_LOOS + 0x464e550)
// gnu
#define ELF_SEGMENT_TYPE_GNU_EH_FRAME	(ELF_SEGMENT_TYPE_LOOS + 0x474e550) // Frame unwind information 
#define ELF_SEGMENT_TYPE_SUNW_EH_FRAME ELF_SEGMENT_TYPE_GNU_EH_FRAME      // Solaris uses the same value 
#define ELF_SEGMENT_TYPE_GNU_STACK	(ELF_SEGMENT_TYPE_LOOS + 0x474e551) // Stack flags 
#define ELF_SEGMENT_TYPE_GNU_RELRO	(ELF_SEGMENT_TYPE_LOOS + 0x474e552) // Read-only after relocation 
#define ELF_SEGMENT_TYPE_GNU_PROPERTY	(ELF_SEGMENT_TYPE_LOOS + 0x474e553) // GNU property 
#define ELF_SEGMENT_TYPE_GNU_SFRAME	(ELF_SEGMENT_TYPE_LOOS + 0x474e554) // SFrame stack trace information 
//OpenBSD
#define ELF_SEGMENT_TYPE_OPENBSD_MUTABLE   (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe5)  // Like bss, but not immutable
#define ELF_SEGMENT_TYPE_OPENBSD_RANDOMIZE (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe6)  // Fill with random data
#define ELF_SEGMENT_TYPE_OPENBSD_WXNEEDED  (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe7)  // Program does W^X violations
#define ELF_SEGMENT_TYPE_OPENBSD_NOBTCFI   (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe8)  // No branch target CFI
#define ELF_SEGMENT_TYPE_OPENBSD_SYSCALLS  (ELF_SEGMENT_TYPE_LOOS + 0x5a3dbe9)  // System call sites
#define ELF_SEGMENT_TYPE_OPENBSD_BOOTDATA  (ELF_SEGMENT_TYPE_LOOS + 0x5a41be6)  // Section for boot arguments
// sun
#define ELF_SEGMENT_TYPE_SUNWBSS	(ELF_SEGMENT_TYPE_LOOS + 0xffffffa)	        // Sun Specific segment
#define ELF_SEGMENT_TYPE_SUNWSTACK	(ELF_SEGMENT_TYPE_LOOS + 0xffffffb)	        // Stack segment.  
#define ELF_SEGMENT_TYPE_SUNWDTRACE   (ELF_SEGMENT_TYPE_LOOS + 0xffffffc)
#define ELF_SEGMENT_TYPE_SUNWCAP      (ELF_SEGMENT_TYPE_LOOS + 0xffffffd)

typedef struct
{
    int64_t d_tag;
    union {
        uint64_t d_val;
        uint64_t d_ptr;
    } d_un;
} Elf64DynEntry;

#define DYN_TYPE_NULL		    0
#define DYN_TYPE_NEEDED	        1               // 指出依赖的动态库(可以多个)
#define DYN_TYPE_PLTRELSZ	    2               // 程序链接表（PLT）相关的重定位条目的总大小（字节）
#define DYN_TYPE_PLTGOT	        3               // 与PLT相关的 或者 GOT 表的虚拟地址, 此字段的具体含义取决于处理器
#define DYN_TYPE_HASH		    4               // 符号哈希表的虚拟地址(现在主要使用 GNU_HASH，因为使用gnu_hash找符号更快)
#define DYN_TYPE_STRTAB	        5               // 字符串表的虚拟地址(包含了动态链接器所需的所有字符串，如共享库的名称和符号名称)
#define DYN_TYPE_SYMTAB	        6               // 符号表的虚拟地址，其中包含了动态链接器所需的所有符号
#define DYN_TYPE_RELA		    7               // 重定位表的虚拟地址 (RELA格式的重定位表) (相当于PE/COFF的.reloc段)
#define DYN_TYPE_RELASZ	        8               // 重定位表的大小(字节)
#define DYN_TYPE_RELAENT	    9               // 重定位表每一项的大小(字节)
#define DYN_TYPE_STRSZ	        10              // 字符串表的大小(字节)
#define DYN_TYPE_SYMENT	        11              // 符号表中每一项的大小(字节)
#define DYN_TYPE_INIT		    12              // 初始化函数的地址，该函数在程序启动时被调用
#define DYN_TYPE_FINI		    13              // 终止函数的地址，该函数在程序退出时被调用
#define DYN_TYPE_SONAME	        14              // 本共享对象的SONAME（共享对象的简单名称）的字符串表偏移索引
#define DYN_TYPE_RPATH	        15              // 用于搜索共享库的库搜索路径的字符串表偏移索引 - 编译时指定的搜索路径, 优先级高于LD_LIBRARY_PATH (RPATH与RUNPATH同时存在时，RPATH无效)
#define DYN_TYPE_SYMBOLIC	    16              // 值忽略。存在这项，代表用于改变了动态链接器的符号解析算法，使得在库内部的符号引用从该库本身开始搜索，而不是从可执行文件开始
#define DYN_TYPE_REL		    17              // 重定位表的地址 (REL格式的重定位表，不带附加值A)
#define DYN_TYPE_RELSZ	        18              // REL格式重定位表的总大小(字节)
#define DYN_TYPE_RELENT	        19              // REL重定位条目每一项的大小(字节)
#define DYN_TYPE_PLTREL	        20              // 指定程序链接表中使用的重定位条目的重定位类型 (可以同时作用于REL和RELA两种重定位表)
#define DYN_TYPE_DEBUG	        21              // 调试信息
#define DYN_TYPE_TEXTREL	    22              // 值忽略。标志，存在这项, 用于重定位 .text 段, 加载器可以修改 .text 段
#define DYN_TYPE_JMPREL	        23              // PLT跳转表重定位。如果存在，其d_ptr成员包含与程序链接表相关的重定位条目的地址。(相当于PE/COFF的IAT)
                                                // 这些重定位条目可以在延迟绑定期间被忽略(如JMPREL存在，那么PLTRELSZ和DT_PLTREL也必须存在, PLTREL指定重定位类型为REL还是RELA)
                                                // 在动态链接阶段，系统通过此表将程序中引用的外部函数地址转换为实际内存中的有效地址，确保程序运行时能够正确调用动态库中的函数
#define DYN_TYPE_BIND_NOW	    24              // 值忽略。存在这项，它指示动态链接器在将控制权交给程序之前，立即处理所有重定位。（通常使用的都是懒加载，即在符号被使用时再对符号进行重定位）
#define DYN_TYPE_INIT_ARRAY	    25              // 初始化函数数组的地址，该函数在程序启动时被调用
#define DYN_TYPE_FINI_ARRAY	    26              // 终止函数的数组的地址，该函数在程序退出时被调用
#define DYN_TYPE_INIT_ARRAYSZ   27              // 初始化函数数组的大小(字节)
#define DYN_TYPE_FINI_ARRAYSZ   28              // 终止函数的数组的大小(字节)
#define DYN_TYPE_RUNPATH	    29              // 指定运行时搜索路径的字符串表偏移索引(RPATH) - 运行时指定搜索路径，优先级低于LD_LIBRARY_PATH但高于系统默认路径（如/lib、/usr/lib）
#define DYN_TYPE_FLAGS	        30              // 控制符号解析策略的标志位

// 从DYN_TYPE_ENCODING 到 DYN_TYPE_LOOS 之间, 偶数项使用 d_un.d_ptr，奇数项使用 d_un.d_val
#define DYN_TYPE_ENCODING	        32
#define DYN_TYPE_PREINIT_ARRAY      32          // 程序启动前需要执行的初始化函数数组的地址
#define DYN_TYPE_PREINIT_ARRAYSZ    33          // 程序启动前需要执行的初始化函数数组的大小(字节)
#define DYN_TYPE_SYMTAB_SHNDX       34          // 
#define DYN_TYPE_RELRSZ	            35          // RELR 格式的重定位表大小(字节)
#define DYN_TYPE_RELR		        36          // RELR 格式的重定位表的地址
#define DYN_TYPE_RELRENT	        37          // RELR 格式的重定位表每一项的大小(字节)

#define DYN_TYPE_LOOS		        0x6000000d
#define DYN_TYPE_HIOS		        0x6ffff000
#define DYN_TYPE_LOPROC	            0x70000000
#define DYN_TYPE_HIPROC	            0x7fffffff

//// 
#define DYN_TYPE_VALRNGLO	    0x6ffffd00
#define DYN_TYPE_GNU_FLAGS_1    0x6ffffdf4
#define DYN_TYPE_GNU_PRELINKED  0x6ffffdf5
#define DYN_TYPE_GNU_CONFLICTSZ 0x6ffffdf6
#define DYN_TYPE_GNU_LIBLISTSZ  0x6ffffdf7
#define DYN_TYPE_CHECKSUM	    0x6ffffdf8
#define DYN_TYPE_PLTPADSZ	    0x6ffffdf9
#define DYN_TYPE_MOVEENT	    0x6ffffdfa
#define DYN_TYPE_MOVESZ	        0x6ffffdfb
#define DYN_TYPE_FEATURE	    0x6ffffdfc
#define DYN_TYPE_POSFLAG_1	    0x6ffffdfd
#define DYN_TYPE_SYMINSZ	    0x6ffffdfe
#define DYN_TYPE_SYMINENT	    0x6ffffdff
#define DYN_TYPE_VALRNGHI	    0x6ffffdff

#define DYN_TYPE_ADDRRNGLO	    0x6ffffe00
#define DYN_TYPE_GNU_HASH	    0x6ffffef5
#define DYN_TYPE_TLSDESC_PLT	0x6ffffef6
#define DYN_TYPE_TLSDESC_GOT	0x6ffffef7
#define DYN_TYPE_GNU_CONFLICT	0x6ffffef8
#define DYN_TYPE_GNU_LIBLIST	0x6ffffef9
#define DYN_TYPE_CONFIG	        0x6ffffefa
#define DYN_TYPE_DEPAUDIT	    0x6ffffefb
#define DYN_TYPE_AUDIT	        0x6ffffefc
#define DYN_TYPE_PLTPAD	        0x6ffffefd
#define DYN_TYPE_MOVETAB	    0x6ffffefe
#define DYN_TYPE_SYMINFO	    0x6ffffeff
#define DYN_TYPE_ADDRRNGHI	    0x6ffffeff

#define DYN_TYPE_RELACOUNT	    0x6ffffff9
#define DYN_TYPE_RELCOUNT	    0x6ffffffa
#define DYN_TYPE_FLAGS_1	    0x6ffffffb      // FLAGS_1 扩展标志位
#define DYN_TYPE_VERDEF	        0x6ffffffc
#define DYN_TYPE_VERDEFNUM	    0x6ffffffd
#define DYN_TYPE_VERNEED	    0x6ffffffe      // VERNEED 主要记录程序所需的版本化依赖信息，例如共享库的版本号或兼容性要求。
#define DYN_TYPE_VERNEEDNUM	    0x6fffffff

// This tag is a GNU extension to the Solaris version scheme
#define DYN_TYPE_VERSYM	        0x6ffffff0

#define DYN_TYPE_LOPROC	0x70000000
#define DYN_TYPE_HIPROC	0x7fffffff

/* These section tags are used on Solaris.  We support them
   everywhere, and hope they do not conflict.  */

#define DYN_TYPE_AUXILIARY	0x7ffffffd
#define DYN_TYPE_USED		0x7ffffffe
#define DYN_TYPE_FILTER	0x7fffffff

typedef struct {
    uint32_t    name;
    uint32_t    value;
    uint32_t    size;
    uint8_t     info;
    uint8_t     other;
    uint16_t    shndx;
} Elf32Symbol;

typedef struct
{
    uint32_t    name;   // Symbol name 
    uint8_t     info;   // Type and Binding attributes
    uint8_t     other;  // Reserved
    uint16_t    shndx;  // Section table index
    uint64_t    value;  // Symbol value
    uint64_t    size;   // Size of object (e.g., common) 
} Elf64Symbol;

#define SYMBOL_BINDINGS_LOCAL	0		// Symbol not visible outside obj 
#define SYMBOL_BINDINGS_GLOBAL	1		// Symbol visible outside obj 
#define SYMBOL_BINDINGS_WEAK	2		// Like globals, lower precedence 
#define SYMBOL_BINDINGS_LOOS	10		// OS-specific semantics 
#define SYMBOL_BINDINGS_GNU_UNIQUE	10	// Symbol is unique in namespace 
#define SYMBOL_BINDINGS_HIOS	12		// OS-specific semantics 
#define SYMBOL_BINDINGS_LOPROC	13		// Processor-specific semantics 
#define SYMBOL_BINDINGS_HIPROC	15		// Processor-specific semantics 

#define SYMBOL_TYPE_NOTYPE	0		// Symbol type is unspecified 
#define SYMBOL_TYPE_OBJECT	1		// Symbol is a data object 
#define SYMBOL_TYPE_FUNC	2		// Symbol is a code object 
#define SYMBOL_TYPE_SECTION	3		// Symbol associated with a section 
#define SYMBOL_TYPE_FILE	4		// Symbol gives a file name 
#define SYMBOL_TYPE_COMMON	5		// An uninitialised common block 
#define SYMBOL_TYPE_TLS		6		// Thread local data object 
#define SYMBOL_TYPE_RELC	8		// Complex relocation expression 
#define SYMBOL_TYPE_SRELC	9		// Signed Complex relocation expression 
#define SYMBOL_TYPE_LOOS	10		// OS-specific semantics 
#define SYMBOL_TYPE_GNU_IFUNC	10		// Symbol is an indirect code object 
#define SYMBOL_TYPE_HIOS	12		// OS-specific semantics 
#define SYMBOL_TYPE_LOPROC	13		// Processor-specific semantics 
#define SYMBOL_TYPE_HIPROC	15		// Processor-specific semantics 

#define SYMBOL_VISIBILITY_DEFAULT	0		// Visibility is specified by binding type 
#define SYMBOL_VISIBILITY_INTERNAL	1		// OS specific version of STV_HIDDEN 
#define SYMBOL_VISIBILITY_HIDDEN	2		// Can only be seen inside currect component 
#define SYMBOL_VISIBILITY_PROTECTED	3		// Treat as STB_LOCAL inside current component 

typedef struct
{
    uint64_t offset;
    uint64_t info;
    int64_t addend;
} Elf64RelAEntry;









