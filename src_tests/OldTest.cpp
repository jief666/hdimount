//
//  TestFile.cpp
//  hdimount--jief666--wip
//
//  Created by jief on 15/07/2018.
//  Copyright © 2018 jf-luce. All rights reserved.
//

#include "Test.h"
#include "FuseVolume.h"

#include <memory> // for unique_ptr
#include <random> // mt19937
#include <iostream> // cout, etc.
#include <string.h> // memcmp
#include <unistd.h> // for windows compatibility (see custom unistd.h in solution dir)

#include <stdio.h>
#include <fcntl.h>
#include <inttypes.h>

#include "../src/darling-dmg/src/exceptions.h"
#include "../src/darling-dmg/src/FileReader.h"
#include "../src/darling-dmg/src/HFSHighLevelVolume.h"
#include "Utils.h"

#ifndef _MSC_VER
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-variable"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif
#endif

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

static int myFiller (void *buf, const char *name, const struct stat *stbuf, off_t off)
#ifndef _MSC_VER
         __attribute__ ((unused))
#endif
	     ;

static int myFiller (void *buf, const char *name, const struct stat *stbuf, off_t off)
{
	printf("myFiller : name=%s\n", name);
	return 0;
}

void TestTmp(std::unique_ptr<FuseVolume>& g_volumePtr)
{
	const char* path;
	//path = "/Backup/Program Files/Prograns/Condor/Condor.exe";
	path = "/root/testfile_compressed.txt";
//	path = "/file+quarantine";
	//path = "/";
	path = "/ATI Driver.txt";
	path = "/.DocumentRevisions-V100/AllUIDs/1619/com.apple.documentVersions";
	try
  	{
		size_t rv1;
		size_t rv2;
		struct FUSE_STAT st_stat1;
		struct FUSE_STAT st_stat2;
		char buf[1000000];

		if ( !g_volumePtr )
			throw io_error("g_volumePtr is null");

		rv1 = g_volumePtr->getattr(path, &st_stat1);
//		TestFile(g_volumePtr, path);

		//uint8_t* buf2 = &buf[192513];

		////struct stat stat = g_volumePtr->stat("titi");
		//char buf[1025];
		//size_t rv;
		//
		//std::string path = std::string("/\0\0\0\0HFS+ Private Data/iNode88", 30);
		//st_stat1 = g_volumePtr->stat(path);

		//rv2 = fuse_getattr("/", &st_stat2);

		//rv1 = stat("/JiefLand/5.Devel/Syno/Fuse/mnt_apfs-fuse/root/HardLink1", &st_stat1);
		//rv2 = stat("/JiefLand/5.Devel/Syno/Fuse/mnt_apfs-fuse/root/HardLink2", &st_stat2);
//		rv1 = stat("/Volumes/bughardlink2APFS/HardLink1", &st_stat1);
//		rv2 = stat("/Volumes/bughardlink2APFS/HardLink2", &st_stat2);
		//rv2 = fuse_getattr("PkgInfo", &st_stat2);
		//printf("rv=%zd, errno=%d\n", rv, errno);
		//rv = fuse_listxattr("/HardLink1", (char*)buf, sizeof(buf));
		//rv = fuse_listxattr("/.apdisk", (char*)buf, sizeof(buf));
		//rv = fuse_listxattr("/ITER/Readme.txt", (char*)buf, sizeof(buf));
		//rv = fuse_getxattr("/ITER/Readme.txt", "com.apple.FinderInfo", NULL, 0, 0);
		//rv = fuse_getattr("/Desktop/FTPServer.app/Contents/Info.plist", &st_stat2);
		//exit(-1);

		//fuse_fill_dir_t
		//using namespace std::literals::string_literals;

		//auto str = "\0\0\0\0HFS+ Private Data"s;
		//std::map<std::string, struct stat> contents;
		//contents = g_volumePtr->listDirectory(str);

//		g_volumePtr->readdir("/", buf, myFiller, 0, NULL);
//		g_volumePtr->readdir("/", buf, myFiller, 0, NULL);
//		g_volumePtr->readdir("/", buf, myFiller, 0, NULL);

		printf("done\n");
	}
	catch(const io_error& e){
		printf("EXCEPTION in TestTmp %s\n", e.what());
	}
	catch(const std::runtime_error& e){
		printf("EXCEPTION in TestTmp %s\n", e.what());
	}
	catch(const std::exception& e){
		printf("EXCEPTION in TestTmp %s\n", e.what());
	}
	catch (...) {
		printf("EXCEPTION in TestTmp\n");
	}
}

