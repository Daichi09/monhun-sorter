#include <stdint.h>
#include <string.h>
#include <windows.h>

typedef int8_t  s8;
typedef int8_t  s08;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef s32 b32;

typedef uint8_t  u8;
typedef uint8_t  u08;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

enum type_id
{
    type_Item,
    type_Material,
    type_AcountItem,
    type_Decoration,
};

#pragma pack(push, 1)
struct item
{
    u32 ID;
    u08 SubType;
    u32 Type;
    u08 Rarity;
    u08 CarryLimit;
    u08 Unknown;
    u16 SortOrder;
    u32 Unknown2;
    u32 IconID;
    u16 IconColourID;
    u32 SellPrice;
    u32 BuyPrice;
};

struct header
{
    u16 Unknown1;
    u16 Unknown2;
    u16 Unknown3;
    u32 Count;
};
#pragma pack(pop)

enum update_flags
{
    update_sort = 1 << 0,
    update_buy  = 1 << 1,
    update_sell = 1 << 2,
}; // end enum update_flags

struct item_sort
{
    char ItemName[64];
    u32 UpdateFlags;
    u16 Sort;
    u32 Buy;
    u32 Sell;
};

enum data_columns
{
    data_id,
    data_name,
    data_new_sort,
    data_new_buy,
    data_new_sell,
    data_comment,
};

inline char *
Trim(char *s)
{
    // Trim Left
    while(isspace(*s)) s++;
    // Trim Right
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';

    return s; 
}

#include <stdlib.h>

// https://unixpapa.com/incnote/string.html
char *strsep(char **sp, char *sep)
{
    char *p, *s;
    if (sp == NULL || *sp == NULL || **sp == '\0') return(NULL);
    s = *sp;
    p = s + strcspn(s, sep);
    if (*p != '\0') *p++ = '\0';
    *sp = p;
    return(s);
}