#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <3ds.h>

#include "archive.h"

typedef int (*menuent_funcptr)(void);

int menu_savedatadat2sd();
int menu_sd2savedatadat();
int enablethemecache();
int menu_enablethemecache_normal();
int menu_enablethemecache_persistent();
int menu_themecache2sd();
int menu_sd2themecache();

int mainmenu_totalentries = 6;
char *mainmenu_entries[6] = {
"Copy extdata SaveData.dat to sd",
"Copy SaveData.dat from sd to extdata",
"Enable normal theme-cache",
"Enable persistent theme-cache",
"Copy theme-data from extdata to sd",
"Copy theme-data from sd to extdata"};
menuent_funcptr mainmenu_entryhandlers[6] = {menu_savedatadat2sd, menu_sd2savedatadat, menu_enablethemecache_normal, menu_enablethemecache_persistent, menu_themecache2sd, menu_sd2themecache};

u8 *filebuffer;
u32 filebuffer_maxsize = 0x400000;

int draw_menu(char **menu_entries, int total_menuentries, int x, int y)
{
	int i;
	int cursor = 0;
	int update_menu = 1;
	int entermenu = 0;

	while(aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();

		if(kDown & KEY_A)
		{
			entermenu = 1;
			break;
		}
		if(kDown & KEY_B)return -1;

		if(kDown & KEY_UP)
		{
			update_menu = 1;
			cursor--;
			if(cursor<0)cursor = total_menuentries-1;
		}

		if(kDown & KEY_DOWN)
		{
			update_menu = 1;
			cursor++;
			if(cursor>=total_menuentries)cursor = 0;
		}

		if(update_menu)
		{
			for(i=0; i<total_menuentries; i++)
			{
				if(cursor!=i)printf("\x1b[%d;%dH   %s", y+i, x, menu_entries[i]);
				if(cursor==i)printf("\x1b[%d;%dH-> %s", y+i, x, menu_entries[i]);
			}
		}
	}

	if(!entermenu)return -2;
	return cursor;
}

int menu_savedatadat2sd()
{
	Result ret=0;
	char filepath[256];

	memset(filebuffer, 0, filebuffer_maxsize);

	memset(filepath, 0, 256);
	strncpy(filepath, "SaveData.dat", 255);

	ret = archive_copyfile(HomeMenu_Extdata, SDArchive, "/SaveData.dat", filepath, filebuffer, 0, filebuffer_maxsize, "SaveData.dat");

	if(ret==0)
	{
		printf("Successfully finished.\n");
	}

	return 0;
}

int menu_sd2savedatadat()
{
	Result ret=0;
	char filepath[256];

	memset(filebuffer, 0, filebuffer_maxsize);

	memset(filepath, 0, 256);
	strncpy(filepath, "SaveData.dat", 255);

	ret = archive_copyfile(SDArchive, HomeMenu_Extdata, filepath, "/SaveData.dat", filebuffer, 0, filebuffer_maxsize, "SaveData.dat");

	if(ret==0)
	{
		printf("Successfully finished.\n");
	}

	return 0;
}

int enablethemecache(u32 type)
{	
	Result ret=0;
	u32 filesize = 0;

	printf("Reading SaveData.dat...\n");

	ret = archive_getfilesize(HomeMenu_Extdata, "/SaveData.dat", &filesize);
	if(ret!=0)
	{
		printf("Failed to get filesize for extdata SaveData.dat: 0x%08x\n", (unsigned int)ret);
		return 0;
	}

	if(filesize > filebuffer_maxsize)
	{
		printf("Extdata SaveData.dat filesize is too large: 0x%08x\n", (unsigned int)filesize);
		return 0;
	}

	ret = archive_readfile(HomeMenu_Extdata, "/SaveData.dat", filebuffer, filesize);
	if(ret!=0)
	{
		printf("Failed to read file: 0x%08x\n", (unsigned int)ret);
		return 0;
	}

	if(ret==0)
	{
		if(filebuffer[0x141b]==0 && filebuffer[0x13b8]!=0 && filebuffer[0x13bc]==0 && filebuffer[0x13bd]==type)
		{
			ret = -3;
			printf("SaveData.dat is already set for using the theme cache with a regular theme.\n");
			return 0;
		}
	}

	if(ret==0)
	{
		filebuffer[0x141b]=0;//Disable theme shuffle.
		memset(&filebuffer[0x13b8], 0, 8);//Clear the regular-theme structure.
		filebuffer[0x13bd]=type;//theme-type
		filebuffer[0x13b8] = 0xff;//theme-index

		printf("Writing updated SaveData.dat...\n");

		ret = archive_writefile(HomeMenu_Extdata, "/SaveData.dat", filebuffer, filesize);
		if(ret!=0)
		{
			printf("Failed to write file: 0x%08x\n", (unsigned int)ret);
		}
	}

	return 0;
}

