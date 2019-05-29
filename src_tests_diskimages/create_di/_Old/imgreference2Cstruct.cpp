#include <stdio.h>
#include <errno.h>
#include <pwd.h> // for getpass()
#include <unistd.h> // for getpass()
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>

#include <memory>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <limits>
#include <functional>
#include <string>
#include <vector>

#include "io.h"
#include "PrintBuf.h"

using namespace ::std;

bool compare(struct dirent a, struct dirent b){
    return strcmp(a.d_name, b.d_name) < 0;
}

int readDirInVector(const string& path, vector<struct dirent>* v)
{
    DIR *dir1 = opendir (path.c_str());
    struct dirent *ent1;

    if ( dir1 != NULL )
    {
        ent1 = readdir (dir1);
        while (ent1 != NULL) {
            if ( strcmp(ent1->d_name, ".") != 0  &&  strcmp(ent1->d_name, "..") != 0 ) v->push_back(*ent1);
//printf ("%s\n", ent1->d_name);
            ent1 = readdir (dir1);
        }
        closedir (dir1);
        std::sort(v->begin(), v->end(), compare);
        return 0;
    } else {
        return -1;
    }
}


struct name_stat
{
    char            d_name[__DARWIN_MAXPATHLEN];
    uint8_t         d_type;
    nlink_t         st_nlink;         /* Number of hard links */
    uid_t           st_uid;           /* User ID of the file */
    gid_t           st_gid;           /* Group ID of the file */
    struct timespec st_atimespec;     /* time of last access */
    struct timespec st_mtimespec;     /* time of last data modification */
    struct timespec st_ctimespec;     /* time of last status change */
    struct timespec st_birthtimespec; /* time of file creation(birth) */
    off_t           st_size;          /* file size, in bytes */
    uint32_t        st_flags;         /* user defined flags for file */
    vector<uint8_t> content;
    std::vector<struct name_stat> subdir;

};

std::vector<struct name_stat> tmp {
    {
        .d_name = "HardLink1",
        .d_type = 8,
        .st_nlink = 2,
        .st_uid = 501,
        .st_gid = 20,
        .st_atimespec.tv_sec = 1531927712,
        .st_atimespec.tv_nsec = 0,
        .st_mtimespec.tv_sec = 1530530783,
        .st_mtimespec.tv_nsec = 0,
        .st_ctimespec.tv_sec = 1530531501,
        .st_ctimespec.tv_nsec = 0,
        .st_birthtimespec.tv_sec = 1530530001,
        .st_birthtimespec.tv_nsec = 0,
        .st_size = 17,
        .st_flags = 0,
        .content = { 1, 2 }
    }

};

    uint8_t         content[3]  = { 1, 2 };

void printHexBufAsVector(FILE* f, const void* vbuf, size_t count, int ident, int line_length)
{
    uint8_t* buf = (uint8_t*)vbuf;
    for (size_t i = 0; i < count;) {
        fprintf(f, "%*s", ident, "");
        for (int j = 0; j < line_length; j++) {
            if ( i + j < count ) {
                fprintf(f, "0x%02x, ", (uint8_t) (buf[i + j]));
            }else{
                fprintf(f, "      ");
            }
            if ( j%4 == 3 ) fprintf(f, " ");
        }
        fprintf(f, "   // ");
        for (int j = 0; j < line_length; j++) {
            if ( i + j < count ) {
                if ( isprint(buf[i + j]))
                    fprintf(f, "%c", (uint8_t) (buf[i + j]));
                else {
                    if ( buf[i + j] == 0) fprintf(f, "_");
                    else fprintf(f, ".");
                }
            }
        }
        i += line_length;
        fprintf(f, "\n");
//        if ( i < count ) fprintf(f, "%*s", ident, "");
    }
}


string timespec_toString(struct timespec& st_timespec)
{
    struct tm* tm = localtime(&st_timespec.tv_sec);
    char buf[1024];
//    uint64_t nsec = st_timespec.tv_nsec + ((uint64_t)tm->tm_sec)*1000000000;
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
    string rv = buf;
    return rv;
}


