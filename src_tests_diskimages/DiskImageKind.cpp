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
#include "DiskImageToTest.h"


//DECLARE_ENUM(DiskImageType, "DISKIMAGE_TYPE_", DISKIMAGE_TYPE_SPARSEBUNDLE, DISKIMAGE_TYPE_SPARSE, DISKIMAGE_TYPE_UDIF, DISKIMAGE_TYPE_UDTO, DISKIMAGE_TYPE_UDRO, DISKIMAGE_TYPE_UDCO, DISKIMAGE_TYPE_UDZO, DISKIMAGE_TYPE_UDBZ, DISKIMAGE_TYPE_ULFO, DISKIMAGE_TYPE_UFBI, DISKIMAGE_TYPE_IPOD, DISKIMAGE_TYPE_UDRW);
vector<DiskImageType> DiskImageTypeElements = { DiskImageTypeElementsMacro };
vector<DiskImageFs> DiskImageFsElements = { DiskImageFsElementsMacro };
vector<DiskImageLayout> DiskImageLayoutElements = { DiskImageLayoutElementsMacro };
vector<DiskImageEncryption> DiskImageEncryptionElements = { DiskImageEncryptionElementsMacro };

map<DiskImageType, string> DiskImageTypeArg = {
    { DISKIMAGE_TYPE_SPARSEBUNDLE, "SPARSEBUNDLE" },
    { DISKIMAGE_TYPE_SPARSE, "SPARSE" },
    { DISKIMAGE_TYPE_UDIF, "UDIF" },
    { DISKIMAGE_TYPE_UDTO, "UDTO" },
    { DISKIMAGE_TYPE_UDRO, "UDRO" },
    { DISKIMAGE_TYPE_UDCO, "UDCO" },
    { DISKIMAGE_TYPE_UDZO, "UDZO" },
    { DISKIMAGE_TYPE_UDBZ, "UDBZ" },
    { DISKIMAGE_TYPE_ULFO, "ULFO" },
    { DISKIMAGE_TYPE_UFBI, "UFBI" },
    { DISKIMAGE_TYPE_IPOD, "IPOD" },
    { DISKIMAGE_TYPE_UDRW, "UDRW" }
};

map<DiskImageType, string> DiskImageTypeName = {
    { DISKIMAGE_TYPE_SPARSEBUNDLE, "Spbu" },
    { DISKIMAGE_TYPE_SPARSE, "Sprs" },
    { DISKIMAGE_TYPE_UDIF, "Udif" },
    { DISKIMAGE_TYPE_UDTO, "Udto" },
    { DISKIMAGE_TYPE_UDRO, "Udro" },
    { DISKIMAGE_TYPE_UDCO, "Udco" },
    { DISKIMAGE_TYPE_UDZO, "Udzo" },
    { DISKIMAGE_TYPE_UDBZ, "Udbz" },
    { DISKIMAGE_TYPE_ULFO, "Ulfo" },
    { DISKIMAGE_TYPE_UFBI, "Ufbi" },
    { DISKIMAGE_TYPE_IPOD, "Ipod" },
    { DISKIMAGE_TYPE_UDRW, "Udrw" }
};

set<DiskImageType> CanBeBlankImageType = { { DISKIMAGE_TYPE_SPARSEBUNDLE, DISKIMAGE_TYPE_SPARSE, DISKIMAGE_TYPE_UDIF, DISKIMAGE_TYPE_UDTO } };
set<DiskImageType> RwDiskImageType = { { DISKIMAGE_TYPE_SPARSEBUNDLE, DISKIMAGE_TYPE_SPARSE, DISKIMAGE_TYPE_UDIF, DISKIMAGE_TYPE_UDTO, DISKIMAGE_TYPE_UDRW } };

//DECLARE_ENUM(RwDiskImageType, "RWDISKIMAGE_TYPE_", RWDISKIMAGE_TYPE_SPARSEBUNDLE, RWDISKIMAGE_TYPE_SPARSE, RWDISKIMAGE_TYPE_UDIF, RWDISKIMAGE_TYPE_UDTO);
//map<int, string> RwDiskImageTypeName = { { SPARSEBUNDLE, "Spbu" }, { SPARSE, "Sprs" }, { UDIF, "Udif" }, { UDTO, "Udto" } };
map<int, string> DiskImageTypeExtension = { { DISKIMAGE_TYPE_SPARSEBUNDLE, "sparsebundle" }, { DISKIMAGE_TYPE_SPARSE, "sparseimage" }, { DISKIMAGE_TYPE_UDIF, "dmg" }, { DISKIMAGE_TYPE_UDTO, "cdr" }, { DISKIMAGE_TYPE_UDRO, "dmg" }, { DISKIMAGE_TYPE_UDCO, "dmg" }, { DISKIMAGE_TYPE_UDZO, "dmg" }, { DISKIMAGE_TYPE_UDBZ, "dmg" }, { DISKIMAGE_TYPE_ULFO, "dmg" }, { DISKIMAGE_TYPE_UFBI, "dmg" }, { DISKIMAGE_TYPE_IPOD, "dmg" } , { DISKIMAGE_TYPE_UDRW, "dmg" } };
//map<int, string> RwDiskImageTypeExtension = { { SPARSEBUNDLE, "sparsebundle" }, { SPARSE, "sparseimage" }, { UDIF, "dmg" }, { UDTO, "cdr" } };

