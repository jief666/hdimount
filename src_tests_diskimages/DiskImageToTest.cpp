//
//  DiskImageToTest.cpp
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
#include "DiskImageToTest.h"

static const DiskImage* getRefDi(const DiskImage& di, const vector<DiskImage>& refDiVector)
{
	for (const DiskImage& refDi : refDiVector) {
		if ( refDi.dik().fs() == di.dik().fs()  &&  refDi.size() == di.size() ) {
			return &refDi;
		}
	}
	return NULL;
}

vector<DiskImageToTest> DiskImageToTest::getDiskImageToTestVectorFor(const vector<DiskImage>& diVector, const vector<DiskImage>* refDiVector)
{
	vector<DiskImageToTest> ditVector;
	for (DiskImage di : diVector) {
		if ( refDiVector != NULL ) {
			const DiskImage* refDi = getRefDi(di, *refDiVector);
			if ( refDi != NULL ) {
				DiskImageToTest dit(di, *refDi);
				ditVector.push_back(dit);
			}else{
				// a refDiVector was given. If no ref was found, it's because the ref cannot be generated on this platform.
				// so far, it happens for all APFS on Os X.11 or before.
			}
		}else{
			DiskImageToTest dit(di, DiskImage());
			ditVector.push_back(dit);
		}
	}
	return ditVector;
}

//
//vector<DiskImageToTest> DiskImageToTest::getDiskImageVectorFor(const string& pathToFolder, const string& pathToFolderOk, DiskImageTestSize size, vector<DiskImageType> types, vector<DiskImageFs> fss, vector<DiskImageLayout> layouts, vector<DiskImageEncryption> encryptions, const vector<DiskImage>& refDiVector)
//{
//    vector<DiskImageType>* types_ = types.size() == 0 ? &DiskImageTypeElements : &types;
//    vector<DiskImageFs>* fss_ = fss.size() == 0 ? &DiskImageFsElements : &fss;
//    vector<DiskImageLayout>* layouts_ = layouts.size() == 0 ? &DiskImageLayoutElements : &layouts;
//    vector<DiskImageEncryption>* encryptions_ = encryptions.size() == 0 ? &DiskImageEncryptionElements : &encryptions;
//    vector<DiskImageToTest> ditVector;
//
//    for (DiskImageType type : *types_) {
//        for (DiskImageFs fs : *fss_) {
//            for (DiskImageLayout layout : *layouts_) {
//                for (DiskImageEncryption encryption : *encryptions_) {
//					for (DiskImage refDi : refDiVector) {
//						if ( refDi.dik().fs() == fs  &&  refDi.size() == size ) {
//							ditVector.push_back(DiskImageToTest(pathToFolder, pathToFolderOk, size, type, fs, layout, encryption, refDi));
//						}
//					}
//                }
//            }
//        }
//    }
//    return ditVector;
//}

DiskImageToTest& DiskImageToTest::operator = (const DiskImageToTest& dit)
{
	m_refDi = dit.m_refDi;
    m_di = dit.m_di;
	return *this;
}

bool DiskImageToTest::canBeGenerated() const
{

    if ( m_di.m_dik.fs() == DISKIMAGE_FS_APFS  &&  m_di.m_dik.layout() == DISKIMAGE_LAYOUT_MBR ) return false;
    if ( m_di.m_dik.fs() == DISKIMAGE_FS_APFS  &&  m_di.m_dik.layout() == DISKIMAGE_LAYOUT_APPLE )  return false;

    // Big disk images are not created from a reference
    if ( m_di.m_size == DISKIMAGE_SIZE_BIG ) {
        if ( m_di.m_dik.isReadOnly() ) return false;
    }

    return true;
}

#ifdef __APPLE__

bool DiskImageToTest::canBeGeneratedOnThisPlateform() const
{
    // 2019-05-07 Travis CI is using the version (17.4.0) for xcode9.3 image, that has some bugs !
    // 17.4.0 = macOs 10.13.3
    // 16.x.x = Sierra (APFS since 10.12.4 = Darwin 16.5.0)
    // 15.x.x = El Capitan
    // 14.x.x = Yosemite
    if ( compareKernelVersion_T(16, 5, 0) < 0  &&  m_di.m_dik.fs() == DISKIMAGE_FS_APFS ) return false;

    return true;
}
#endif

bool DiskImageToTest::canBeMountedByOS() const
{
#ifdef __APPLE__

    if ( compareKernelVersion_T(16, 5, 0) < 0  &&  m_di.m_dik.fs() == DISKIMAGE_FS_APFS ) return false;

// These images can be created on Darwin 17.4.0, but not mounted. It's a bug...
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_SPARSE && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_SPARSEBUNDLE && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_UDIF && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_UDTO && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_UDRO && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_UDCO && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_IPOD && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_UDBZ && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_UDRW && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_UFBI && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_ULFO && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;
    if ( isKernelVersion_T(17, 4, 0) && m_di.m_dik.type() == DISKIMAGE_TYPE_UDZO && m_di.m_dik.fs() == DISKIMAGE_FS_APFS && m_di.m_dik.layout() == DISKIMAGE_LAYOUT_NONE ) return false;

    return true;
#else
    return false;
#endif
}

