#ifndef APFSFUSEOPS_H
#define APFSFUSEOPS_H

//#include <sys/types.h>
#include "apfs-fuse/ApfsLib/ApfsVolume.h"
namespace apfs
{

extern ApfsVolume *g_volume;

int apfs_stat(const char* path, struct FUSE_STAT* stat);
int apfs_readlink(const char* path, char* buf, size_t size);
int apfs_open(const char* path, struct fuse_file_info* info);
int apfs_read(const char* path, char* buf, size_t bytes, off_t offset, struct fuse_file_info* info);
int apfs_release(const char* path, struct fuse_file_info* info);
int apfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* info);
#if defined(__APPLE__) && !defined(DARLING)
int apfs_getxattr(const char* path, const char* name, char* value, size_t vlen, uint32_t position);
#else
int apfs_getxattr(const char* path, const char* name, char* value, size_t vlen);
#endif
int apfs_listxattr(const char* path, char* buffer, size_t size);

}

#endif
