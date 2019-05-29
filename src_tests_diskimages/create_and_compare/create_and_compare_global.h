//
//  DiskImageTestCommon
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//
#include "../common.h"

#ifdef DEBUG
#define DELETE_REFERENCE_AT_STARTUP
#define DELETE_GENERATEDIMAGES_AT_STARTUP
//#define DELETE_GENERATEDIMAGESTESTED_AT_STARTUP
//#  define MOVE_TESTED_DISKIMAGE_TO_TESTED_FOLDER
#endif

#ifdef DELETE_REFERENCE_AT_STARTUP
#    define delete_reference_at_startup true
#else
#    define delete_reference_at_startup false
#endif

#ifdef MOVE_TESTED_DISKIMAGE_TO_TESTED_FOLDER
    #define move_tested_diskimage_to_tested true
#else
	#define move_tested_diskimage_to_tested false
#endif
