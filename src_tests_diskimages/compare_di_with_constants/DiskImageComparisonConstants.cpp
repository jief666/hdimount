#include <unistd.h>
#include <stdlib.h> // for alloca
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _MSC_VER
#ifndef __XATTR_H__
#include <sys/xattr.h>
#endif
#include <sys/mount.h>
#include <sys/dir.h>
#endif
#include <fuse.h> // for FUSE_STAT

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace ::std;

#include "../../src/Utils.h"
#include "../io.h"
#include "../../src/FuseVolume.h"
#include "../../src/OpenDisk.h"
#include "../DiskImageTestUtils.h"
#include "DiskImageComparisonConstants.h"

#if defined(__linux__)  ||  defined(_MSC_VER)
#define st_mtimespec st_mtim
#endif


struct FUSE_STAT fuse_stat_T(FuseVolume* fuseVolume, const string& root, const string& path)
{
    struct FUSE_STAT st_stat;
    char lastchar;
    if ( root.length() > 0 ) lastchar = root[root.length()-1];
    else lastchar = ' ';
    char firstchar;
    if ( path.length() > 0 ) firstchar = path[0];
    else firstchar = ' ';
    int rv;
    if ( firstchar == '/'  &&  lastchar == '/' ) {
        rv = fuseVolume->getattr((root+path.substr(1)).c_str(), &st_stat);
        if ( rv != 0 ) throw stringPrintf("Cannot stat '%s' on fuse volume", (root+path.substr(1)).c_str());
    }else
    if ( firstchar == '/'  ||  lastchar == '/' ) {
        rv = fuseVolume->getattr((root+path).c_str(), &st_stat);
        if ( rv != 0 ) {
rv = fuseVolume->getattr((root+path).c_str(), &st_stat);
            throw stringPrintf("Cannot stat '%s' on fuse volume", (root+path).c_str());
        }
    }else{
        rv = fuseVolume->getattr((root+"/"+path).c_str(), &st_stat);
        if ( rv != 0 ) throw stringPrintf("Cannot stat '%s' on fuse volume", (root+"/"+path).c_str());
    }
    return st_stat;
}

struct fuse_file_info fuse_open_T(FuseVolume* fuseVolume, const string& path)
{
    struct fuse_file_info info;
    info.flags = O_RDONLY;
    int rv = fuseVolume->open(path.c_str(), &info);
    if ( rv != 0 ) {
        throw stringPrintf("Cannot open '%s' on fuse volume", path.c_str());
    }
    return info;
}

string fuse_read_T(FuseVolume* fuseVolume, const string& path, struct fuse_file_info& info, int count, uint64_t offset)
{
    char* buf = (char*)alloca(count);
    int nblu = fuseVolume->read(path.c_str(), buf, count, offset, &info);
    if ( nblu != count ) {
        throw stringPrintf("Cannot read content of '%s' on fuse volume : nblu = %d", path.c_str(), nblu);
    }
    return string(buf, nblu);
}

void fuse_release_T(FuseVolume* fuseVolume, const string& path, struct fuse_file_info& info)
{
    int rv = fuseVolume->release(path.c_str(), &info);
    if ( rv != 0 ) throw stringPrintf("Cannot release '%s' on fuse volume", path.c_str());
}

string getContentOfFileAtPathInFuseVolume(FuseVolume* fuseVolume, const string& path)
{
    struct FUSE_STAT st_stat = fuse_stat_T(fuseVolume, "", path);

    struct fuse_file_info info;
    info.flags = O_RDONLY;
    int rv = fuseVolume->open(path.c_str(), &info);
    if ( rv != 0 ) {
        throw stringPrintf("Cannot open '%s' on fuse volume", path.c_str());
    }

    char* buf = (char*)alloca(st_stat.st_size);
    ssize_t nblu = fuseVolume->read(path.c_str(), buf, st_stat.st_size, 0, &info);
    if ( nblu != st_stat.st_size ) {
        throw stringPrintf("Cannot read content of '%s' on fuse volume : nblu = %zd", path.c_str(), nblu);
    }
    rv = fuseVolume->release(path.c_str(), &info);
    if ( rv != 0 ) throw stringPrintf("Cannot release '%s' on fuse volume", path.c_str());
    return string(buf, nblu);
}

