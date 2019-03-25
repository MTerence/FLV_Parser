//
//  main.m
//  FLV_Parser
//
//  Created by Ternence on 2019/3/25.
//  Copyright © 2019 Ternence. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "flv_parser.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        simple_flv_parser("cuc_ieschool.flv");
    }
    return 0;
}
