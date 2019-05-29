//#include <Carbon/CFAvailability.h>
//#include <Carbon/Carbon.h>

#include <unistd.h>
#include <stdlib.h> // for realpath
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _MSC_VER
#ifndef __XATTR_H__
#include <sys/xattr.h>
#endif
#include <sys/mount.h>
#include <sys/dir.h>
#else
#include <fuse.h> // for FUSE_STAT
#endif
#include <fcntl.h> // for open

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace ::std;

#include "io.h"
#include "../src/Utils.h"
#include "DiskImageToTest.h"
#include "DiskImageTestUtils.h"
#include "FolderReferenceCreation.h"

#ifdef __APPLE__

//
//time_t appleToUnixTime(uint32_t apple)
//{
//    const time_t offset = 2082844800L; // Nb of seconds between 1st January 1904 12:00:00 and unix epoch
//    if (apple == 0)
//        return 0; // 0 stays 0, even if it change the date from 1904 to 1970.
//    // Force time to wrap around and stay positive number. That's how Mac does it.
//    // File from before 70 will have date in the future.
//    // Example : Time value 2082844799, that should be Dec 31st 12:59:59 PM will become February 7th 2106 6:28:15 AM.
//    return uint32_t(apple - offset);
//}

//#pragma gcc diagnostic push
//#pragma gcc diagnostic ignored "-Wdeprecated-declarations"
//
///*
// * seconds since 1904
// */
//void setFileTime_T(const string& path, int whichTime, uint64_t seconds, UInt16 fraction)
//{
//    Boolean fsrefIsDirectory;
//    FSRef fsRef;
//    OSStatus err;
//    FSCatalogInfo fsCatalogInfo;
//
//    memset(&fsRef, 0, sizeof(fsRef));
//    err = FSPathMakeRefWithOptions((const UInt8 *)path.c_str(), kFSPathMakeRefDoNotFollowLeafSymlink, &fsRef, &fsrefIsDirectory);
//    if ( err != noErr ) throw stringPrintf("FSPathMakeRefWithOptions(%s) error", path.c_str());
//    err = FSGetCatalogInfo (&fsRef, kFSCatInfoNodeFlags | kFSCatInfoAllDates | kFSCatInfoPermissions, &fsCatalogInfo, NULL, NULL, NULL);
//    if ( err != noErr ) {
//        // Happend to me that it doesn't work the first but the second.
////printf("retry FSGetCatalogInfo\n");
//        err = FSPathMakeRefWithOptions((const UInt8 *)path.c_str(), kFSPathMakeRefDoNotFollowLeafSymlink, &fsRef, &fsrefIsDirectory);
//        if ( err != noErr ) throw stringPrintf("FSPathMakeRefWithOptions(%s) error", path.c_str());
//        err = FSGetCatalogInfo (&fsRef, kFSCatInfoNodeFlags | kFSCatInfoAllDates | kFSCatInfoPermissions, &fsCatalogInfo, NULL, NULL, NULL);
//        if ( err != noErr ) {
//            throw stringPrintf("FSGetCatalogInfo(%s) error %d", path.c_str(), err);
//        }
//    }
//    UTCDateTime* dt;
//    if ( whichTime == kFSCatInfoCreateDate ) dt = &fsCatalogInfo.createDate;
//    else if ( whichTime == kFSCatInfoContentMod ) dt = &fsCatalogInfo.contentModDate;
//    else if ( whichTime == kFSCatInfoAttrMod ) dt = &fsCatalogInfo.attributeModDate;
//    else throw "bug";
//    (*dt).highSeconds = (seconds >> 32ULL);
//    (*dt).lowSeconds = uint32_t(seconds);
//    (*dt).fraction = fraction;
//
//    err = FSSetCatalogInfo(&fsRef, whichTime, &fsCatalogInfo);
//    if ( err != noErr ) {
//        throw stringPrintf("FSSetCatalogInfo(%s) error %d", path.c_str(), err);
//    }
//}
//
//#pragma gcc diagnostic pop

