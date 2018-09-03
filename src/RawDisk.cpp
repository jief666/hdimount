#include "../darling-dmg/src/DMGDisk.h"
#include <stdexcept>
#include "../darling-dmg/src/be.h"
#include <iostream>
#include <cstring>
#include <memory>
#include <sstream>

#include "RawDisk.h"
#include "../darling-dmg/src/CachedReader.h"

#include "../darling-dmg/src/exceptions.h"
#include "Utils.h"

RawDisk::RawDisk(std::shared_ptr<Reader> reader)
	: m_reader(reader), m_zone(40000)
{
	Partition p;
	p.name = "raw";
	p.size = reader->length();
	p.offset = 0;
	m_partitions.push_back(p);
}

RawDisk::~RawDisk()
{
}

std::shared_ptr<Reader> RawDisk::readerForPartition(int index)
{
	if (index != 0)
		throw io_error("index must 0");

	return m_reader;
}