void handle_folder(FILE* f, const string& mountPoint, int ident)
{
    std::vector<struct dirent> direntVector;
    int rv = readDirInVector(mountPoint, &direntVector);
    if ( rv == -1 ) {
        fprintf(stderr, "Cannot read dir '%s'. Errno %d\n", mountPoint.c_str(), errno);
    }

    bool neeeSeparator = false;
    for (struct dirent ent : direntVector)
    {
        struct stat stat_buf;
        int ret = lstat((mountPoint+"/"+ent.d_name).c_str(), &stat_buf);
        if ( ret != 0 ) {
            fprintf(stderr, "Cannot stat '%s', errno %d\n", (mountPoint+"/"+ent.d_name).c_str(), errno);
        }
//        if ( neeeSeparator ) fprintf(f, ",\n");
//        else neeeSeparator = true;
        fprintf(f, "%*s{\n", ident, "");
        fprintf(f, "%*s.d_name = \"%s\",\n", ident+4, "", ent.d_name);
        fprintf(f, "%*s.d_type = %d,\n", ident+4, "", ent.d_type);
        fprintf(f, "%*s.st_nlink = %d,\n", ident+4, "", stat_buf.st_nlink);
        fprintf(f, "%*s.st_uid = %d,\n", ident+4, "", stat_buf.st_uid);
        fprintf(f, "%*s.st_gid = %d,\n", ident+4, "", stat_buf.st_gid);
        fprintf(f, "%*s.st_birthtimespec.tv_sec = %ld, // %s\n", ident+4, "", stat_buf.st_birthtimespec.tv_sec, timespec_toString(stat_buf.st_birthtimespec).c_str());
        fprintf(f, "%*s.st_birthtimespec.tv_nsec = %ld,\n", ident+4, "", stat_buf.st_birthtimespec.tv_nsec);
        fprintf(f, "%*s.st_mtimespec.tv_sec = %ld, // %s\n", ident+4, "", stat_buf.st_mtimespec.tv_sec, timespec_toString(stat_buf.st_mtimespec).c_str());
        fprintf(f, "%*s.st_mtimespec.tv_nsec = %ld,\n", ident+4, "", stat_buf.st_mtimespec.tv_nsec);
        fprintf(f, "%*s.st_ctimespec.tv_sec = %ld, // %s\n", ident+4, "", stat_buf.st_ctimespec.tv_sec, timespec_toString(stat_buf.st_ctimespec).c_str());
        fprintf(f, "%*s.st_ctimespec.tv_nsec = %ld,\n", ident+4, "", stat_buf.st_ctimespec.tv_nsec);
        fprintf(f, "%*s.st_atimespec.tv_sec = %ld, // %s\n", ident+4, "", stat_buf.st_atimespec.tv_sec, timespec_toString(stat_buf.st_atimespec).c_str());
        fprintf(f, "%*s.st_atimespec.tv_nsec = %ld,\n", ident+4, "", stat_buf.st_atimespec.tv_nsec);
        fprintf(f, "%*s.st_size = %lld,\n", ident+4, "", stat_buf.st_size);
        fprintf(f, "%*s.st_flags = %d,\n", ident+4, "", stat_buf.st_flags);
        if ( ent.d_type == DT_REG )
        {
            fprintf(f, "%*s.content = {\n", ident+4, "");

            int fd = open_T((mountPoint+"/"+ent.d_name).c_str(), O_RDONLY, 0644, "Cannot open '%s'", (mountPoint+"/"+ent.d_name).c_str());
            char buf[4096];
            ssize_t nblu = read(fd, buf, sizeof(buf));
            while ( nblu > 0)
            {
                printHexBufAsVector(f, buf, nblu, ident+8, 32);
                nblu = read(fd, buf, sizeof(buf));
            }
            fprintf(f, "%*s},\n", ident+4, "");

        }
        if ( ent.d_type == DT_DIR )
        {
            fprintf(f, "%*s.subdir = { \n", ident+4, "");
            handle_folder(f, (mountPoint+"/"+ent.d_name).c_str(), ident+8);
            fprintf(f, "%*s},\n", ident+4, "");
        }

        fprintf(f, "%*s},\n", ident, "");
    }
//    fprintf(f, "%*s};\n", ident+4, "");

//    for (struct dirent ent : direntVector)
//    {
//    }
//
//    for (struct dirent ent : direntVector)
//    {
//        if ( ent.d_type == DT_DIR )
//        {
//            handle_folder(f, (mountPoint+"/"+ent.d_name).c_str(), ent.d_name);
//        }
//    }

}

int main(int argc, char** argv)
{
    string imagesFolderPath = "/JiefLand/5.Devel/Syno/Fuse/hdimount--jief666--wip/src_tests/test_disk_images/ImgReference.dmg";
    // TODO mount ?
    string mountPoint = "/JiefLand/5.Devel/Syno/Fuse/hdimount--jief666--wip/src_tests/test_disk_images/ImgReference";
    
    string outputfile = "/JiefLand/5.Devel/Syno/Fuse/hdimount--jief666--wip/src_tests/test_disk_images/ImgReference.cpp";
    FILE* f = fopen(outputfile.c_str(), "w");
    
    fprintf(f, "struct name_stat\n");
    fprintf(f, "{\n");
    fprintf(f, "    char            d_name[__DARWIN_MAXPATHLEN];\n");
    fprintf(f, "    char            d_type;\n");
    fprintf(f, "    nlink_t         st_nlink;         /* Number of hard links */\n");
    fprintf(f, "    uid_t           st_uid;           /* User ID of the file */\n");
    fprintf(f, "    gid_t           st_gid;           /* Group ID of the file */\n");
    fprintf(f, "    struct timespec st_atimespec;     /* time of last access */\n");
    fprintf(f, "    struct timespec st_mtimespec;     /* time of last data modification */\n");
    fprintf(f, "    struct timespec st_ctimespec;     /* time of last status change */\n");
    fprintf(f, "    struct timespec st_birthtimespec; /* time of file creation(birth) */\n");
    fprintf(f, "    off_t           st_size;          /* file size, in bytes */\n");
    fprintf(f, "    uint32_t        st_flags;         /* user defined flags for file */\n");
    fprintf(f, "    vector<uint8_t> content;\n");
    fprintf(f, "    std::vector<struct name_stat> subdir;\n");
    fprintf(f, "};\n");
    fprintf(f, "\n");

    fprintf(f, "std::vector<struct name_stat> ref_root_list {\n");
    handle_folder(f, mountPoint, 8);
    fprintf(f, "};\n");


}


//Device* g_DevicePtr;
