//
//  Disk.m
//  MountDaemon
//
//  Created by Jief on 18/07/17.
//  Copyright © 2017 Jief. All rights reserved.
//
// jief : see https://stackoverflow.com/questions/16922158/how-do-i-iterate-through-io-kits-keys

#include <sys/param.h>
#include <sys/ucred.h>
#include <sys/mount.h>
#include <sys/stat.h>

#import <DiskArbitration/DiskArbitration.h>

#import "NSURL.h"
#import "Partition.h"
#import "Disk.h"
#import "Utils.h"
#import "IOObjectT.h"


//-------------------------------------------------------------------------------------------------------------------------------------------
@implementation Disk


-(void)getPartitionArrayForNodeOfKindDiskImage:(IOObjectT*)IOBlockStorageDriver_node
{
//    NSArray<NSString*>* MediaNodeName = [NSArray arrayWithObjects: @"Apple sparse bundle disk image Media", @"Apple sparse disk image Media", @"Apple read/write Media", @"Apple UDIF read-only Media", @"Apple UDIF read-only compressed (ADC) Media", @"Apple UDIF read-only compressed (zlib) Media", nil];

    IOObjectT* IOGUIDPartitionScheme = [IOBlockStorageDriver_node getTheOnlyChildNamed:@"IOGUIDPartitionScheme"];
    if ( IOGUIDPartitionScheme != 0 ) {
        IOIteratorT* IOGUIDPartitionSchemeIterator = [IOGUIDPartitionScheme iterator];
        IOObjectT* partition;
        while ( (partition = [IOGUIDPartitionSchemeIterator next]) )
        {  
//NSLog(@"Partition name is :  %@\n",[partition name]);
            NSMutableDictionary* partitionProperties = [partition properties];
            NSString *bsdName = [partitionProperties valueForKey:@"BSD Name"];
//NSLog(@"bsdName=%@\n",bsdName);
            
            // Same as below
            IOObjectT* AppleAPFSContainerScheme = [partition getTheOnlyChildNamed:@"AppleAPFSContainerScheme"];
            if ( AppleAPFSContainerScheme != 0 ) {
                IOIteratorT* AppleAPFSContainerSchemeIterator = [AppleAPFSContainerScheme iterator];
                IOObjectT* media;
                while ( (media = [AppleAPFSContainerSchemeIterator next]) )
                {
                    IOObjectT* AppleAPFSContainer = [media getTheOnlyChildNamed:@"AppleAPFSContainer"];
                    if ( AppleAPFSContainer != 0 ) {
                        IOIteratorT* AppleAPFSContainerIterator = [AppleAPFSContainer iterator];
                        IOObjectT* AppleAPFSContainerChild;
                        while ( (AppleAPFSContainerChild = [AppleAPFSContainerIterator next]) )
                        {
                            //NSLog(@"Partition name is :  %@\n",[partition name]);
                            NSMutableDictionary* AppleAPFSContainerChildProperties = [AppleAPFSContainerChild properties];
                            NSString *AppleAPFSContainerChildBsdName = [AppleAPFSContainerChildProperties valueForKey:@"BSD Name"];
                            //NSLog(@"bsdName=%@\n",bsdName);
                            [_partitionArray addObject:[[Partition alloc] initWithDisk:self bsdDevice:AppleAPFSContainerChildBsdName]];
                        }
                    }
                }
            }else{
                [_partitionArray addObject:[[Partition alloc] initWithDisk:self bsdDevice:bsdName]];
            }
        }
        return;
    }
    // same as before
    IOObjectT* AppleAPFSContainerScheme = [IOBlockStorageDriver_node getTheOnlyChildNamed:@"AppleAPFSContainerScheme"];
    if ( AppleAPFSContainerScheme != 0 ) {
        IOIteratorT* AppleAPFSContainerSchemeIterator = [AppleAPFSContainerScheme iterator];
        IOObjectT* media;
        while ( (media = [AppleAPFSContainerSchemeIterator next]) )
        {
            IOObjectT* AppleAPFSContainer = [media getTheOnlyChildNamed:@"AppleAPFSContainer"];
            if ( AppleAPFSContainer != 0 ) {
                IOIteratorT* AppleAPFSContainerIterator = [AppleAPFSContainer iterator];
                IOObjectT* partition;
                while ( (partition = [AppleAPFSContainerIterator next]) )
                {
                    //NSLog(@"Partition name is :  %@\n",[partition name]);
                    NSMutableDictionary* partitionProperties = [partition properties];
                    NSString *bsdName = [partitionProperties valueForKey:@"BSD Name"];
                    //NSLog(@"bsdName=%@\n",bsdName);
                    [_partitionArray addObject:[[Partition alloc] initWithDisk:self bsdDevice:bsdName]];
                }
            }
        }
        return;
    }
    IOObjectT* IOApplePartitionScheme = [IOBlockStorageDriver_node getTheOnlyChildNamed:@"IOApplePartitionScheme"];
    if ( IOApplePartitionScheme != 0 ) {
        IOIteratorT* IOApplePartitionSchemeIterator = [IOApplePartitionScheme iterator];
        IOObjectT* partition;
        while ( (partition = [IOApplePartitionSchemeIterator next]) )
        {
//NSLog(@"Partition name is :  %@\n",[partition name]);
NSMutableDictionary* partitionProperties = [partition properties];
NSString *bsdName = [partitionProperties valueForKey:@"BSD Name"];
//NSLog(@"bsdName=%@\n",bsdName);
            [_partitionArray addObject:[[Partition alloc] initWithDisk:self bsdDevice:bsdName]];
        }
        return;
    }
    IOObjectT* IOFDiskPartitionScheme = [IOBlockStorageDriver_node getTheOnlyChildNamed:@"IOFDiskPartitionScheme"];
    if ( IOFDiskPartitionScheme != 0 ) {
        IOIteratorT* IOFDiskPartitionSchemeIterator = [IOFDiskPartitionScheme iterator];
        IOObjectT* partition;
        while ( (partition = [IOFDiskPartitionSchemeIterator next]) )
        {
//NSLog(@"Partition name is :  %@\n",[partition name]);
NSMutableDictionary* partitionProperties = [partition properties];
NSString *bsdName = [partitionProperties valueForKey:@"BSD Name"];
//NSLog(@"bsdName=%@\n",bsdName);
            [_partitionArray addObject:[[Partition alloc] initWithDisk:self bsdDevice:bsdName]];
        }
        return;
    }

    IOObjectT* IOMediaBSDClient = [IOBlockStorageDriver_node getTheOnlyChildNamed:@"IOMediaBSDClient"];
    if ( IOMediaBSDClient != 0 ) {
        NSMutableDictionary* AppleSparseBundleDiskImageMediaProperties = [IOBlockStorageDriver_node properties];
        NSString *bsdName = [AppleSparseBundleDiskImageMediaProperties valueForKey:@"BSD Name"];
        [_partitionArray addObject:[[Partition alloc] initWithDisk:self bsdDevice:bsdName]];
    }
}