string getXAttrFromFileAtPathInFuseVolume(FuseVolume* fuseVolume, const string& path, const string& xattrName)
{
#ifdef __APPLE__
    int rv = fuseVolume->getxattr(path.c_str(), xattrName.c_str(), NULL, 0, 0);
#else
    int rv = fuseVolume->getxattr(path.c_str(), xattrName.c_str(), NULL, 0);
#endif
    if ( rv < 0 ) throw stringPrintf("Cannot get xttar size %s from '%s' on fuse volume", xattrName.c_str(), path.c_str());
	char* buf = (char*)alloca(rv);
#ifdef __APPLE__
    int rv2 = fuseVolume->getxattr(path.c_str(), xattrName.c_str(), buf, rv, 0);
#else
    int rv2 = fuseVolume->getxattr(path.c_str(), xattrName.c_str(), buf, rv);
#endif
    if ( rv2 < 0 ) throw stringPrintf("Cannot getxttar %s from '%s' on fuse volume", xattrName.c_str(), path.c_str());
    return string(buf, rv);
}

/*---------------------- All the above methods must have an equivalent in FolderReferenceCreation.cpp ---------------------------*/
// COT prefix means : create or test

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

static string COT_Dir_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path)
{
    (void)dik;
    struct FUSE_STAT st_stat = fuse_stat_T(fuseVolume, "", path);
    if ( !S_ISDIR(st_stat.st_mode) ) throw stringPrintf("Dir '%s' doesn't exist", path.c_str());
    return path;
}

static string COT_File_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, bool check = true)
{
    (void)dik;
    string content = getContentOfFileAtPathInFuseVolume(fuseVolume, path);
    if ( check  &&  content != stringPrintf("Content of '%s'", path.c_str()) ) {
        throw stringPrintf("'%s' : file content differs. It's '%s' and should be '%s'", path.c_str(), content.c_str(), stringPrintf("Content of '%s'", path.c_str()).c_str());
    }
    return path;
}

/*
 * seconds since 1970
 */
void COT_FileCreationTime_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, struct timespec* ts)
{
#ifdef __APPLE__
    struct FUSE_STAT st_stat = fuse_stat_T(fuseVolume, "", path);
    
    
    if (     dik.type()==DISKIMAGE_TYPE_UDRO
         ||  dik.type()==DISKIMAGE_TYPE_UDCO
         ||  dik.type()==DISKIMAGE_TYPE_UDZO
         ||  dik.type()==DISKIMAGE_TYPE_UDBZ
         ||  dik.type()==DISKIMAGE_TYPE_ULFO
         ||  dik.type()==DISKIMAGE_TYPE_UFBI
         ||  dik.type()==DISKIMAGE_TYPE_IPOD
    ) {
        // All this format don't handle crtimes and set it to mtime
    }
    else
    {
        
        struct timespec ts2 = {(*ts).tv_nsec, 0};
        
        if ( st_stat.st_birthtimespec.tv_sec != (*ts).tv_sec ) {
            throw stringPrintf("'%s' -> st_birthtimespec is '%s' and should be '%s'", path.c_str(), timespec_toString(st_stat.st_birthtimespec).c_str(), timespec_toString(ts2).c_str());
        }
        // Currently ALL format return 0 in tv_nsec, including APFS diskimage, altough native APFS handle nanoseconds. Bug ?
        if ( st_stat.st_birthtimespec.tv_nsec != 0 ) {
            throw stringPrintf("'%s' -> st_birthtimespec is '%s' and should be '%s'", path.c_str(), timespec_toString(st_stat.st_birthtimespec).c_str(), timespec_toString(ts2).c_str());
        }
    }
#else
	(void)fuseVolume;
	(void)dik;
	(void)path;
	(void)ts;
#endif
    ts->tv_sec += 65;
    ts->tv_nsec += 65;
}

