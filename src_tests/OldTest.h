//
//  TestFile.hpp
//  hdimount--jief666--wip
//
//  Created by jief on 15/07/2018.
//  Copyright © 2018 jf-luce. All rights reserved.
//

#ifndef TestFile_hpp
#define TestFile_hpp

#include "FuseVolume.h"
#include "../src/darling-dmg/src/HFSHighLevelVolume.h"
#include "../src/darling-dmg/src/FileReader.h"
#include <memory> // for unique_ptr

void TestTmp(std::unique_ptr<FuseVolume>& g_volumePtr);
void TestFile(std::unique_ptr<FuseVolume>& g_volumePtr, const char* path);
void TestReader(std::shared_ptr<Reader> reader, const char* path_to_reference);
void FindFileForBand(HFSHighLevelVolume* g_volumePtr, uint64_t band_number);

#endif /* TestFile_hpp */
