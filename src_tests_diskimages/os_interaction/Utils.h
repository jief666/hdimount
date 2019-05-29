//
//  Utils.h
//  MountDaemon
//
//  Created by Jief on 18/07/17.
//  Copyright © 2017 Jief. All rights reserved.
//

#import <Foundation/Foundation.h>

extern const double runModeWaitingTime;

@interface Utils : NSObject

+ (int)execCommand:(NSString*)command arguments:(NSArray *)arguments output:(NSArray**)outputLines;

@end
