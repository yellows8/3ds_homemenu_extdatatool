#include <string.h>
#include <stdio.h>
#include <3ds.h>

int main()
{
	// Initialize services
	gfxInit();

	consoleInit(GFX_BOTTOM, NULL);

	printf("3ds_homemenu_extdatatool\n");

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

	// Exit services
	gfxExit();
	return 0;
}

