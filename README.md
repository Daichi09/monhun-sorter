# monhun-sorter #

A simple command line program to edit Monster Hunter World Item Data.

You can easily change item box sort order or item prices via a simple tab delimited data file.

I take no responsiblity for how you use this. BACK UP YOUR SAVE BEFORE USING!

Place the "itemData.itm" file in "steamapps\common\Monster Hunter World\nativePC\common\item"

Then head to your Item Box in game and hit the sort button.

# Usage: #
Edit ItemData:
    `monhun_sorter.exe <UpdateItemData.txt> <Output_file.itm> [itemData.itm]`

Export ItemData as tab delimited data file:
    `monhun_sorter.exe -e <Output_file.txt> [NameData.txt] [itemData.itm]`

# Tab Delimited File Format #

The program will look at the first line in your tab delimited file for header information.

| ID | Name | NewSort | NewBuy | NewSell |
|----|------|---------|--------|---------|

It will look for header lines in any order. You can add other lines for your own reference.
(Name is actually not a required field.)

# Other files
## NameData.txt ##
"NameData.txt" is all the Item Names I borrowed from the "MHW Shop List Editor".

This is strictly for data export funcationality.

I didn't put any effort into making this program Unicode friendly, so sorry if you want item names to appear in another language.

## Custom_Sort ##
I've included my own "Custom_Sort" data as an excel sheet and tab delimited file you can use as a reference.

Usage: `monhun_sorter.exe Custom_Sort.txt Output_file.itm`

The first group of entries I consider "Priority Items", stuff I'm likely to take with me on any mission. Stuff like Mega Potions, Max Potions, Herbal Medicine, Mega Demondrugs/ArmorSkin. Then stuff like Pods, Bombs, Traps, and ATK/DEF Charms are grouped after that.

I also don't bring normal Potions or Antidotes with me anymore, or use normal Demondrug/Armorskin so I don't need those at the front of my item box. I also prefer Rations over Well-done Steak, as they don't take forever to eat.

My secondary weapon class is Light Bowgun, so I wanted all my related ammo crafting materials to appear right next to each other.

If you use other classes, you might have different priorities, so my sheet also has added reference columns for Bows and Mushroommancer.

If you rearrange things, remember to update the NewSortOrder column and export it as a text file.

## 50k_Potion ##
Also included a "50k_Potion.txt" file, which is a simple two line line data file for a cheat Potion that sells for 50k.

Usage: `monhun_sorter.exe 50k_Potion.txt Output_file.itm`