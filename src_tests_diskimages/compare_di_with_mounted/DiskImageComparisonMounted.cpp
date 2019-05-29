#include <unistd.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef __XATTR_H__
#include <sys/xattr.h>
#endif
#include <sys/mount.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm> // for sort

using namespace ::std;

#include "../../src/Utils.h"
#include "../io.h"
#include "../../src/FuseVolume.h"
#include "../../src/OpenDisk.h"
#include "../DiskImageTestUtils.h"
#include "DiskImageComparisonMounted.h"
//#include "DiskImageCreation.h"


static int myFiller (void *buf, const char *name, const struct stat *stbuf, off_t off)
{
    (void)off;
    (void)stbuf;
    vector<string>* root_listPtr = (vector<string>*)buf;

//printf("myFiller : name=%s\n", name);
     if ( strcmp(name, ".") != 0  &&  strcmp(name, "..") != 0  &&  strcmp(name, ".fseventsd") != 0  &&  strcmp(name, ".Trashes") != 0 ) root_listPtr->push_back(name);
    return 0;
}

/*
 * fuseDir MUST end with a slash
 */
static void compare_diskimage_dir(const string& nativeDir, FuseVolume* fuseVolume, const string& fuseDir)
{
//printf("Testing '%s' against '%s' in fuse volume\n", nativeDir.c_str(), fuseDir.c_str());
    vector<string> fuseDirContent;
    struct fuse_file_info info;
    
    int rv = fuseVolume->readdir(fuseDir.substr(0, fuseDir.length()-1).c_str(), &fuseDirContent, myFiller, 0, &info);
    if ( rv != 0 ) throw stringPrintf("Cannot read '%s' in fuse volume", fuseDir.c_str());
    std::sort(fuseDirContent.begin(), fuseDirContent.end(), compareString);
    
    std::vector<struct dirent> nativeDirentVector = dirContent(nativeDir);

    for (size_t i=0 ;  i < fuseDirContent.size() ; i++)
    {
        string currentDirEntryName = fuseDirContent[i];
#ifdef DEBUG
//printf("Checking '%s'\n", (fuseDir+(fuseDir==""?"/":"")+currentDirEntryName).c_str());
#endif
        struct stat fuse_stat_buf;
        struct stat native_stat;
        try
        {
            if ( i >= nativeDirentVector.size() ) {
                throw stringPrintf("More file in fuse than for real\n");
            }
            string nativeName(nativeDirentVector[i].d_name, nativeDirentVector[i].d_namlen);
            if ( nativeName != fuseDirContent[i] ) {
                throw stringPrintf("ref_root_list[%zd].d_name(%s) != root_list[%zd](%s)\n", i, nativeName.c_str(), i, fuseDirContent[i].c_str());
            }
            int ret = fuseVolume->getattr((fuseDir+fuseDirContent[i]).c_str(), &fuse_stat_buf);
            if ( ret != 0 ) {
                throw stringPrintf("Cannot stat '%s', ret %d\n", (fuseDir+fuseDirContent[i]).c_str(), ret);
            }
            if ( nativeDirentVector[i].d_type != (((fuse_stat_buf.st_mode) & S_IFMT) >> 12) ) {
                throw stringPrintf("d_type[%zd].d_type(%x) != st_mode(%x)\n", i, nativeDirentVector[i].d_type, (((fuse_stat_buf.st_mode) & S_IFMT) >> 12));
            }
            lstat_T(nativeDir+"/"+fuseDirContent[i], &native_stat, stringPrintf("Cannot stat '%s'", (nativeDir+"/"+fuseDirContent[i]).c_str()));
            if ( native_stat.st_nlink != fuse_stat_buf.st_nlink ) {
                throw stringPrintf("ref_root_list[%zd].st_nlink(%d) != st_nlink(%d)\n", i, native_stat.st_nlink, fuse_stat_buf.st_nlink);
            }
            if ( native_stat.st_uid != fuse_stat_buf.st_uid ) {
                throw stringPrintf("ref_root_list[%zd].st_uid(%d) != st_uid(%d)\n", i, native_stat.st_uid, fuse_stat_buf.st_uid);
            }
            if ( native_stat.st_gid != fuse_stat_buf.st_gid ) {
                throw stringPrintf("ref_root_list[%zd].st_gid(%d) != st_gid(%d)\n", i, native_stat.st_gid, fuse_stat_buf.st_gid);
            }
            if ( native_stat.st_atimespec.tv_sec != fuse_stat_buf.st_atimespec.tv_sec  ||  native_stat.st_atimespec.tv_nsec != fuse_stat_buf.st_atimespec.tv_nsec) {
                throw stringPrintf("ref_root_list[%zd].st_atimespec(%s) != st_atimespec(%s)\n", i, timespec_toString(native_stat.st_atimespec).c_str(), timespec_toString(fuse_stat_buf.st_atimespec).c_str());
            }
            if ( native_stat.st_mtimespec.tv_sec != fuse_stat_buf.st_mtimespec.tv_sec  ||  native_stat.st_mtimespec.tv_nsec != fuse_stat_buf.st_mtimespec.tv_nsec ) {
                throw stringPrintf("ref_root_list[%zd].st_mtimespec(%ld, %ld) != st_mtimespec(%ld, %ld)\n", i, native_stat.st_mtimespec.tv_sec, native_stat.st_mtimespec.tv_nsec, fuse_stat_buf.st_mtimespec.tv_sec, fuse_stat_buf.st_mtimespec.tv_nsec);
            }
            if ( native_stat.st_ctimespec.tv_sec != fuse_stat_buf.st_ctimespec.tv_sec  ||  native_stat.st_ctimespec.tv_nsec != fuse_stat_buf.st_ctimespec.tv_nsec) {
                throw stringPrintf("ref_root_list[%zd].st_ctimespec(%s) != st_ctimespec(%s)\n", i, timespec_toString(native_stat.st_ctimespec).c_str(), timespec_toString(fuse_stat_buf.st_mtimespec).c_str());
            }
            if (  (native_stat.st_birthtimespec.tv_sec != fuse_stat_buf.st_birthtimespec.tv_sec  ||  native_stat.st_birthtimespec.tv_nsec != fuse_stat_buf.st_birthtimespec.tv_nsec) ) {
                throw stringPrintf("ref_root_list[%zd].st_birthtimespec(%ld, %ld) != st_birthtimespec(%ld, %ld)\n", i, native_stat.st_birthtimespec.tv_sec, native_stat.st_birthtimespec.tv_nsec, fuse_stat_buf.st_mtimespec.tv_sec, fuse_stat_buf.st_mtimespec.tv_nsec);
            }
            if ( native_stat.st_size != fuse_stat_buf.st_size ) {
                throw stringPrintf("ref_root_list[%zd].st_size(%lld) != st_size(%lld)\n", i, native_stat.st_size, fuse_stat_buf.st_size);
            }
            if ( native_stat.st_nlink != fuse_stat_buf.st_nlink ) {
                throw stringPrintf("ref_root_list[%zd].st_flags(%d) != st_flags(%d)\n", i, native_stat.st_flags, fuse_stat_buf.st_flags);
            }
        }
        catch (const string& s)
        {
            throw stringPrintf("Checking '%s' -> %s\n", fuseDirContent[i].c_str(), s.c_str());
        }
        if ( S_ISDIR(fuse_stat_buf.st_mode) ) {
            compare_diskimage_dir(nativeDir+"/"+currentDirEntryName, fuseVolume, fuseDir+currentDirEntryName+"/");
        }
    }
}