/*---------------------- All the above methods must have an equivalent in DiskImageCreation.cpp ---------------------------*/
// COT prefix means : create or test

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

/*
 * seconds since 1970
 */

void COT_FileCreationTime_T(const string& root, const DiskImageKind& dik, const string& path, struct timespec* ts)
{
    (void)dik;
//    const time_t offset = 2082844800L; // Nb of seconds between 1st January 1904 12:00:00 and unix epoch
//    setFileTime_T(root+"/"+path, kFSCatInfoCreateDate, seconds == 0 ? 0 : seconds + offset, fraction);
    set_create_time((root+path).c_str(), *ts );
    ts->tv_sec += 65;
    ts->tv_nsec += 65;
}

void COT_ModificationTime_T(const string& root, const DiskImageKind& dik, const string& path, struct timespec* ts)
{
    (void)dik;
//    const time_t offset = 2082844800L; // Nb of seconds between 1st January 1904 12:00:00 and unix epoch
//
//    struct timeval times[2];
//    times[0].tv_sec = seconds;
//    times[0].tv_usec = 0;
//    times[1].tv_sec = seconds;
//    times[1].tv_usec = 0;
////    setFileTime_T(root+"/"+path, kFSCatInfoContentMod, seconds, fraction);
//    int rv = utimes( (root+path).c_str(), times);
//    if ( rv != 0 ) throw stringPrintf("Cannot set times for '%s'", path.c_str());
    set_modification_time((root+path).c_str(), *ts );
    ts->tv_sec += 65;
    ts->tv_nsec += 65;
}

/*
 * not sure if kFSCatInfoAttrMod is same as change_time
 */
//
//void COT_FileAttrModTime_T(const string& root, const DiskImageKind& dik, const string& path, uint64_t seconds, UInt16 fraction)
//{
//    (void)dik;
//    const time_t offset = 2082844800L; // Nb of seconds between 1st January 1904 12:00:00 and unix epoch
//    setFileTime_T(root+"/"+path, kFSCatInfoAttrMod, seconds == 0 ? 0 : seconds + offset, fraction);
//}

/*
 * set_access_time and set_change_time : is it working ? Can't see a difference.
 */
//void COT_AccessTime_T(const string& root, const DiskImageKind& dik, const string& path, struct timespec* ts)
//{
//    set_access_time((root+path).c_str(), *ts );
//    ts->tv_sec += 65;
//    ts->tv_nsec += 65;
//}
//
//void COT_ChangeTime_T(const string& root, const DiskImageKind& dik, const string& path, struct timespec* ts)
//{
//    set_change_time((root+path).c_str(), *ts );
//    ts->tv_sec += 65;
//    ts->tv_nsec += 65;
//}

/* ti : seconds from 1970 */
static void COT_Times_T(const string& root, const DiskImageKind& dik, const string& path, struct timespec* ts)
{
// utimes cannot change creation time
//    if ( utimes( path.c_str(), times) != 0 ) throw stringPrintf("Cannot utimes '%s'\n", path.c_str());
    COT_FileCreationTime_T(root, dik, path, ts );
    COT_ModificationTime_T(root, dik, path, ts );
//    COT_AccessTime_T(root, type, path, ts );
//    COT_ChangeTime_T(root, type, path, ts );
}

static string COT_File_T(const string& root, const DiskImageKind& dik, const string& path, bool check = true)
{
    (void)dik;
    (void)check;
    write_at_path_T((root+path).c_str(), stringPrintf("Content of '%s'", path.c_str()), stringPrintf("Cannot write to '%s'", path.c_str()));
    return path;
}

static void COT_FileContent_T(const string& root, const DiskImageKind& dik, const string &path, const string& value) {
    (void)dik;
    write_at_path_T((root+path), value.c_str(), stringPrintf("Cannot write '%s' to '%s'", value.c_str(), (root+path).c_str()));
}


