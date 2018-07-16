#include "fuse.h"

#include <memory>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <limits>
#include <functional>

#include <errno.h>

#include "../darling-dmg/src/be.h"
#include "../darling-dmg/src/HFSVolume.h"
#include "../darling-dmg/src/AppleDisk.h"
#include "../darling-dmg/src/GPTDisk.h"
#include "../darling-dmg/src/DMGDisk.h"
#include "RawDisk.h"
#include "../darling-dmg/src/FileReader.h"
#include "../darling-dmg/src/CachedReader.h"
#include "../darling-dmg/src/exceptions.h"
#include "../darling-dmg/src/HFSHighLevelVolume.h"
//#include "SparsefsReader.h"
#include <random>
#include <stdint.h>
#include <inttypes.h>
#include "../apfs-fuse/ApfsLib/ApfsContainer.h"
#include "FuseVolume.h"
#include "HfsFuseVolume.h"
#include "ApfsFuseOps.h"
#include "ApfsFuseVolume.h"
#include "SparsebundleReader.h"
#include "Test.h"

std::unique_ptr<FuseVolume> g_volumePtr; // must be global. Will be called bu fuse methods.

#include "../apfs-fuse/ApfsLib/ApfsContainer.h"


int m_debuglog = 0;
int g_break = 0;

#ifdef DARLING_DMG_DEBUG
int myFiller (void *buf, const char *name, const struct stat *stbuf, off_t off)
{
	printf("myFiller : name=%s\n", name);
	return 0;
}
const char* debugFile = "no file yet";
#endif


int main(int argc, const char** argv)
{
	try
	{
		struct fuse_operations ops;
		struct fuse_args args = FUSE_ARGS_INIT(0, NULL);
	
		if (argc < 3)
		{
			showHelp(argv[0]);
			return 1;
		}
//TestReader(FileReader::getReaderForPath("/JiefLand/5.Devel/Syno/Fuse/ImagesTest/SbNoMapHfsEnc.sparsebundle"), "/JiefLand/5.Devel/Syno/Fuse/ImagesTest/SbNoMapHfsEnc.sparsebundle.dump");

		char* path = realpath(argv[1], NULL);
		if (!path)
			throw file_not_found_error(std::string("File '")+argv[1]+"' is not found");
		openDisk(path);

#ifdef DEBUG
//TestTmp(g_volumePtr);
system((std::string("umount ")+argv[2]).c_str());
#endif
	
		memset(&ops, 0, sizeof(ops));
	
		ops.getattr = fuse_getattr;
		ops.open = fuse_open;
		ops.read = fuse_read;
		ops.release = fuse_release;
		//ops.opendir = fuse_opendir;
		ops.readdir = fuse_readdir;
		//ops.releasedir = fuse_releasedir;
		ops.readlink = fuse_readlink;
		ops.getxattr = fuse_getxattr;
		ops.listxattr = fuse_listxattr;
	
		for (int i = 0; i < argc; i++)
		{
			if (i == 1) {
				;
			}else
				fuse_opt_add_arg(&args, argv[i]);
		}
		fuse_opt_add_arg(&args, "-oro");
		fuse_opt_add_arg(&args, "-s");
		fuse_opt_add_arg(&args, "-o");
		fuse_opt_add_arg(&args, "use_ino");
		fuse_opt_add_arg(&args, "-f");
		#ifdef DARLING_DMG_DEBUG
			fuse_opt_add_arg(&args, "-o");
			fuse_opt_add_arg(&args, "allow_other");
		#endif

		std::cerr << "Everything looks OK, giving up to fuse\n";

#ifdef BEFORE_MOUNT_EXTRA // Darling only
		BEFORE_MOUNT_EXTRA;
#endif

		free(path);
		return fuse_main(args.argc, args.argv, &ops, 0);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << std::endl;

		std::cerr << "Possible reasons:\n"
			"1) The file is corrupt.\n"
			"2) The file is not really a DMG file, although it resembles one.\n"
			"3) There is a bug in darling-dmg.\n";

		return 1;
	}
}


void showHelp(const char* argv0)
{
	std::cerr << "Usage: " << argv0 << " <file> <mount-point> [fuse args]\n\n";
	std::cerr << ".DMG files and raw disk images can be mounted.\n";
	std::cerr << argv0 << " automatically selects the first HFS+/HFSX partition.\n";
}

