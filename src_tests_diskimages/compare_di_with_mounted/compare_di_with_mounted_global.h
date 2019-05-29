//
//  DiskImageTestCommon
//
//  Created by Jief on 2018-05-03.
//  Copyright © 2018 Jief. All rights reserved.
//

#ifdef DEBUG
//#  define MOVE_TESTED_DISKIMAGE_TO_TESTED_FOLDER
#else
#endif


#ifdef MOVE_TESTED_DISKIMAGE_TO_TESTED_FOLDER
    #define move_tested_diskimage_to_tested true
#else
	#define move_tested_diskimage_to_tested false
#endif