-(void)getPartitionArrayForNodeOfKindIOHDIXHDDriveOutKernel:(IOObjectT*)node
{
    IOObjectT* IODiskImageBlockStorageDeviceOutKernel = [node getTheOnlyChildNamed:@"IODiskImageBlockStorageDeviceOutKernel"];
    if ( IODiskImageBlockStorageDeviceOutKernel != 0 ) {
        IOObjectT* IOBlockStorageDriver = [IODiskImageBlockStorageDeviceOutKernel getTheOnlyChildNamed:@"IOBlockStorageDriver"];
        if ( IOBlockStorageDriver != 0 ) {
            IOObjectT* DiskImageMedia = [IOBlockStorageDriver getTheOnlyChild_T];
            if ( DiskImageMedia != 0 ) {
                [self getPartitionArrayForNodeOfKindDiskImage:DiskImageMedia];
            }
        }
    }
}

-(NSString*)getBSDDeviceForNode:(IOObjectT*)IOHDIXHDDriveOutKernel_node
{

//NSMutableDictionary* IOHDIXHDDriveOutKernel_node_properties = [IOHDIXHDDriveOutKernel_node properties];
//NSString *anImage_path = [[NSString alloc] initWithData:(NSData *)[IOHDIXHDDriveOutKernel_node_properties valueForKey:@"image-path"] encoding:NSASCIIStringEncoding];
//NSString *image_alias = [[NSString alloc] initWithData:(NSData *)[IOHDIXHDDriveOutKernel_node_properties valueForKey:@"image-alias"] encoding:NSASCIIStringEncoding];
//NSLog(@"  Disk image-path: %@", anImage_path);
//NSLog(@"  Disk image_alias: %@", image_alias);
    
    IOObjectT* IODiskImageBlockStorageDeviceOutKernel_node = [IOHDIXHDDriveOutKernel_node getTheOnlyChildNamed:@"IODiskImageBlockStorageDeviceOutKernel" ];
    IOObjectT* IOBlockStorageDriver_node = [IODiskImageBlockStorageDeviceOutKernel_node getTheOnlyChildNamed:@"IOBlockStorageDriver"];
    
    IOObjectT* Media_node = [IOBlockStorageDriver_node getTheOnlyChild_T];
    if ( Media_node != 0 ) {
        NSMutableDictionary* Media_node_properties = [Media_node properties];
        NSString* bsdName = [Media_node_properties valueForKey:@"BSD Name"];
        if ( bsdName ) return bsdName;
    }
    return nil;
}

