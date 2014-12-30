This is a Nintendo 3DS homebrew tool for accessing the Home Menu SD extdata, this is usable from ninjhax. This allows reading/writing the home-menu SD extdata SaveData.dat, and the extdata theme-cache.

See also: http://3dbrew.org/wiki/Home_Menu http://3dbrew.org/wiki/Home_Menu/Themes

This essentially allows the user to write custom themes to SD extdata which home-menu will automatically load at next startup.

# Compiling
This requires the latest ctrulib from git.

If you build a CXI/CIA for this, the accessinfo must have bitmask 0x01 set(in the RSF that's "FileSystemAccess" with field "- CategoryFileSystemTool").

# Usage
If you want to backup extdata before modifying it at all, you can copy the data from "sdmc:/Nintendo 3DS/{ID0}/{ID1}/extdata/00000000/{extdataIDlow}/" to elsewhere, where extdataIDlow is the extdataIDs for home-menu extdata and theme extdata: http://3dbrew.org/wiki/Extdata

If you want to disable using the theme-cache, you can either enter home-menu theme-settings then select none, restore a backup of SaveData.dat with this tool, restore the entire home-menu and/or theme  extdata with the above backup, or delete the homemenu and/or theme extdata.

Once the main-menu loads(after opening the extdata archives), use the D-Pad/Circle-Pad to select a menu entry. Press A to enter a menu, once finished it will delay 5 seconds before automatically returning from the menu. When at the main-menu, press B to return to the hbmenu.

The data this app reads/writes to/from sdmc is located in the same directory that the app runs from(SD root for CXI/CIA).
The theme-cache data this app uses on sdmc is the following:
* "ThemeManage.bin" This is copied from extdata to sdmc. This can be copied to extdata from sdmc, if it doesn't exist on sdmc for that then data is generated for it then writen to extdata.
* "BodyCache.bin" and "body_LZ.bin": The former is copied from extdata to sdmc. When copying from sdmc to extdata, the app will first attempt to use "BodyCache.bin", then "body_LZ.bin". When ThemeManage.bin exists on sdmc with sdmc->extdata, this body-data is only copied when the size field in ThemeManage.bin for this is non-zero.
* "BgmCache.bin" and "bgm.bcstm": Handled basically the same way as the body-data, except with the bgm sizes.

