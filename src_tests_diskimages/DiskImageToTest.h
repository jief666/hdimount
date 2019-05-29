//
//  DiskImageTestCommon
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#ifndef __DISKIMAGETOTEST_H__
#define __DISKIMAGETOTEST_H__


#include "../src/Utils.h"
#include "DiskImageKind.h"
#include "DiskImage.h"

class DiskImageToTest
{
  public:
//    static vector<DiskImageToTest> getDiskImageVectorFor(const string& pathToFolder, const string& pathToFolderOk, DiskImageTestSize size, vector<DiskImageType> types, vector<DiskImageFs> fss, vector<DiskImageLayout> layouts, vector<DiskImageEncryption> encryptions, const vector<DiskImage>& refDiVector);
    static vector<DiskImageToTest> getDiskImageToTestVectorFor(const vector<DiskImage>& diVector, const vector<DiskImage>* refDiVector);

  protected:
    DiskImage m_di;
	DiskImage m_refDi;

  public:
  	DiskImageToTest(const DiskImageToTest& dit) : m_di(dit.di()), m_refDi(dit.refDi()) {};
	DiskImageToTest(const DiskImage& di, DiskImage refDi) : m_di(di), m_refDi(refDi) {};
    DiskImageToTest(const string& pathToFolder, const string& pathToFolderOk, DiskImageTestSize size, DiskImageType type, DiskImageFs fs, DiskImageLayout layout, DiskImageEncryption encryption, DiskImage refDi) : m_di(pathToFolder, pathToFolderOk, size, type, fs, layout, encryption), m_refDi(refDi) {};
    DiskImageToTest& operator = (const DiskImageToTest& dit);

//    const string& referenceFolderPath() const { return m_ImgReferenceFolderPath; }
//    const string& generatedImageFolderPath() const { if ( m_di.pathToFolder().length() == 0 ) throw "Bug"; return m_di.pathToFolder(); }
    const string pathToDiskImageFile() const { return m_di.pathToDiskImageFile(); }
    const string& pathToFolder() const { return m_di.pathToFolder(); }


//
//    const string generatedImagePath() const { return generatedImageFolderPath()+"/"+nameAndExtension(); }
//    const string generatedImageOkPath() const { return generatedImageOkFolderPath().length()>0 ? generatedImageOkFolderPath()+"/"+nameAndExtension() : ""; }
//    const string generatedImagePathWithoutExtension() const { if ( m_di.mountPoint().length() <= 0 ) throw "Bug"; return m_di.mountPoint(); }
//    const string path() const;

	
    const DiskImage& refDi() const { return m_refDi; }
    const DiskImage& di() const { return m_di; }
    const DiskImageKind dik() const { return m_di.m_dik; }

    DiskImageTestSize size() const { return m_di.m_size; }
    DiskImageType type() const { return m_di.m_dik.type(); }
    DiskImageFs fs() const { return m_di.m_dik.fs(); }
    DiskImageLayout layout() const { return m_di.m_dik.layout(); }
    DiskImageEncryption encryption() const { return m_di.m_dik.encryption(); }

    bool operator < (const DiskImageToTest &other) const { return dik().nameWithoutExtension() < other.dik().nameWithoutExtension(); }
    
    bool canBeGenerated() const;
    bool canBeMountedByOS() const;
#ifdef __APPLE__
    bool canBeGeneratedOnThisPlateform() const;
#endif
};


#endif

