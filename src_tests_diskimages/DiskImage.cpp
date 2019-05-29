//
//  DiskImage.cpp
//  hdimount--jief666--wip
//
//  Created by jief on 16/07/2018.
//  Copyright © 2018 jf-luce. All rights reserved.
//

#include <unistd.h>
#include <string.h> // for strlen
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _MSC_VER
#ifndef __XATTR_H__
#include <sys/xattr.h>
#endif
#include <sys/mount.h>
#include <sys/dir.h>
#else
#include <fuse.h> // for FUSE_STAT
#endif

#include <string>
#include <vector>
#include <map>
#include <set>

using namespace ::std;

#include "io.h"
#include "DiskImageTestUtils.h"
#include "DiskImage.h"

#ifdef __APPLE__
#include "os_interaction/NSURL.h"
#include "os_interaction/Disk.h"
#include "os_interaction/Partition.h"
#endif

vector<DiskImage> DiskImage::getDiskImageVectorFor(const string& pathToFolder, const string& pathToFolderOk, DiskImageTestSize size, vector<DiskImageType> types, vector<DiskImageFs> fss, vector<DiskImageLayout> layouts, vector<DiskImageEncryption> encryptions)
{
    vector<DiskImageType>* types_ = types.size() == 0 ? &DiskImageTypeElements : &types;
    vector<DiskImageFs>* fss_ = fss.size() == 0 ? &DiskImageFsElements : &fss;
    vector<DiskImageLayout>* layouts_ = layouts.size() == 0 ? &DiskImageLayoutElements : &layouts;
    vector<DiskImageEncryption>* encryptions_ = encryptions.size() == 0 ? &DiskImageEncryptionElements : &encryptions;
    vector<DiskImage> ditVector;
	
    for (DiskImageType type : *types_) {
        for (DiskImageFs fs : *fss_) {
            for (DiskImageLayout layout : *layouts_) {
                for (DiskImageEncryption encryption : *encryptions_) {
					ditVector.push_back(DiskImage(pathToFolder, pathToFolderOk, size, type, fs, layout, encryption));
                }
            }
        }
    }
    return ditVector;
}




DiskImage::DiskImage(const string& pathToFolder, const string& pathToFolderOk, DiskImageTestSize size, DiskImageType type, DiskImageFs fs, DiskImageLayout layout, DiskImageEncryption encryption)
    : m_pathToFolder(pathToFolder), m_pathToFolderOk(pathToFolderOk), m_size(size), m_dik(type, fs, layout, encryption)
{
}

DiskImage& DiskImage::operator = (const DiskImage& di)
{
	m_pathToFolder = di.m_pathToFolder;
	m_size = di.m_size;
	m_dik = di.m_dik;
	return *this;
}


bool DiskImage::exist() const
{
    return alreadyGenerated() || alreadyTested();
}

bool DiskImage::alreadyGenerated() const
{
    if ( alreadyTested() ) return true;
    return file_or_dir_exists(pathToDiskImageFile());
}

bool DiskImage::alreadyTested() const
{
    std::string diskImagePath = pathToFolderOk() + "/" + dik().nameAndExtension();
    return file_or_dir_exists(diskImagePath);
}

bool DiskImage::canBeGenerated() const
{

    if ( m_dik.fs() == DISKIMAGE_FS_APFS  &&  m_dik.layout() == DISKIMAGE_LAYOUT_MBR ) return false;
    if ( m_dik.fs() == DISKIMAGE_FS_APFS  &&  m_dik.layout() == DISKIMAGE_LAYOUT_APPLE )  return false;

    // Big disk images are not created from a reference
    if ( m_size == DISKIMAGE_SIZE_BIG ) {
        if ( m_dik.isReadOnly() ) return false;
    }

    return true;
}

bool DiskImage::canBeMountedByOS() const
{
#ifdef __APPLE__

    if ( compareKernelVersion_T(16, 5, 0) < 0  &&  m_dik.fs() == DISKIMAGE_FS_APFS ) return false;

// These images can be created on Darwin 17.4.0, but not mounted. It's a bug...
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_SPARSE && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_SPARSEBUNDLE && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_UDIF && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_UDTO && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_UDRO && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_UDCO && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_IPOD && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_UDBZ && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_UDRW && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_UFBI && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_ULFO && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_dik.type() == DISKIMAGE_TYPE_UDZO && m_dik.fs() == DISKIMAGE_FS_APFS && m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;

    return true;
#else
    return false;
#endif
}


#ifdef __APPLE__

bool DiskImage::canBeGeneratedOnThisPlateform() const
{
    // 2019-05-07 Travis CI is using the version (17.4.0) for xcode9.3 image, that has some bugs !
    // 17.4.0 = macOs 10.13.3
    // 16.x.x = Sierra (APFS since 10.12.4 = Darwin 16.5.0)
    // 15.x.x = El Capitan
    // 14.x.x = Yosemite
    if ( compareKernelVersion_T(16, 5, 0) < 0  &&  m_dik.fs() == DISKIMAGE_FS_APFS ) return false;

    return true;
}