static string COT_Dir_T(const string& root, const DiskImageKind& dik, const string& path)
{
    (void)dik;
    mkdir_T((root+path).c_str(), 0755, stringPrintf("Cannot create dir '%s'", path.c_str()));
    return path;
}

static void COT_FileCreator_T(const string& root, const DiskImageKind& dik, const string& path, const string& creator)
{
    (void)dik;
    execAndPrintOutput_T(stringPrintf("SetFile -c '%s' %s", creator.c_str(), (root+path).c_str()).c_str());
}

static void COT_FileType_T(const string& root, const DiskImageKind& dik, const string& path, const string& creator)
{
    (void)dik;
    execAndPrintOutput_T(stringPrintf("SetFile -t '%s' %s", creator.c_str(), (root+path).c_str()).c_str());
}

static void COT_OwnerAndGroup_T(const string& root, const DiskImageKind& dik, const string& path, int uid, int gid)
{
    (void)dik;
    execAndPrintOutput_T(stringPrintf("sudo chown %d:%d %s", uid, gid, (root+path).c_str()));
//            if ( chown(currentfile.c_str(), 0, 0) != 0 ) throw stringPrintf("Cannot set 0:0 at '%s'", currentfile.c_str());

}

static void COT_XAttr_T(const string& root, const DiskImageKind& dik, const string& path, const string& xattrName, const string& xattrValue)
{
    (void)dik;
    if ( setxattr((root+path).c_str(), xattrName.c_str(), xattrValue.data(), xattrValue.length(), 0, XATTR_NOFOLLOW) != 0 )
        throw stringPrintf("Cannot set xattr '%s' at '%s' to '%s'", xattrName.c_str(), (root+path).c_str(), xattrValue.c_str());
}

/*
 * root and path is just for error msg
 */
static void COT_Write_T(const string& root, const DiskImageKind& dik, const string &path, int fd, const string& value, size_t size, uint64_t* offset)
{
    (void)dik;
    (void)offset;
    write_all_T(fd, value.c_str(), value.length(), stringPrintf("Cannot write %ld bytes in '%s'\n", size, (root+path).c_str()));
}

static int COT_Open_T(const string& root, const DiskImageKind& dik, const string& path)
{
    (void)dik;
    return open_T(root+path, O_TRUNC | O_CREAT | O_RDWR, 0644, stringPrintf("Cannot open '%s'", (root+path).c_str()));
}

static int COT_Close_T(const string& root, const DiskImageKind& dik, const string& path, int fd)
{
    (void)dik;
    return close_T(fd, stringPrintf("Cannot close '%s'", (root+path).c_str()));
}

static void COT_Alias_T(const string& root, const DiskImageKind& dik, const string& path, const string& aliasName)
{
    (void)dik;
    char buf[PATH_MAX+1];
    if ( realpath(root.c_str(), buf) == NULL ) throw stringPrintf("realpath(%s) failed", root.c_str());
    string currentDirRealpath = buf;
    if ( realpath((root+path).c_str(), buf) == NULL ) throw stringPrintf("realpath(%s) failed", (root+path).c_str());
    string currentfileRealpath = buf;

//    execAndPrintOutput_T(stringPrintf("osascript -e 'tell application \"Finder\"\n make new alias to file (posix file \"%s\") at (posix file \"%s\")\n set name of result to \"%s\"\n end tell'", currentfileRealpath.c_str(), currentDirRealpath.c_str(), aliasName.c_str()));
    // set name of result doesn't work when the alias is on a mounted volume (except /). This means that alias name must the default name
    execAndPrintOutput_T(stringPrintf("osascript -e 'tell application \"Finder\"\n make new alias to file (posix file \"%s\") at (posix file \"%s\")\n end tell'", currentfileRealpath.c_str(), currentDirRealpath.c_str()));
    if ( basename(path)+" alias" != aliasName )
        throw stringPrintf("%s", "Because of a bug, we can't rename alias name");
}

