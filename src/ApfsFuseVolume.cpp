#include "ApfsFuseVolume.h"
#include "ApfsFuseOps.h"

#include "../apfs-fuse/ApfsLib/ApfsContainer.h"
#include "../apfs-fuse/ApfsLib/ApfsVolume.h"
#include "../apfs-fuse/ApfsLib/ApfsDir.h"
#include "../apfs-fuse/ApfsLib/Decmpfs.h"

#include "Utils.h"

#include <inttypes.h>
#include <limits.h>
#include <string.h> // for memset
#include <strings.h>
#include <assert.h>

#include <iostream>


#ifdef DEBUG
static void trace_fuse_op(const char* path, const char* format, ...) __attribute__((__format__(printf, 2, 3)));
static void trace_fuse_op(const char* path, const char* format, ...)
{
	if ( strcmp("/", path) == 0 ) return;

    if (!(g_debug & Dbg_Info)) return;

    va_list valist;
	va_start(valist, format);

	char buf[2000];
	vsnprintf(buf, sizeof(buf)-1, format, valist);
	fprintf(stderr, "%s", buf);
}
#else
	#define trace_fuse_op(path, format, ...)
#endif


struct Directory
{
	Directory() {}
	~Directory() {}

	std::vector<char> dirbuf;
};

struct File
{
	File() {}
	~File() {}

	bool IsCompressed() const { return ino.ino.isCompressed(); }

	ApfsDir::Inode ino;
	std::vector<uint8_t> decomp_data;
};


int ApfsFuseVolume::apfs_getInodeId(uint64_t* id, const char* path)
{
    ApfsDir dir(*apfsVolume);
    ApfsDir::Name res;
    *id = 1;
    bool rc;

    path ++;
    if ( *path == 0 ) return 0;
	
    const char* pathend = index(path, '/');
    while (pathend != NULL) {
        rc = dir.LookupName(res, *id, std::string(path, pathend-path).c_str());
        if ( !rc ) return -ENOENT;
        *id = res.inode_id;
        path = pathend + 1;
        pathend = index(path, '/');
    }
    rc = dir.LookupName(res, *id, path);
    *id = res.inode_id;
    if ( !rc ) return -ENOENT;
    return 0;
}