//	int fd = open("/JiefLand/5.Devel/Syno/Fuse/hdimount--jief666--wip/dump", O_CREAT|O_RDWR|O_TRUNC, 0644);

void ReadAndCompare(std::shared_ptr<Reader> reader1, std::shared_ptr<Reader> reader2, int32_t count, uint64_t offset)
{
	char* buf1 = (char*)alloca(count);
	char* buf2 = (char*)alloca(count);

	int32_t nb_read_ref = reader1->read(buf1, count, offset);
	if (nb_read_ref != count)
		throw std::runtime_error(stringPrintf("Cannot read %d bytes. Returns %d. Errno %d.", count, nb_read_ref, errno));

	int32_t nb_read = reader2->read(buf2, count, offset);
	if (nb_read != count)
		throw std::runtime_error(stringPrintf("Cannot read %d bytes. Returns %d. Errno %d.", count, nb_read, errno));

	if (memcmp(buf1, buf2, count)!=0)
		throw std::runtime_error("oups");

}

void TestReader(std::shared_ptr<Reader> reader, const char* path_to_reference)
{
	std::shared_ptr<FileReader> refReader = std::make_shared<FileReader>(path_to_reference);
	
	int32_t blockSize = 512;
	char* bufRef = (char*)alloca(blockSize);
	char* buf = (char*)alloca(blockSize);

	printf("test reading by block of %d\n", blockSize);
	uint64_t fileLen = reader->length();
	for(uint64_t offset=0 ; offset<fileLen ; offset+=blockSize)
	{
		int32_t len = (int32_t)MIN(uint64_t(blockSize), fileLen-offset);
//printf("Read offset=%" PRId64 ", count=%" PRId64 "\n", offset, len);
		try {
			ReadAndCompare(refReader, reader, blockSize, offset);
		}catch(...){
			printf("Failed for offset=%" PRId64 ", count=%d\n", offset, blockSize);
			ReadAndCompare(refReader, reader, blockSize, offset);
		}
	}

	printf("test reading random offset and length\n");
	{
		std::mt19937_64 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937_64::result_type> dist1(0, fileLen-1); // distribution in range [1, 6]

printf("dist1(rng)=%" PRId64 "\n", dist1(rng));

		for(size_t i=0 ; i<10000 ; i++)
		{
			uint64_t offset = dist1(rng);
			std::uniform_int_distribution<std::mt19937::result_type> dist2(1, (unsigned int)MIN(2000000L, fileLen-offset));
			int32_t len = dist2(rng);
printf("Read offset=%" PRId64 ", count=%d\n", offset, len);
			try {
				ReadAndCompare(refReader, reader, len, offset);
			}catch(...){
				printf("Failed for offset=%" PRId64 ", count=%d\n", offset, blockSize);
				try {
					ReadAndCompare(refReader, reader, len, offset);
				}catch(...){
					try{
						ReadAndCompare(refReader, reader, len, offset);
					}catch(...){
						try{
							ReadAndCompare(refReader, reader, len, offset);
						}catch(...){
						}
					}
				}
			}
		}
	}
}