void COT_ModificationTime_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, struct timespec* ts)
{
    (void)dik;
    struct FUSE_STAT st_stat = fuse_stat_T(fuseVolume, "", path);
    
#ifdef __APPLE__
    if (     dik.type()==DISKIMAGE_TYPE_IPOD
         ||  dik.type()==DISKIMAGE_TYPE_UDBZ
         ||  dik.type()==DISKIMAGE_TYPE_UDCO
         ||  dik.type()==DISKIMAGE_TYPE_UDRO
         ||  dik.type()==DISKIMAGE_TYPE_UDZO
         ||  dik.type()==DISKIMAGE_TYPE_UFBI
         ||  dik.type()==DISKIMAGE_TYPE_ULFO
    ) {
        // All this format don't handle crtimes and set it to mtime
        if ( st_stat.st_birthtimespec.tv_sec == (*ts).tv_sec  &&  st_stat.st_birthtimespec.tv_nsec == 0 ) {
            // ok
        }else{
            struct timespec ts2 = {(*ts).tv_nsec, 0};
            throw stringPrintf("'%s' -> st_birthtimespec is '%s' and should be '%s'", path.c_str(), timespec_toString(st_stat.st_birthtimespec).c_str(), timespec_toString(ts2).c_str());
        }
    }
#endif
    bool timeRoundedToSeconds =
             dik.fs() == DISKIMAGE_FS_HFS
         ||  dik.type()==DISKIMAGE_TYPE_IPOD
         ||  dik.type()==DISKIMAGE_TYPE_UDBZ
         ||  dik.type()==DISKIMAGE_TYPE_UDCO
         ||  dik.type()==DISKIMAGE_TYPE_UDRO
         ||  dik.type()==DISKIMAGE_TYPE_UDZO
         ||  dik.type()==DISKIMAGE_TYPE_UFBI
         ||  dik.type()==DISKIMAGE_TYPE_ULFO;

    if ( st_stat.st_mtimespec.tv_sec == (*ts).tv_sec   &&  st_stat.st_mtimespec.tv_nsec == (timeRoundedToSeconds ? 0 : (*ts).tv_nsec) ) {
        // ok
    }else{
        throw stringPrintf("'%s' -> st_mtimespec is '%s' and should be '%s'", path.c_str(), timespec_toString(st_stat.st_mtimespec).c_str(), timespec_toString(*ts).c_str());
    }
    ts->tv_sec += 65;
    ts->tv_nsec += 65;
}

/*
 * set_access_time and set_change_time : is it working ? Can't see a difference.
 */
//void COT_AccessTime_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, struct timespec* ts)
//{
//    //set_access_time((root+path).c_str(), { time_t(seconds), fraction } );
//    ts->tv_sec += 65;
//    ts->tv_nsec += 65;
//}
//
//void COT_ChangeTime_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, struct timespec* ts)
//{
//    //set_change_time((root+path).c_str(), { time_t(seconds), fraction } );
//    ts->tv_sec += 65;
//    ts->tv_nsec += 65;
//}

/* ti : seconds from 1970 */
static void COT_Times_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, struct timespec* ts)
{
    COT_FileCreationTime_T(fuseVolume, dik, path, ts );
    COT_ModificationTime_T(fuseVolume, dik, path, ts );
//    COT_AccessTime_T(fuseVolume, dik, path, ts );
//    COT_ChangeTime_T(fuseVolume, dik, path, ts );
}

