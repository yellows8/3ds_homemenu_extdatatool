typedef enum {
	HomeMenu_Extdata,
	Theme_Extdata,
	TotalExtdataArchives
} ExtdataArchive;

Result open_extdata();
void close_extdata();
Result archive_getfilesize(int archive, char *path, u32 *outsize);
Result archive_readfile(int archive, char *path, u8 *buffer, u32 size);
Result archive_writefile(int archive, char *path, u8 *buffer, u32 size);

