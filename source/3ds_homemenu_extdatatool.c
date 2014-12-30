#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <3ds.h>

#include "archive.h"

typedef int (*menuent_funcptr)(void);

int menu_savedatadat2sd();
int menu_sd2savedatadat();
int menu_enablethemecache();
int menu_themecache2sd();
int menu_sd2themecache();

int mainmenu_totalentries = 5;
char *mainmenu_entries[5] = {
"Copy extdata SaveData.dat to sd",
"Copy SaveData.dat from sd to extdata",
"Enable theme-cache",
"Copy theme cache from extdata to sd",
"Copy theme cache from sd to extdata"};
menuent_funcptr mainmenu_entryhandlers[5] = {menu_savedatadat2sd, menu_sd2savedatadat, menu_enablethemecache, menu_themecache2sd, menu_sd2themecache};

char *sdpath_prefix = "/3ds/3ds_homemenu_extdatatool/";

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

			gfxFlushBuffers();
			gfxSwapBuffers();
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
	snprintf(filepath, 255, "%sSaveData.dat", sdpath_prefix);

	ret = archive_copyfile(HomeMenu_Extdata, SDArchive, "/SaveData.dat", filepath, filebuffer, 0x2cb0, filebuffer_maxsize, "SaveData.dat");

	if(ret==0)printf("Successfully finished.\n");

	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(5000000000LL);
	return 0;
}

int menu_sd2savedatadat()
{
	Result ret=0;
	char filepath[256];

	memset(filebuffer, 0, filebuffer_maxsize);

	memset(filepath, 0, 256);
	snprintf(filepath, 255, "%sSaveData.dat", sdpath_prefix);

	ret = archive_copyfile(SDArchive, HomeMenu_Extdata, filepath, "/SaveData.dat", filebuffer, 0x2cb0, filebuffer_maxsize, "SaveData.dat");

	if(ret==0)printf("Successfully finished.\n");

	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(5000000000LL);
	return 0;
}

int menu_enablethemecache()
{	
	Result ret=0;

	printf("Reading SaveData.dat...\n");
	gfxFlushBuffers();
	gfxSwapBuffers();

	ret = archive_readfile(HomeMenu_Extdata, "/SaveData.dat", filebuffer, 0x2cb0);
	if(ret!=0)
	{
		printf("Failed to read file: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
	}

	if(ret==0)
	{
		if(filebuffer[0x131b]==0  && filebuffer[0x13bc]==0 && filebuffer[0x13bd]==2)
		{
			ret = -3;
			printf("SaveData.dat is already set for using the theme cache.\n");
			gfxFlushBuffers();
			gfxSwapBuffers();
		}
	}

	if(ret==0)
	{
		filebuffer[0x131b]=0;
		filebuffer[0x13bd]=2;
		memset(&filebuffer[0x13b8], 0, 5);

		printf("Writing updated SaveData.dat...\n");
		gfxFlushBuffers();
		gfxSwapBuffers();

		ret = archive_writefile(HomeMenu_Extdata, "/SaveData.dat", filebuffer, 0x2cb0);
		if(ret!=0)
		{
			printf("Failed to write file: 0x%08x\n", (unsigned int)ret);
			gfxFlushBuffers();
			gfxSwapBuffers();
		}
	}

	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(5000000000LL);
	return 0;
}

int menu_themecache2sd()
{
	printf("themecache2sd N/A\n");
	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(2000000000LL);
	return 0;
}

int menu_sd2themecache()
{
	printf("sd2themecache N/A\n");
	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(2000000000LL);
	return 0;
}

int handle_menus()
{
	int ret;

	gfxFlushBuffers();
	gfxSwapBuffers();

	while(aptMainLoop())
	{
		consoleClear();

		ret = draw_menu(mainmenu_entries, mainmenu_totalentries, 0, 0);
		consoleClear();

		if(ret<0)return ret;

		ret = mainmenu_entryhandlers[ret]();
		if(ret==-2)return ret;
	}

	return -2;
}

int main()
{
	Result ret = 0;

	// Initialize services
	gfxInit();

	consoleInit(GFX_BOTTOM, NULL);

	printf("3ds_homemenu_extdatatool\n");
	gfxFlushBuffers();
	gfxSwapBuffers();

	filebuffer = (u8*)malloc(0x400000);
	if(filebuffer==NULL)
	{
		printf("Failed to allocate memory.\n");
		gfxFlushBuffers();
		gfxSwapBuffers();
		ret = -1;
	}
	else
	{
		memset(filebuffer, 0, filebuffer_maxsize);
	}

	if(ret>=0)
	{
		printf("Opening extdata archives...\n");
		gfxFlushBuffers();
		gfxSwapBuffers();
		ret = open_extdata();
		if(ret==0)
		{
			printf("Finished opening extdata.\n");
			gfxFlushBuffers();
			gfxSwapBuffers();

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

			// Flush and swap framebuffers
			gfxFlushBuffers();
			gfxSwapBuffers();
		}
	}

	free(filebuffer);
	close_extdata();

	// Exit services
	gfxExit();
	return 0;
}

