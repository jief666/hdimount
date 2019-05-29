//
//  io.m
//
//  Created by Jief on 05/April/2017.
//  Copyright (c) 2017 Jief. All rights reserved.
//

#include <unistd.h> // for exec
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _MSC_VER
#include <direct.h>
#else
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/wait.h>
#endif

#include <string>
#include <vector>

#include "../src/Utils.h"

using namespace std;

#include "io.h"

class __fd
{
public:
    int m_fd;
    __fd(int fd) : m_fd(fd) {};
    __fd (const __fd& ) { throw "disabled"; }
    ~__fd() { if (m_fd > 0) { /*printf("close fd=%d\n", m_fd);*/ close(m_fd);} }
    __fd& operator= (int fd) { m_fd = fd; return *this; }
    operator int() const { return m_fd; }
    bool operator< (int val) { return m_fd < val; }
};


void throwf_msg(string errorLabel, string msg)
{
    throw stringPrintf("%s. %s", errorLabel.c_str(), msg.c_str());
}

void throwf_msg_errno(string errorLabel, string msg)
{
    int errnosaved = errno;
    char* strerr = strerror(errnosaved);
    throw stringPrintf("%s. %s : errno(%d) : %s", errorLabel.c_str(), msg.c_str(), errnosaved, strerr ? strerr : "NULL");
}

void throwf_errno(string errorLabel)
{
    int errnosaved = errno;
    throw stringPrintf("%s, errno(%d) : %s", errorLabel.c_str(), errnosaved, strerror(errnosaved));
}


#ifndef MIN
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#endif

//
//void system_T(const string& cmd, string errorLabel)
//{
//    int ret = system(cmd.c_str());
//    if ( ret != 0 ) throwf_errno(errorLabel);
//}
//
//void exec_T(const string& cmd)
//{
//    system_T(cmd.c_str(), stringPrintf("Exec of '%s' failed.", cmd.c_str()));
//}

void split_in_args(std::string command, std::vector<std::string>* qargs)
{
    size_t len = command.length();
    string arg;
    for ( size_t i = 0 ; i < len ; )
    {
        if ( command[i] == '\"' ) {
            i++;
            while ( i < len  &&  command[i] != '\"' ) {
                arg.push_back(command[i]);
                i++;
            }
            i++;
        }
        else if ( command[i] == '\'' ) {
            i++;
            while ( i < len  &&  command[i] != '\'' ) {
                arg.push_back(command[i]);
                i++;
            }
            i++;
        }
        else if ( command[i] == ' ') {
            (*qargs).push_back(arg);
            arg = "";
            while ( i < len  &&  command[i] == ' ' ) i++;
        }else{
            arg.push_back(command[i]);
            i++;
        }
    }
    if (arg.length() > 0 ) (*qargs).push_back(arg);
}

