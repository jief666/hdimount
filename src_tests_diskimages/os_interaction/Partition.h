//
//  Disk.h
//  MountDaemon
//
//  Created by Jief on 18/07/17.
//  Copyright © 2017 Jief. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <DiskArbitration/DiskArbitration.h>

@class Disk;

@interface Partition : NSObject
{
    bool callbackDone;
    
}

@property (readonly) Disk* disk;
@property (readonly) NSString* bsdDevice; // just diskXxX
@property (readonly) NSString* bsdDevicePath; // with "/dev/" in front of diskX

@property (readonly) DAReturn status;
@property (readonly) NSString* statusString;


-(id)init __attribute__((unavailable("Must use initWithDisk:mountPoint: instead.")));
-(Partition*)initWithDisk:(Disk*)disk bsdDevice:(NSString*)bsdDevice;

-(NSString*)getCurrentMountPoint;
-(NSString*)asString;

-(bool)eject;

@end
