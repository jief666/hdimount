//
//  DiskImageTestCommon
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//
#include "../common.h"

#ifdef DEBUG
//#define DELETE_REFERENCE_AT_STARTUP
//#define DELETE_GENERATEDIMAGES_AT_STARTUP
#endif

#ifdef DELETE_REFERENCE_AT_STARTUP
#    define delete_reference_at_startup true
#else
#    define delete_reference_at_startup false
#endif
