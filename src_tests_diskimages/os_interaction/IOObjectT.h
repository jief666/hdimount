//
//  IOObjectT.h
//  FsUtilsX.10_xcode9
//
//  Created by jief on 29/03/2018.
//  Copyright © 2018 Jief. All rights reserved.
//

#import <Foundation/Foundation.h>

@class IOIteratorT;

//-------------------------------------------------------------------------------------------------------------------------------------------

@interface IOObjectT : NSObject
{
@public
    io_object_t object;
}
+(IOObjectT*)with:(io_object_t)object;
-(NSString*)name;
-(NSMutableDictionary*)properties;
-(IOIteratorT*)iterator;
-(IOObjectT*)getTheOnlyChildNamed:(NSString*)childName;
-(IOObjectT*)getTheOnlyChildNamed_T:(NSString*)childName;
-(IOObjectT*)getTheOnlyChild_T;
@end

//-------------------------------------------------------------------------------------------------------------------------------------------

@interface IOIteratorT : NSObject
{
@public
    io_iterator_t iterator;
    
}
//+(IOIteratorT*)get:(IOObjectT*)object;
-(IOObjectT*)next;
@end


