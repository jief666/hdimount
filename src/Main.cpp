#ifndef CrossPlateform_h
#error Include CrossPlatform.h from command line
#endif

#include "Main.h"

#include <fuse.h>

#include "darling-dmg/src/be.h"
#include "darling-dmg/src/HFSVolume.h"
#include "darling-dmg/src/AppleDisk.h"
#include "darling-dmg/src/GPTDisk.h"
#include "darling-dmg/src/DMGDisk.h"
#include "RawDisk.h"
#include "darling-dmg/src/FileReader.h"
#include "darling-dmg/src/CachedReader.h"
#include "darling-dmg/src/exceptions.h"
#include "darling-dmg/src/HFSHighLevelVolume.h"
#include "darling-dmg/src/EncryptReader.h"
#include "FuseVolume.h"
#include "HfsFuseVolume.h"
#include "SparsebundleReader.h"
#include "Utils.h"
#include "Version.h"

#include <stdint.h>
#include <inttypes.h>
#include <random>

//namespace xapfs {
//#include "apfs-fuse/ApfsLib/Endian.h"
//}
//using namespace ::xapfs;
//#include "apfs-fuse/ApfsLib/ApfsContainer.h"
//#include "ApfsFuseOps.h"
//#include "ApfsFuseVolume.h"
// apfs bridge
//#include "Device.h"
#include "OpenDisk.h"



#include <errno.h>
#ifndef _MSC_VER
#include <pwd.h> // for getpass()
#endif
#include <unistd.h> // for getpass()
#ifdef __APPLE__
#include <sys/param.h> // statfs
#include <sys/mount.h> // statfs
#endif

#include <memory>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <limits>
#include <functional>

#include "Utils.h"

std::unique_ptr<FuseVolume> g_volumePtr = nullptr; // must be global. Will be called by fuse methods.



//int m_debuglog = 0;
//int g_break = 0;


#include "HfsFuseVolume.h"




void showHelp(const char* argv0)
{
	printf("The ultimate hard drive and hard drives images mounter. Version %s\n", HDIMOUNT_VERSION);
	printf("Usage: %s [fuse args] <file or physical device> <mount-point>\n", argv0);
	printf("    First HFS or APFS partition will be mounted. If encrypted, a password will be asked in the console\n");
}


char* diskImagepath = NULL;
char* mountPoint = NULL;

static int fuse_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
	(void)data;
	(void)outargs;
    switch (key) {
        case FUSE_OPT_KEY_NONOPT: {
//printf("%s\n", arg);
            if ( !diskImagepath ) {
                diskImagepath = strdup(arg);
                return 0; // do not add to fuse_main args
            }
            if ( !mountPoint ) {
                mountPoint = strdup(arg);
                return 1; // fuse_main args
            }
            fprintf(stderr, "Invalid option '%s'\n", arg);
            return -1;
        }
    }
    return 1;
}

int main(int argc, char** argv)
{
	try
	{
        if (argc < 3)
        {
            showHelp(argv[0]);
            return 1;
        }

		struct fuse_operations ops;
		struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
        struct fuse_opt sparsebundle_options[] = {
            { 0, 0, 0 } // End of options
        };
        if ( fuse_opt_parse(&args, NULL, sparsebundle_options, fuse_opt_proc) == -1 ) {
            showHelp(argv[0]);
            return 1;
        }
        if ( !diskImagepath ) {
            fprintf(stderr, "No disk image specified\n");
            showHelp(argv[0]);
            return 1;
        }
        if ( !mountPoint ) {
            fprintf(stderr, "No mount point specified\n");
            showHelp(argv[0]);
            return 1;
        }

        {
            char* diskImageRealpath = realpath(diskImagepath, NULL);
			if (!diskImageRealpath)
				//throw file_not_found_error(std::string("File '")+diskImagepath+"' is not found");
				diskImageRealpath = diskImagepath;
            
            FuseVolume* fv;
#ifdef DEBUG
            fv = openDisk(diskImageRealpath, "foo");
#else
            char* thePassword = getpass("Password: ");
            fv = openDisk(diskImageRealpath, thePassword);
            for ( size_t i = 0 ; i < strlen(thePassword) ; i++ )
                thePassword[i] = ' ';
#endif
            if ( !fv )
                throw io_error("Cannot OpenDisk");
            free(diskImageRealpath);
            g_volumePtr = std::unique_ptr<FuseVolume>(fv);
        }

        #if defined(DEBUG) && !defined(_WIN32)
        	system(stringPrintf("umount %s 2>&1 >/dev/null", mountPoint).c_str());
        #endif
//
//        char* mountPointRealpath = realpath(mountPoint, NULL);
////        if (!mountPointRealpath)
////            throw file_not_found_error(std::string("File '")+mountPointRealpath+"' is not found");
//
//        struct statfs statfs_buf;
//        int ret = statfs(mountPointRealpath, &statfs_buf);
//        if (ret != 0) {
//            throw file_not_found_error(std::string("Cannot statfs at path '")+mountPointRealpath+"', errno "+std::to_string(errno));
//        }
//
////NSLog(@"Disk type '%s' mounted on:%s from:%s", mount->f_fstypename, mount->f_mntonname, mount->f_mntfromname);
//        if ( strcmp(statfs_buf.f_mntonname, mountPointRealpath) == 0 ) {
//            printf("%s\n", statfs_buf.f_mntonname);
//            system((std::string("umount ")+mountPointRealpath).c_str());
//        }
	
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
	
		fuse_opt_add_arg(&args, "-oro");
		fuse_opt_add_arg(&args, "-s");
		fuse_opt_add_arg(&args, "-o");
		fuse_opt_add_arg(&args, "use_ino");
		#ifdef DEBUG
		fuse_opt_add_arg(&args, "-f");
			fuse_opt_add_arg(&args, "-o");
			fuse_opt_add_arg(&args, "allow_other");
		#endif

#ifdef DEBUG
		printf("Hdimount version %s\n", HDIMOUNT_VERSION);
		printf("Everything looks ok. Giving to Fuse\n");
#endif
		return fuse_main(args.argc, args.argv, &ops, 0);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << std::endl;

		return 1;
	}
}