#ifndef _MSC_VER
void execAndPrintOutput_T(const string cmd)
{
#ifdef DEBUG
        printf("==>%s\n", cmd.c_str());
#endif
    vector<string> argsVector;
    split_in_args(cmd, &argsVector);
//for(size_t i=0;i<argsVector.size();i++){
//        printf("argsVector[%zd]=%s\n", i, argsVector[i].c_str());
//}

    int pipeStdOut[2];
    if ( pipe(pipeStdOut) < 0 ) throw stringPrintf("Cannot exec pipe(). Errno %d", errno);
    
    int pipeStdIn[2];
    if ( pipe(pipeStdIn) < 0 ) throw stringPrintf("Cannot exec pipe(). Errno %d", errno);
    
    pid_t pid = fork();
    if ( pid == -1 ) throw stringPrintf("Cannot exec '%s'", cmd.c_str());
    
    if ( pid == 0 )
    {
        // child
        if ( close(pipeStdIn[1]) != 0 ) {    // close write end in the child
            throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
        }
        if ( close(pipeStdOut[0]) != 0 ) {    // close reading end in the child
            throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
        }

        dup2(pipeStdIn[0], 0);  // send stdout to the pipe
        dup2(pipeStdOut[1], 1);  // send stdout to the pipe
        dup2(pipeStdOut[1], 2);  // send stderr to the pipe
        
        if ( close(pipeStdOut[1]) ) {    // this descriptor is no longer needed
            throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
        }

        

        const char* argv[argsVector.size()+1];
        for ( size_t i = 0 ; i < argsVector.size() ; i++ ) argv[i] = strdup(argsVector[i].c_str());
        argv[argsVector.size()] = NULL;
        
        execvp(argv[0], (char* const *)argv);
        throw stringPrintf("execvp failed. Errno %d", errno);
    }
    // parent
    
    char buffer[1024];
    
    close(pipeStdIn[0]);  // close the read end of the pipe in the parent
    close(pipeStdOut[1]);  // close the write end of the pipe in the parent

    if ( write(pipeStdIn[1], "foo", 3) != 3 ) {
        throw stringPrintf("Cannot write to a pipe(). Errno %d", errno);
    }
    if ( close(pipeStdIn[1]) != 0 ) {  // close the read end of the pipe in the parent
        throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
    }

    string output;
    ssize_t nbread = read(pipeStdOut[0], buffer, sizeof(buffer));
    while ( nbread > 0 )
    {
        output.assign(buffer, nbread);
#ifdef DEBUG
		write_all(1, output.data(), output.length()); // if this doesn't work, don't know what to do...
#endif
        nbread = read(pipeStdOut[0], buffer, sizeof(buffer));
    }
    if ( close(pipeStdOut[0]) != 0 ) {  // close the read end of the pipe in the parent
        throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
    }
    int exitCode = -485;
    pid_t wpid = wait(&exitCode);
    while ( wpid != pid ) {
        wpid = wait(&exitCode);
    }
#ifdef DEBUG
        printf("--> exit code : %d\n", exitCode);
#endif
    if ( exitCode != 0 ) {
		throw stringPrintf("Exec of '%s' failed. Process returned %d", cmd.c_str(), exitCode);
	}

}

string execAndGetOutput_T(const string cmd)
{
	string returnValue;
#ifdef DEBUG
        printf("==>%s\n", cmd.c_str());
#endif
    vector<string> argsVector;
    split_in_args(cmd, &argsVector);
//for(size_t i=0;i<argsVector.size();i++){
//        printf("argsVector[%zd]=%s\n", i, argsVector[i].c_str());
//}

    int pipeStdOut[2];
    if ( pipe(pipeStdOut) < 0 ) throw stringPrintf("Cannot exec pipe(). Errno %d", errno);
	
    int pipeStdIn[2];
    if ( pipe(pipeStdIn) < 0 ) throw stringPrintf("Cannot exec pipe(). Errno %d", errno);
	
    pid_t pid = fork();
    if ( pid == -1 ) throw stringPrintf("Cannot exec '%s'", cmd.c_str());
	
    if ( pid == 0 )
    {
        // child
        if ( close(pipeStdIn[1]) != 0 ) {    // close write end in the child
            throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
        }
        if ( close(pipeStdOut[0]) != 0 ) {    // close reading end in the child
            throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
        }

        dup2(pipeStdIn[0], 0);  // send stdout to the pipe
        dup2(pipeStdOut[1], 1);  // send stdout to the pipe
        dup2(pipeStdOut[1], 2);  // send stderr to the pipe
		
        if ( close(pipeStdOut[1]) ) {    // this descriptor is no longer needed
            throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
        }

		

        const char* argv[argsVector.size()+1];
        for ( size_t i = 0 ; i < argsVector.size() ; i++ ) argv[i] = strdup(argsVector[i].c_str());
        argv[argsVector.size()] = NULL;
		
        execvp(argv[0], (char* const *)argv);
        throw stringPrintf("execvp failed. Errno %d", errno);
    }
    // parent
	
    char buffer[1024];
	
    close(pipeStdIn[0]);  // close the read end of the pipe in the parent
    close(pipeStdOut[1]);  // close the write end of the pipe in the parent

    if ( write(pipeStdIn[1], "foo", 3) != 3 ) {
        throw stringPrintf("Cannot write to a pipe(). Errno %d", errno);
    }
    if ( close(pipeStdIn[1]) != 0 ) {  // close the read end of the pipe in the parent
        throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
    }

    string output;
    ssize_t nbread = read(pipeStdOut[0], buffer, sizeof(buffer));
    while ( nbread > 0 )
    {
        returnValue += string(buffer, nbread);
#ifdef DEBUG
//		write_all(1, output.data(), output.length()); // if this doesn't work, don't know what to do...
#endif
        nbread = read(pipeStdOut[0], buffer, sizeof(buffer));
    }
    if ( close(pipeStdOut[0]) != 0 ) {  // close the read end of the pipe in the parent
        throw stringPrintf("Cannot close a pipe(). Errno %d", errno);
    }
    int exitCode = -485;
    pid_t wpid = wait(&exitCode);
    while ( wpid != pid ) {
        wpid = wait(&exitCode);
    }
#ifdef DEBUG
        printf("--> exit code : %d\n", exitCode);
#endif
    if ( exitCode != 0 ) {
		throw stringPrintf("Exec of '%s' failed. Process returned %d", cmd.c_str(), exitCode);
	}
	return returnValue;
}

