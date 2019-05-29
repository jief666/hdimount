//
//  DiskImageTestCommon
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#ifndef __DISKIMAGE_H__
#define __DISKIMAGE_H__


#include "../src/Utils.h"
#include "DiskImageKind.h"

class DiskImage
{
    friend class DiskImageToTest;
    
  public:
    static vector<DiskImage> getDiskImageVectorFor(const string& pathToFolder, const string& pathToFolderOk, DiskImageTestSize size, vector<DiskImageType> types, vector<DiskImageFs> fss, vector<DiskImageLayout> layouts, vector<DiskImageEncryption> encryptions);

  protected:
    string m_pathToFolder;
    string m_pathToFolderOk;
    DiskImageTestSize m_size;
    DiskImageKind m_dik;

  public:
    DiskImage() : m_size(DISKIMAGE_SIZE_SMALL) {};
    DiskImage(const string& pathToFolder, const string& pathToFolderOk, DiskImageTestSize size, DiskImageType type, DiskImageFs fs, DiskImageLayout layout, DiskImageEncryption encryption);
    DiskImage(const string& pathToFolder, const string& pathToFolderOk, DiskImageTestSize size, const DiskImageKind& dik) : DiskImage(pathToFolder, pathToFolderOk, size, dik.type(), dik.fs(), dik.layout(), dik.encryption()) {};
    DiskImage(const DiskImage& other) : DiskImage(other.pathToFolder(), other.pathToFolderOk(), other.size(), other.dik().type(), other.dik().fs(), other.dik().layout(), other.dik().encryption()) {};
    DiskImage& operator = (const DiskImage& dik);


    const string pathToDiskImageFile() const { return m_pathToFolder + "/" + m_dik.nameAndExtension(); }
    const string& pathToFolder() const { return m_pathToFolder; }
    const string& pathToFolderOk() const { return m_pathToFolderOk; }
    const string mountPoint() const { return m_pathToFolder.length() > 0 ? pathToFolder() + "/" + m_dik.nameWithoutExtension() : ""; }

	DiskImageTestSize size() const { return m_size; }
    const DiskImageKind dik() const { return m_dik; }

    bool operator < (const DiskImage &other) const { return dik().nameWithoutExtension() < other.dik().nameWithoutExtension(); }

	bool exist() const;
    bool alreadyGenerated() const;
    bool alreadyTested() const;


    bool canBeGenerated() const;
    bool canBeMountedByOS() const;
#ifdef __APPLE__
    bool canBeGeneratedOnThisPlateform() const;
    void create_T(const string& sizeArgument) const;
	void create_from_folder_T(const string& folder) const;
	bool isMounted() const;
    void mount_T(bool readOnly) const;
    void eject_T() const;
    void eject() const;
	void convertFrom_T(const DiskImage& fromDi) const;
#endif
};

#endif

