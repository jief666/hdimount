#ifndef DARLINGFUSEVOLUME_H
#define DARLINGFUSEVOLUME_H

#include "FuseVolume.h"
//#include "darling-dmg/src/HFSHighLevelVolume.h"

#include <sys/types.h>
#include <memory>

#include <fuse.h>

class HFSHighLevelVolume;

class HfsFuseVolume : public FuseVolume
{
public:
//	Volume(std::shared_ptr<Reader> reader);
	HfsFuseVolume(std::unique_ptr<HFSHighLevelVolume> volume) : hfsHighLevelVolume(std::move(volume)) {};
	virtual ~HfsFuseVolume() {};

    virtual int fsType() { return HFS_FUSE_VOLUME; };

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
	std::unique_ptr<HFSHighLevelVolume> hfsHighLevelVolume;
};

#endif
