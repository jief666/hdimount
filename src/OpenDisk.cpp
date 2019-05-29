#include "OpenDisk.h"

#include <errno.h>

#include <memory>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <stdexcept>
#include <limits>
#include <functional>

#include <fuse.h>

#include "darling-dmg/src/be.h"
#include "darling-dmg/src/HFSVolume.h"
#include "darling-dmg/src/AppleDisk.h"
#include "darling-dmg/src/GPTDisk.h"
#include "MBRDisk.h"
#include "darling-dmg/src/DMGDisk.h"
#include "RawDisk.h"
#include "darling-dmg/src/FileReader.h"
#include "darling-dmg/src/CachedReader.h"
#include "darling-dmg/src/exceptions.h"
#include "darling-dmg/src/HFSHighLevelVolume.h"
#include "darling-dmg/src/EncryptReader.h"
//#include "SparsefsReader.h"
#include <random>
#include <stdint.h>
#include <inttypes.h>
#include "FuseVolume.h"
#include "HfsFuseVolume.h"
#include "SparsebundleReader.h"
#include "SparseimageDisk.h"

#define be apfs_be
#define le apfs_le
//namespace apfs {
//#include "apfs-fuse/ApfsLib/Endian.h"
//}
//using namespace ::apfs;
#include "apfs-fuse/ApfsLib/ApfsContainer.h"
//#include "ApfsFuseOps.h"
#include "ApfsFuseVolume.h"
//// apfs bridge
#include "Device.h"


#include "HfsFuseVolume.h"
#include "Utils.h"


#define APFS_SIGNATURE 0x4253584E  // "NXSB"

bool isAPFS(std::shared_ptr<Reader> reader)
{
    uint32_t apfs_magic; // APSB, 0x42535041
    if (reader->read(&apfs_magic, sizeof(apfs_magic), 32) != sizeof(apfs_magic))
        return false;

    return apfs_magic == APFS_SIGNATURE;
}

Device* g_DevicePtr;

FuseVolume* openDisk(const char* path, const char* password)
{
    std::shared_ptr<Reader> fileReader;
    std::shared_ptr<HFSVolume> volume;
    std::unique_ptr<PartitionedDisk> partitions;

    std::shared_ptr<Reader> fileReaderToCheckEncryption;
    if ( strstr(path, ".sparsebundle") == path + strlen(path) - strlen(".sparsebundle") ) {
           fileReaderToCheckEncryption = std::make_shared<SparsebundleReader>(path);
    }else{
        fileReaderToCheckEncryption = std::make_shared<FileReader>(path);
    }

    if (EncryptReader::isEncrypted(fileReaderToCheckEncryption)) {
		if ( password == NULL  || password[0] == 0 ) {
			char* thePassword = getpass("Password: ");
			fileReader = std::make_shared<EncryptReader>(fileReaderToCheckEncryption, thePassword); // password can be NULL
			for ( size_t i = 0 ; i < strlen(thePassword) ; i++ )
				thePassword[i] = ' ';
		}else{
			fileReader = std::make_shared<EncryptReader>(fileReaderToCheckEncryption, password); // password can be NULL
		}
    }else{
        fileReader = fileReaderToCheckEncryption;
    }

    if (SparseimageDisk::isSparseimageDisk(fileReader)) {
//        std::cerr << "Detected a sparse image." << std::endl;
        fileReader = std::make_shared<SparseimageDisk>(fileReader);
    }
    
    if (DMGDisk::isDMG(fileReader)) {
//        std::cerr << "Detected an image from device." << std::endl;
        partitions.reset(new DMGDisk(fileReader));
    }else if (GPTDisk::isGPTDisk(fileReader)) {
//        std::cerr << "Detected a GPT partionned disk." << std::endl;
        partitions.reset(new GPTDisk(fileReader));
    }else if (MBRDisk::isMBRDisk(fileReader)) {
//        std::cerr << "Detected a GPT partionned disk." << std::endl;
        partitions.reset(new MBRDisk(fileReader));
    }else if (AppleDisk::isAppleDisk(fileReader)) {
//        std::cerr << "Detected a Apple map partionned disk." << std::endl;
        partitions.reset(new AppleDisk(fileReader));
    }else{
//        std::cerr << "Fallback to raw disk." << std::endl;
        partitions.reset(new RawDisk(fileReader));
    }

    std::shared_ptr<Reader> partitionsReaderPtr;
    const std::vector<PartitionedDisk::Partition>& parts = partitions->partitions();

    for (int i = 0; size_t(i) < parts.size(); i++) //*************************************************************************
    {
        partitionsReaderPtr = partitions->readerForPartition(i);
        if ( partitionsReaderPtr == NULL ) {
            throw function_not_implemented_error(stringPrintf("Cannot get reader for partition %d", i));
        }

        if (HFSVolume::isHFSPlus(partitionsReaderPtr))
        {
//std::cout << "Using HFS partition #" << i << " of type " << parts[i].type << std::endl;
            HFSHighLevelVolume* hfsv = new HFSHighLevelVolume(std::make_shared<HFSVolume>(partitionsReaderPtr));
            return new HfsFuseVolume(std::unique_ptr<HFSHighLevelVolume>(hfsv));
        }
        else if (isAPFS(partitionsReaderPtr)) {
//std::cout << "Using APFS partition #" << i << " of type " << parts[i].type << std::endl;
            g_DevicePtr = new Device(partitionsReaderPtr);
            ApfsContainer* apfsContainer = new ApfsContainer(g_DevicePtr, 0, partitionsReaderPtr->length(), NULL, 0, 0);
            if ( apfsContainer->Init() ) {
                unsigned int volume_id = 0;
                ApfsVolume* apfsVolume;
                if ( apfsContainer->IsVolumeEncrypted(volume_id) ) {
					if ( password == NULL  || password[0] == 0 ) {
						char* thePassword = getpass("Password: ");
						if ( password == NULL  || password[0] == 0 ) {
							std::string hint = apfsContainer->GetPasswordHintForVolumeIndex(volume_id);
							if ( !hint.empty() )
								printf("Hint: %s\n", hint.c_str());
							thePassword = getpass("Password: ");
						}
						apfsVolume = apfsContainer->GetVolume(volume_id, thePassword);
						for ( size_t idx = 0 ; idx < strlen(thePassword) ; idx++ )
							thePassword[idx] = ' ';
					}else{
						apfsVolume = apfsContainer->GetVolume(volume_id, password);
					}
				}else{
					apfsVolume = apfsContainer->GetVolume(volume_id);
				}
                if ( apfsVolume ) {
                    return new ApfsFuseVolume(std::unique_ptr<ApfsVolume>(apfsVolume));
                }
            }
            delete apfsContainer;
            delete g_DevicePtr;
        }
    }
    throw function_not_implemented_error("Unsupported file format");
}


