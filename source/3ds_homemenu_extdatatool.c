#include <string.h>
#include <stdio.h>
#include <3ds.h>

u32 extdata_homemenu_lowpathdata[3];
u32 extdata_theme_lowpathdata[3];

FS_archive extdata_archive_homemenu = {
	.id = ARCH_EXTDATA,
	.lowPath = {
		.type = PATH_BINARY,
		.size = 0xc,
		.data = (u8*)extdata_homemenu_lowpathdata
	}
};

FS_archive extdata_archive_theme = {
	.id = ARCH_EXTDATA,
	.lowPath = {
		.type = PATH_BINARY,
		.size = 0xc,
		.data = (u8*)extdata_theme_lowpathdata
	}
};

u32 extdata_initialized = 0;

typedef int (*menuent_funcptr)(void);

int menu_savedatadat2sd();
int menu_sd2savedatadat();
int menu_enablethemecache();
int menu_themecache2sd();
int menu_sd2themecache();

char *mainmenu_entries[5] = {
"Copy extdata SaveData.dat to sd",
"Copy SaveData.dat from sd to extdata",
"Enable theme-cache",
"Copy theme cache from extdata to sd",
"Copy theme cache from sd to extdata"};
int mainmenu_totalentries = 5;
menuent_funcptr mainmenu_entryhandlers[5] = {menu_savedatadat2sd, menu_sd2savedatadat, menu_enablethemecache, menu_themecache2sd, menu_sd2themecache};

Result open_extdata()
{
	Result ret=0;
	u32 extdataID_homemenu, extdataID_theme;

	extdataID_homemenu = 0x0000008f;//Hard-coded for USA for now.
	extdataID_theme = 0x000002cd;

	memset(extdata_homemenu_lowpathdata, 0, 0xc);
	memset(extdata_theme_lowpathdata, 0, 0xc);

	extdata_homemenu_lowpathdata[0] = 1;//mediatype, 1=SD
	extdata_homemenu_lowpathdata[1] = extdataID_homemenu;//extdataID-low

	extdata_theme_lowpathdata[0] = 1;//mediatype, 1=SD
	extdata_theme_lowpathdata[1] = extdataID_theme;//extdataID-low

	ret = FSUSER_OpenArchive(NULL, &extdata_archive_homemenu);
	if(ret!=0)
	{
		printf("Failed to open homemenu extdata with extdataID=0x%08x, retval: 0x%08x\n", (unsigned int)extdataID_homemenu, (unsigned int)ret);
		return ret;
	}
	extdata_initialized |= 0x1;

	ret = FSUSER_OpenArchive(NULL, &extdata_archive_theme);
	if(ret!=0)
	{
		printf("Failed to open theme extdata with extdataID=0x%08x, retval: 0x%08x\n", (unsigned int)extdataID_theme, (unsigned int)ret);
		return ret;
	}
	extdata_initialized |= 0x2;

	return 0;
}

void close_extdata()
{
	if(extdata_initialized & 0x1)
	{
		FSUSER_CloseArchive(NULL, &extdata_archive_homemenu);
	}

	if(extdata_initialized & 0x2)
	{
		FSUSER_CloseArchive(NULL, &extdata_archive_theme);
	}
}

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
	printf("savedatadat2sd N/A\n");
	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(2000000000LL);
	return -1;
}

int menu_sd2savedatadat()
{
	printf("sd2savedatadat N/A\n");
	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(2000000000LL);
	return -1;
}

int menu_enablethemecache()
{	
	printf("enablethemecache N/A\n");
	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(2000000000LL);
	return -1;
}

int menu_themecache2sd()
{
	printf("themecache2sd N/A\n");
	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(2000000000LL);
	return -1;
}

int menu_sd2themecache()
{
	printf("sd2themecache N/A\n");
	gfxFlushBuffers();
	gfxSwapBuffers();
	svcSleepThread(2000000000LL);
	return -1;
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
	}

	return -2;
}

int main()
{
	Result extdata_openretval = 0;
	Result ret=0;

	// Initialize services
	gfxInit();

	consoleInit(GFX_BOTTOM, NULL);

	printf("3ds_homemenu_extdatatool\n");

	printf("Opening extdata archives...\n");
	extdata_openretval = open_extdata();
	if(extdata_openretval==0)
	{
		printf("Finished opening extdata.\n");

		consoleClear();
		ret = (Result)handle_menus();
	}

	if(ret>=0)
	{
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

	close_extdata();

	// Exit services
	gfxExit();
	return 0;
}

