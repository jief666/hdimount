#include "MBRDisk.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstring>
#include "darling-dmg/src/gpt.h"
#include "darling-dmg/src/SubReader.h"
#include "darling-dmg/src/exceptions.h"

MBRDisk::MBRDisk(std::shared_ptr<Reader> reader)
: m_reader(reader)
{
	loadPartitions(m_reader);
}

MBRDisk::MBRDisk(std::shared_ptr<Reader> protectiveMBR, std::shared_ptr<Reader> partitionTable)
	: m_reader(nullptr)
{
	if (!isMBRDisk(protectiveMBR))
		throw io_error("Not a MBR disk!");
	loadPartitions(partitionTable);
}

bool MBRDisk::isMBRDisk(std::shared_ptr<Reader> reader)
{
    uint64_t offset = 512;
    GPTHeader header;
	if (reader->read(&header, sizeof(header), offset) != sizeof(header))
		return false;
	if (strncmp(header.signature, "EFI PART", 8) == 0) // It's a MBR/GPT disk. Consider it as GPT
		return false;
	ProtectiveMBR mbr;
	if (reader->read(&mbr, sizeof(mbr), 0) != sizeof(mbr))
		return false;
	
	return mbr.signature == MBR_SIGNATURE;
}

void MBRDisk::loadPartitions(std::shared_ptr<Reader> table)
{
    ProtectiveMBR mbr;
    if (table->read(&mbr, sizeof(mbr), 0) != sizeof(mbr))
        throw new io_error("Not MBR");
    
    for ( int i=0 ; i<4 ; i++ ) {
        if ( mbr.partitions[i].type != 0  &&  mbr.partitions[0].type != MPT_GPT_FAKE_TYPE ) {
            Partition p;
            p.name = "Mbr" + std::to_string(mbr.partitions[i].type);
            p.offset = uint64_t(mbr.partitions[i].lbaFirst) * 512;
            p.size = uint64_t(mbr.partitions[i].numSectors) * 512;
            m_partitions.push_back(p);
        }
    }
}

std::shared_ptr<Reader> MBRDisk::readerForPartition(int index)
{
	const Partition& part = m_partitions.at(index);
	return std::shared_ptr<Reader>(new SubReader(m_reader, part.offset, part.size));
}

