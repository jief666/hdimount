#ifndef FUSE_H_pazoiefdhqsiodfvnbcdviuoretyo
#define FUSE_H_pazoiefdhqsiodfvnbcdviuoretyo

#ifndef FUSE_USE_VERSION
#  define FUSE_USE_VERSION 26
#endif

#include <fuse.h>

static void showHelp(const char* argv0);
static void openDisk(const char* path);

int fuse_getattr(const char* path, struct stat* stat);
int fuse_readlink(const char* path, char* buf, size_t size);
int fuse_open(const char* path, struct fuse_file_info* info);
int fuse_read(const char* path, char* buf, size_t bytes, off_t offset, struct fuse_file_info* info);
int fuse_release(const char* path, struct fuse_file_info* info);
int fuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* info);
#if defined(__APPLE__) && !defined(DARLING)
  int fuse_getxattr(const char* path, const char* name, char* value, size_t vlen, uint32_t position);
#else
  int fuse_getxattr(const char* path, const char* name, char* value, size_t vlen);
#endif
int fuse_listxattr(const char* path, char* buffer, size_t size);

#endif

