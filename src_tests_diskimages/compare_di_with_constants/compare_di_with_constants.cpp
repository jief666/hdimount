#include <unistd.h>
#include <string.h>
//#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/mount.h>
#ifndef _WIN32
#include <sys/dir.h>
#endif

#include <string>
#include <vector>
#include <map>
#include <set>
#include <typeinfo>

using namespace ::std;

#include "../../src/Utils.h"
#include "../common.h"
#include "../io.h"

#include "compare_di_with_constants_globals.h"
#include "../DiskImageTestUtils.h"
#include "DiskImageComparisonConstants.h"

#ifdef DEBUG
// to force non inline to ease debugging
template class std::vector<struct dirent>;
template class std::vector<string>;
template class std::vector<uint8_t>;
#endif

int main(int argc, char** argv)
{
    bool doBigTest = argc > 1  && strcmp(argv[1], "big");

    string pathSuffix;
    DiskImageTestSize size = DISKIMAGE_SIZE_SMALL;
    if ( doBigTest ) {
        pathSuffix = "Big";
        size = DISKIMAGE_SIZE_BIG;
    }
    #ifdef DEBUG
        pathSuffix += "Debug";
    #endif

    printf("Disk images comparison with constants\n");
    #ifdef __APPLE__
        printf("Kernel %s\n", kernelVersionAsString_T().c_str());
    #endif

    char cwd[2048];
    getcwd(cwd, sizeof(cwd));
    printf("Current working directory is '%s'\n", cwd);

    string generatedDiskImagesPathToFolder = GENERATED_DISKIMAGE_FOLDER_PREFIX + pathSuffix;
	string generatedDiskImagesOkPathToFolder = GENERATED_DISKIMAGE_OK_FOLDER_PREFIX + pathSuffix;

    map<DiskImageToTest, vector<string>> report;


    vector<DiskImage> diVector =
	    DiskImage::getDiskImageVectorFor(generatedDiskImagesPathToFolder, generatedDiskImagesOkPathToFolder, size,
                     vector<DiskImageType>(),
                     vector<DiskImageFs>(),
                     vector<DiskImageLayout>(),
                     vector<DiskImageEncryption>()
        );

#ifdef DEBUG
	/*
	 * {DISKIMAGE_TYPE_UDTO}
	 * {DISKIMAGE_FS_APFS}
	 * {DISKIMAGE_LAYOUT_APPLE}
	 * {DISKIMAGE_ENCRYPTION_NONE}
	 */
        //diVector = DiskImage::getDiskImageVectorFor(generatedDiskImagesPathToFolder, generatedDiskImagesOkPathToFolder, size,
        //        vector<DiskImageType>({DISKIMAGE_TYPE_UDRW}),
        //        vector<DiskImageFs>({DISKIMAGE_FS_HFS}),
        //        vector<DiskImageLayout>({DISKIMAGE_LAYOUT_APPLE}),
        //        vector<DiskImageEncryption>({DISKIMAGE_ENCRYPTION_NONE})
        //);
	
//        // To compare one image
//        {
//			DiskImageToTest dit("GeneratedDiskImages", "", DISKIMAGE_SIZE_SMALL, DISKIMAGE_TYPE_UDBZ, DISKIMAGE_FS_HFS, DISKIMAGE_LAYOUT_APPLE, DISKIMAGE_ENCRYPTION_NONE, DiskImage());
//			compare_diskimage_with_constants(dit, false);
//		}
#endif

    try
    {
		vector<DiskImageToTest> ditVector = DiskImageToTest::getDiskImageToTestVectorFor(diVector, NULL);
        compare_diskimage_with_constants(ditVector, move_tested_diskimage_to_tested, &report);

		if ( report.size() == 0 ) {
			printf("All disk images tested sucessfully\n");
		}
		else
		{
			printf("==============================================================================================\n");
			printf("======================================== FAIL ================================================\n");
			printf("==============================================================================================\n");
			printf("%zd test%s failed\n", report.size(), report.size() > 1 ? "s" : "");
			auto it = report.begin();
			while( it != report.end() )
			{
				const DiskImageToTest& dit = it->first;
				printf("-----------------------------------------> %s\n", dit.dik().nameAndExtension().c_str());
				const vector<string>& errors = it->second;
				for ( string s : errors ) {
					printf("    %s\n", s.c_str());
				}
				it++;
			}
		}
	
		if ( report.size() == 0 ) {
			return 0;
		}else{
			return 1;
		}


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
	return 1;
}
