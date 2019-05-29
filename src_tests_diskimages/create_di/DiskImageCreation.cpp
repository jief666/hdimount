#include <unistd.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef __XATTR_H__
#include <sys/xattr.h>
#endif
#include <sys/mount.h>

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace ::std;

#include "io.h"
#include "../../src/FuseVolume.h"
#include "../../src/OpenDisk.h"
#include "../../src/Utils.h"
#include "../DiskImageTestUtils.h"
#include "DiskImageCreation.h"
#include "FolderReferenceCreation.h"


#ifdef __APPLE__

/*
 *
 */
void create_diskimage_(const DiskImageToTest& dit)
{
//    std::string name = dit.di().dik().nameWithoutExtension();

    if ( !file_or_dir_exists(dit.pathToFolder()) ) {
        mkdir_T(dit.pathToFolder(), 0755, stringPrintf("Cannot create '%s'", dit.pathToFolder().c_str()));
    }

    bool fromFolder = dit.dik().canBeCreatedBlank();
    bool readonly = dit.dik().isReadOnly();
    bool directCreate = dit.size()==DISKIMAGE_SIZE_BIG;
    if ( (fromFolder || readonly) && directCreate ) throw "Bug";

	string sizeArgument;
	if ( dit.size() == DISKIMAGE_SIZE_BIG ) sizeArgument = "5g";
	else if ( dit.fs() == DISKIMAGE_FS_HFS ) sizeArgument = "2m";
	else sizeArgument = "3000k";

	// Creating diskimages by convertion works less good than from srcfolder.

	try
	{
		// Create
		if ( fromFolder ) {
			dit.refDi().mount_T(true);
			dit.di().create_from_folder_T(dit.refDi().mountPoint());
			execAndPrintOutput_T(stringPrintf("sudo chown -R %d:%d %s", getuid(), getgid(), dit.pathToDiskImageFile().c_str()));
		}
		else
		{
			dit.di().create_T(sizeArgument);
		}
		if ( !readonly )
		{
			dit.di().mount_T(false);
			try
			{
				if ( directCreate )
				{
					createFolderReference(dit.di());
				}
				else
				{
					dit.refDi().mount_T(true);
					
					string cmd;
					// ditto
					cmd = stringPrintf("sudo ditto --preserveHFSCompression \"%s\"/ \"%s\"", dit.refDi().mountPoint().c_str(), dit.di().mountPoint().c_str());
					execAndPrintOutput_T(cmd);
					// rsync
					cmd = stringPrintf("sudo %s --out-format='%%i  - %%n  - %%M' -a -H -X --crtimes --exclude=/.Trashes --exclude=/.fseventsd --delete-excluded \"%s\"/ \"%s\" -c", rsyncTool().c_str(), dit.refDi().mountPoint().c_str(), dit.di().mountPoint().c_str());
					execAndPrintOutput_T(cmd);
				}
			}
			catch(...) {
				dit.di().eject();
				throw;
			}
			dit.di().eject_T();
		}
	}
	catch(...)
	{
		throw;
	}

    return;
}

void create_diskimage(const DiskImageToTest& dit)
{
    if ( !dit.canBeGenerated()) {
        printf("This kind cannot exist (%s)\n", dit.di().dik().nameAndExtension().c_str());
        return;
    }
    if ( !dit.canBeGeneratedOnThisPlateform()) {
        printf("This kind cannot be generated on this platform (%s)\n", dit.di().dik().nameAndExtension().c_str());
        return;
    }
    if ( !dit.canBeMountedByOS() ) {
        printf("This OS version cannot mount this kind (saved in '%s')\n", dit.di().dik().nameWithoutExtension().c_str());
        return;
    }
    if ( dit.di().alreadyTested() ) {
        printf("Already tested ok (saved in '%s')\n", dit.di().pathToFolderOk().c_str());
        return;
    }
    if ( dit.di().alreadyGenerated() ) {
        printf("Already generated (saved in '%s')\n", dit.di().pathToFolder().c_str());
        return;
    }
    if ( dit.di().exist() ) {
        printf("Already exists (but don't know where !)\n");
        return;
    }

    create_diskimage_(dit);
}

void create_diskimage(const vector<DiskImageToTest>& ditVector, map<DiskImageToTest, vector<string>>* report)
{
    for (size_t i = 0 ; i < ditVector.size() ; i++)
    {
        const DiskImageToTest& dit = ditVector[i];
        try {
            fprintf(stderr, "=================> create diskimage '%s' (%zd/%zd)\n", ditVector[i].pathToDiskImageFile().c_str(), i+1, ditVector.size());
            create_diskimage(dit);
            printf("---------------------------------> done\n");
        }
        catch (const std::exception & e)
        {
            printf("Exeception type '%s', what '%s'\n", typeid(e).name(), e.what());
            (*report)[dit].push_back(stringPrintf("Exeception type '%s', what '%s'", typeid(e).name(), e.what()));
        }
        catch(const string& e) {
            printf("Failure : %s\n", e.c_str());
            (*report)[dit].push_back(stringPrintf("Failure : %s", e.c_str()));
            printf("---------------------------------> FAILED\n");
        }
        catch(...)
        {
            printf("Unknow exception\n");
            (*report)[dit].push_back(stringPrintf("Unknow exception"));
            printf("---------------------------------> FAILED\n");
        }
    }
}

#endif