static void COT_FileCreator_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, const string& creator)
{
    (void)dik;
    string attr = getXAttrFromFileAtPathInFuseVolume(fuseVolume, path, "com.apple.FinderInfo");
    if ( attr.substr(4, 4) != creator ) {
        throw stringPrintf("File creator is '%s', but should be '%s' at '%s'", attr.substr(4, 4).c_str(), creator.c_str(), path.c_str());
    }
}

static void COT_FileType_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, const string& fileType)
{
    (void)dik;
    string attr = getXAttrFromFileAtPathInFuseVolume(fuseVolume, path, "com.apple.FinderInfo");
    if ( attr.substr(0, 4) != fileType ) {
        throw stringPrintf("'%s' -> attr.substr(0, 4) != '%s'", fileType.c_str(), path.c_str());
    }
}

static void COT_OwnerAndGroup_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, uid_t uid, gid_t gid)
{
    (void)dik;
    struct FUSE_STAT st_stat = fuse_stat_T(fuseVolume, "", path);
    if ( st_stat.st_uid != uid ) {
        throw stringPrintf("'%s' -> st_uid != getuid()", path.c_str());
    }
    if ( st_stat.st_gid != gid ) {
        throw stringPrintf("'%s' -> st_gid != getgid()", path.c_str());
    }
}

static void COT_XAttr_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, const string& xattrName, const string& expectedValue)
{
//if ( path == "/file+lotxattrs.txt" ) {
//printf("");
//}
    string value = getXAttrFromFileAtPathInFuseVolume(fuseVolume, path, xattrName);
    if ( value == expectedValue ) return;
    if ( xattrName == "com.apple.quarantine" &&
           (
            dik.type()==DISKIMAGE_TYPE_UDRO
            ||  dik.type()==DISKIMAGE_TYPE_UDCO
            ||  dik.type()==DISKIMAGE_TYPE_UDZO
            ||  dik.type()==DISKIMAGE_TYPE_UDBZ
            ||  dik.type()==DISKIMAGE_TYPE_ULFO
            ||  dik.type()==DISKIMAGE_TYPE_UFBI
            ||  dik.type()==DISKIMAGE_TYPE_IPOD
           )
    ) {
        if ( value.substr(4) == expectedValue.substr(4) ) return;
    }
    throw stringPrintf("wrong xattr '%-25s' : %s - %s - path: '%s:%s'\n", path.c_str(), value.c_str(), expectedValue.c_str(), path.c_str(), xattrName.c_str());
}

static void COT_FileContent_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string &path, const string& expectedValue)
{
    (void)dik;
    string content = getContentOfFileAtPathInFuseVolume(fuseVolume, path);
    if ( content != expectedValue ) {
        throw stringPrintf("'%s' file content differs. It's '%s' and should be '%s'", path.c_str(), content.c_str(), expectedValue.c_str());
    }

}

static void COT_Write_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string &path, struct fuse_file_info& fd, const string& expectedvalue, size_t size, uint64_t* offset)
{
    (void)dik;
    string value = fuse_read_T(fuseVolume, path, fd, size, *offset);
    *offset += size;
    if ( value != expectedvalue ) {
        throw stringPrintf("'%s' -> value is '%s' and should be '%s'", path.c_str(), value.c_str(), expectedvalue.c_str());
    }
}

static struct fuse_file_info COT_Open_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path)
{
    (void)dik;
    return fuse_open_T(fuseVolume, path);
}

static void COT_Close_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, struct fuse_file_info fd)
{
    (void)dik;
    fuse_release_T(fuseVolume, path, fd);
}

static void COT_Alias_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, const string& aliasName)
{
    (void)fuseVolume;
    (void)dik;
    (void)path;
    (void)aliasName;
    // TODO : check if the alias is ok
}

static void COT_HardLink_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, const string& linkName)
{
    (void)fuseVolume;
    (void)dik;
    (void)path;
    (void)linkName;
    // TODO : check if the hard link is ok
}

