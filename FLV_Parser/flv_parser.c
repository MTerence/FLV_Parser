//
//  flv_parser.c
//  FLV_Parser
//
//  Created by Ternence on 2019/3/25.
//  Copyright © 2019 Ternence. All rights reserved.
//

#include "flv_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)

#define TAG_TYPE_SCRIPT 18
#define TAG_TYPE_AUDIO  8
#define TAG_TYPE_VIDEO  9

typedef unsigned char byte;
typedef unsigned int  uint;

typedef struct {
    byte Signature[3];
    byte Version;
    byte Flags;
    uint DataOffset;
} FLV_HEADER;

typedef struct {
    byte TagType;
    byte DataSize[3];
    byte Timestamp[3];
    uint Reserved;
} TAG_HEADER;

//reverse_bytes - turn a BigEndian byte Array into a LittleENdian integer
uint reverse_bytes(byte *p, char c){
    int r= 0;
    int i;
    for (i = 0; i < c; i ++) {
        r |= (*(p+i) << (((c-1)*8)-8*i));
    }
    return i;
}


/**
 Analysis FLV file

 @param url Location of input Flv file
 */
int simple_flv_parser(char *url){
    //whether output audio/video stream
    int output_a = 1;
    int output_v = 1;
    
    FILE *ifh = NULL, *vfh = NULL, *afh = NULL;
    
    FILE *myout = stdout;
    
    FLV_HEADER flv;
    TAG_HEADER tagheader;
    uint previoustagsize, previoustagsize_z = 0;
    uint ts =0, ts_new = 0;
    
    ifh = fopen(url, "rb+");
    if (ifh == NULL) {
        printf("Failed to open files!");
        return -1;
    }
    
    //FLV file header
    fread((char *)&flv, 1, sizeof(FLV_HEADER), ifh);
    
    fprintf(myout, "============== FLV Header ==============\n");
    fprintf(myout, "Sinature:  0x %c %c %c\n",flv.Signature[0], flv.Signature[1], flv.Signature[2]);
    fprintf(myout, "Version :  0x %X\n", flv.Version);
    fprintf(myout, "Flags   :  0x %X\n", flv.Flags);
    fprintf(myout, "HeaderSize:0x %X\n",reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)));
    fprintf(myout,"========================================\n");
    
    //move the file pointer to the end of the header
    fseek(ifh, reverse_bytes((byte *)&flv.DataOffset, sizeof(flv.DataOffset)), SEEK_SET);
    
    //process each tag
    do {
        
        previoustagsize = _getw(ifh);
        
        fread((void *)&tagheader, sizeof(TAG_HEADER), 1, ifh);
        
        //int temp_datasize1 = reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize));
        int tagheader_datasize = tagheader.DataSize[0]*65536 + tagheader.DataSize[1]*256 + tagheader.DataSize[2];
        int tagheader_timestamp = tagheader.Timestamp[0] * 65536 + tagheader.Timestamp[1]*256 + tagheader.Timestamp[2];
        
        char tagtype_str[10];
        switch (tagheader.TagType) {
            case TAG_TYPE_AUDIO:    sprintf(tagtype_str, "AUDIO");     break;
            case TAG_TYPE_VIDEO:    sprintf(tagtype_str, "VIDEO");     break;
            case TAG_TYPE_SCRIPT:   sprintf(tagtype_str, "SCRIPT");    break;
            default:                sprintf(tagtype_str, "unknow");    break;
        }
        fprintf(myout, "[%6s] %6d %6d |",tagtype_str, tagheader_datasize, tagheader_timestamp);
        
        //if we are not past the end of file, process the tag
        if (feof(ifh)) {
            break;
        }
    }
    
    
    return 0;
}
