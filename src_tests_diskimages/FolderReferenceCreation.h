//
//  DiskImageCreation.hpp
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#ifndef FolderReferenceCreation_h
#define FolderReferenceCreation_h

#include <string>
#include "DiskImageToTest.h"

/*
 * Create a folder at imgRefPath with a test set inside.
 * If folder already exists, do nothing, not even checking what's inside
 */
//void createFolderReference(const string& imgRefPath, DiskImageTestSize size);
void createFolderReference(DiskImage di);
vector<DiskImage> createFolderReferenceUsedByDiskImageVector(const vector<DiskImage>& diVector, const string& generatedReferencePathToFolder, bool deleteAndRecreate);
void ejectFolderReferenceUsedByDiskImageToTestVector_T(const vector<DiskImageToTest>& ditVector);
void ejectFolderReferenceUsedByDiskImageToTestVector(const vector<DiskImageToTest>& ditVector);


#endif /* FolderReferenceCreation.h */