static void compare_diskimage_with_mounted(const DiskImageToTest &dit, bool moveToFolderOk, map<DiskImageToTest, vector<string>>* report)
{
    if ( !dit.di().canBeGenerated()) {
        printf("This kind cannot exist (%s)\n", dit.di().dik().nameAndExtension().c_str());
        return;
    }
    if ( dit.di().alreadyTested() ) {
        printf("Already tested ok (saved in '%s')\n", dit.di().pathToFolderOk().c_str());
        return;
    }
    if ( !dit.canBeMountedByOS() ) {
        printf("This OS version cannot mount this kind (saved in '%s')\n", dit.pathToDiskImageFile().c_str());
        return;
    }

	
    FuseVolume* volume = NULL;
    try
    {
		if ( !file_or_dir_exists(dit.di().pathToDiskImageFile()) ) {
			throw stringPrintf("Disk image '%s' doesn't exist\n", dit.pathToDiskImageFile().c_str());
		}

		dit.di().mount_T(true);

        volume = openDisk(dit.pathToDiskImageFile().c_str(), "foo");
        compare_diskimage_dir(dit.di().mountPoint(), volume, "/");
		printf("---------------------------------> passed\n");
		dit.di().eject_T();

		if ( moveToFolderOk ) {
			if ( !file_or_dir_exists(dit.di().pathToFolderOk()) ) {
				mkdir_T(dit.di().pathToFolderOk(), 0755, stringPrintf("Cannot create '%s'", dit.di().pathToFolderOk().c_str()));
			}
			if ( rename(dit.pathToDiskImageFile().c_str(), (dit.di().pathToFolderOk()+"/"+dit.dik().nameAndExtension()).c_str()) != 0 ) {
				throw stringPrintf("rename '%s' to '%s' failed", dit.pathToDiskImageFile().c_str(), (dit.di().pathToFolderOk()+"/"+dit.dik().nameAndExtension()).c_str());
			}
		}
    }
	catch (const std::exception & e)
	{
		printf("Exeception type '%s', what '%s'\n", typeid(e).name(), e.what());
		(*report)[dit].push_back(stringPrintf("Exeception type '%s', what '%s'", typeid(e).name(), e.what()));
        printf("---------------------------------> FAILED : Test of '%s' <-------------------------\n", dit.dik().nameAndExtension().c_str());
	}
    catch (const string& s) {
        printf("%s\n", s.c_str());
		(*report)[dit].push_back(stringPrintf("Failure : %s", s.c_str()));
        printf("---------------------------------> FAILED : Test of '%s' <-------------------------\n", dit.dik().nameAndExtension().c_str());
    }
	catch(...)
	{
		printf("Unknow exception\n");
		(*report)[dit].push_back(stringPrintf("Unknow exception"));
        printf("---------------------------------> FAILED : Test of '%s' <-------------------------\n", dit.dik().nameAndExtension().c_str());
	}
    if ( volume ) delete volume;
	if ( file_or_dir_exists(dit.di().pathToDiskImageFile()) ) {
		if ( dit.di().isMounted() ) dit.di().eject_T();
	}
}

void compare_diskimage_with_mounted(const vector<DiskImageToTest>& ditVector, bool moveToFolderOk, map<DiskImageToTest, vector<string>>* report)
{
    for (size_t i = 0 ; i < ditVector.size() ; i++)
    {
        const DiskImageToTest& dit = ditVector[i];
		printf("=================> Test disk image with the same mounted by OS : '%s' (%zd/%zd)\n", ditVector[i].di().dik().nameAndExtension().c_str(), i+1, ditVector.size());
		compare_diskimage_with_mounted(dit, moveToFolderOk, report);
    }
}
