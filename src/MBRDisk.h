#ifndef MBRDISK_H
#define MBRDISK_H
#include "darling-dmg/src/Reader.h"
#include "darling-dmg/src/PartitionedDisk.h"
#include <memory>

class MBRDisk : public PartitionedDisk
{
public:
	MBRDisk(std::shared_ptr<Reader> readerWholeDisk);
	MBRDisk(std::shared_ptr<Reader> protectiveMBR, std::shared_ptr<Reader> partitionTable);
	
	virtual const std::vector<Partition>& partitions() const override { return m_partitions; }
	virtual std::shared_ptr<Reader> readerForPartition(int index) override;
	
	static bool isMBRDisk(std::shared_ptr<Reader> reader);
private:
	void loadPartitions(std::shared_ptr<Reader> table);
private:
	std::shared_ptr<Reader> m_reader;
	std::vector<Partition> m_partitions;
};

#endif