-(IOObjectT*)getIOKitNodeForImagePath
{

    struct stat sstat;
    int ret = stat(self.diskImagePath.path.UTF8String, &sstat);

    if ( ret != 0 )
    {
//        if ( [self.diskImagePath characterAtIndex:0] != '/' ) {
//            @throw [NSString stringWithFormat:@"Stat failed : %s (errno %d) for '%@'. Current dir is '%@'.\n", strerror(errno), errno, self.diskImagePath, [[NSFileManager defaultManager] currentDirectoryPath]];
//        }else{
            @throw [NSString stringWithFormat:@"Stat failed : %s (errno %d) for '%@'\n", strerror(errno), errno, self.diskImagePath];
//        }
    }
            
    NSString* the_backingstore_id = [NSString stringWithFormat:@"d%d:i%llu", sstat.st_dev, sstat.st_ino];
//NSLog(@"%@", the_backingstore_id);



  IOObjectT* returnValue = 0;
    IOObjectT* IOHDIXController_service = [IOObjectT with:IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("IOHDIXController"))];
    assert(IOHDIXController_service);
    //    io_connect_t connect;
    //    assert(!IOServiceOpen(service, mach_task_self(), 0, &connect));

    IOIteratorT* IOHDIXController_child_node_iterator = [IOHDIXController_service iterator];
    IOObjectT* IOHDIXController_child_node;
    while ( (IOHDIXController_child_node = [IOHDIXController_child_node_iterator next] ) )
    {  
        NSString* IOHDIXController_child_name = [IOHDIXController_child_node name];
        NSMutableDictionary* IOHDIXController_child_node_properties = [IOHDIXController_child_node properties];
        
//NSLog(@"Name is :  %@@%x\n",IOHDIXController_child_name, [iounit intValue]); // must display IOHDIXController@x
//for (NSString *key in IOHDIXController_child_properties) NSLog(@"  key=%@", key);

        // **** it's an IOHDIXHDDriveOutKernel node
        if ( [@"IOHDIXHDDriveOutKernel" isEqualToString:IOHDIXController_child_name] )
        {
            IOObjectT* IOHDIXHDDriveOutKernel_node = IOHDIXController_child_node; // just for convinience of reading
            NSMutableDictionary* IOHDIXHDDriveOutKernel_node_properties = IOHDIXController_child_node_properties; // just for convinience of reading
            
            if ( IOHDIXHDDriveOutKernel_node_properties ) {
                NSDictionary* backingstore_id = [IOHDIXController_child_node_properties valueForKey:@"backingstore-id"];
                if ( backingstore_id ) {
                   NSArray* writeableComponents = [backingstore_id valueForKey:@"writeable-components"];
                   if ( writeableComponents ) {
                        if ( writeableComponents.count != 1 ) @throw @"writeableComponents.count != 1";
                        NSString* writeableComponentsElement = [writeableComponents objectAtIndex:0];
                        if ( ! [writeableComponentsElement isKindOfClass:NSString.class] ) @throw @"! [writeableComponentsElement isKindOfClass:NSString.class]";
                        if ( [the_backingstore_id isEqualToString:writeableComponentsElement] )
                        {
                            
//NSString *image_path = [[NSString alloc] initWithData:(NSData *)[IOHDIXHDDriveOutKernel_node_properties valueForKey:@"image-path"] encoding:NSASCIIStringEncoding];
//NSString *image_alias = [[NSString alloc] initWithData:(NSData *)[IOHDIXHDDriveOutKernel_node_properties valueForKey:@"image-alias"] encoding:NSASCIIStringEncoding];
//NSLog(@"  Disk image-path: %@", image_path);
//NSLog(@"  Disk image_alias: %@", image_alias);
                            return IOHDIXHDDriveOutKernel_node;
                        }
                    }else{
                        NSArray* readonlyComponents = [backingstore_id valueForKey:@"readonly-components"];
                        if ( readonlyComponents ) {
                            if ( readonlyComponents.count != 1 ) @throw @"readonlyComponents.count != 1";
                            NSString* readonlyComponentsElement = [readonlyComponents objectAtIndex:0];
//NSLog(@"%@", readonlyComponentsElement);
                            if ( ! [readonlyComponentsElement isKindOfClass:NSString.class] ) @throw @"! [readonlyComponentsElement isKindOfClass:NSString.class]";
                            if ( [the_backingstore_id isEqualToString:readonlyComponentsElement] )
                            {
                                
//NSString *image_path = [[NSString alloc] initWithData:(NSData *)[IOHDIXHDDriveOutKernel_node_properties valueForKey:@"image-path"] encoding:NSASCIIStringEncoding];
//NSString *image_alias = [[NSString alloc] initWithData:(NSData *)[IOHDIXHDDriveOutKernel_node_properties valueForKey:@"image-alias"] encoding:NSASCIIStringEncoding];
//NSLog(@"  Disk image-path: %@", image_path);
//NSLog(@"  Disk image_alias: %@", image_alias);
                                return IOHDIXHDDriveOutKernel_node;
                            }
                        }
                    }
                }
            }
        }
    }
    return returnValue;
}

