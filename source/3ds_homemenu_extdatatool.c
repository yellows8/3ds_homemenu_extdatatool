#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <3ds.h>

typedef enum {
	HomeMenu_Extdata,
	Theme_Extdata,
	TotalExtdataArchives
} ExtdataArchive;

u32 extdata_archives_lowpathdata[TotalExtdataArchives][3];
FS_archive extdata_archives[TotalExtdataArchives];
u32 extdata_initialized = 0;

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

char *sdpath_prefix = "/3ds_homemenu_extdatatool/";

Result open_extdata()
{
	Result ret=0;
	u32 pos;
	u32 extdataID_homemenu, extdataID_theme;

	extdataID_homemenu = 0x0000008f;//Hard-coded for USA for now.
	extdataID_theme = 0x000002cd;

	for(pos=0; pos<TotalExtdataArchives; pos++)
	{
		extdata_archives[pos].id = ARCH_EXTDATA;
		extdata_archives[pos].lowPath.type = PATH_BINARY;
		extdata_archives[pos].lowPath.size = 0xc;
		extdata_archives[pos].lowPath.data = (u8*)extdata_archives_lowpathdata[pos];

		memset(extdata_archives_lowpathdata[pos], 0, 0xc);
		extdata_archives_lowpathdata[pos][0] = 1;//mediatype, 1=SD
	}

	extdata_archives_lowpathdata[HomeMenu_Extdata][1] = extdataID_homemenu;//extdataID-low
	extdata_archives_lowpathdata[Theme_Extdata][1] = extdataID_theme;//extdataID-low

	ret = FSUSER_OpenArchive(NULL, &extdata_archives[HomeMenu_Extdata]);
	if(ret!=0)
	{
		printf("Failed to open homemenu extdata with extdataID=0x%08x, retval: 0x%08x\n", (unsigned int)extdataID_homemenu, (unsigned int)ret);
		return ret;
	}
	extdata_initialized |= 0x1;

	ret = FSUSER_OpenArchive(NULL, &extdata_archives[Theme_Extdata]);
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
	u32 pos;

	for(pos=0; pos<TotalExtdataArchives; pos++)
	{
		if(extdata_initialized & (1<<pos))FSUSER_CloseArchive(NULL, &extdata_archives[pos]);
	}
}

Result archive_getfilesize(int archive, char *path, u32 *outsize)
{
	Result ret=0;
	struct stat filestats;
	u64 tmp64=0;
	Handle filehandle=0;

	if(archive==-1)
	{
		if(stat(path, &filestats)==-1)return -1;

		*outsize = filestats.st_size;

		return 0;
	}

	ret = FSUSER_OpenFile(NULL, &filehandle, extdata_archives[archive], FS_makePath(PATH_CHAR, path), 1, 0);
	if(ret!=0)return ret;

	ret = FSFILE_GetSize(filehandle, &tmp64);
	if(ret==0)*outsize = (u32)tmp64;

	FSFILE_Close(filehandle);

	return ret;
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
	Result ret=0;
	u32 size=0;

	ret = archive_getfilesize(HomeMenu_Extdata, "/SaveData.dat", &size);
	printf("ret=0x%08x size=0x%08x\n", (unsigned int)ret, (unsigned int)size);

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
		handle_menus();
	}

	if(extdata_openretval<0)
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

	close_extdata();

	// Exit services
	gfxExit();
	return 0;
}