bool ApfsFuseVolume::apfs_stat_internal(uint64_t ino, struct stat* stPtr)
{
	ApfsDir dir(*apfsVolume);
	ApfsDir::Inode rec;
	bool rc = false;
	struct stat& st = *stPtr;

	memset(&st, 0, sizeof(st));

	if (ino == 1)
	{
		st.st_ino = ino;
		st.st_mode = S_IFDIR | 0755;
		st.st_nlink = 2;
		return true;
	}

	rc = dir.GetInode(rec, ino);

	if (!rc)
	{
		return false;
	}
	constexpr uint64_t div_nsec = 1000000000;

	st.st_dev = 0; // st_dev?
	st.st_rdev = 0;	// st_rdev?
	st.st_ino = ino;
	st.st_mode = rec.ino.mode;
	st.st_nlink = rec.ino.nchildren;
	st.st_uid = rec.ino.uid;
	st.st_gid = rec.ino.gid;
	st.st_blksize = apfsVolume->getContainer().GetBlocksize();
	assert(st.st_blksize == 4096); // TODO : one day, we will have to support bigger block ?
	st.st_blocks = rec.sizes.alloced_size / 512; // number of 512 bytes blocks

	if (S_ISREG(st.st_mode))
	{
		if (rec.ino.isCompressed())
		{
			std::vector<uint8_t> data;
			rc = dir.GetAttribute(data, ino, "com.apple.decmpfs");
			if (rc)
			{
				const CompressionHeader *decmpfs = reinterpret_cast<const CompressionHeader *>(data.data());

				if (IsDecompAlgoSupported(decmpfs->algo))
				{
					st.st_size = decmpfs->size;
					if (g_debug & Dbg_Cmpfs)
						std::cout << "Compressed size " << decmpfs->size << " bytes." << std::endl;
				}
				else if (IsDecompAlgoInRsrc(decmpfs->algo))
				{
					rc = dir.GetAttribute(data, ino, "com.apple.ResourceFork");

					if (!rc)
						st.st_size = 0;
					else
						// Compressed size
						st.st_size = data.size();
				}
				else
				{
					st.st_size = data.size();
					std::cerr << "Unknown compression algorithm " << decmpfs->algo << std::endl;
					if (!g_lax)
						return false;
				}
			}
			else
			{
				std::cerr << "Flag 0x20 set but no com.apple.decmpfs attribute!!!" << std::endl;
				if (!g_lax)
					return false;
			}
		}
		else
		{
			st.st_size = rec.sizes.size;
		}
	}
	else if (S_ISDIR(st.st_mode))
	{
		st.st_size = rec.ino.nchildren;
	}
	else if (S_ISLNK(st.st_mode))
	{
		st.st_size = dir.GetAttributeSize(ino, "com.apple.fs.symlink") -1;
	}

#ifdef __linux__
	// What about this?
	// st.st_birthtime.tv_sec = rec.ino.birthtime / div_nsec;
	// st.st_birthtime.tv_nsec = rec.ino.birthtime % div_nsec;

	st.st_mtim.tv_sec = rec.ino.mtime / div_nsec;
	st.st_mtim.tv_nsec = rec.ino.mtime % div_nsec;
	st.st_ctim.tv_sec = rec.ino.ctime / div_nsec;
	st.st_ctim.tv_nsec = rec.ino.ctime % div_nsec;
	st.st_atim.tv_sec = rec.ino.atime / div_nsec;
	st.st_atim.tv_nsec = rec.ino.atime % div_nsec;
#endif
#ifdef __APPLE__
	st.st_birthtimespec.tv_sec = rec.ino.birthtime / div_nsec;
	st.st_birthtimespec.tv_nsec = rec.ino.birthtime % div_nsec;
	st.st_mtimespec.tv_sec = rec.ino.mtime / div_nsec;
	st.st_mtimespec.tv_nsec = rec.ino.mtime % div_nsec;
	st.st_ctimespec.tv_sec = rec.ino.ctime / div_nsec;
	st.st_ctimespec.tv_nsec = rec.ino.ctime % div_nsec;
	st.st_atimespec.tv_sec = rec.ino.ctime / div_nsec;
	st.st_atimespec.tv_nsec = rec.ino.ctime % div_nsec;
#endif
	return true;
}

int ApfsFuseVolume::getattr(const char* path, struct stat* stPtr)
{
	trace_fuse_op(path, "apfs_stat(%s)\n", path);

    uint64_t inode_id;
//int rc2 = apfs_getInode(&res, "/folder1/Folder1file1.txt");
    int rc2 = apfs_getInodeId(&inode_id, path);
    if ( rc2 != 0 ) {
//printf("apfs_stat(%s) return rc2 %d\n", path, rc2);
        return rc2;
    }

//    if (g_debug & Dbg_Info)
//        std::cout << "apfs_stat: ino=" << std::hex << inode_id << " => ";

	bool rc = apfs_stat_internal(inode_id, stPtr);

//    if (g_debug & Dbg_Info)
//        std::cout << (rc ? "OK" : "FAIL") << std::endl;

	if (rc) {
//printf("apfs_stat(%s) return 0\n", path);
		return 0;
	}else{
//printf("apfs_stat(%s) return -ENOENT\n", path);
		return -ENOENT;
    }
}
#define min(x, y) ((x) < (y) ? (x) : (y))

int ApfsFuseVolume::readlink(const char* path, char* buf, size_t size)
{
	trace_fuse_op(path, "apfs_readlink(%s, %" PRIxPTR ", %zd)\n", path, (intptr_t)buf, size);

    uint64_t inode_id;
    int err = apfs_getInodeId(&inode_id, path);
    if ( err != 0 ) return err;

	ApfsDir dir(*apfsVolume);
	bool rc = false;
	std::vector<uint8_t> data;

	rc = dir.GetAttribute(data, inode_id, "com.apple.fs.symlink");
	if (!rc)
		return -ENOENT;
    memcpy(buf, data.data(), min(size, data.size()));
    return 0;
}

