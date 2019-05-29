#include "SparseimageDisk.h"
#include <stdexcept>
#include "darling-dmg/src/be.h"
#include <iostream>
#include <cstring>
#include <memory>
#include <sstream>
#include "darling-dmg/src/DMGPartition.h"
#include "darling-dmg/src/AppleDisk.h"
#include "darling-dmg/src/GPTDisk.h"
#include "darling-dmg/src/CachedReader.h"
#include "darling-dmg/src/SubReader.h"
#include "darling-dmg/src/exceptions.h"
#include "darling-dmg/src/DarlingDMGCrypto.h" // for base64decode


bool SparseimageDisk::isSparseimageDisk(std::shared_ptr<Reader> reader)
{
    uint32_t sprsSignature;
    if (reader->read(&sprsSignature, 4, 0) != 4)
        return false;
    return sprsSignature == 0x73727073;
}

#pragma pack(1)

struct st_sparseimageHeader {
    uint32_t sprsSignature;
    uint32_t unknown1;
    uint32_t nbSectorsInBand;
    uint32_t unknown2;
    uint32_t nonSparseNbSectors;
    uint64_t nextIndex;
    uint32_t unknown4[9];
    uint32_t indexArray[1024-16];
} /*__attribute__ ((packed))*/;
static_assert(sizeof(struct st_sparseimageHeader) == 4096, "sizeof(struct st_sparseimageHeader) != 4096");

struct st_sparseimageHeaderSecondary {
    uint32_t sprsSignature;
    uint32_t unknown1[2];
    uint64_t nextIndex;
    uint32_t unknown2[9];
    uint32_t indexArray[1024-14];
} /*__attribute__ ((packed))*/;
static_assert(sizeof(struct st_sparseimageHeaderSecondary) == 4096, "sizeof(struct st_sparseimageHeaderSecondary) != 4096");

#pragma pack()

SparseimageDisk::SparseimageDisk(std::shared_ptr<Reader> reader)
	: m_reader(reader)
{
    indexBlock.nextIndex = NULL;
    struct st_sparseimageHeader sparseimageHeader;
    int nblu = m_reader->read(&sparseimageHeader, sizeof(sparseimageHeader), 0);
    if ( nblu != 4096 ) throw new io_error("Cannot read header");
    if ( sparseimageHeader.sprsSignature != 0x73727073 ) throw new io_error("Cannot find sparse signature");
    m_size = uint64_t(be(sparseimageHeader.nonSparseNbSectors))*512;
    sectors_per_band = be(sparseimageHeader.nbSectorsInBand);
    if ( sectors_per_band > UINT32_MAX/512 ) throw io_error("Sectors per band is too big (>UINT32_MAX/512)");
    m_bytes_per_band = sectors_per_band*512;
    if ( m_bytes_per_band > INT32_MAX ) throw io_error("Nb bytes per band is too big (>INT32_MAX)");

//printf("reading index array from 1st bloc\n");
    for (unsigned int i = 0 ; sparseimageHeader.indexArray[i] != 0 && i < sizeof(sparseimageHeader.indexArray)/sizeof(sparseimageHeader.indexArray[0]) ; i++) {
        indexBlock.indexVector.push_back(be(sparseimageHeader.indexArray[i]));
//printf("index[%d]=%d\n", i, be(sparseimageHeader.indexArray[i]));
    }
    
    ty_indexBlock* currentBlock = &indexBlock;
    uint64_t nextIndex = be(sparseimageHeader.nextIndex);
    while ( nextIndex )
    {
        currentBlock->nextIndex = new ty_indexBlock;
        currentBlock->nextIndex->nextIndex = NULL;
        struct st_sparseimageHeaderSecondary sparseimageHeaderSeconday;
        int nblu2 = m_reader->read(&sparseimageHeaderSeconday, sizeof(sparseimageHeaderSeconday), nextIndex);
        if ( nblu2 != 4096 ) throw new io_error("Cannot read header");
        if ( sparseimageHeaderSeconday.sprsSignature != 0x73727073 ) throw new io_error("Cannot find sparse signature");
//printf("reading index array from secondary bloc at address %llx\n", nextIndex);
        for (unsigned int i = 0 ; sparseimageHeaderSeconday.indexArray[i] != 0 && i < sizeof(sparseimageHeaderSeconday.indexArray)/sizeof(sparseimageHeaderSeconday.indexArray[0]) ; i++) {
            currentBlock->nextIndex->indexVector.push_back(be(sparseimageHeaderSeconday.indexArray[i]));
//printf("index[%d]=%d\n", i, be(sparseimageHeaderSeconday.indexArray[i]));
        }
        nextIndex = be(sparseimageHeaderSeconday.nextIndex);
        currentBlock = currentBlock->nextIndex;
    }
}

SparseimageDisk::~SparseimageDisk()
{
}


uint64_t SparseimageDisk::length()
{
    return m_size;
}

int32_t SparseimageDisk::_read(void* buffer, int32_t nbytes, uint64_t offset)
{
    if ( nbytes < 0 ) return -1;
    if ( offset > m_size ) return -1;
    
    uint32_t band_start_offset = uint32_t(offset & ~(uint64_t(m_bytes_per_band-1))); // m_bytes_per_band is uint32_t, but a check is made in ctor to ensure m_bytes_per_band < INT32_MAX
    if ( nbytes > int32_t(m_bytes_per_band - (offset-band_start_offset)) ) {           // but a check is made in ctor to ensure m_bytes_per_band < INT32_MAX
        nbytes = int32_t(m_bytes_per_band - (offset-band_start_offset));
    }
    uint64_t band_number = band_start_offset / m_bytes_per_band;

    ty_indexBlock* currentBlock = &indexBlock;
    uint32_t idx_offset = 0;
    uint64_t block_offset = 4096;
    while ( currentBlock )
    {
        for (size_t idx = 0 ; idx < currentBlock->indexVector.size() ; idx++)
        {
            if ( currentBlock->indexVector[idx]-1  ==  band_number ) {
                uint64_t fileOffset = block_offset + uint64_t(idx+idx_offset) * m_bytes_per_band + offset-band_start_offset;
                return m_reader->read(buffer, nbytes, fileOffset);
            }
        }
        idx_offset += currentBlock->indexVector.size();
        currentBlock = currentBlock->nextIndex;
        block_offset += 4096;
    }
//printf("band for offset %lld doesn't exist : returning %d zeros\n", offset, nbytes);
    memset(buffer, 0, nbytes);
    return nbytes;
}

int32_t SparseimageDisk::read(void* buffer, int32_t nbytes, uint64_t offset)
{
    int32_t nbread = 0;
    int32_t nbreadTotal = 0;
    do {
        nbread = _read(((uint8_t*)buffer)+nbreadTotal, nbytes-nbreadTotal, offset+nbreadTotal);
        if ( nbread < 0 ) return nbread;
        nbreadTotal += nbread;
        if ( nbread == 0 ) return nbreadTotal;
    }while ( nbreadTotal < nbytes );
    return nbreadTotal;
}
