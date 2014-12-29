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

int main()
{
	Result extdata_openretval = 0;

	// Initialize services
	gfxInit();

	consoleInit(GFX_BOTTOM, NULL);

	printf("3ds_homemenu_extdatatool\n");

	printf("Opening extdata archives...\n");
	extdata_openretval = open_extdata();
	if(extdata_openretval==0)printf("Finished opening extdata.\n");

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

	close_extdata();

	// Exit services
	gfxExit();
	return 0;
}

