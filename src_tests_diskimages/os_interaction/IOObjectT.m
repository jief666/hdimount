//
//  IOObjectT.m
//  FsUtilsX.10_xcode9
//
//  Created by jief on 29/03/2018.
//  Copyright © 2018 Jief. All rights reserved.
//

#import "IOObjectT.h"


@implementation IOObjectT

+(IOObjectT*)with:(io_object_t)object;
{
    if ( !object ) return nil;
    IOObjectT* newIOObjectT = [[self alloc] init];
    if ( !newIOObjectT ) return nil;
    newIOObjectT->object = object;
    return newIOObjectT;
}

-(void)dealloc
{
    //NSLog(@"IOObjectT dealloc");
    IOObjectRelease(object);
}

-(NSString*)name
{
    io_name_t name;
    IORegistryEntryGetName(self->object, name);
    return [NSString stringWithCString:name encoding:NSMacOSRomanStringEncoding];
}

-(NSMutableDictionary*)properties
{
    CFMutableDictionaryRef propertiesTmp;
    IORegistryEntryCreateCFProperties(self->object, &propertiesTmp, kCFAllocatorDefault, kNilOptions);
    NSMutableDictionary* properties = [NSMutableDictionary dictionaryWithDictionary:(__bridge_transfer NSMutableDictionary *)propertiesTmp];
    return properties;
}

-(IOIteratorT*)iterator;
{
    IOIteratorT* newIOIteratorT = [[IOIteratorT alloc] init];
    if ( !newIOIteratorT ) return nil;
    IORegistryEntryGetChildIterator(self->object, kIOServicePlane, &(newIOIteratorT->iterator));
    return newIOIteratorT;
}

-(IOObjectT*)getTheOnlyChildNamed:(NSString*)childName
{
    IOObjectT* returnValue = 0;
    IOIteratorT* childIterator = 0;
    IOObjectT* child = 0;
    
    childIterator = [self iterator];
    
    while ( (child = [childIterator next]) )
    {
        //NSLog(@"Device name is :  %@\n",[self getNodeName:child]);
        if ( [childName isEqualToString:[child name]] ) {
            if ( returnValue == 0 ) {
                returnValue = child;
            }else {
                @throw @"image-path exists twice in IORegistry";
            }
        }
    }
    return returnValue;
}

-(IOObjectT*)getTheOnlyChildNamed_T:(NSString*)childName
{
    IOObjectT* returnValue = [self getTheOnlyChildNamed:childName];
    if ( returnValue == 0 ) {
        @throw [NSString stringWithFormat:@"Object %@ doesn't have a child %@", [self name], childName];
    }
    return returnValue;
}

-(IOObjectT*)getTheOnlyChild_T
{
    IOObjectT* returnValue = 0;
    IOIteratorT* childIterator = 0;
    IOObjectT* child = 0;
    
    childIterator = [self iterator];
    
    while ( (child = [childIterator next]) )
    {
        //NSLog(@"Device name is :  %@\n",[self getNodeName:child]);
        if ( returnValue == 0 ) {
            returnValue = child;
        }else {
            @throw [NSString stringWithFormat:@"Node %@ has 2 childs", [self name]];
        }
    }
    return returnValue;
}

@end


/****************************************************************************************************************************/

@implementation IOIteratorT

-(void)dealloc
{
    //NSLog(@"IOIteratorT dealloc");
    IOObjectRelease(iterator);
}

-(IOObjectT*)next;
{
    return [IOObjectT with:IOIteratorNext(iterator)];
}

@end


