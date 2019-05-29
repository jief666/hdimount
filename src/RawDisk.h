#ifndef RAWDISK_H
#define RAWDISK_H
#include "darling-dmg/src/PartitionedDisk.h"
#include "darling-dmg/src/Reader.h"
#include "darling-dmg/src/dmg.h"
#include "darling-dmg/src/CacheZone.h"

class RawDisk : public PartitionedDisk
{
public:
	RawDisk(std::shared_ptr<Reader>reader);
	virtual ~RawDisk();

	virtual const std::vector<Partition>& partitions() const override { return m_partitions; }
	virtual std::shared_ptr<Reader> readerForPartition(int index) override;

private:
	std::shared_ptr<Reader> m_reader;
	std::vector<Partition> m_partitions;
	CacheZone m_zone;
};

#endif