#endif

bool file_or_dir_exists(const string& path)
{
    struct stat stat_buf;
    int rv = stat(path.c_str(), &stat_buf);
    return rv == 0;
}

void stat_T(const string& name, struct stat* st, string errorLabel)
{
    int ret = stat(name.c_str(), st);
    if ( ret != 0 ) throwf_errno(errorLabel);
}

#ifndef _MSC_VER
void lstat_T(const string& name, struct stat* st, string errorLabel)
{
    int ret = lstat(name.c_str(), st);
    if ( ret != 0 ) throwf_errno(errorLabel);
}
#endif

#ifdef __APPLE__
void statfs_T(const string& name, struct statfs* st, string errorLabel)
{
    int ret = statfs(name.c_str(), st);
    if ( ret != 0 ) throwf_errno(errorLabel);
}
#endif

void mkdir_T(const string& path, mode_t mode, string errorLabel)
{
#ifdef _MSC_VER
		int rv = _mkdir(path.c_str());
#else
    int rv = mkdir(path.c_str(), mode);
#endif
    if ( rv == -1 ) throwf_errno(errorLabel);
}

int open_T(const string& name, int flags, mode_t mode, string errorLabel)
{
    int fd = open(name.c_str(), flags, mode);
    if ( fd == -1 ) throwf_errno(errorLabel);
    return fd;
}

int close_T(int fd, string errorLabel)
{
    int ret = close(fd);
    if ( ret == -1 ) throwf_errno(errorLabel);
    return fd;
}
//
//size_t readZ_T(int fd, void* buf, size_t size, const string& printFormat, ...)
//{
//
//    size_t nbluTotal = 0;
//    while ( 1 )
//    {
//        ssize_t nblu = read(fd, ((uint8_t*)buf)+nbluTotal, MIN(4096, size-nbluTotal));
//        //        NSLog(@"read %zd bytes, size=%zd, nbluTotal=%zd, size-nbluTotal=%zd", nblu, size, nbluTotal, size-nbluTotal);
//        if ( nblu == -1 ) vthrow_errno(printFormat);
//        if ( nblu == 0 ) {
//            return nbluTotal;
//        }
//        nbluTotal += nblu;
//    }
//}

/*
 * Crash if buf is not big enough
 */
size_t read_until_end_T(int fd, void* buf, string errorLabel)
{
    
    size_t nbluTotal = 0;
    while ( 1 )
    {
        ssize_t nblu = read(fd, ((uint8_t*)buf)+nbluTotal, 65536);
//        NSLog(@"read %zd bytes, size=%zd, nbluTotal=%zd, size-nbluTotal=%zd", nblu, size, nbluTotal, size-nbluTotal);
        if ( nblu == -1 ) throwf_errno(errorLabel);
        if ( nblu == 0 ) {
            return nbluTotal;
        }
        nbluTotal += nblu;
    }
}

/*
 * Read size bytes from fd
 * throw an exception if less than size bytes are read
 */
