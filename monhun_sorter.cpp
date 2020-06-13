#include "monhun_sorter.h"
#include <stdio.h>

// Currently 2775 items in the game, so 3k should be plenty.
#define MAX_ITEMS 3000
#define ITEM_NAME_LENGTH 64

// 32 tabbed Columns in your data sheet ought to be enough for anyone...
#define MAX_COLUMNS 32

int
DumpItemData(char *OutputDataFileName, char *ItemNamesFileName, char *ItemDataFileName)
{
    char ItemNames[MAX_ITEMS][ITEM_NAME_LENGTH] = {};
    char Line[1024];

    FILE *FP, *FP2;
    char *Delim = "\t";

    // Read Item Names from Data file
    errno_t Err = fopen_s(&FP, ItemNamesFileName, "r");
    if (!Err)
    {
        while ( fgets(Line, sizeof(Line), FP) )
        {
            u32 ID;
            char *HexID, *ItemName;

            char *Running = Trim(Line);

            HexID = strsep(&Running, Delim);
            ID = strtol(HexID, 0, 16);
            ItemName = strsep(&Running, Delim);
            sprintf_s(ItemNames[ID], "%s", ItemName);
        }
        fclose(FP);
    }
    else
    {
        // Error
        printf("File Open Error: <%s> error %d\n", ItemNamesFileName, Err);
        return Err;
    }

    Err = fopen_s(&FP, ItemDataFileName, "rb");
    errno_t Err2 = fopen_s(&FP2, OutputDataFileName, "w");
    if (!Err && !Err2)
    {
        header Header;
        fread(&Header, sizeof(header), 1, FP);
        item Item;

        // Skip first item
        fread(&Item, sizeof(item), 1, FP);

        // Write Header
        sprintf_s(Line, sizeof(Line),
            "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
            "ID",
            "Name",
            "Type",
            "SubType",
            "Rarity",
            "CarryLimit",
            "Unknown",
            "SortOrder",
            "Unknown2",
            "IconID",
            "IconColourID",
            "SellPrice",
            "BuyPrice"
        );
        fwrite(Line, strlen(Line), 1, FP2);

        for (u32 i = 1; i < Header.Count; i++)
        {
            fread(&Item, sizeof(item), 1, FP);

            // Could add a command parameter to disables these filters, but I'm lazy.
            // skip blank IconID. If it doesn't have an icon, it's probably not something we can sort anyway.
            if (Item.IconID == 0)
                continue;

            // type 0 & 3 only
            if (Item.Type == type_Item || Item.Type == type_Decoration)
            {
                sprintf_s(Line, sizeof(Line),
                    "%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
                    Item.ID,
                    ItemNames[i], 
                    Item.Type,
                    Item.SubType,
                    Item.Rarity,
                    Item.CarryLimit,
                    Item.Unknown,
                    Item.SortOrder,
                    Item.Unknown2,
                    Item.IconID,
                    Item.IconColourID,
                    Item.SellPrice,
                    Item.BuyPrice
                );
                fwrite(Line, strlen(Line), 1, FP2);
            }
        }
        fclose(FP);
        fclose(FP2);
    }
    else
    {
        // Error
        if (Err)
            printf("File Open Error: <%s> error %d\n", ItemDataFileName, Err);
        if (Err2)
            printf("File Open Error: <%s> error %d\n", OutputDataFileName, Err2);
        return Err;
    }
    return 0;
}

void
PrintUsage()
{
        printf(
R"FOO(
Usage:
    Edit ItemData:
        monhun_sorter.exe <UpdateItemData.txt> <Output_file.itm> [itemData.itm]

    Export ItemData:
        monhun_sorter.exe -e <Output_file.txt> [NameData.txt] [itemData.itm]
)FOO");
}

