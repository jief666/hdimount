//#import <Foundation/Foundation.h>
//#import <Cocoa/Cocoa.h>

#include <stdio.h>
#include <stddef.h>
#include <string.h> // for memcmp
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>


//#include <iconv.h>

/* ICU4C */
#include <unicode/utypes.h>
#include <unicode/ustring.h>
#include <unicode/ucnv.h>
#include <unicode/unorm2.h>
#include <unicode/unistr.h>
#include <unicode/ucnv.h>
#include <unicode/errorcode.h>

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "../src/Utils.h"
#include "../src/FuseVolume.h"
#include "../src/OpenDisk.h"
#include "../src_tests_diskimages/io.h"
#include "../src/utf8proc/utf8proc.h"
#include "icu_Convertion.h"
#include "ns_Convertion.h"

#include "../src/conversion/fast_unicode_compare_apple.h"
#include "../src/conversion/utf816Conversion.h"

#include "../src/utf8proc/utf8proc.h"

#if defined(__linux__)  ||  defined(_MSC_VER)
#define st_mtimespec st_mtim
#endif

using namespace std;

int main(int argc, char **argv)
{
//    printf("sizeof(HFSPlusCatalogFile)=%zd\n", sizeof(HFSPlusCatalogFile));



    FuseVolume* volume = NULL;

	try
	{
		int fd = open("/JiefLand/5.Devel/Syno/Fuse/hdimount--jief666--wip/README.md", O_RDONLY, 0644);
		char buf[256];
		int nbread = read(fd, buf, 2);
		nbread = read(fd, buf, 0);
		printf("nbread=%d\n", nbread);
//		volume = openDisk("/dev/disk4", "foo");
//
//		struct FUSE_STAT st_stat;
//		int rv = volume->getattr("/", &st_stat);
	}
    catch(const string& s)
    {
        fprintf(stderr, "%s\n", s.c_str());
    }
    catch (const std::exception& e)
    {
        fprintf(stderr, "Exeception type '%s', what '%s'\n", typeid(e).name(), e.what());
    }
    catch(...)
    {
        fprintf(stderr, "Unknow exception\n");
    }

}