void openDisk(const char* path)
{
	std::shared_ptr<Reader> g_fileReader;
	std::shared_ptr<HFSVolume> volume;
	std::unique_ptr<PartitionedDisk> g_partitionsPtr;

    g_fileReader = FileReader::getReaderForPath(path);

	if (DMGDisk::isDMG(g_fileReader)) {
		std::cerr << "Detected an image from device." << std::endl;
		g_partitionsPtr.reset(new DMGDisk(g_fileReader));
	}else if (GPTDisk::isGPTDisk(g_fileReader)) {
		std::cerr << "Detected a GPT partionned disk." << std::endl;
		g_partitionsPtr.reset(new GPTDisk(g_fileReader));
	}else if (AppleDisk::isAppleDisk(g_fileReader)) {
		std::cerr << "Detected a Apple map partionned disk." << std::endl;
		g_partitionsPtr.reset(new AppleDisk(g_fileReader));
	}else{
		std::cerr << "Fallback to raw disk." << std::endl;
		g_partitionsPtr.reset(new RawDisk(g_fileReader));
	}

	std::shared_ptr<Reader> partitionsReaderPtr;
	const std::vector<PartitionedDisk::Partition>& parts = g_partitionsPtr->partitions();

	for (int i = 0; size_t(i) < parts.size(); i++)
	{
		partitionsReaderPtr = g_partitionsPtr->readerForPartition(i);
		if (HFSVolume::isHFSPlus(partitionsReaderPtr))
		{
			std::cerr << "Using HFS partition #" << i << " of type " << parts[i].type << std::endl;
//			extern std::unique_ptr<HFSHighLevelVolume> g_volume; // ugly because I don't want to modify darling main-fuse.cpp too much. I hope we'll work together for a better integration.
			std::unique_ptr<HFSHighLevelVolume> volume = std::unique_ptr<HFSHighLevelVolume>(new HFSHighLevelVolume(std::make_shared<HFSVolume>(partitionsReaderPtr)));
			g_volumePtr = std::unique_ptr<HfsFuseVolume>(new HfsFuseVolume(std::move(volume)));
			return;
		}
		else if (ApfsContainer::isAPFS(partitionsReaderPtr)) {
			std::cerr << "Using APFS partition #" << i << " of type " << parts[i].type << std::endl;
			ApfsContainer* apfsContainer = new ApfsContainer(partitionsReaderPtr);
			apfsContainer->Init();
			unsigned int volume_id = 0;
			ApfsVolume* volume = apfsContainer->GetVolume(volume_id, "foo"); // TODO password
			g_volumePtr = std::unique_ptr<ApfsFuseVolume>(new ApfsFuseVolume(std::unique_ptr<ApfsVolume>(volume)));
			return;
		}
	}
	throw function_not_implemented_error("Unsupported file format");
}



#ifdef DEBUG
static void trace_fuse_op(const char* path, const char* format, ...) __attribute__((__format__(printf, 2, 3)));
static void trace_fuse_op(const char* path, const char* format, ...)
{
	//if ( strcmp("/", path) == 0 ) return;

    va_list valist;
	va_start(valist, format);
	
	char buf[2000];
	vsnprintf(buf, sizeof(buf)-1, format, valist);
	fprintf(stderr, "%s", buf);
}
#else
	#define trace_fuse_op(path, format, ...)
#endif


int fuse_getattr(const char* path, struct stat* stat)
{
	trace_fuse_op(path, "fuse_stat(%s)\n", path);
	return g_volumePtr->getattr(path, stat);
}

int fuse_readlink(const char* path, char* buf, size_t size)
{
	trace_fuse_op(path, "fuse_readlink(%s, %" PRIxPTR ", %zd)\n", path, (intptr_t)buf, size);
	return g_volumePtr->readlink(path, buf, size);
}

int fuse_open(const char* path, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_open(%s)\n", path);
	return g_volumePtr->open(path, info);
}

int fuse_read(const char* path, char* buf, size_t bytes, off_t offset, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_read(%s, size=%zd, offset=%lld)\n", path, bytes, offset);
	return g_volumePtr->read(path, buf, bytes, offset, info);
}

int fuse_release(const char* path, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_release(%s)\n", path);
	return g_volumePtr->release(path, info);
}

int fuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_readdir path '%s'\n", path);
	return g_volumePtr->readdir(path, buf, filler, offset, info);
}

#if defined(__APPLE__) && !defined(DARLING)
int fuse_getxattr(const char* path, const char* name, char* value, size_t vlen, uint32_t position)
#else
int fuse_getxattr(const char* path, const char* name, char* value, size_t vlen)
#endif
{
	#ifdef __APPLE__
		if (position > 0) return -ENOSYS; // it's not supported... yet. I think it doesn't happen anymore since osx use less ressource fork
	#endif

	trace_fuse_op(path, "fuse_getxattr(%s, %s, % " PRIdPTR " , %zd)\n", path, name, (intptr_t)value, vlen);


	#if defined(__APPLE__) && !defined(DARLING)
		return g_volumePtr->getxattr(path, name, value, vlen, position);
	#else
		return g_volumePtr->getxattr(path, name, value, vlen);
	#endif
}

int fuse_listxattr(const char* path, char* buffer, size_t size)
{
	trace_fuse_op(path, "fuse_listxattr(%s, %" PRIdPTR ", %zd)\n", path, (intptr_t)buffer, size);
	return g_volumePtr->listxattr(path, buffer, size);
}
