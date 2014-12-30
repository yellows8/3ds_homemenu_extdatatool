#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <3ds.h>

#include "archive.h"

u32 extdata_archives_lowpathdata[TotalExtdataArchives][3];
FS_archive extdata_archives[TotalExtdataArchives];
u32 extdata_initialized = 0;

Result open_extdata()
{
	Result ret=0;
	u32 pos;
	u32 extdataID_homemenu, extdataID_theme;
	u8 region=0;

	ret = initCfgu();
	if(ret!=0)
	{
		printf("initCfgu() failed: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return ret;
	}

	ret = CFGU_SecureInfoGetRegion(&region);
	if(ret!=0)
	{
		printf("CFGU_SecureInfoGetRegion() failed: 0x%08x\n", (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
		return ret;
	}

	exitCfgu();

	if(region==1)//USA
	{
		extdataID_homemenu = 0x0000008f;
		extdataID_theme = 0x000002cd;
	}
	else if(region==2)//EUR
	{
		extdataID_homemenu = 0x00000098;
		extdataID_theme = 0x000002ce;
	}
	else//JPN/elsewhere
	{
		extdataID_homemenu = 0x00000082;
		extdataID_theme = 0x000002cc;
	}

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
		gfxFlushBuffers();
		gfxSwapBuffers();
		return ret;
	}
	extdata_initialized |= 0x1;

	ret = FSUSER_OpenArchive(NULL, &extdata_archives[Theme_Extdata]);
	if(ret!=0)
	{
		printf("Failed to open theme extdata with extdataID=0x%08x, retval: 0x%08x\n", (unsigned int)extdataID_theme, (unsigned int)ret);
		gfxFlushBuffers();
		gfxSwapBuffers();
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

Result archive_readfile(int archive, char *path, u8 *buffer, u32 size)
{
	Result ret=0;
	Handle filehandle=0;
	u32 tmpval=0;
	FILE *f;

	char filepath[256];

	if(archive==-1)
	{
		memset(filepath, 0, 256);
		snprintf(filepath, 255, "sdmc:%s", path);

		f = fopen(filepath, "rb");
		if(f==NULL)return -1;

		tmpval = fread(buffer, 1, size, f);

		fclose(f);

		if(tmpval!=size)return -2;

		return 0;
	}

	ret = FSUSER_OpenFile(NULL, &filehandle, extdata_archives[archive], FS_makePath(PATH_CHAR, path), 1, 0);
	if(ret!=0)return ret;

	ret = FSFILE_Read(filehandle, &tmpval, 0, buffer, size);

	FSFILE_Close(filehandle);

	if(ret==0 && tmpval!=size)ret=-2;

	return ret;
}

Result archive_writefile(int archive, char *path, u8 *buffer, u32 size)
{
	Result ret=0;
	Handle filehandle=0;
	u32 tmpval=0;
	FILE *f;

	char filepath[256];

	if(archive==-1)
	{
		memset(filepath, 0, 256);
		snprintf(filepath, 255, "sdmc:%s", path);

		f = fopen(filepath, "wb");
		if(f==NULL)return -1;

		tmpval = fwrite(buffer, 1, size, f);

		fclose(f);

		if(tmpval!=size)return -2;

		return 0;
	}

	ret = FSUSER_OpenFile(NULL, &filehandle, extdata_archives[archive], FS_makePath(PATH_CHAR, path), 1, 0);
	if(ret!=0)return ret;

	ret = FSFILE_Write(filehandle, &tmpval, 0, buffer, size, FS_WRITE_FLUSH);

	FSFILE_Close(filehandle);

	if(ret==0 && tmpval!=size)ret=-2;

	return ret;
}

