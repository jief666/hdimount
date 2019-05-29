//
//  io.h
//
//  Created by Jief on 05/April/2017.
//  Copyright (c) 2017 Jief. All rights reserved.
//
//std::string stringPrintf(const std::string fmt, ...) /*__attribute__ (( format( printf, 1, 2 ) )) */ ;
#include <string>

//void system_T(const std::string& cmd, std::string errorLabel);
void lstat_T(const std::string& name, struct stat* st, std::string errorLabel);
void stat_T(const std::string& name, struct stat* st, std::string errorLabel);
#ifdef __APPLE__
void statfs_T(const std::string& name, struct statfs* st, std::string errorLabel);
#endif
void mkdir_T(const std::string& path, mode_t mode, std::string errorLabel);
int open_T(const std::string& name, int flags, mode_t mode, std::string errorLabel);
int close_T(int fd, std::string errorLabel);
size_t readZ_T(int fd, void* buf, size_t size, std::string errorLabel);

std::string read_all_T(int fd, size_t size, std::string errorLabel);
void read_all_T(int fd, void* buf, size_t size, std::string errorLabel);
std::string read_all_at_path_T(const std::string& path, std::string errorLabel);

size_t read_until_end_T(int fd, void* buf, std::string errorLabel);


ssize_t write_all(int fd, const void* buf, size_t size);
void write_all_T(int fd, const void* buf, size_t size, std::string errorLabel);
void write_at_path_T(const std::string& path, const void* buf, size_t count, std::string errorLabel);
void write_at_path_T(const std::string& path, const std::string& buf, std::string errorLabel);
void append_at_path_T(const std::string& path, const std::string& buf, std::string errorLabel);



void lseek_set_T(int fd, off_t offset, std::string errorLabel);
off_t lseek_end_T(int fd, std::string errorLabel);
size_t mysearch_T(int fd1, size_t blocksize, const void* buf_to_search, off_t size, std::string errorLabel);




//void exec_T(const std::string cmd);
void execAndPrintOutput_T(const std::string cmd);
std::string execAndGetOutput_T(const std::string cmd);
bool file_or_dir_exists(const std::string& path);
