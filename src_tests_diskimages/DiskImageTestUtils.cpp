#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>
#ifndef _MSC_VER
#ifndef __XATTR_H__
#include <sys/xattr.h>
#endif
#include <sys/mount.h>
#include <sys/dir.h>
#include <sys/sysctl.h>
#else
#include <fuse.h> // for FUSE_STAT
#endif

#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm> // for sort
#include <sstream>

using namespace ::std;

#include "../src/Utils.h"
#include "io.h"
#include "../src/FuseVolume.h"
#include "../src/OpenDisk.h"

std::string listToString(std::vector<string> l)
{
    std::stringstream ss;
    for(std::vector<string>::iterator it = l.begin(); it!=l.end(); ++it){
        ss << *it;
        if(std::distance(it,l.end())>1)
            ss << ", ";
    }
    return "[" + ss.str()+ "]";
}

vector<string> binPath = { "./bin", "./src_tests_diskimages/bin", "../hdimount/src_tests_diskimages/bin" };
string ejectTool()
{
    for (string s : binPath) {
        if ( file_or_dir_exists(s+"/eject_diskimage") ) return s+"/eject_diskimage";
    }
    throw stringPrintf("Cannot find eject_diskimage in %s", listToString(binPath).c_str());
}

string rsyncTool()
{
    for (string s : binPath) {
        if ( file_or_dir_exists(s+"/rsync-v3.1.3+crtimes") ) return s+"/rsync-v3.1.3+crtimes";
    }
    throw stringPrintf("Cannot find rsync-v3.1.3+crtimes in %s", listToString(binPath).c_str());
}


bool compareDirEnt(struct dirent a, struct dirent b){
    return strcmp(a.d_name, b.d_name) < 0;
}

bool compareString(const string& a, const string& b){
    return strcmp(a.c_str(), b.c_str()) < 0;
}

std::vector<struct dirent> dirContent(const string& path)
{
    std::vector<struct dirent> returnValue;
    
    DIR *dir1 = opendir (path.c_str());
    struct dirent ent1_buf;
    struct dirent* ent1;

    if ( dir1 != NULL )
    {
        int res = readdir_r(dir1, &ent1_buf, &ent1);
        while ( res == 0  &&  ent1 != NULL) {
            if ( strcmp(ent1->d_name, ".") != 0  &&  strcmp(ent1->d_name, "..") != 0  &&  strcmp(ent1->d_name, ".fseventsd") != 0  &&  strcmp(ent1->d_name, ".Trashes") != 0 ) {
//printf("dirContent add '%s'\n", ent1->d_name);
                returnValue.push_back(*ent1);
            }
//printf ("%s\n", ent1->d_name);
            res = readdir_r(dir1, &ent1_buf, &ent1);
        }
        closedir (dir1);
        std::sort(returnValue.begin(), returnValue.end(), compareDirEnt);
        return returnValue;
    } else {
        throw stringPrintf("Cannot open dir '%s'", path.c_str());
    }
}

string intThousandsSeparator(uint64_t n) {
//    if (n < 0) {
//        printf ("-");
//        printfcomma (-n);
//        return;
//    }
    if (n < 1000) {
        char buf[1024];
        snprintf(buf, sizeof(buf), "%" PRIu64, n);
        string rv = buf;
        return rv;
    }
    string rv;
    rv += intThousandsSeparator(n/1000);
//    printf (",%03d", n%1000);
    char buf[1024];
    snprintf(buf, sizeof(buf), ",%03" PRIu64, n%1000);
    rv += buf;
    return rv;
}

string timespec_toString(struct timespec& st_timespec)
{
    struct tm* tm = localtime(&st_timespec.tv_sec);
    char buf[1024];
//    uint64_t nsec = st_timespec.tv_nsec + ((uint64_t)tm->tm_sec)*1000000000;
#ifdef _MSC_VER
    snprintf(buf, sizeof(buf), "%" PRId64 "=%02d/%02d/%04d %02d:%02d:%02d.%ld", st_timespec.tv_sec, tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec, st_timespec.tv_nsec);
#else
    snprintf(buf, sizeof(buf), "%ld=%02d/%02d/%04d %02d:%02d:%02d.%ld", st_timespec.tv_sec, tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min, tm->tm_sec, st_timespec.tv_nsec);
#endif
    string rv = buf;
    return rv;
}


string getFileName(const string& s)
{
   char sep = '/';
#ifdef _WIN32
   sep = '\\';
#endif
   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      return(s.substr(i+1, s.length() - i));
   }
   return("");
}

#ifdef __APPLE__
/* kernel version as major minor component*/
struct kern {
    short int version[3];
};

