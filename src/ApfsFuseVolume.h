#ifndef APFSFUSEFUSEVOLUME_H
#define APFSFUSEFUSEVOLUME_H

#include "FuseVolume.h"
#include "apfs-fuse/ApfsLib/ApfsVolume.h"

#include <sys/types.h>
#include <fuse.h>

class ApfsFuseVolume : public FuseVolume
{
public:
//	Volume(std::shared_ptr<Reader> reader);
	ApfsFuseVolume(std::unique_ptr<ApfsVolume> volume) : apfsVolume(std::move(volume)) {};
	virtual ~ApfsFuseVolume() {};
	
    virtual int fsType() { return APFS_FUSE_VOLUME; };

	virtual int getattr(const char* path, struct FUSE_STAT* stat);
	virtual int readlink(const char* path, char* buf, size_t size);
	virtual int open(const char* path, struct fuse_file_info* info);
	virtual int read(const char* path, char* buf, size_t bytes, off_t offset, struct fuse_file_info* info);
	virtual int release(const char* path, struct fuse_file_info* info);
	virtual int readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* info);
#if defined(__APPLE__) && !defined(DARLING)
	virtual int getxattr(const char* path, const char* name, char* value, size_t vlen, uint32_t position);
#else
	virtual int getxattr(const char* path, const char* name, char* value, size_t vlen);
#endif
	virtual int listxattr(const char* path, char* buffer, size_t size);

private:
	int apfs_getInodeId(uint64_t* id, const char* path);
	bool apfs_stat_internal(uint64_t ino, struct FUSE_STAT* stPtr);

	std::unique_ptr<ApfsVolume> apfsVolume;

};

#endif