int
main(u32 ArgCount, char *ArgValues[])
{
    // If no Args Print Usage Info.
    if (ArgCount == 1)
    {
        PrintUsage();
        return 1;
    }

    char UpdateDataFileName[MAX_PATH];
    char ItemNamesFileName[MAX_PATH];
    char OutputFileName[MAX_PATH];
    char ItemDataFileName[MAX_PATH];

    // Lazy Arg parsing
    if (ArgCount >= 2 && !strcmp(ArgValues[1], "-e"))
    {
        // Output_file.txt
        if (ArgCount >= 3)
        {
            strcpy_s(OutputFileName, MAX_PATH, ArgValues[2]);
        }
        else
        {
            printf("Output Filename Required for Data Export.\n");
            PrintUsage();
            return 1;
        }
        // Optional [NameData.txt]
        if (ArgCount >= 4)
        {
            strcpy_s(ItemNamesFileName, MAX_PATH, ArgValues[3]);
        }
        else
        {
            strcpy_s(ItemNamesFileName, MAX_PATH, "NameData.txt");
        }
        // Optional [itemData.itm]
        if (ArgCount >= 5)
        {
            strcpy_s(ItemDataFileName, MAX_PATH, ArgValues[4]);
        }
        else
        {
            strcpy_s(ItemDataFileName, MAX_PATH, "itemData.itm");
        }
        return DumpItemData(OutputFileName, ItemNamesFileName, ItemDataFileName );
    }

    if (ArgCount >= 3)
    {
        strcpy_s(UpdateDataFileName, MAX_PATH, ArgValues[1]);
        strcpy_s(OutputFileName, MAX_PATH, ArgValues[2]);
    }

    // Optional [itemData.itm]
    if (ArgCount >= 4)
    {
        strcpy_s(ItemDataFileName, MAX_PATH, ArgValues[3]);
    }
    else
    {
        strcpy_s(ItemDataFileName, MAX_PATH, "itemData.itm");
    }

    item_sort ItemSorts[MAX_ITEMS] = {};
    char Line[1024];

    FILE *FP, *FP2;

    char *Delim = "\t";

    errno_t Err = fopen_s(&FP, UpdateDataFileName, "r");
    if (!Err)
    {
        char *Str;
        printf("Opened %s\n", UpdateDataFileName);

        u32 Columns[MAX_COLUMNS];
        u32 ColNum = 0;
        // Use first line for Column Info
        fgets(Line, sizeof(Line), FP);

        char OutputBuffer[2048] = {};

        char *Running = Trim(Line);
        Str = strsep(&Running, Delim);
        sprintf_s(OutputBuffer, sizeof(OutputBuffer), "Columns Detected: ");
        while (Str)
        {
            if (ColNum > MAX_COLUMNS)
            {
                printf("Too many Columns detected, max is: %d\n", MAX_COLUMNS);
                return ColNum;
            }

            if (!strcmp(Str, "ID"))
            {
                Columns[data_id] = ColNum;
                sprintf_s(OutputBuffer, sizeof(OutputBuffer), "%s %s", OutputBuffer, Str);
            }
            else if (!strcmp(Str, "Name"))
            {
                Columns[data_name] = ColNum;
                sprintf_s(OutputBuffer, sizeof(OutputBuffer), "%s %s", OutputBuffer, Str);
            }
            else if (!strcmp(Str, "NewSortOrder"))
            {
                Columns[data_new_sort] = ColNum;
                sprintf_s(OutputBuffer, sizeof(OutputBuffer), "%s %s", OutputBuffer, Str);
            }
            else if (!strcmp(Str, "NewBuyPrice"))
            {
                Columns[data_new_buy] = ColNum;
                sprintf_s(OutputBuffer, sizeof(OutputBuffer), "%s %s", OutputBuffer, Str);
            }
            else if (!strcmp(Str, "NewSellPrice"))
            {
                Columns[data_new_sell] = ColNum;
                sprintf_s(OutputBuffer, sizeof(OutputBuffer), "%s %s", OutputBuffer, Str);
            }
            Str = strsep(&Running, Delim);
            ++ColNum;
        } // END while (true)
        printf("%s.\n", OutputBuffer);
 
        u32 TotalUpdateCount = 0;
        u32 SortCount = 0;
        u32 BuyCount = 0;
        u32 SellCount = 0;
        while ( fgets(Line, sizeof(Line), FP) )
        {
            // Length check
            Running = Trim(Line);
            if (strlen(Running))
            {
                u32 ID = 0;
                char *ItemName = 0;
                u16 NewSort = 0;
                u32 NewBuy = 0;
                u32 NewSell = 0;

                u32 Flags = 0;

                Str = strsep(&Running, Delim);
                // Read Line's Column data
                for (u32 i = 0; i < ColNum; i++)
                {
                    if (Str && strlen(Str))
                    {
                        if (Columns[data_id] == i)
                        {
                            ID = atoi(Str);
                        }
                        else if (Columns[data_name] == i)
                        {
                            ItemName = Str;
                        }
                        else if (Columns[data_new_sort] == i)
                        {
                            NewSort = (u16)atoi(Str);
                            Flags |= update_sort;
                        }
                        else if (Columns[data_new_buy] == i)
                        {
                            NewBuy = atoi(Str);
                            Flags |= update_buy;
                        }
                        else if (Columns[data_new_sell] == i)
                        {
                            NewSell = atoi(Str);
                            Flags |= update_sell;
                        }
                    }
                    Str = strsep(&Running, Delim);
                } // END for i

                // Don't add to database if no Update values found
                if (Flags)
                {
                    ItemSorts[ID].UpdateFlags = Flags;
                    if (Flags & update_sort)
                    {
                        ItemSorts[ID].Sort = NewSort;
                        ++SortCount;
                    }
                    if (Flags & update_buy)
                    {
                        ItemSorts[ID].Buy = NewBuy;
                        ++BuyCount;
                    }
                    if (Flags & update_sell)
                    {
                        ItemSorts[ID].Sell = NewSell;
                        ++SellCount;
                    }
                    ++TotalUpdateCount;
                }
                else
                {
                    if (ItemName)
                    {
                        // If the name is included Print it.
                        printf("Skipped Item ID: %d, %s\n", ID, ItemName);
                    }
                    else
                    {
                        printf("Skipped Item ID: %d\n", ID);
                    }
                }
            }
        }
        fclose(FP);
        printf("Found %d Item Entries to Update.\nNew Sort Entries: %d, New Buy Entries: %d, New Sell Entries %d.\n\n",
               TotalUpdateCount,
               SortCount,
               BuyCount,
               SellCount);
    }
    else
    {
        // Error
        printf("File Open Error: %s error %d\n", UpdateDataFileName, Err);
        return Err;
    }

    Err = fopen_s(&FP, ItemDataFileName, "rb");
    errno_t Err2 = fopen_s(&FP2, OutputFileName, "wb");
    if (!Err && !Err2)
    {
        header Header;
        fread(&Header, sizeof(header), 1, FP);
        fwrite(&Header, sizeof(header), 1, FP2);

        printf("%s Opened. %d Entries Found.\n", ItemDataFileName, Header.Count);

        item Item;

        fread(&Item, sizeof(item), 1, FP);
        fwrite(&Item, sizeof(item), 1, FP2);

        for (u32 i = 1; i < Header.Count; i++)
        {
            fread(&Item, sizeof(item), 1, FP);

            // Use Custom value if it exists
            u32 Flags = ItemSorts[i].UpdateFlags;
            if (Flags)
            {
                if (Flags & update_sort)
                {
                    Item.SortOrder = ItemSorts[i].Sort;
                }
                if (Flags & update_buy)
                {
                    Item.BuyPrice = ItemSorts[i].Buy;
                }
                if (Flags & update_sell)
                {
                    Item.SellPrice = ItemSorts[i].Sell;
                }
            }
            fwrite(&Item, sizeof(item), 1, FP2);

        } // END for i
        fclose(FP);
        fclose(FP2);
        printf("Finished writing %s.\n", OutputFileName);
    }
    else
    {
        if (Err)
            printf("File Open Error: %s error %d\n", ItemDataFileName, Err);
        if (Err2)
            printf("File Open Error: %s error %d\n", OutputFileName, Err2);
        return Err;
    }

    return 0;
}