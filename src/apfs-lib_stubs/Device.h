#ifndef __DEVICE_H__
#define __DEVICE_H__


#include "../../darling-dmg/src/Reader.h"

class Device
{
  public:

	Device(std::shared_ptr<Reader> reader) : m_reader(reader) {}
	virtual ~Device() {};

	virtual bool Open(const char *name) { throw "not used?"; }; // not used in APFSContainer
	virtual void Close() { throw "not used?"; }; // not used in APFSContainer

	virtual bool Read(void *data, uint64_t offs, uint64_t len) { return m_reader->read(data, len, offs); }
	virtual uint64_t GetSize() { throw "not used?"; }; // not used in APFSContainer

	static Device *OpenDevice(const char *name) { throw "not used?"; }; // not used in APFSContainer

  private:
	std::shared_ptr<Reader> m_reader;
};


#endif