void read_all_T(int fd, void* buf, size_t size, string errorLabel)
{
    
    size_t nbluTotal = 0;
    while ( nbluTotal < size )
    {
        ssize_t nblu = read(fd, ((uint8_t*)buf)+nbluTotal, MIN(65536, size-nbluTotal));
        //        NSLog(@"read %zd bytes, size=%zd, nbluTotal=%zd, size-nbluTotal=%zd", nblu, size, nbluTotal, size-nbluTotal);
        if ( nblu == -1 ) throwf_errno(errorLabel);
        if ( nblu == 0 ) {
            if ( nbluTotal != size ) {
                //throwf_msg(printFormat, [NSString stringWithFormat:@"Read only %zd chars instead of %zd", nbluTotal, size]);
                char msg[2048];
                snprintf(msg, sizeof(msg), "Read only %zd chars instead of %zd", nbluTotal, size);
                if ( nbluTotal != size ) throwf_msg(errorLabel, msg);
            }
            //            if ( nbluTotal != size ) {
            //                va_list args;
            //                va_start(args, printFormat);
            //                NSString* msg1 = [[NSString alloc] initWithFormat:[NSString stringWithUTF8String:printFormat] arguments:args];
            //                NSString* msg2 = [NSString stringWithFormat:@"%@. Read only %zd chars instead of %zd", msg1, nbluTotal, size];
            //                va_end(args);
            //                @throw msg2;
            //            }
            return;
        }
        nbluTotal += nblu;
    }
}

/*
 * Read size bytes from fd
 * throw an exception if less than size bytes are read
 */
string read_all_T(int fd, size_t size, string errorLabel)
{
    char* buf = (char*)alloca(size);
    size_t nbluTotal = 0;
    while ( nbluTotal < size )
    {
        ssize_t nblu = read(fd, ((uint8_t*)buf)+nbluTotal, MIN(65536, size-nbluTotal));
        //        NSLog(@"read %zd bytes, size=%zd, nbluTotal=%zd, size-nbluTotal=%zd", nblu, size, nbluTotal, size-nbluTotal);
        if ( nblu == -1 ) throwf_errno(errorLabel);
        if ( nblu == 0 ) {
            if ( nbluTotal != size ) throwf_msg(errorLabel, stringPrintf("Read only %zd chars instead of %zd", nbluTotal, size));
            return string(buf, nblu);
        }
        nbluTotal += nblu;
    }
    return string(buf, nbluTotal);
}

ssize_t write_all(int fd, const void* buf, size_t size)
{
	assert(size < SSIZE_MAX);
	
	size_t nbwrTotal = 0;
	while ( nbwrTotal < size )
	{
		ssize_t nbwr = write(fd, ((uint8_t*)buf)+nbwrTotal, MIN(65536, size-nbwrTotal));
		//NSLog(@"write %zd bytes, size=%zd, nbluTotal=%zd, size-nbluTotal=%zd", nblu, size, nbluTotal, size-nbluTotal);
		if ( nbwr == -1 ) return -1;
		nbwrTotal += nbwr;
		if ( nbwr == 0  &&  nbwrTotal < size ) return -1;
	}
	return nbwrTotal;
}

void write_all_T(int fd, const void* buf, size_t size, string errorLabel)
{
     size_t nbwrTotal = 0;
     while ( nbwrTotal < size )
     {
         ssize_t nbwr = write(fd, ((uint8_t*)buf)+nbwrTotal, MIN(65536, size-nbwrTotal));
 //NSLog(@"write %zd bytes, size=%zd, nbluTotal=%zd, size-nbluTotal=%zd", nblu, size, nbluTotal, size-nbluTotal);
         if ( nbwr == -1 ) throwf_msg_errno(errorLabel, stringPrintf("Cannot write"));
         if ( nbwr == 0 ) {
             if ( nbwrTotal != size ) {
                 throwf_msg_errno(errorLabel, stringPrintf("Wrote only %zd chars instead of %zd", nbwrTotal, size));
             }
             return;
         }
         nbwrTotal += nbwr;
     }
 }

void write_at_path_T(const string& path, const void* buf, size_t count, string errorLabel)
{
	__fd fd = open(path.c_str(), O_TRUNC|O_CREAT|O_WRONLY, 0644);
	if ( fd < 0 ) {
        throwf_msg_errno(errorLabel, "Cannot open");
	}
    write_all_T(fd, buf, count, errorLabel);
}

