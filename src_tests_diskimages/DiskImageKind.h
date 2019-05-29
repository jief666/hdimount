//
//  DiskImageTestCommon
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#ifndef __DISKIMAGEKIND_H__
#define __DISKIMAGEKIND_H__


#include "../src/Utils.h"

enum DiskImageTestSize { DISKIMAGE_SIZE_SMALL, DISKIMAGE_SIZE_BIG };

#define DiskImageTypeElementsMacro DISKIMAGE_TYPE_SPARSEBUNDLE, DISKIMAGE_TYPE_SPARSE, DISKIMAGE_TYPE_UDIF, DISKIMAGE_TYPE_UDTO, DISKIMAGE_TYPE_UDRO, DISKIMAGE_TYPE_UDCO, DISKIMAGE_TYPE_UDZO, DISKIMAGE_TYPE_UDBZ, DISKIMAGE_TYPE_ULFO, DISKIMAGE_TYPE_UFBI, DISKIMAGE_TYPE_IPOD, DISKIMAGE_TYPE_UDRW
enum DiskImageType { DiskImageTypeElementsMacro };
extern vector<DiskImageType> DiskImageTypeElements;

#define DiskImageFsElementsMacro DISKIMAGE_FS_HFS, DISKIMAGE_FS_APFS
enum DiskImageFs { DiskImageFsElementsMacro };
extern vector<DiskImageFs> DiskImageFsElements;

#define DiskImageLayoutElementsMacro DISKIMAGE_LAYOUT_NONE, DISKIMAGE_LAYOUT_APPLE, DISKIMAGE_LAYOUT_MBR, DISKIMAGE_LAYOUT_GPT
enum DiskImageLayout { DiskImageLayoutElementsMacro };
extern vector<DiskImageLayout> DiskImageLayoutElements;

#define DiskImageEncryptionElementsMacro DISKIMAGE_ENCRYPTION_NONE, DISKIMAGE_ENCRYPTION_AES_128, DISKIMAGE_ENCRYPTION_AES_256
enum DiskImageEncryption { DiskImageEncryptionElementsMacro };
extern vector<DiskImageEncryption> DiskImageEncryptionElements;


class DiskImageKind
{
  
  protected:
    DiskImageType m_type;
    DiskImageFs m_fs;
    DiskImageLayout m_layout;
    DiskImageEncryption m_encryption;
    
  public:
	DiskImageKind() : m_type(DISKIMAGE_TYPE_SPARSE), m_fs(DISKIMAGE_FS_HFS), m_layout(DISKIMAGE_LAYOUT_NONE), m_encryption(DISKIMAGE_ENCRYPTION_NONE) {};
	DiskImageKind(const DiskImageKind& dik) : m_type(dik.type()), m_fs(dik.fs()), m_layout(dik.layout()), m_encryption(dik.encryption()) {};
    DiskImageKind(DiskImageType type, DiskImageFs fs, DiskImageLayout layout, DiskImageEncryption encryption) : m_type(type), m_fs(fs), m_layout(layout), m_encryption(encryption) {};
    DiskImageKind& operator = (const DiskImageKind& dik);

    virtual DiskImageType type() const { return m_type; }
    virtual string typeArg() const;
    virtual DiskImageFs fs() const { return m_fs; }
    virtual string fsArg() const;
    virtual DiskImageLayout layout() const { return m_layout; }
    virtual string layoutArg() const;
    virtual DiskImageEncryption encryption() const { return m_encryption; }
    virtual string encryptionArg() const;
    virtual string encryptionFullArgForCreate() const;

    virtual string nameWithoutExtension() const;
    virtual string extension() const;
    virtual string nameAndExtension() const;
    
    virtual bool isReadOnly() const;
    virtual bool canBeCreatedBlank() const;

};


class DiskImageKindFolder : public DiskImageKind
{
    
  public:
    DiskImageKindFolder() : DiskImageKind(DISKIMAGE_TYPE_UDCO, DISKIMAGE_FS_HFS, DISKIMAGE_LAYOUT_NONE, DISKIMAGE_ENCRYPTION_NONE) {};

    virtual DiskImageType type() const { throw stringPrintf("bug"); }
    virtual string typeArg() const { throw stringPrintf("bug"); }
    virtual DiskImageFs fs() const { throw stringPrintf("bug"); }
    virtual string fsArg() const { throw stringPrintf("bug"); }
    virtual DiskImageLayout layout() const  { throw stringPrintf("bug"); }
    virtual string layoutArg() const { throw stringPrintf("bug"); }
    virtual DiskImageEncryption encryption() const  { throw stringPrintf("bug"); }
    virtual string encryptionArg() const { throw stringPrintf("bug"); }

    virtual string name() const { throw stringPrintf("bug"); }
    virtual string extension() const { throw stringPrintf("bug"); }
    virtual string nameAndExtension() const { throw stringPrintf("bug"); }
    
    virtual bool isReadOnly() const { throw stringPrintf("bug"); }
    virtual bool canBeCreatedBlank() const { throw stringPrintf("bug"); }
};



#endif