int ApfsFuseVolume::open(const char* path, struct fuse_file_info* fi)
{
	trace_fuse_op(path, "apfs_open(%s)\n", path);

    uint64_t inode_id;
    int err = apfs_getInodeId(&inode_id, path);
    if ( err != 0 ) return err;

	if (g_debug & Dbg_Info)
		std::cout << std::hex << "apfs_open: " << inode_id << std::endl;

	bool rc;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;
	else
	{
		File *f = new File();
		ApfsDir dir(*apfsVolume);

		rc = dir.GetInode(f->ino, inode_id);

		if (!rc)
		{
			std::cerr << "Couldn't get inode " << inode_id << std::endl;
			delete f;
            return -ENOENT;
		}

		if (f->IsCompressed())
		{
			std::vector<uint8_t> attr;

			rc = dir.GetAttribute(attr, f->ino.id, "com.apple.decmpfs");

			if (!rc)
			{
				std::cerr << "Couldn't get attribute com.apple.decmpfs for " << f->ino.id << std::endl;
				delete f;
                return -ENOENT;
			}

			if (g_debug & Dbg_Info)
			{
				std::cout << "Inode info: size=" << f->ino.sizes.size
				          << ", size_on_disk=" << f->ino.sizes.alloced_size << std::endl;
			}
			rc = DecompressFile(dir, f->ino.id, f->decomp_data, attr);
			// In strict mode, do not return uncompressed data.
			if (!rc && !g_lax)
			{
				delete f;
				return -EIO;
			}
		}

		fi->fh = reinterpret_cast<uint64_t>(f);
        return 0;
	}
}

int ApfsFuseVolume::read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi)
{
	trace_fuse_op(path, "apfs_read(%s, size=%zd, offset=%lld)\n", path, size, offset);
	
    ApfsDir dir(*apfsVolume);
    File *file = reinterpret_cast<File *>(fi->fh);

	if ( offset < 0 ) {
		return 0;
	}
	if ( size == 0 ) { // because we do minus-1 later, we can't have size=0;
		return 0;
	}
if ( offset==0xff000 ) {
printf("break\n");
}
    if (!file->IsCompressed())
    {
        off_t offsetInBlock = offset;
        off_t size2 = size;

        offsetInBlock &= 0xFFF; // if off doesn't start on a block boundary, substract the distance between begin of block and offset
        size2 += offset & 0xFFF; // if off doesn't start on a block boundary, add the distance between begin of block and offset
        size2 = ( (size2-1) | 0xFFF) + 1; // make size2 a multiple of block size by extending it to the next block boundary.
		
        char* bufTmp[size2];
        bool rc = dir.ReadFile(bufTmp, file->ino.ino.private_id, offset-offsetInBlock, size2);
        memcpy(buf, bufTmp+offsetInBlock, size);
        if ( !rc ) return -EIO;
		assert(size < INT_MAX);
		return (int)size;
    }
    else
    {
        if (uoff_t(offset) >= file->decomp_data.size()) {
            return 0;
        }
        if (offset + size > file->decomp_data.size())
            size = file->decomp_data.size() - offset;
        memcpy(buf, file->decomp_data.data()+offset, size);
		assert(size < INT_MAX);
		return (int)size;
    }
}

int ApfsFuseVolume::release(const char* path, struct fuse_file_info* fi)
{
	trace_fuse_op(path, "apfs_release(%s)\n", path);
	
    if (g_debug & Dbg_Info)
        std::cout << std::hex << "apfs_release " << path << std::endl;

    File *file = reinterpret_cast<File *>(fi->fh);
    delete file;

    return 0;
}

int ApfsFuseVolume::readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* info)
{
	trace_fuse_op(path, "apfs_readdir path '%s'\n", path);
	if (offset != 0) {
		fprintf(stderr, "Don't know what offset is yet\n");
		return 0;
	}
	(void)info;
	
    uint64_t inode_id;
    int err = apfs_getInodeId(&inode_id, path);
    if ( err != 0 ) return err;

    ApfsDir dir(*apfsVolume);
    std::vector<ApfsDir::Name> dir_list;
    std::vector<char> dirbuf;
    bool rc;

    if (g_debug & Dbg_Info)
        std::cout << "apfs_readdir: " << std::hex << inode_id << std::endl;

    dirbuf.reserve(0x1000);

    rc = dir.ListDirectory(dir_list, inode_id);
    if (!rc)
    {
        return -EIO;
    }

    for (size_t k = 0; k < dir_list.size(); k++)
    {
        if (filler(buf, dir_list[k].name.c_str(), NULL, 0) != 0)
            return -ENOMEM;

    }
    return 0;
}

