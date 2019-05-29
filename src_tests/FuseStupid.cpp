#ifndef CrossPlateform_h
#error Include CrossPlatform.h from command line
#endif

#include <fuse.h>

#include <stdint.h>
#include <inttypes.h>
#include <random>



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


void showHelp(const char* argv0)
{
    std::cerr << "Usage: " << argv0 << " <file> <mount-point> [fuse args]\n\n";
    std::cerr << ".DMG files and raw disk images can be mounted.\n";
    std::cerr << argv0 << " automatically selects the first HFS+/HFSX partition.\n";
}

static int fuse_opt_proc(void *data, const char *arg, int key, struct fuse_args *outargs)
{
	(void)data;
	(void)outargs;
    switch (key) {
        case FUSE_OPT_KEY_NONOPT: {
           return 1; // fuse_main args
        }
    }
    return 1;
}


int fuse_getattr(const char* path, struct FUSE_STAT* stbuf)
{
	trace_fuse_op(path, "fuse_stat(%s)\n", path);
    memset(stbuf, 0, sizeof(struct stat));
 
    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0555;
        stbuf->st_nlink = 3;
        stbuf->st_size = 3*34;
printf("stat dir\n");
    } else {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = 11;
printf("stat file\n");
    }
    
    stbuf->st_atime = time(NULL);
    stbuf->st_mtime = time(NULL);
    stbuf->st_ctime = time(NULL);
    stbuf->st_birthtimespec.tv_sec = time(NULL);
    
    return 0;
}

int fuse_readlink(const char* path, char* buf, size_t size)
{
	trace_fuse_op(path, "fuse_readlink(%s, %" PRIxPTR ", %zd)\n", path, (intptr_t)buf, size);
    return -1;
}

int fuse_open(const char* path, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_open(%s)\n", path);
//    if ( strcmp(path, "/file.txt") == 0 ) {
//        return 0;
//    }
    return -1;
}

int fuse_read(const char* path, char* buf, size_t bytes, FUSE_OFF_T offset, struct fuse_file_info* info)
{
    trace_fuse_op(path, "fuse_read(%s, size=%zd, offset=%lld)\n", path, bytes, offset);
    strncpy(buf, "Hello World", bytes);
	return 11;
}

int fuse_release(const char* path, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_release(%s)\n", path);
    return 0;
}

int fuse_readdir(const char* path, void* buf, fuse_fill_dir_t filler, FUSE_OFF_T offset, struct fuse_file_info* info)
{
	trace_fuse_op(path, "fuse_readdir path '%s'\n", path);
    
    struct stat image_stat;
    fuse_getattr("", &image_stat);
    
    filler(buf, ".", 0, 0);
    filler(buf, "..", 0, 0);
    filler(buf, "file.txt", &image_stat, 0);
    return 0;
}

#if defined(__APPLE__)
int fuse_getxattr(const char* path, const char* name, char* value, size_t vlen, uint32_t position)
#else
int fuse_getxattr(const char* path, const char* name, char* value, size_t vlen)
#endif
{
	#ifdef __APPLE__
		if (position > 0) return -ENOSYS; // it's not supported... yet. I think it doesn't happen anymore since osx use less ressource fork
	#endif

	trace_fuse_op(path, "fuse_getxattr(%s, %s, % " PRIdPTR " , %zd)\n", path, name, (intptr_t)value, vlen);

	return -93;
}

int fuse_listxattr(const char* path, char* buffer, size_t size)
{
	trace_fuse_op(path, "fuse_listxattr(%s, %" PRIdPTR ", %zd)\n", path, (intptr_t)buffer, size);
    return 0;
}

int main(int argc, char** argv)
{
	try
	{
        
		struct fuse_operations ops;
		struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
        struct fuse_opt sparsebundle_options[] = {
            { 0, 0, 0 } // End of options
        };
        if ( fuse_opt_parse(&args, NULL, sparsebundle_options, fuse_opt_proc) == -1 ) {
            showHelp(argv[0]);
            return 1;
        }
        
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
		fuse_opt_add_arg(&args, "-f");
        fuse_opt_add_arg(&args, "-o");
        fuse_opt_add_arg(&args, "allow_other");
        fuse_opt_add_arg(&args, "/Users/jief/mnt_stupid");

        system(stringPrintf("umount /Users/jief/mnt_stupid 2>&1 >/dev/null").c_str());

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


