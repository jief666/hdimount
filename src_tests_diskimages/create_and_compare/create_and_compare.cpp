/*
 * This CANNOT work on non Apple
 */

#include <unistd.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace ::std;


#include "../../src/Utils.h"
#include "../io.h"

#include "create_and_compare_global.h"
#include "../DiskImageTestUtils.h"
#include "../create_di/DiskImageCreation.h"
#include "../FolderReferenceCreation.h"
#include "../compare_di_with_constants/DiskImageComparisonConstants.h"

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

    printf("Disk images creation and comparison\n");
    printf("Kernel %s\n", kernelVersionAsString_T().c_str());

    char cwd[2048];
    getcwd(cwd, sizeof(cwd));
    printf("Current working directory is '%s'\n", cwd);

    string generatedReferencePathToFolder = GENERATED_REFERENCE_FOLDER_PREFIX+pathSuffix;
    string generatedDiskImagesPathToFolder = GENERATED_DISKIMAGE_FOLDER_PREFIX+pathSuffix;
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
        diVector = DiskImage::getDiskImageVectorFor(generatedDiskImagesPathToFolder, generatedDiskImagesOkPathToFolder, size,
                vector<DiskImageType>({DISKIMAGE_TYPE_IPOD}),
                vector<DiskImageFs>({DISKIMAGE_FS_HFS}),
                vector<DiskImageLayout>({DISKIMAGE_LAYOUT_GPT}),
                vector<DiskImageEncryption>({DISKIMAGE_ENCRYPTION_NONE})
        );

#endif

    try
    {
		
        umountAllInFolder(generatedDiskImagesPathToFolder); // just in case.
#ifdef DELETE_GENERATEDIMAGES_AT_STARTUP
        execAndPrintOutput_T(stringPrintf("rm -rf %s", generatedDiskImagesPathToFolder.c_str()));
#endif
        umountAllInFolder(generatedDiskImagesOkPathToFolder); // just in case.
#ifdef DELETE_GENERATEDIMAGESTESTED_AT_STARTUP
        execAndPrintOutput_T(stringPrintf("rm -rf %s", generatedDiskImagesOkPathToFolder.c_str()));
#endif

		vector<DiskImage> refDiVector = createFolderReferenceUsedByDiskImageVector(diVector, generatedReferencePathToFolder, delete_reference_at_startup);
		vector<DiskImageToTest> ditVector = DiskImageToTest::getDiskImageToTestVectorFor(diVector, &refDiVector);


		create_diskimage(ditVector, &report);
        compare_diskimage_with_constants(ditVector, move_tested_diskimage_to_tested, &report);

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

    if ( report.size() == 0 ) {
        printf("All tests passed\n");
        return 0;
    }

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
        return 1;
    }
}