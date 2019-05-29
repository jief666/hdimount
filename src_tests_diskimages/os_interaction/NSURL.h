//
//  NSURL.h
//  iTunesFolderSync
//
//  Created by Jief on 13/02/17.
//  Copyright (c) 2017 Jief. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSURL (realpath)

- (NSURL*)resolveURL;
- (NSURL*)isSameOrSubDirOf:(NSURL*)parentUrl;
- (int)isSameDir:(NSURL*)otherUrl;
- (BOOL)isLooping;

@end