int menu_enablethemecache_normal()
{
	return enablethemecache(2);
}

int menu_enablethemecache_persistent()
{
	return enablethemecache(3);
}

int menu_themecache2sd()
{
	Result ret=0;
	u32 thememanage[0x20>>2];
	char filepath[256];

	memset(thememanage, 0, 0x20);

	memset(filepath, 0, 256);
	strncpy(filepath, "ThemeManage.bin", 255);

	ret = archive_copyfile(Theme_Extdata, SDArchive, "/ThemeManage.bin", filepath, filebuffer, 0x800, filebuffer_maxsize, "ThemeManage.bin");

	if(ret==0)
	{
		printf("Successfully finished copying ThemeManage.bin.\n");
	}
	else
	{
		return 0;
	}

	memcpy(thememanage, filebuffer, 0x20);

	if(thememanage[0x8>>2] == 0)
	{
		printf("Skipping copying of BodyCache.bin since the size field is zero.\n");
	}
	else
	{
		memset(filepath, 0, 256);
		strncpy(filepath, "BodyCache.bin", 255);

		ret = archive_copyfile(Theme_Extdata, SDArchive, "/BodyCache.bin", filepath, filebuffer, thememanage[0x8>>2], 0x150000, "BodyCache.bin");

		if(ret==0)
		{
			printf("Successfully finished copying BodyCache.bin.\n");
		}
		else
		{
			return 0;
		}
	}

	if(thememanage[0xC>>2] == 0)
	{
		printf("Skipping copying of BgmCache.bin since the size field is zero.\n");
	}
	else
	{
		memset(filepath, 0, 256);
		strncpy(filepath, "BgmCache.bin", 255);

		ret = archive_copyfile(Theme_Extdata, SDArchive, "/BgmCache.bin", filepath, filebuffer, thememanage[0xC>>2], 0x337000, "BgmCache.bin");

		if(ret==0)
		{
			printf("Successfully finished copying BgmCache.bin.\n");
		}
	}

	return 0;
}

