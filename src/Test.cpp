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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "../darling-dmg/src/exceptions.h"
#include "Utils.hpp"
#include "../darling-dmg/src/FileReader.h"

#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-variable"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

#define min(x, y) ((x) < (y) ? (x) : (y))

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
	try
  	{
		size_t rv1;
		size_t rv2;
		struct stat st_stat1;
		struct stat st_stat2;
		char buf[1000000];

		if ( !g_volumePtr )
			throw io_error("g_volumePtr is null");

//		rv1 = g_volumePtr->getattr(path, &st_stat1);
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

		g_volumePtr->readdir("/", buf, myFiller, 0, NULL);
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
	char buf1[count];
	char buf2[count];
	
	int32_t nb_read_ref = reader1->read(buf1, count, offset);
	if (nb_read_ref != count)
		throw std::runtime_error(stprintf("Cannot read %d bytes. Returns %d. Errno %d.", count, nb_read_ref, errno));

	int32_t nb_read = reader2->read(buf2, count, offset);
	if (nb_read != count)
		throw std::runtime_error(stprintf("Cannot read %d bytes. Returns %d. Errno %d.", count, nb_read, errno));

	if (memcmp(buf1, buf2, count)!=0)
		throw std::runtime_error("oups");

}

void TestReader(std::shared_ptr<Reader> reader, const char* path_to_reference)
{
	std::shared_ptr<FileReader> refReader = std::make_shared<FileReader>(path_to_reference);
	
	int32_t blockSize = 512;
	char bufRef[blockSize];
	char buf[blockSize];
	
	printf("test reading by block of %d\n", blockSize);
	uint64_t fileLen = reader->length();
	for(uint64_t offset=0 ; offset<fileLen ; offset+=blockSize)
	{
		int32_t len = (int32_t)min(uint64_t(blockSize), fileLen-offset);
//printf("Read offset=%lld, count=%lld\n", offset, len);
		try {
			ReadAndCompare(refReader, reader, blockSize, offset);
		}catch(...){
			printf("Failed for offset=%lld, count=%d\n", offset, blockSize);
			ReadAndCompare(refReader, reader, blockSize, offset);
		}
	}

	printf("test reading random offset and length\n");
	{
		std::mt19937_64 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937_64::result_type> dist1(0, fileLen-1); // distribution in range [1, 6]

printf("dist1(rng)=%lld\n", dist1(rng));

		for(size_t i=0 ; i<10000 ; i++)
		{
			uint64_t offset = dist1(rng);
			std::uniform_int_distribution<std::mt19937::result_type> dist2(1, (uint)min(2000000L, fileLen-offset));
			int32_t len = dist2(rng);
printf("Read offset=%lld, count=%d\n", offset, len);
			try {
				ReadAndCompare(refReader, reader, len, offset);
			}catch(...){
				printf("Failed for offset=%lld, count=%d\n", offset, blockSize);
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

		struct stat st;
		rv = g_volumePtr->getattr(path, &st);
		if (rv != 0)
			throw std::runtime_error(stprintf("Cannot stat '%s'. Error %d.", path, rv));

		struct fuse_file_info info;
		rv = g_volumePtr->open(path, &info);
		if (rv != 0)
			throw std::runtime_error(stprintf("Cannot open '%s'. Error %d.", path, rv));
		uint64_t fileLen = st.st_size;
		uint8_t buf1[fileLen];
		uint8_t buf2[fileLen];
		int32_t nb_read = g_volumePtr->read(path, (char*)buf1, sizeof(buf1), 0, &info);
		if (nb_read != int32_t(sizeof(buf1))) // Don't use a buffer bigger than 2GB !! :-)
			throw std::runtime_error(stprintf("Cannot read %d bytes from '%s'. Returns %d. Errno %d.", sizeof(buf1), path, nb_read, errno));

		std::mt19937_64 rng;
		rng.seed(std::random_device()());
		std::uniform_int_distribution<std::mt19937_64::result_type> dist1(0, fileLen-1); // distribution in range [1, 6]

		printf("dist1(rng)=%lld\n", dist1(rng));


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
			int32_t len = (int32_t)min(uint64_t(blockSize), fileLen-offset);
printf("Read offset=%lld, count=%d\n", offset, len);
			try {
				nb_read = g_volumePtr->read(path, (char*)buf2+offset, len, offset, &info);
				if (nb_read != len)
					throw std::runtime_error(stprintf("Cannot read %d bytes from '%s'. Returns %d. Errno %d.", len, path, nb_read, errno));
				if (memcmp(buf1+offset, buf2+offset, len)!=0)
					throw std::runtime_error("oups");
			}catch(...){
				rv = g_volumePtr->read(path, (char*)buf2+i, len, offset, &info);
				if (nb_read != len)
					throw std::runtime_error(stprintf("Cannot read %d bytes from '%s'. Returns %d. Errno %d.", len, path, nb_read, errno));
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
