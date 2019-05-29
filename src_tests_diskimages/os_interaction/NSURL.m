//
//  NSURL.m
//  iTunesFolderSync
//
//  Created by Jief on 13/02/17.
//  Copyright (c) 2017 Jief. All rights reserved.
//

#import "NSURL.h"

#include <stdlib.h>

@implementation NSURL (realpath)

- (BOOL)isLooping
{
    NSURL* resolvedSelf = self.resolveURL;
    
    NSArray* pathComponent = self.path.pathComponents;
    for ( NSUInteger idx = 0 ; idx < [pathComponent count]-1 ; idx += 1 )
    {
        NSString* path = [NSString pathWithComponents:[pathComponent subarrayWithRange:NSMakeRange(0, idx+1)]];
        if ( resolvedSelf.path && [path caseInsensitiveCompare:(NSString*)resolvedSelf.path] == NSOrderedSame ) return YES;
    }
    return NO;
}

- (NSURL*)resolveURL
{
    NSURL* aliasResolvedUrl;

    NSData* bookmarkData = [NSURL bookmarkDataWithContentsOfURL:self error:nil];
    BOOL isAlias = (bookmarkData) ? YES : NO;
    if ( isAlias ) {
        NSError* error;
        aliasResolvedUrl = [NSURL URLByResolvingBookmarkData:bookmarkData options:NSURLBookmarkResolutionWithoutUI relativeToURL:nil bookmarkDataIsStale:nil error:&error];
    }else{
        aliasResolvedUrl = self;
    }

    NSString* expandedPath = [aliasResolvedUrl.path stringByStandardizingPath];
    const char *cpath = [expandedPath cStringUsingEncoding:NSUTF8StringEncoding];
    char resolved[PATH_MAX+1];
    char *returnValue = realpath(cpath, resolved);

    if ( returnValue == NULL ) {
        //printf("Error with path: %s\n", resolved);
        // if there is an error then resolved is set with the path which caused the issue
        // returning nil will prevent further action on this path
        return self;
    }

    return [NSURL fileURLWithPath:(NSString *)[NSString stringWithCString:returnValue encoding:NSUTF8StringEncoding]]; // returnValue is nonnull, so [NSString stringWithCString:encoding:] must be.
}

/*
    return parentUrl is self is a subdirectory of parentUrl
    returns self if not
    returns nil if error in realpath function. Most likely dir doesn't exist.
*/
- (NSURL*)isSameOrSubDirOf:(NSURL*)parentUrl;
{
    if ( ![self isFileURL] ) @throw @"NSURL isSubDirOf -> must be a file url";
    if ( ![parentUrl isFileURL] ) @throw @"NSURL isSubDirOf -> parentUrl must be a file url";

//    if ( [self.path hasPrefix:parentUrl.path] ) {
//        if ( self.path.length == parentUrl.path.length ) return parentUrl; // same url
//        if ( self.path.length < parentUrl.path.length ) return self; // can't be sub dir or same
//        if ( [[self.path substringWithRange:NSMakeRange(parentUrl.path.length, 1)] isEqualToString:@"/"] ) return parentUrl;
//    }

    {
        NSArray *parentComponent = parentUrl.path.pathComponents;
        NSArray *selfComponent = self.path.pathComponents;
        
        if ( parentComponent.count > selfComponent.count ) return self; // self is shorter than parent. Can't be same or subdir
        bool identical = true;
        for ( NSUInteger idx = 0 ; identical  &&  idx < [parentComponent count] ; idx += 1 )
        {
            if ( ![[parentComponent objectAtIndex:idx] isEqual:[selfComponent objectAtIndex:idx]] ) identical = false;
        }
        if ( identical ) return parentUrl;
    }
    
    
    // trying the same, but after resolving path. If one path exist and not the other result can be wrong !!!
    {
        NSURL* parentResolvedUrl = parentUrl.resolveURL;
        //if ( parentResolvedUrl == parentUrl ) return nil;
        NSURL* resolvedUrl = self.resolveURL;
        //if ( [resolvedUrl.path isEqualToString:parentResolvedUrl.path] ) return nil;
        
        NSArray *parentComponent = parentResolvedUrl.path.pathComponents;
        NSArray *selfComponent = resolvedUrl.path.pathComponents;
        
        if ( parentComponent.count > selfComponent.count ) return self; // self is shorter than parent. Can't be same or subdir
        for ( NSUInteger idx = 0 ; idx < [parentComponent count] ; idx += 1 )
        {
            if ( ![[parentComponent objectAtIndex:idx] isEqual:[selfComponent objectAtIndex:idx]] ) return self;
        }
    }
    return parentUrl;
}


/*
    return parentUrl is self is a subdirectory of parentUrl
    returns self if not
    returns nil if error in realpath function. Most likely dir doesn't exist.
*/
- (int)isSameDir:(NSURL*)otherUrl;
{
    if ( ![self isFileURL] ) @throw @"NSURL isSubDirOf -> must be a file url";
    if ( ![otherUrl isFileURL] ) @throw @"NSURL isSubDirOf -> otherUrl must be a file url";

//    if ( [self.path hasPrefix:parentUrl.path] ) {
//        if ( self.path.length == parentUrl.path.length ) return parentUrl; // same url
//        if ( self.path.length < parentUrl.path.length ) return self; // can't be sub dir or same
//        if ( [[self.path substringWithRange:NSMakeRange(parentUrl.path.length, 1)] isEqualToString:@"/"] ) return parentUrl;
//    }

    {
        NSArray *otherComponent = otherUrl.path.pathComponents;
        NSArray *selfComponent = self.path.pathComponents;
        
        if ( otherComponent.count == selfComponent.count ) {
            bool identical = true;
            for ( NSUInteger idx = 0 ; identical  &&  idx < [otherComponent count] ; idx += 1 )
            {
                if ( ![[otherComponent objectAtIndex:idx] isEqual:[selfComponent objectAtIndex:idx]] ) identical = false;
            }
            if ( identical ) return 1;
        }
    }
    
    
    // trying the same, but after resolving path. If one path exist and not the other result can be wrong !!!
    {
        NSURL* otherResolvedUrl = otherUrl.resolveURL;
        if ( otherResolvedUrl == otherUrl ) return -1;
        NSURL* resolvedUrl = self.resolveURL;
        if ( resolvedUrl == self) return -1;
        
        NSArray *otherComponent = otherResolvedUrl.path.pathComponents;
        NSArray *selfComponent = resolvedUrl.path.pathComponents;
        
        if ( otherComponent.count != selfComponent.count ) return 0;
        for ( NSUInteger idx = 0 ; idx < [otherComponent count] ; idx += 1 )
        {
            if ( ![[otherComponent objectAtIndex:idx] isEqual:[selfComponent objectAtIndex:idx]] ) return 0;
        }
    }
    return 1;
}

@end
