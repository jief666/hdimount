#ifndef SparseimageDisk_H
#define SparseimageDisk_H
#include "darling-dmg/src/PartitionedDisk.h"
#include "darling-dmg/src/Reader.h"
#include "darling-dmg/src/dmg.h"
#include "darling-dmg/src/CacheZone.h"
#include <vector>

class SparseimageDisk : public Reader
{
public:
    static bool isSparseimageDisk(std::shared_ptr<Reader> reader);

	SparseimageDisk(std::shared_ptr<Reader> reader);
	~SparseimageDisk();

    virtual int32_t read(void* buf, int32_t count, uint64_t offset);
    virtual uint64_t length();
  private:

    int32_t _read(void* buffer, int32_t nbytes, uint64_t offset);

private:
	std::shared_ptr<Reader> m_reader;
    uint64_t m_size;
    uint32_t sectors_per_band;
    uint32_t m_bytes_per_band;
    
    typedef struct st_indexBlock {
        std::vector<uint32_t> indexVector;
        st_indexBlock* nextIndex;
    } ty_indexBlock;
    ty_indexBlock indexBlock;
};

#endif