static void COT_SymLink_T(FuseVolume* fuseVolume, const DiskImageKind& dik, const string& path, const string& linkName)
{
    (void)dik;
    char buf[1024];
    int rv = fuseVolume->readlink(linkName.c_str(), buf, sizeof(buf));
    if ( rv != 0 ) throw stringPrintf("readlink(%s) failed. Returned value=%d", path.c_str(), rv);
    if ( path != buf ) {
        throw stringPrintf("symlink '%s' content is wrong", path.c_str());
    }
}

#ifdef __clang__
#pragma clng diagnostic pop
#endif
/*---------------------- End of methods that must have an equivalent in DiskImageCreation.cpp ---------------------------*/

#define COMPARISON
#define FD_TYPE struct fuse_file_info
#define VOLUME_TYPE FuseVolume*

#include "../FolderReferenceCreation_include.cpp"

void compare_diskimage_with_constants(const DiskImageToTest &dit, bool moveToFolderOk)
{
    if ( !dit.canBeGenerated()) {
        printf("This kind cannot exist (%s)\n", dit.dik().nameAndExtension().c_str());
        return;
    }
    if ( dit.di().alreadyTested() ) {
        printf("Already tested ok (saved in '%s')\n", dit.pathToFolder().c_str());
        return;
    }
    if ( !file_or_dir_exists(dit.pathToDiskImageFile()) ) {
        throw stringPrintf("Disk image '%s' doesn't exist\n", dit.pathToDiskImageFile().c_str());
    }
    FuseVolume* volume = NULL;
    try {
        volume = openDisk(dit.pathToDiskImageFile().c_str(), "foo");
#ifdef DEBUG
struct FUSE_STAT st_stat;
int rv = volume->getattr("/utf8_names/pomme.txt", &st_stat);
#endif
        _createFolderReference(volume, dit.size(), dit.dik());
    }
    catch (const string& s) {
        if ( volume ) delete volume;
        throw;
    }
    if ( volume ) delete volume;
    
    if ( moveToFolderOk ) {
		if ( !file_or_dir_exists(dit.di().pathToFolderOk()) ) {
			mkdir_T(dit.di().pathToFolderOk(), 0755, stringPrintf("Cannot create '%s'", dit.di().pathToFolderOk().c_str()));
		}
        if ( rename(dit.pathToDiskImageFile().c_str(), (dit.di().pathToFolderOk()+"/"+dit.dik().nameAndExtension()).c_str()) != 0 ) {
            throw stringPrintf("rename '%s' to '%s' failed", dit.di().pathToFolderOk().c_str(), (dit.di().pathToFolderOk()+dit.dik().nameAndExtension()).c_str());
        };
    }
}

void compare_diskimage_with_constants(const vector<DiskImageToTest>& ditVector, bool moveToFolderOk, map<DiskImageToTest, vector<string>>* report)
{
    for (size_t i = 0 ; i < ditVector.size() ; i++)
    {
        const DiskImageToTest& dit = ditVector[i];
        try {
            printf("=================> Test disk image with constants '%s' (%zd/%zd)\n", dit.dik().nameAndExtension().c_str(), i+1, ditVector.size());
            compare_diskimage_with_constants(dit, moveToFolderOk);
            printf("---------------------------------> passed\n");
        }
        catch (const std::exception & e)
        {
            printf("Exeception type '%s', what '%s'\n", typeid(e).name(), e.what());
            (*report)[dit].push_back(stringPrintf("Exeception type '%s', what '%s'", typeid(e).name(), e.what()));
        }
        catch(const string& e) {
            printf("Failure : %s\n", e.c_str());
            (*report)[dit].push_back(stringPrintf("Failure : %s", e.c_str()));
            printf("---------------------------------> FAILED\n");
        }
        catch(...)
        {
            printf("Unknow exception\n");
            (*report)[dit].push_back(stringPrintf("Unknow exception"));
            printf("---------------------------------> FAILED\n");
        }
    }
}