-(Disk*)initWithPath:(NSURL*)diskImagePath;
{
    self = [super init];
    if (!self) return nil;
    
    _diskImagePath = diskImagePath.resolveURL;
    
    _partitionArray = [[NSMutableArray alloc] init];
    
//NSLog(@"Init for image %@", diskImagePath.path);
    
    IOObjectT* node = [self getIOKitNodeForImagePath];
    if ( node == 0 ) return nil; // if image isn't attached, return nil
    _bsdDevice = [self getBSDDeviceForNode:node];
    if ( _bsdDevice == nil ) return nil; // no bsd device ?
    [self getPartitionArrayForNodeOfKindIOHDIXHDDriveOutKernel:node];
    if ( self.partitionArray.count == 0 ) @throw [NSString stringWithFormat:@"Couldn't find any BSD name for %@", diskImagePath];
  
    return self;
 }

-(NSString*)bsdDevicePath;
{
    return [NSString stringWithFormat:@"/dev/%@", self.bsdDevice];
}

-(NSUInteger)getPartitionNumber:(Partition*)partition;
{
    NSUInteger idx = [_partitionArray indexOfObject:partition];
    if ( idx == NSNotFound ) {
        @throw [NSString stringWithFormat:@"getPartitionNumber called with invalid parameter"];
    }
    return idx+1;
}

-(bool)mountedOn:(NSString*)path;
{
    if ( path )
    {
        for ( Partition* partition in _partitionArray ) {
            if ( [[partition getCurrentMountPoint] isEqualToString:path] ) return true; // if [partition getCurrentMountPoint] return nil, [[partition getCurrentMountPoint] isEqualToString:path] is false
        }
    }else{
        for ( Partition* partition in _partitionArray ) {
            if ( [partition getCurrentMountPoint] != nil ) return true;
        }
    }
    return false;
    
}

#pragma mark umounting
void Disk_DiskUnmountCallback(DADiskRef diskRef, DADissenterRef dissenter, void *context);

void Disk_DiskUnmountCallback(DADiskRef diskRef, DADissenterRef dissenter, void *context)
{
    Disk* self = (__bridge Disk*)context;
    self->_statusString = nil;

//    NSMutableDictionary *info = nil;

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
    
//    [[NSNotificationCenter defaultCenter] postNotificationName:@"DADiskDidAttemptMountNotification" object:(__bridge id _Nullable)(context) userInfo:info];

    self->callbackDone = true;
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
    DADiskEject(daDiskRef, kDADiskUnmountOptionDefault, Disk_DiskUnmountCallback, (__bridge void *)(self));

    while (!self->callbackDone) {
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:runModeWaitingTime]];
    }
    
    if ( session != nil ) {
        DASessionUnscheduleFromRunLoop(session, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        CFRelease(session);
    }

    return _statusString == nil;
}

-(NSString*)asString;
{
    return [NSString stringWithFormat:@"Disk image %@, device %@", self.diskImagePath, self.bsdDevice];
}

- (bool)eject;
{
    bool sucess = true;
    for ( Partition* partition in _partitionArray ) {
        if ( ![partition eject] ) sucess = false;
    }
    if ( ![self eject2] )
    {
        fprintf(stderr, "eject %s failed (%s).\n", [self.asString UTF8String], [_statusString UTF8String]);
        return NO;                        // <--- return
    }
#ifdef DEBUG
  printf("%s successfully ejected.\n", [self.asString UTF8String]);
#endif
    return sucess;
}

@end
