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

#include "create_di_global.h"
#include "../DiskImageTestUtils.h"
#include "DiskImageCreation.h"
#include "../FolderReferenceCreation.h"

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

    printf("Disk images creation\n");
    printf("Kernel %s\n", kernelVersionAsString_T().c_str());

    char cwd[2048];
    getcwd(cwd, sizeof(cwd));
    printf("Current working directory is '%s'\n", cwd);

    string generatedReferencePathToFolder = GENERATED_REFERENCE_FOLDER_PREFIX+pathSuffix;
    string generatedDiskImagesPathToFolder = GENERATED_DISKIMAGE_FOLDER_PREFIX+pathSuffix;
	string generatedDiskImagesOkPathToFolder = GENERATED_DISKIMAGE_OK_FOLDER_PREFIX + pathSuffix;

	if ( !file_or_dir_exists(generatedDiskImagesPathToFolder) ) {
        mkdir_T(generatedDiskImagesPathToFolder, 0755, stringPrintf("Cannot create '%s'", generatedDiskImagesPathToFolder.c_str()));
    }

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
                vector<DiskImageType>({DISKIMAGE_TYPE_UDRW}),
                vector<DiskImageFs>({DISKIMAGE_FS_HFS}),
                vector<DiskImageLayout>({DISKIMAGE_LAYOUT_APPLE}),
                vector<DiskImageEncryption>({DISKIMAGE_ENCRYPTION_NONE})
        );

#endif

    
    try
    {
		vector<DiskImage> refDiVector = createFolderReferenceUsedByDiskImageVector(diVector, generatedReferencePathToFolder, delete_reference_at_startup);
		vector<DiskImageToTest> ditVector = DiskImageToTest::getDiskImageToTestVectorFor(diVector, &refDiVector);




        umountAllInFolder(generatedDiskImagesPathToFolder); // just in case.
#ifdef DELETE_GENERATEDIMAGES_AT_STARTUP
        execAndPrintOutput_T(stringPrintf("rm -rf %s", generatedDiskImagesPathToFolder.c_str()));
#endif

		
		

		try {
			create_diskimage(ditVector, &report);

			if ( report.size() == 0 ) {
				printf("All disk images created sucessfully\n");
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
			
			ejectFolderReferenceUsedByDiskImageToTestVector_T(ditVector);
			
			if ( report.size() == 0 ) {
				return 0;
			}else{
				return 1;
			}

		}
		catch(...){
			ejectFolderReferenceUsedByDiskImageToTestVector(ditVector);
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