#if defined(__APPLE__) && !defined(DARLING)
int ApfsFuseVolume::getxattr(const char* path, const char* name, char* value, size_t size, uint32_t position)
#else
int ApfsFuseVolume::getxattr(const char* path, const char* name, char* value, size_t size)
#endif
{
	trace_fuse_op(path, "apfs_getxattr(%s, %s, % " PRIdPTR " , %zd)\n", path, name, (intptr_t)value, size);

	#ifdef __APPLE__
		if ( position != 0 ) {
			printf("apfs_getxattr_mac path=%s, name=%s, value=%llx, size=%zd, position=%d\n", path, name, (uint64_t)value, size, position);
			fprintf(stderr, "WARINNG : position unsupported yet");
			return -EPERM;
		}
	#endif
    uint64_t inode_id;
    int err = apfs_getInodeId(&inode_id, path);
    if ( err != 0 ) return err;

	ApfsDir dir(*apfsVolume);
	bool rc = false;
	std::vector<uint8_t> data;

//    if (g_debug & Dbg_Info)
//        if ( strcmp(path, "/") != 0 ) std::cout << "apfs_getxattr: " << std::hex << inode_id << " " << name << " => ";

	rc = dir.GetAttribute(data, inode_id, name);

//    if (g_debug & Dbg_Info)
//        if ( strcmp(path, "/") != 0 ) std::cout << (rc ? "OK" : "FAIL") << std::endl;

	if (!rc) return -ENOENT;
    if (!value) {
        return (int)data.size();
    }
    if ( size < data.size() )
	{
        memcpy(value, data.data(), size);
        return (int)size;
//        return -ERANGE; // we are supposed to return -ERANGE, but High Sierra doesn't
	}
    memcpy(value, data.data(), data.size());
    return (int)data.size();
}

int ApfsFuseVolume::listxattr(const char* path, char* buffer, size_t size)
{
	trace_fuse_op(path, "apfs_listxattr(%s, %" PRIdPTR ", %zd)\n", path, (intptr_t)buffer, size);

    uint64_t inode_id;
    int err = apfs_getInodeId(&inode_id, path);
    if ( err != 0 ) return err;

	ApfsDir dir(*apfsVolume);
	bool rc = false;
	std::vector<std::string> names;
	std::string reply;

	rc = dir.ListAttributes(names, inode_id);
    if ( !rc ) return 0;
	
//    if (g_debug & Dbg_Info)
//        std::cout << "apfs_listxattr:" << std::endl;

    ApfsDir::Inode rec;
    bool rc2 = dir.GetInode(rec, inode_id);
    size_t len = 0;
    for (size_t k = 0; k < names.size(); k++) {
        if ( names[k] != "com.apple.decmpfs"  &&  names[k] != "com.apple.fs.symlink" ) {
            if ( !rc2 || !rec.isCompressed() || names[k] != "com.apple.ResourceFork" ) {
                len += names[k].length()+1;
            }
        }
    }
	
    if (!buffer)
    {
        return (int)len;
    }
    if ( size < len ) {
        return -ERANGE;
    }

    for (size_t k = 0; k < names.size(); k++)
    {
        if ( names[k] != "com.apple.decmpfs"  &&  names[k] != "com.apple.fs.symlink" ) {
            if ( !rc2 || !rec.isCompressed() || names[k] != "com.apple.ResourceFork" ) {
                if (g_debug & Dbg_Info) {
                    std::cout << names[k] << std::endl;
                }
                memcpy(buffer, names[k].c_str(), names[k].length());
                buffer[names[k].length()] = 0;
                buffer += names[k].length()+1;
            }
        }
    }
    assert(len < INT_MAX);
    return (int)len;
}
