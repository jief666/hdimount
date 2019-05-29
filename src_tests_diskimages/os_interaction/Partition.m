//
//  Disk.m
//  MountDaemon
//
//  Created by Jief on 18/07/17.
//  Copyright © 2017 Jief. All rights reserved.
//
 #include <sys/param.h>
 #include <sys/ucred.h>
 #include <sys/mount.h>

#import <DiskArbitration/DiskArbitration.h>

#import "Disk.h"
#import "Partition.h"
#import "Utils.h"

//#include <lsof.h>

@implementation Partition

@synthesize status=_status;
@synthesize statusString=_statusString;

-(Partition*)initWithDisk:(Disk*)disk bsdDevice:(NSString*)bsdDevice;
{
    self = [super init];
    if (!self) return nil;
    
    _disk = disk;
    _bsdDevice = bsdDevice;
    
    return self;
 }

-(NSString*)bsdDevicePath;
{
    return [NSString stringWithFormat:@"/dev/%@", self.bsdDevice];
}

-(NSString*)getCurrentMountPoint;
{
    struct statfs* mounts;
    int num_mounts = getmntinfo(&mounts, MNT_WAIT);
    if (num_mounts < 0) {
      // do something with the error
    }
    
    for (int i = 0; i < num_mounts; i++) {
        struct statfs* mount = &mounts[i];
//NSLog(@"Disk type '%s' mounted on:%s from:%s", mount->f_fstypename, mount->f_mntonname, mount->f_mntfromname);
        if ( [self.bsdDevicePath isEqual:[NSString stringWithUTF8String:mount->f_mntfromname]] ) {
            return [NSString stringWithUTF8String:mounts[i].f_mntonname];
        }
    }
    return nil;
}


-(NSString*)asString;
{
    return [NSString stringWithFormat:@"Disk image %@, partition %lu (%@)", self.disk.diskImagePath, (unsigned long)[self.disk getPartitionNumber:self], self.bsdDevice];
}


#pragma mark umounting
void Partition_DiskUnmountCallback(DADiskRef diskRef, DADissenterRef dissenter, void *context);

void Partition_DiskUnmountCallback(DADiskRef diskRef, DADissenterRef dissenter, void *context)
{
    Partition* self = (__bridge Partition*)context;
    self->_statusString = nil;

//	NSMutableDictionary *info = nil;

//NSLog(@"%s %@ dissenter: %p", __func__, context, dissenter);
	
	if (dissenter) {
		self->_status = DADissenterGetStatus(dissenter);
        if ( self->_status != kDAReturnNotMounted )
        {
            self->_statusString = (NSString *) CFBridgingRelease(DADissenterGetStatusString(dissenter));
            if (!self->_statusString) {
                self->_statusString = [NSString stringWithFormat:@"%@: %s (0x%x)", NSLocalizedString(@"Dissenter status code", nil), strerror(err_get_code(self->_status)), self->_status];
            }
//NSLog(@"%s %@ dissenter: (%#x) %@", __func__, context, status, _statusString);

//            info = [NSMutableDictionary dictionary];
//            [info setObject:_statusString forKey:NSLocalizedFailureReasonErrorKey];
//            [info setObject:[NSNumber numberWithInt:status] forKey:@"DAStatusErrorKey"];
        }
	}
	else {
//NSLog(@"%s disk %@ unmounted", __func__, context);
	}
	
//	[[NSNotificationCenter defaultCenter] postNotificationName:@"DADiskDidAttemptMountNotification" object:(__bridge id _Nullable)(context) userInfo:info];

    self->callbackDone = true;
}

- (bool)umount2
{
    self->callbackDone = false;
    self->_status = 0;
    self->_statusString = nil;

    const char* bsdDevice = self.bsdDevice.fileSystemRepresentation;

    DASessionRef session = DASessionCreate(kCFAllocatorDefault);
    DASessionScheduleWithRunLoop(session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

    DADiskRef daDiskRef = DADiskCreateFromBSDName(kCFAllocatorDefault, session, bsdDevice);
//NSLog(@"daDiskRef %d", daDiskRef);
    DADiskUnmount(daDiskRef, kDADiskUnmountOptionDefault, Partition_DiskUnmountCallback, (__bridge void *)(self));

    while (!self->callbackDone) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:runModeWaitingTime]];
    }
    
    if ( session != nil ) {
        DASessionUnscheduleFromRunLoop(session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        CFRelease(session);
    }

    return _statusString == nil;
}

- (bool)eject2
{
    self->callbackDone = false;
    self->_status = 0;
    self->_statusString = nil;

    const char* bsdDevice = self.bsdDevice.fileSystemRepresentation;

    DASessionRef session = DASessionCreate(kCFAllocatorDefault);
    DASessionScheduleWithRunLoop(session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);

    DADiskRef daDiskRef = DADiskCreateFromBSDName(kCFAllocatorDefault, session, bsdDevice);
//NSLog(@"daDiskRef %d", daDiskRef);
    DADiskEject(daDiskRef, kDADiskUnmountOptionDefault, Partition_DiskUnmountCallback, (__bridge void *)(self));

    while (!self->callbackDone) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:runModeWaitingTime]];
    }
    
    if ( session != nil ) {
        DASessionUnscheduleFromRunLoop(session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        CFRelease(session);
    }

    return _statusString == nil;
}

- (bool)eject
{
    bool umountedSuccessfully = false;
    
    @try
    {

        NSString* currentMountPoint = [self getCurrentMountPoint];
        if ( currentMountPoint == nil ) {
//fprintf(stderr, "%s not monted, good!\n", [self.asString UTF8String]);
        }
        else
        {
            if ( ![self umount2] )
            {
                fprintf(stderr, "umount %s failed (%s).\n", [self.asString UTF8String], [_statusString UTF8String]);
                return NO;                        // <--- return
            }
// Only disks are ejected
//            if ( ![self eject2] )
//            {
//                fprintf(stderr, "eject %s failed (%s).\n", [self.asString UTF8String], [_statusString UTF8String]);
//                return NO;                        // <--- return
//            }
            // double checking
            {
                currentMountPoint = [self getCurrentMountPoint];
                if ( currentMountPoint != nil )
                {
                    _statusString = [NSString stringWithFormat:@"Unmount %@ succeded, but it's still mounted on %@", self.asString, currentMountPoint];
                    fprintf(stderr, "%s\n", [_statusString UTF8String]);
                    return NO;                        // <--- return
                }
            }

#ifdef DEBUG
  printf("%s successfully unmonted.\n", [self.asString UTF8String]);
#endif
        }
        umountedSuccessfully = true;
        return YES;
    }
    @catch (NSException* e) {
//        [appDelegate alertFromThread:@"%@", e.reason];

    }
//    if ( session != nil ) {
//        DASessionUnscheduleFromRunLoop(session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
//        CFRelease(session);
//    }
    return NO;

}


@end
