//
//  DiskImageCreation.hpp
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#ifndef DiskImageCreation_h
#define DiskImageCreation_h

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h> // size_t
#include <string>
#include <map>

#include "../DiskImageToTest.h"


void create_diskimage(const vector<DiskImageToTest>& ditVector, map<DiskImageToTest, vector<string>>* report);


#endif /* DiskImageCreation.h */
