This is a Nintendo 3DS homebrew tool for accessing the Home Menu SD extdata, this is usable from ninjhax. This allows reading/writing the home-menu SD extdata SaveData.dat, and the extdata theme-cache.

See also: http://3dbrew.org/wiki/Home_Menu http://3dbrew.org/wiki/Home_Menu/Themes

This essentially allows the user to write custom themes to SD extdata, which home-menu will automatically load at Home Menu process next startup.

# Compiling
This requires the latest ctrulib from git.

If you build a CXI/CIA for this, the accessinfo must have bitmask 0x01 set(in the RSF that's "FileSystemAccess" with field "- CategorySystemApplication").

# Usage
Backing up extdata before modifying it at all is recommended: you can copy the data from "sdmc:/Nintendo 3DS/{ID0}/{ID1}/extdata/00000000/{extdataIDlow}/" to elsewhere, where extdataIDlow is the extdataIDs for home-menu extdata and theme extdata: http://3dbrew.org/wiki/Extdata

If you want to disable using the theme-cache, you can either enter home-menu theme-settings then select none, restore a backup of SaveData.dat with this tool, restore the entire home-menu and/or theme  extdata with the above backup, or delete the homemenu and/or theme extdata.

Once the main-menu loads(after opening the extdata archives), use the D-Pad/Circle-Pad to select a menu entry. Press A to enter a menu. When at the main-menu, press B to return to the hbmenu.

Main menu:
* "Copy extdata SaveData.dat to sd" This can be used to backup your SaveData.dat to SD(this contains the current theme settings and other things).
* "Copy SaveData.dat from sd to extdata" This can be used to restore your SaveData.dat from SD.
* "Enable normal theme-cache" This enables using the theme-data from SD. Do not use this if you don't have any theme DLC installed([#1](../../issues/1)), use the below menu option instead.
* "Enable persistent theme-cache" Same as above except the custom themes are persistent even without any theme DLC installed.
* "Copy theme-data from extdata to sd" This can be used to backup the theme-data stored in extdata.
* "Copy theme-data from sd to extdata" This can be used to write a custom theme to extdata.

The data this app reads/writes to/from sdmc is located in the same directory that the app runs from(SD root for CXI/CIA).
The theme-data this app uses on sdmc is the following:
* "ThemeManage.bin" This is copied from extdata to sdmc. This can be copied to extdata from sdmc, if it doesn't exist on sdmc for that then data is generated for it then writen to extdata.
* "BodyCache.bin" and "body_LZ.bin": The former is copied from extdata to sdmc. When copying from sdmc to extdata, the app will first attempt to use "BodyCache.bin", then "body_LZ.bin". When ThemeManage.bin exists on sdmc with sdmc->extdata, this body-data is only copied when the size field in ThemeManage.bin for this is non-zero. When copying to extdata, the filesize must be smaller than or exactly 0x150000-bytes.
* "BgmCache.bin" and "bgm.bcstm": Handled basically the same way as the body-data, except with the bgm sizes. When copying to extdata, the filesize must be smaller than or exactly 0x337000-bytes. When writing BGM to extdata, Home Menu *must* *not* be already using any BGM from the theme-cache(prior to when this tool wrote anything to extdata), otherwise this issue occurs: [#2](../../issues/2).