void TestFile(std::unique_ptr<FuseVolume>& g_volumePtr, const char* path)
{
	try
  	{
		size_t rv;

		struct FUSE_STAT st;
		rv = g_volumePtr->getattr(path, &st);
		if (rv != 0)
            throw std::runtime_error(stringPrintf("Cannot stat '%s'. Error %zu.", path, rv));

		struct fuse_file_info info;
		rv = g_volumePtr->open(path, &info);
		if (rv != 0)
            throw std::runtime_error(stringPrintf("Cannot open '%s'. Error %zu.", path, rv));
		uint64_t fileLen = st.st_size;
		char* buf1 = (char*)alloca(fileLen);
		char* buf2 = (char*)alloca(fileLen);
		int32_t nb_read = g_volumePtr->read(path, (char*)buf1, sizeof(buf1), 0, &info);
		if (nb_read != int32_t(sizeof(buf1))) // Don't use a buffer bigger than 2GB !! :-)
            throw std::runtime_error(stringPrintf("Cannot read %lu bytes from '%s'. Returns %d. Errno %d.", sizeof(buf1), path, nb_read, errno));

		std::mt19937_64 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937_64::result_type> dist1(0, fileLen-1); // distribution in range [1, 6]

		printf("dist1(rng)=%" PRId64 "\n", dist1(rng));


//		for(size_t i=0 ; i<10000 ; i++)
//		{
//			size_t offset = dist1(rng);
//			std::uniform_int_distribution<std::mt19937::result_type> dist2(1, fileLen-offset);
//			size_t len = dist2(rng);
//		printf("Read offset=%zd, count=%ld\n", offset, len);
//			try {
//				rv = file->read(buf2+offset, len, offset);
//				if (memcmp(buf1+offset, buf2+offset, len)!=0)
//					throw io_error("oups");
//			}catch(...){
//				rv = file->read(buf2+i, len, offset);
//			}
//		}
		// test read seq
		int32_t blockSize = 65536;
		for(uint64_t i=0 ; i<fileLen ; i+=blockSize)
		{
			uint64_t offset = i;
			int32_t len = (int32_t)MIN(uint64_t(blockSize), fileLen-offset);
printf("Read offset=%" PRId64 ", count=%d\n", offset, len);
			try {
				nb_read = g_volumePtr->read(path, (char*)buf2+offset, len, offset, &info);
				if (nb_read != len)
					throw std::runtime_error(stringPrintf("Cannot read %d bytes from '%s'. Returns %d. Errno %d.", len, path, nb_read, errno));
				if (memcmp(buf1+offset, buf2+offset, len)!=0)
					throw std::runtime_error("oups");
			}catch(...){
				rv = g_volumePtr->read(path, (char*)buf2+i, len, offset, &info);
				if (nb_read != len)
					throw std::runtime_error(stringPrintf("Cannot read %d bytes from '%s'. Returns %d. Errno %d.", len, path, nb_read, errno));
			}
		}
	}
	catch(const std::runtime_error& e){
		printf("EXCEPTION in TestFile %s\n", e.what());
	}
	catch (std::exception e) {
		printf("EXCEPTION in TestFile : %s\n", e.what());
	}
	catch (...) {
		printf("EXCEPTION in TestFile\n");
	}
}

