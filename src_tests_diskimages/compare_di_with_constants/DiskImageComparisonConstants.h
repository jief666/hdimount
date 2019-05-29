//
//  DiskImageCreation.hpp
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#ifndef DiskImageComparison_h
#define DiskImageComparison_h

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h> // size_t

#include "../DiskImageToTest.h"

void compare_diskimage_with_constants(const DiskImageToTest &dit, bool moveToFolderOk);
void compare_diskimage_with_constants(const vector<DiskImageToTest>& ditVector, bool moveToFolderOk, map<DiskImageToTest, vector<string>>* report);

#endif /* DiskImageCreation.h */
