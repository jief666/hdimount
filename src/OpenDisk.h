#ifndef OPENDISK_H
#define OPENDISK_H

#include <memory> // for unique_ptr

class FuseVolume;
//extern std::unique_ptr<FuseVolume> g_volumePtr;

FuseVolume* openDisk(const char* path, const char* password);

#endif

