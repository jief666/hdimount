//
//  Disk.h
//  MountDaemon
//
//  Created by Jief on 18/07/17.
//  Copyright © 2017 Jief. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <DiskArbitration/DiskArbitration.h>


@class Partition;

@interface Disk : NSObject
{
    bool callbackDone;
}
@property (readonly) DAReturn status;
@property (readonly) NSString* statusString;

@property (readonly) NSURL* diskImagePath;
@property (readonly) NSString* bsdDevice; // just diskX
@property (readonly) NSString* bsdDevicePath; // with "/dev/" in front of diskX
//@property (readonly) NSArray* mountPointArray;
@property (readonly) NSMutableArray* partitionArray;

//+(NSString*)getBSDDeviceForImagePath:(NSString*)image_path;

-(id)init __attribute__((unavailable("Must use initWithPath:mountPointArray: instead.")));
-(Disk*)initWithPath:(NSURL*)diskImagePath;

-(NSUInteger)getPartitionNumber:(Partition*)partition;
-(bool)mountedOn:(NSString*)path;

-(bool)eject;


@end