int menu_sd2themecache()
{
	Result ret=0;
	u32 body_size=0, bgm_size=0;
	u32 thememanage[0x20>>2];
	char filepath[256];
	char body_filepath[256];
	char bgm_filepath[256];

	memset(thememanage, 0, 0x20);

	memset(body_filepath, 0, 256);
	strncpy(body_filepath, "BodyCache.bin", 255);

	ret = archive_getfilesize(SDArchive, body_filepath, &body_size);
	if(ret!=0)
	{
		memset(body_filepath, 0, 256);
		strncpy(body_filepath, "body_LZ.bin", 255);

		ret = archive_getfilesize(SDArchive, body_filepath, &body_size);
		if(ret!=0)
		{
			printf("Failed to stat BodyCache.bin and body_LZ.bin on SD, copying for the body-data will be skipped.\n");

			memset(body_filepath, 0, 256);
		}
		else
		{
			printf("Using body-filepath body_LZ.bin.\n");
		}
	}
	else
	{
		printf("Using body-filepath BodyCache.bin.\n");
	}

	memset(bgm_filepath, 0, 256);
	strncpy(bgm_filepath, "BgmCache.bin", 255);

	ret = archive_getfilesize(SDArchive, bgm_filepath, &bgm_size);
	if(ret!=0)
	{
		memset(bgm_filepath, 0, 256);
		strncpy(bgm_filepath, "bgm.bcstm", 255);

		ret = archive_getfilesize(SDArchive, bgm_filepath, &bgm_size);
		if(ret!=0)
		{
			printf("Failed to stat BgmCache.bin and bgm.bcstm on SD, copying for the bgm-data will be skipped.\n");

			memset(bgm_filepath, 0, 256);
		}
		else
		{
			printf("Using bgm-filepath bgm.bcstm.\n");
		}
	}
	else
	{
		printf("Using bgm-filepath BgmCache.bin.\n");
	}

	memset(filepath, 0, 256);
	strncpy(filepath, "ThemeManage.bin", 255);

	ret = archive_copyfile(SDArchive, Theme_Extdata, filepath, "/ThemeManage.bin", filebuffer, 0x800, filebuffer_maxsize, "ThemeManage.bin");

	if(ret==0)
	{
		printf("Successfully finished copying ThemeManage.bin.\n");

		memcpy(thememanage, filebuffer, 0x20);
	}
	else
	{
		printf("Failed to copy ThemeManage.bin, generating one then trying again...\n");

		memset(thememanage, 0, 0x20);
		thememanage[0x0>>2] = 1;
		thememanage[0x8>>2] = body_size;
		thememanage[0xC>>2] = bgm_size;
		thememanage[0x10>>2] = 0xff;
		thememanage[0x14>>2] = 1;
		thememanage[0x18>>2] = 0xff;
		thememanage[0x1c>>2] = 0x200;

		memset(filebuffer, 0, 0x800);
		memcpy(filebuffer, thememanage, 0x20);
		ret = archive_writefile(Theme_Extdata, "/ThemeManage.bin", filebuffer, 0x800);

		if(ret!=0)
		{
			printf("Failed to write ThemeManage.bin to extdata, aborting.\n");
			return 0;
		}
	}

	if(body_filepath[0])
	{
		if(thememanage[0x8>>2] == 0)
		{
			printf("Skipping copying of body-data since the size field is zero.\n");
		}
		else
		{
			ret = archive_copyfile(SDArchive, Theme_Extdata, body_filepath, "/BodyCache.bin", filebuffer, thememanage[0x8>>2], 0x150000, "body-data");

			if(ret==0)
			{
				printf("Successfully finished copying body-data.\n");
			}
			else
			{
				return 0;
			}
		}
	}

	if(bgm_filepath[0])
	{
		if(thememanage[0xC>>2] == 0)
		{
			printf("Skipping copying of bgm-data since the size field is zero.\n");
		}
		else
		{
			ret = archive_copyfile(SDArchive, Theme_Extdata, bgm_filepath, "/BgmCache.bin", filebuffer, thememanage[0xC>>2], 0x337000, "bgm-data");

			if(ret==0)
			{
				printf("Successfully finished copying bgm-data.\n");
			}
		}
	}

	return 0;
}

int handle_menus()
{
	int ret;

	while(aptMainLoop())
	{
		consoleClear();

		ret = draw_menu(mainmenu_entries, mainmenu_totalentries, 0, 0);
		consoleClear();

		if(ret<0)return ret;

		ret = mainmenu_entryhandlers[ret]();
		if(ret==-2)return ret;

		printf("\nPress A to return to the main-menu.\n");

		while(1)
		{
			gspWaitForVBlank();
			hidScanInput();

			if(hidKeysDown() & KEY_A)break;
		}
	}

	return -2;
}

int main()
{
	Result ret = 0;

	// Initialize services
	gfxInitDefault();

	consoleInit(GFX_BOTTOM, NULL);

	printf("3ds_homemenu_extdatatool\n");

	filebuffer = (u8*)malloc(0x400000);
	if(filebuffer==NULL)
	{
		printf("Failed to allocate memory.\n");
		ret = -1;
	}
	else
	{
		memset(filebuffer, 0, filebuffer_maxsize);
	}

	if(ret>=0)
	{
		printf("Opening extdata archives...\n");

		ret = open_extdata();
		if(ret==0)
		{
			printf("Finished opening extdata.\n");

			consoleClear();
			handle_menus();
		}
	}

	if(ret<0)
	{
		printf("Press the START button to exit.\n");
		// Main loop
		while (aptMainLoop())
		{
			gspWaitForVBlank();
			hidScanInput();

			u32 kDown = hidKeysDown();
			if (kDown & KEY_START)
				break; // break in order to return to hbmenu
		}
	}

	free(filebuffer);
	close_extdata();

	// Exit services
	gfxExit();
	return 0;
}