int depth = 3;
void Browse(HFSHighLevelVolume* g_volumePtr, int level, const std::string& path, const std::map<std::string, struct FUSE_STAT>& contents)
{
	for (auto it = contents.begin(); it != contents.end(); it++)
	{
		std::string filename = it->first;
		const struct FUSE_STAT* st = &it->second;
		std::string subPath = path + "/" + filename;
//printf("%s\n", subPath());

		if ( S_ISDIR(st->st_mode) )
		{
			std::map<std::string, struct FUSE_STAT> subContents = g_volumePtr->listDirectory(subPath.c_str());
			if ( level < depth ) printf("%s\n", subPath.c_str());
			Browse(g_volumePtr, level+1, subPath, subContents);
		}
		else
		if ( S_ISREG(st->st_mode) )
		{
			// get address of data block
			std::string absPath = path + "/" + filename;
			try
			{
//				printf("%s -> open\n", absPath.c_str());
				std::shared_ptr<Reader> file = g_volumePtr->openFile(absPath);
				if ( level < depth ) printf("%s -> read\n", absPath.c_str());
				char buf[512];
				ssize_t nblu;
				uint64_t offset = 0;
				do {
					try {
						nblu = file->read(buf, sizeof(buf), offset);
					}
					catch(const io_error& e){
						printf("***** error reading %s : %s\n", filename.c_str(), e.what());
					}
					catch(...){
						printf("***** error reading %s at offset %" PRId64 ", error %s. Skipped\n", filename.c_str(), offset, strerror(errno));
					}
					offset += nblu;
				}while ( nblu > 0);
				if (nblu < 0) {
					printf("***** error reading %s, error %s. Skipped\n", filename.c_str(), strerror(errno));
				}
			}
			catch (...) {
				printf("***** error opening %s, error %s. Skipped\n", filename.c_str(), strerror(errno));
			}
		}
		else
		if ( S_ISLNK(st->st_mode) )
		{
			std::string absPath = path + "/" + filename;
			try
			{
//				printf("%s -> open\n", absPath.c_str());
				std::shared_ptr<Reader> file = g_volumePtr->openFile(absPath);
				if ( level < depth ) printf("%s -> read\n", absPath.c_str());
				char buf[4096];
				ssize_t nblu;
				uint64_t offset = 0;
				try {
					nblu = file->read(buf, sizeof(buf), offset);
				}
				catch(const io_error& e){
					printf("***** error reading %s : %s\n", filename.c_str(), e.what());
				}
				catch(...){
					printf("***** error reading %s at offset %" PRId64 ", error %s. Skipped\n", filename.c_str(), offset, strerror(errno));
				}
				if (nblu < 0) {
					printf("***** error reading %s, error %s. Skipped\n", filename.c_str(), strerror(errno));
				}
			}
			catch (...) {
				printf("***** error opening %s, error %s. Skipped\n", filename.c_str(), strerror(errno));
			}
		}
		else
		{
			printf("%s -> unhandled d_type 0%o\n", subPath.c_str(), (st->st_mode & S_IFMT) );
		}
	}

}

void BrowseRoot(HFSHighLevelVolume* g_volumePtr, const std::string& path)
{
		std::map<std::string, struct FUSE_STAT> contents = g_volumePtr->listDirectory(path);
		printf("%s\n", path.c_str());
//		contents.erase(std::string(".DocumentRevisions-V100"));
//		contents.erase(std::string(".Spotlight-V100"));
//		contents.erase(std::string(".TemporaryItems"));
//		contents.erase(std::string(".DS_Store"));
//		contents.erase(std::string(".Trashes"));
//		contents.erase(std::string(".fseventsd"));
//		contents.erase(std::string("Allocine"));
//		contents.erase(std::string(".Trashes"));
		Browse(g_volumePtr, 0, path=="/"?"":path, contents);
}

void FindFileForBand(HFSHighLevelVolume* g_volumePtr, uint64_t band_number)
{
	try
  	{
		
//		BrowseRoot(g_volumePtr, "/");
//		BrowseRoot(g_volumePtr, "/Git test");
		BrowseRoot(g_volumePtr, "/Git test/eclipse-jee-oxygen-3-macosx-cocoa-x86_64/Eclipse-GitTest.app/Contents");
//		BrowseRoot(g_volumePtr, "/iTunes");
		printf("done\n");
		exit(1);
	}
	catch(const io_error& e){
		printf("EXCEPTION in FindFileForBand %s\n", e.what());
	}
	catch(const std::runtime_error& e){
		printf("EXCEPTION in FindFileForBand %s\n", e.what());
	}
	catch(const std::exception& e){
		printf("EXCEPTION in FindFileForBand %s\n", e.what());
	}
	catch (...) {
		printf("EXCEPTION in FindFileForBand\n");
	}
}