//DECLARE_ENUM(DiskImageFs , "", DISKIMAGE_FS_HFS, APFS);
map<int, string> DiskImageFsArg = { { DISKIMAGE_FS_HFS, "HFS+" }, { DISKIMAGE_FS_APFS, "APFS" } };
map<int, string> DiskImageFsName = { { DISKIMAGE_FS_HFS, "Hfs" }, { DISKIMAGE_FS_APFS, "Apfs" } };
//DECLARE_ENUM(DiskImageLayout, "DISKIMAGE_LAYOUT_", DISKIMAGE_LAYOUT_NONE, DISKIMAGE_LAYOUT_APPLE, DISKIMAGE_LAYOUT_MBR, DISKIMAGE_LAYOUT_GPT);
map<int, string> DiskImageLayoutArg = { { DISKIMAGE_LAYOUT_NONE, "NONE" }, { DISKIMAGE_LAYOUT_APPLE, "SPUD" }, { DISKIMAGE_LAYOUT_MBR, "MBRSPUD" }, { DISKIMAGE_LAYOUT_GPT, "GPTSPUD" } };
map<int, string> DiskImageLayoutName = { { DISKIMAGE_LAYOUT_NONE, "" }, { DISKIMAGE_LAYOUT_APPLE, "AppleMap" }, { DISKIMAGE_LAYOUT_MBR, "Mbr" }, { DISKIMAGE_LAYOUT_GPT, "Gpt" } };
//DECLARE_ENUM(DiskImageEncryption, "DISKIMAGE_ENCRYPTION_", DISKIMAGE_ENCRYPTION_NONE, DISKIMAGE_ENCRYPTION_AES_128, DISKIMAGE_ENCRYPTION_AES_256);
map<int, string> DiskImageEncryptionName = { { DISKIMAGE_ENCRYPTION_NONE, "" }, { DISKIMAGE_ENCRYPTION_AES_128, "Aes128" }, { DISKIMAGE_ENCRYPTION_AES_256, "Aes256" } };
map<int, string> DiskImageEncryptionArg = { { DISKIMAGE_ENCRYPTION_NONE, "" }, { DISKIMAGE_ENCRYPTION_AES_128, "AES-128" }, { DISKIMAGE_ENCRYPTION_AES_256, "AES-256" } };


DiskImageKind& DiskImageKind::operator = (const DiskImageKind& dik)
{
	m_type = dik.type();
	m_fs = dik.fs();
	m_layout = dik.layout();
	m_encryption = dik.encryption();
	return *this;
}



string DiskImageKind::typeArg() const
{
    return DiskImageTypeArg[m_type];
}

string DiskImageKind::fsArg() const
{
    return DiskImageFsArg[m_fs];
}

string DiskImageKind::layoutArg() const
{
    return DiskImageLayoutArg[m_layout];
}

string DiskImageKind::encryptionArg() const
{
    return DiskImageEncryptionArg[m_encryption];
}

string DiskImageKind::encryptionFullArgForCreate() const
{
    if ( DiskImageEncryptionArg[m_encryption].length() == 0 ) return "";
    return stringPrintf("-encryption %s -stdinpass", DiskImageEncryptionArg[m_encryption].c_str());
}

string DiskImageKind::nameWithoutExtension() const
{
    if ( DiskImageTypeName[m_type].length() == 0) throw "DiskImageTypeName[type].length() == 0";
    if ( DiskImageFsName[m_fs].length() == 0) throw "DiskImageFsName[fs].length() == 0";
    string name = DiskImageTypeName[m_type] + DiskImageFsName[m_fs] + DiskImageLayoutName[m_layout] + DiskImageEncryptionName[m_encryption];
    return name;
}

string DiskImageKind::extension() const
{
    return DiskImageTypeExtension[m_type];
}

string DiskImageKind::nameAndExtension() const
{
    return nameWithoutExtension() + "." + extension();
}

bool DiskImageKind::isReadOnly() const
{
    return RwDiskImageType.find(m_type) == RwDiskImageType.end();
}


bool DiskImageKind::canBeCreatedBlank() const
{
    return CanBeBlankImageType.find(m_type) == CanBeBlankImageType.end();
}