void DiskImage::create_T(const string& sizeArgument) const
{
    string cmd = stringPrintf("hdiutil create -verbose \"%s\" -volname \"%s\" -type %s -size %s -fs %s -layout %s %s", pathToDiskImageFile().c_str(), m_dik.nameWithoutExtension().c_str(), m_dik.typeArg().c_str(), sizeArgument.c_str(), m_dik.fsArg().c_str(), m_dik.layoutArg().c_str(), m_dik.encryptionFullArgForCreate().c_str());
    execAndPrintOutput_T(cmd);
}

void DiskImage::create_from_folder_T(const string& folder) const
{
	string cmd = stringPrintf("sudo hdiutil create -verbose \"%s\" -volname \"%s\" -format %s -srcfolder %s -fs %s -layout %s %s", pathToDiskImageFile().c_str(), dik().nameWithoutExtension().c_str(), dik().typeArg().c_str(), folder.c_str(), dik().fsArg().c_str(), dik().layoutArg().c_str(), dik().encryptionFullArgForCreate().c_str());
        try {
			execAndPrintOutput_T(cmd);
		}catch(...){
#ifdef DEBUG
			printf("Create failed, retrying\n");
#endif
			sleep(1);
			// at least on my computer, it sometimes fail with 'hdiutil: create failed - Resource busy'
			unlink(pathToDiskImageFile().c_str());
			execAndPrintOutput_T(cmd);
		}
}

bool DiskImage::isMounted() const
{
	try {
		Disk* disk = [[Disk alloc] initWithPath:[NSURL fileURLWithPath:[NSString stringWithUTF8String:pathToDiskImageFile().c_str()]]];
		if ( disk == NULL ) return false;
		for ( Partition* partition in disk.partitionArray )
		{
			NSString* currentMountPoint = [partition getCurrentMountPoint];
			if ( currentMountPoint ) {
				return true;
			}
		}
		return false;
	}
	catch (NSString* nsString) {
		throw std::string(nsString.UTF8String);
	}
	catch(...) {
		throw;
	}
}

void DiskImage::mount_T(bool readOnly) const
{
    string cmd = stringPrintf("hdiutil attach %s -owners on -mountpoint \"%s\" \"%s\" -stdinpass", readOnly ? "-readonly" : "", mountPoint().c_str(), pathToDiskImageFile().c_str());
    execAndPrintOutput_T(cmd);
	Disk* disk = [[Disk alloc] initWithPath:[NSURL fileURLWithPath:[NSString stringWithUTF8String:pathToDiskImageFile().c_str()]]];
	if ( disk == NULL ) return throw(stringPrintf("Disk image '%s' mounted by hdiutil, but no sign in IOKit", pathToDiskImageFile().c_str()));
	
	bool found = false;
	for ( Partition* partition in disk.partitionArray )
	{
		NSString* currentMountPoint = [partition getCurrentMountPoint];
		if ( currentMountPoint ) {
			if ( [[[NSURL fileURLWithPath:[NSString stringWithUTF8String:mountPoint().c_str()]] resolveURL].path isEqualToString:currentMountPoint] ) {
				if ( found ) throw(stringPrintf("2 partitions of disk image '%s' is mounted on the same mount point '%s'", pathToDiskImageFile().c_str(), currentMountPoint.UTF8String));
				found = true;
			}
			else
			{
				if ( found ) throw(stringPrintf("A partition of disk image '%s' mounted on '%s'. Disk image is supposed to be one partition only (except EFI)", pathToDiskImageFile().c_str(), currentMountPoint.UTF8String));
			}
		}
	}
	if ( !found ) {
		throw(stringPrintf("No partition of disk image '%s' is mounted", pathToDiskImageFile().c_str()));
	}
}

void DiskImage::eject_T() const
{
//	// Eject with diskutil // diskutil doesn't not eject APFS disk !
//	execAndPrintOutput_T(stringPrintf("diskutil eject \"%s\"", mountPoint().c_str()));
//	execAndPrintOutput_T(stringPrintf("%s \"%s\"", ejectTool().c_str(), pathToDiskImageFile().c_str()));
	Disk* disk = [[Disk alloc] initWithPath:[NSURL fileURLWithPath:[NSString stringWithUTF8String:pathToDiskImageFile().c_str()]]];
	if ( disk == NULL ) throw(stringPrintf("Disk image '%s' is not even attached", pathToDiskImageFile().c_str()));
	if ( ! [disk eject] ) {
		throw(stringPrintf("Eject of disk image '%s' failed", pathToDiskImageFile().c_str()));
	}
}

void DiskImage::eject() const
{
    try
    {
		eject_T();
	}
	catch(const string& s) {
		fprintf(stderr, "Cannot eject disk image '%s' : %s\n", pathToDiskImageFile().c_str(), s.c_str());
	}
    catch(...){
		fprintf(stderr, "Cannot eject disk image '%s'\n", pathToDiskImageFile().c_str());
	}
}

void DiskImage::convertFrom_T(const DiskImage& fromDi) const
{
    string cmd = stringPrintf("hdiutil convert -format %s %s -o \"%s\" \"%s\"", dik().typeArg().c_str(), dik().encryptionFullArgForCreate().c_str(), pathToDiskImageFile().c_str(), fromDi.pathToDiskImageFile().c_str());
    execAndPrintOutput_T(cmd);
}


#endif // __APPLE__