void getKernelVersion_T(short int* actual_major, short int* actual_minor, short int* actual_component)
{
    char str[256] = {0};
    size_t size = sizeof(str);
    int ret = sysctlbyname("kern.osrelease", str, &size, NULL, 0);
    if (ret != 0) throw string("sysctlbyname(\"kern.osrelease\") failed");
    if ( sscanf(str, "%hd.%hd.%hd", actual_major, actual_minor, actual_component) != 3 ) throw "sysctlbyname(\"kern.osrelease\") sscanf failed";
//printf("kern.osrelease %d %d %d\n", actual_major, actual_minor, actual_component);
}

/* compare os version with a specific one
0 is equal
negative value if the installed version is less
positive value if the installed version is more
*/
int compareKernelVersion_T(short int major, short int minor, short int component)
{
    short int actual_major;
    short int actual_minor;
    short int actual_component;

    getKernelVersion_T(&actual_major, &actual_minor, &actual_component);
    if ( actual_major !=  major) return actual_major - major;
    if ( actual_minor !=  minor) return actual_minor - minor;
    if ( actual_component !=  component) return actual_component - component;
    return 0;
}

string kernelVersionAsString_T()
{
    short int actual_major;
    short int actual_minor;
    short int actual_component;

    getKernelVersion_T(&actual_major, &actual_minor, &actual_component);
    return stringPrintf("%hd.%hd.%hd", actual_major, actual_minor, actual_component);
}

bool isKernelVersion_T(short int major, short int minor, short int component)
{
//printf("isKernelVersion_T(%d, %d, %d) %d\n", major, minor, component, compareKernelVersion_T(major, minor, component) == 0);
    return compareKernelVersion_T(major, minor, component) == 0;
}

void umountAllInFolder(const string& folder)
{
    if ( file_or_dir_exists(folder) )
    {
        struct statfs statfs_folder;
        statfs_T(folder, &statfs_folder, stringPrintf("Cannot statfs '%s'", folder.c_str()));
        
        std::vector<struct dirent> direntVector = dirContent(folder);
        for (dirent d : direntVector)
        {
            struct statfs statfs_d;
            statfs_T(folder+"/"+d.d_name, &statfs_d, stringPrintf("Cannot statfs '%s'", (folder+"/"+d.d_name).c_str()));
            
            if ( strcmp(statfs_folder.f_mntonname, statfs_d.f_mntonname) != 0 ) { // I donmt have eject_mountpoint yet, so call eject_diskimage on everything.
                string cmd = stringPrintf("diskutil eject \"%s/%s\"", folder.c_str(), d.d_name);
                printf("==>%s\n", cmd.c_str());
                execAndPrintOutput_T(cmd);
            }
        }
    }
}


#pragma pack(push, 4)
struct create_time {
    uint32_t length;
    struct timespec time;
};
#pragma pack(pop)

struct timespec _get_time(const char *path, attrgroup_t attr)
{
    static struct create_time attrBuf;
    struct attrlist attrList;

    memset(&attrList, 0, sizeof attrList);
    attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
    attrList.commonattr = attr;
    if (getattrlist(path, &attrList, &attrBuf, sizeof attrBuf, FSOPT_NOFOLLOW) < 0) return { 0, 0 };
//rprintf(FINFO, "get_create_time %s %ld\n", path, attrBuf.crtime.tv_sec);
    return attrBuf.time;
}

struct timespec get_create_time(const char *path)
{
    return _get_time(path, ATTR_CMN_CRTIME);
}

struct timespec get_modification_time(const char *path)
{
    return _get_time(path, ATTR_CMN_MODTIME);
}

struct timespec get_access_time(const char *path)
{
    return _get_time(path, ATTR_CMN_ACCTIME);
}

struct timespec get_change_time(const char *path)
{
    return _get_time(path, ATTR_CMN_CHGTIME);
}

struct timespec get_backup_time(const char *path)
{
    return _get_time(path, ATTR_CMN_BKUPTIME);
}

static int _set_time(const char *path, attrgroup_t attr, struct timespec ts)
{
    struct attrlist attrList;
    memset(&attrList, 0, sizeof attrList);
    attrList.bitmapcount = ATTR_BIT_MAP_COUNT;
    attrList.commonattr = attr;
    return setattrlist(path, &attrList, &ts, sizeof ts, FSOPT_NOFOLLOW);
}

int set_create_time(const char *path, struct timespec ts)
{
    return _set_time(path, ATTR_CMN_CRTIME, ts);
}

int set_modification_time(const char *path, struct timespec ts)
{
    return _set_time(path, ATTR_CMN_MODTIME, ts);
}

int set_access_time(const char *path, struct timespec ts)
{
    return _set_time(path, ATTR_CMN_ACCTIME, ts);
}

int set_change_time(const char *path, struct timespec ts)
{
    return _set_time(path, ATTR_CMN_CHGTIME, ts);
}

int set_backup_time(const char *path, struct timespec ts)
{
    return _set_time(path, ATTR_CMN_BKUPTIME, ts);
}

#endif
