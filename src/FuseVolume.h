#ifndef FUSEVOLUME_H
#define FUSEVOLUME_H

#include <string>

#include <sys/types.h>
#include <fuse.h>

#define HFS_FUSE_VOLUME 1
#define APFS_FUSE_VOLUME 2

class FuseVolume
{
public:
//	Volume(std::shared_ptr<Reader> reader);
	FuseVolume() {};
	virtual ~FuseVolume() {};
 
    virtual int fsType() = 0;
	
	virtual int getattr(const char* path, struct FUSE_STAT* stat) = 0;
	virtual int readlink(const char* path, char* buf, size_t size) = 0;
	virtual int open(const char* path, struct fuse_file_info* info) = 0;
	virtual int read(const char* path, char* buf, size_t bytes, off_t offset, struct fuse_file_info* info) = 0;
	virtual int release(const char* path, struct fuse_file_info* info) = 0;
	virtual int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* info) = 0;
#if defined(__APPLE__) && !defined(DARLING)
	virtual int getxattr(const char* path, const char* name, char* value, size_t vlen, uint32_t position) = 0;
#else
	virtual int getxattr(const char* path, const char* name, char* value, size_t vlen) = 0;
#endif
	virtual int listxattr(const char* path, char* buffer, size_t size) = 0;

};

#endif
