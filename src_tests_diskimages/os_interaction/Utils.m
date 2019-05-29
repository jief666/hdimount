//
//  Utils.m
//  MountDaemon
//
//  Created by Jief on 18/07/17.
//  Copyright © 2017 Jief. All rights reserved.
//

#import "Utils.h"

const double runModeWaitingTime = 0.05;

@implementation Utils


+ (int)execCommand:(NSString*)command arguments:(NSArray *)arguments output:(NSArray**)outputLines;
{
	NSTask *newTask;
	
	newTask = [NSTask new];
	newTask.launchPath = command;
	
	newTask.standardOutput = NSPipe.pipe;
	newTask.standardError = newTask.standardOutput;
	newTask.standardInput = NSPipe.pipe;
	
	
	if ( arguments ) newTask.arguments = arguments;
    [newTask launch];
    
    NSFileHandle* stdoutHandle = [newTask.standardOutput fileHandleForReading];
    NSMutableData* outputData = [[NSMutableData alloc] init];
    
    while ( newTask.isRunning ) {
        NSData* availableData = stdoutHandle.availableData;
        [outputData appendData:availableData];
    }
    
//	[newTask waitUntilExit];

    if ( outputLines )
     {
//        NSData* outputData = [[newTask.standardOutput fileHandleForReading] readDataToEndOfFile];
        NSString* outputString = [[NSString alloc] initWithData:outputData encoding:NSUTF8StringEncoding];
        
        NSArray* lines = [outputString componentsSeparatedByString:@"\n"];
        
        *outputLines = lines;
    }

	return newTask.terminationStatus;
}

@end