static void COT_HardLink_T(const string& root, const DiskImageKind& dik, const string& path, const string& linkName)
{
    (void)dik;
    if ( link((root+path).c_str(), (root+linkName).c_str()) != 0 ) throw stringPrintf("Cannot hard link '%s' to '%s'", path.c_str(), linkName.c_str());
}

static void COT_SymLink_T(const string& root, const DiskImageKind& dik, const string& path, const string& linkName)
{
    (void)dik;
    if ( symlink((path).c_str(), (root+linkName).c_str()) != 0 ) {
        throw stringPrintf("Cannot sym link '%s' to '%s'", path.c_str(), (root+linkName).c_str());
    }
}

#ifdef __clang__
#pragma clang diagnostic pop
#endif
/*---------------------- End of methods that must have an equivalent in DiskImageCreation.cpp ---------------------------*/


#define CREATION
#define FD_TYPE int
#define VOLUME_TYPE string

#include "FolderReferenceCreation_include.cpp"
//
//void createFolderReference(const string& imgRefPath, DiskImageTestSize size)
//{
//
//    string volume = imgRefPath;
//    DiskImageKindFolder dik; // DiskImageKindFolder is a "fake" DiskImageKind just to be sure that we don't call any method in it.
//
//    _createFolderReference(volume, size, dik);
//}

void createFolderReference(DiskImage di)
{

    string volume = di.mountPoint();
    _createFolderReference(volume, di.size(), di.dik());
}

static bool existInVector(const vector<DiskImage>& refDiVector, DiskImageFs fs, DiskImageTestSize size)
{
	for ( DiskImage di : refDiVector) {
		if ( di.dik().fs() == fs  &&  di.size() == size ) return true;
	}
	return false;
}


vector<DiskImage> createFolderReferenceUsedByDiskImageVector(const vector<DiskImage>& diVector, const string& generatedReferencePathToFolder, bool deleteAndRecreate)
{
	umountAllInFolder(generatedReferencePathToFolder); // just in case.
	if ( deleteAndRecreate ) {
        execAndPrintOutput_T(stringPrintf("rm -rf %s", generatedReferencePathToFolder.c_str()));
	}

	if ( !file_or_dir_exists(generatedReferencePathToFolder) ) {
		mkdir_T(generatedReferencePathToFolder, 0755, stringPrintf("Cannot create '%s'", generatedReferencePathToFolder.c_str()));
	}


	vector<DiskImage> refDiVector;

	for (const DiskImage& dit : diVector) {
		if ( dit.canBeGeneratedOnThisPlateform()  &&  dit.canBeMountedByOS() ) {
			if ( !existInVector(refDiVector, dit.dik().fs(), dit.size()) ) {
				string sizeArgument;
				if ( dit.size() == DISKIMAGE_SIZE_BIG ) sizeArgument = "5g";
				else if ( dit.dik().fs() == DISKIMAGE_FS_APFS) sizeArgument = "3000k";
				else sizeArgument = "2000k";
				
				DiskImage refDi(generatedReferencePathToFolder, "", dit.size(), DISKIMAGE_TYPE_SPARSE, dit.dik().fs(), DISKIMAGE_LAYOUT_GPT, DISKIMAGE_ENCRYPTION_NONE);
				if ( !refDi.exist() ) {
					refDi.create_T(sizeArgument);
					refDi.mount_T(false);
					createFolderReference(refDi);
					refDi.eject_T();
				}
				refDiVector.push_back(refDi);
			}
		}
	}
	return refDiVector;
}


void ejectFolderReferenceUsedByDiskImageToTestVector_T(const vector<DiskImageToTest>& ditVector)
{
	for (const DiskImageToTest& dit : ditVector) {
		if ( dit.refDi().isMounted() ) {
			dit.refDi().eject_T();
		}
	}
}

void ejectFolderReferenceUsedByDiskImageToTestVector(const vector<DiskImageToTest>& ditVector)
{
	for (const DiskImageToTest& dit : ditVector) {
		if ( dit.refDi().isMounted() ) {
			dit.refDi().eject();
		}
	}
}

#endif