int fuse_getattr(const char* path, struct FUSE_STAT* stat)
{
	trace_fuse_op(path, "fuse_stat(%s)\n", path);
	int rv = g_volumePtr->getattr(path, stat);
#ifdef DEBUG
//	if ( rv == 0 )
//	{
//		printf("fuse_stat(%s) -> stat->mode=%x\n", path, stat->st_mode);
//	}
#endif
	return rv;
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

int fuse_read(const char* path, char* buf, size_t bytes, FUSE_OFF_T offset, struct fuse_file_info* info)
{
//    trace_fuse_op(path, "fuse_read(%s, size=%zd, offset=%lld)\n", path, bytes, offset);
	return g_volumePtr->read(path, buf, bytes, offset, info);
}

int fuse_release(const char* path, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_release(%s)\n", path);
	return g_volumePtr->release(path, info);
}

#ifdef DEBUG
//#include <fuse_lowlevel.h>
//#include <fuse_kernel.h>
//struct fuse_dh {
//	pthread_mutex_t lock;
//	struct fuse *fuse;
//	fuse_req_t req;
//	char *contents;
//	int allocated;
//	unsigned len;
//	unsigned size;
//	unsigned needlen;
//	int filled;
//	uint64_t fh;
//	int error;
//	fuse_ino_t nodeid;
//};
#endif

int fuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, FUSE_OFF_T offset, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_readdir path '%s'\n", path);
    filler(buf, ".", 0, 0);
    filler(buf, "..", 0, 0);
	int rv = g_volumePtr->readdir(path, buf, filler, offset, info);
#ifdef DEBUG
//if ( rv == 0 ) {
//  struct fuse_dh *dh = (struct fuse_dh *)buf;
//  for (unsigned int i=0 ; i<dh->len ; ) {
//	  struct fuse_dirent *dirent = (struct fuse_dirent *) (dh->contents + i);
//	  std::string name(dirent->name, dirent->namelen);
//	  printf("fuse_readdir path '%s' -> returns '%s'\n", path, name.c_str());
////printf("FUSE_NAME_OFFSET=%d namelen=%d, rv=%d\n", FUSE_NAME_OFFSET, strlen(dirent->name), FUSE_DIRENT_ALIGN(FUSE_NAME_OFFSET + strlen(dirent->name)) );
//	  i += FUSE_DIRENT_ALIGN(FUSE_NAME_OFFSET + name.length());
//  }
//}else{
//  printf("fuse_readdir path '%s' -> failed '%d'\n", path, rv);
//}
#endif
	return rv;
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

	int rv;
	#if defined(__APPLE__) && !defined(DARLING)
		rv = g_volumePtr->getxattr(path, name, value, vlen, position);
	#else
		rv = g_volumePtr->getxattr(path, name, value, vlen);
	#endif

//	#ifdef DEBUG
//	printf("%s/%s : ", path, name);
//	if ( rv > 0 && value != NULL ) {
//		for ( int i = 0 ; i < MIN(rv, vlen) ; i++) {
//			printf("%02x ", value[i]);
//		}
//	}
//	printf("  rv=%d\n", rv);
//	#endif
	
	trace_fuse_op(path, "fuse_getxattr(%s, %s, % " PRIdPTR " , %zd) -> return %d\n", path, name, (intptr_t)value, vlen, rv);
	return rv;
}

int fuse_listxattr(const char* path, char* buffer, size_t size)
{
	trace_fuse_op(path, "fuse_listxattr(%s, %" PRIdPTR ", %zd)\n", path, (intptr_t)buffer, size);
	return g_volumePtr->listxattr(path, buffer, size);
}
