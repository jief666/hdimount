//
//  Utils
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#include <string>
#include <time.h> // timespec


bool compareDirEnt(struct dirent a, struct dirent b);
bool compareString(const string& a, const string& b);

std::vector<struct dirent> dirContent(const string& path);
string intThousandsSeparator(uint64_t n);
string timespec_toString(struct timespec& st_timespec);
string getFileName(const string& s);

#ifdef __APPLE__

string ejectTool(); // Something like "./bin/eject_diskimage"
string rsyncTool(); // Something like "./bin/rsync-v3.1.3+crtimes"

int compareKernelVersion_T(short int major, short int minor, short int component);
string kernelVersionAsString_T();
bool isKernelVersion_T(short int major, short int minor, short int component);
void umountAllInFolder(const string& folder);

struct timespec get_create_time(const char *path);
struct timespec get_modification_time(const char *path);
struct timespec get_access_time(const char *path);
struct timespec get_change_time(const char *path);
struct timespec get_backup_time(const char *path);

int set_create_time(const char *path, struct timespec ts);
int set_modification_time(const char *path, struct timespec ts);
int set_access_time(const char *path, struct timespec ts);
int set_change_time(const char *path, struct timespec ts);
int set_backup_time(const char *path, struct timespec ts);

#endif