void write_at_path_T(const string& path, const string& buf, string errorLabel)
{
    __fd fd = open(path.c_str(), O_TRUNC|O_CREAT|O_WRONLY, 0644);
    if ( fd < 0 ) {
        throwf_msg_errno(errorLabel, "Cannot open");
    }
    write_all_T(fd, buf.data(), buf.length(), errorLabel);
}

void append_at_path_T(const string& path, const string& buf, string errorLabel)
{
    __fd fd = open(path.c_str(), O_APPEND|O_CREAT|O_WRONLY, 0644);
    if ( fd < 0 ) {
        throwf_msg_errno(errorLabel, "Cannot open");
    }
    write_all_T(fd, buf.data(), buf.length(), errorLabel);
}

string read_all_at_path_T(const string& path, string errorLabel)
{
    struct stat st_stat;
    int rv = stat(path.c_str(), &st_stat);
    if ( rv != 0 ) throwf_msg_errno(errorLabel, "Cannot stat");

	char* buf = (char*)alloca(st_stat.st_size);

    __fd fd = open(path.c_str(), O_RDONLY, 0644);
    if ( fd < 0 ) {
        throwf_msg_errno(errorLabel, "Cannot open");
    }
    read_all_T(fd, buf, st_stat.st_size, errorLabel);
    return string(buf, st_stat.st_size);
}

void lseek_set_T(int fd, off_t offset, string errorLabel)
{
    off_t ret = lseek(fd, offset, SEEK_SET);
    if ( ret != offset ) throwf_errno(errorLabel);
}

off_t lseek_end_T(int fd, string errorLabel)
{
    off_t ret = lseek(fd, 0, SEEK_END);
    if ( ret == -1 ) throwf_errno(errorLabel);
    return ret;
}

//
//size_t mysearch_T(int fd1, size_t blocksize, const void* buf_to_search, off_t size_to_search, const string& printFormat, ...)
//{
//    blocksize = MAX(blocksize, size_to_search);
//    uint8_t* buf = (uint8_t*)malloc(blocksize*2);
//
//    size_t offset = 0;
//    size_t nblu = readZ_T(fd1, buf, blocksize*2, "mysearch_T trying to read.");
//size_t next_read_offset = nblu;
//size_t offset_block1 = 0;
//size_t offset_block2 = blocksize;
////NSLog(@"read %zd bytes", nblu);
//    if ( nblu < size_to_search ) return -1;
//    while ( 1 )
//    {
//        for (int i=0 ; i<=nblu-size_to_search ; i++ )
//        {
//            if ( memcmp(buf_to_search, buf+i, size_to_search) == 0 )
//            {
////NSLog(@"found");
//                return offset + i;
//            }
//        }
//        if ( nblu < blocksize*2 ) return -1;
//offset_block1 = next_read_offset;
//        size_t nblu2 = readZ_T(fd1, buf, blocksize, "mysearch_T trying to read.");
//next_read_offset += nblu2;
//        if ( nblu2 == 0 ) return -1;
////NSLog(@"read2 %zd bytes", nblu2);
//        size_t nb_2part_search = MIN(nblu2, size_to_search);
//        for (int i=0 ; i<nb_2part_search ; i++ )
//        {
//            if ( memcmp(buf_to_search, buf+blocksize*2-(size_to_search-i), size_to_search-i) == 0 ) {
//                if ( memcmp(((uint8_t*)buf_to_search)+size_to_search-i, buf, i) == 0 )
//                {
////NSLog(@"found");
//                    return offset + blocksize + i;
//                }
//            }
//        }
//        if ( nblu2 == blocksize ) {
//offset_block2 = next_read_offset;
//            nblu = readZ_T(fd1, buf+blocksize, blocksize, "mysearch_T trying to read.");
//next_read_offset += nblu;
////NSLog(@"read3 %zd bytes", nblu2);
//            nblu = blocksize + nblu;
//        }else{
//            nblu = nblu2;
//        }
//        offset += blocksize*2;
//    }
//}





