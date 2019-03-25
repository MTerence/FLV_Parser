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
        printf("Failed to open files!\n");
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
        
        previoustagsize = getc(ifh);
        
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
        
        //process tag by type
        switch (tagheader.TagType) {
                
            case TAG_TYPE_AUDIO:
            {
                char audiotag_str[100]={0};
                strcat(audiotag_str, "| ");
                char tagdata_first_byte;
                tagdata_first_byte = fgetc(ifh);
                int x = tagdata_first_byte&0xF0;
                x=x>>4;
                switch (x)
                {
                    case 0:  strcat(audiotag_str, "Liner PCM, platform endian");    break;
                    case 1:  strcat(audiotag_str, "AUPCM");                         break;
                    case 2:  strcat(audiotag_str, "MP3");                           break;
                    case 3:  strcat(audiotag_str, "Liner PCM, little endian");      break;
                    case 4:  strcat(audiotag_str, "Nellymoser 16-kHZ mono");        break;
                    case 5:  strcat(audiotag_str, "Nellymoser 8-kHZ mono");         break;
                    case 6:  strcat(audiotag_str, "Nellymoser");                    break;
                    case 7:  strcat(audiotag_str, "G.711 A-law logarithmic PCM");   break;
                    case 8:  strcat(audiotag_str, "G.711 mu-law logarithmic PCM");  break;
                    case 9:  strcat(audiotag_str, "reserved");                      break;
                    case 10: strcat(audiotag_str, "AAC");                           break;
                    case 11: strcat(audiotag_str, "Speex");                         break;
                    case 14: strcat(audiotag_str, "MP3 8-Khz");                     break;
                    case 15: strcat(audiotag_str, "Device-specific sound");         break;
                    default: strcat(audiotag_str, "unknow");                        break;
                }
                strcat(audiotag_str, "| ");
                x = tagdata_first_byte&0x0C;
                x=x>>2;
                switch (x)
                {
                    case 0: strcat(audiotag_str, "5.5-kHz");                        break;
                    case 1: strcat(audiotag_str, "1-kHz");                          break;
                    case 2: strcat(audiotag_str, "22-kHz");                         break;
                    case 3: strcat(audiotag_str, "44-kHz");                         break;
                    default:strcat(audiotag_str, "unknow");                         break;
                }
                strcat(audiotag_str, "| ");
                x = tagdata_first_byte&0x02;
                x=x>>1;
                switch (x)
                {
                    case 0:  strcat(audiotag_str, "8Bit");   break;
                    case 1:  strcat(audiotag_str, "16Bit");  break;
                    default: strcat(audiotag_str, "unknow"); break;
                }
                strcat(audiotag_str, "| ");
                x = tagdata_first_byte&0x01;
                switch (x)
                {
                    case 0:  strcat(audiotag_str, "Mono");   break;
                    case 1:  strcat(audiotag_str, "Stereo"); break;
                    default: strcat(audiotag_str, "unknow"); break;
                }
                fprintf(myout, "%s", audiotag_str);
                
                if (output_a!=0 && afh==NULL)
                {
                    afh = fopen("output.mp3", "wb");
                }
                
                //TagData - First Byte Data
                int data_size = reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))-1;
                if (output_a!=0)
                {
                    //TagData+1
                    for (int i = 0; i < data_size; i++) {
                        fputc(fgetc(ifh), afh);
                    }
                }
                else
                {
                    for (int i = 0; i < data_size; i++) {
                        fgetc(ifh);
                    }
                }
                
                break;
            }
                
            case TAG_TYPE_VIDEO:
            {
                char videotag_str[100] = {0};
                strcat(videotag_str, "| ");
                char tagdata_first_byte;
                tagdata_first_byte = fgetc(ifh);
                int x = tagdata_first_byte&0xF0;
                x=x>>4;
                switch (x)
                {
                    case 1:  strcat(videotag_str, "key frame");                  break;
                    case 2:  strcat(videotag_str, "inter frame");                break;
                    case 3:  strcat(videotag_str, "disposable internframe");     break;
                    case 4:  strcat(videotag_str, "generated keyframe");         break;
                    case 5:  strcat(videotag_str, "video info/command frame");   break;
                    default: strcat(videotag_str, "unknow");                     break;
                }
                strcat(videotag_str, "| ");
                x = tagdata_first_byte&0x0F;
                switch (x)
                {
                    case 1:  strcat(videotag_str, "JPEG (currently unused)");       break;
                    case 2:  strcat(videotag_str, "Sorenson H.263");                break;
                    case 3:  strcat(videotag_str, "Sorenson video");                break;
                    case 4:  strcat(videotag_str, "On2 VP6");                       break;
                    case 5:  strcat(videotag_str, "On2 VP6 with alpha channel");    break;
                    case 6:  strcat(videotag_str, "Screen video version 2");        break;
                    case 7:  strcat(videotag_str, "AVC");                           break;
                    default: strcat(videotag_str, "unknow");                        break;
                }
                
                fprintf(myout, "%s", videotag_str);
                
                fseek(ifh, -1, SEEK_CUR);
                
                //if the output file hasn't been opened, open it
                if (vfh == NULL && output_v!=0)
                {
                    //write the flv header (reuse the original file's hdr) and first previoustagsize
                    vfh = fopen("output.flv", "wb");
                    fwrite((char *)&flv, 1, sizeof(flv), vfh);
                    fwrite((char *)&previoustagsize_z, 1, sizeof(previoustagsize_z), vfh);
                }
                
#if 0
                //change timestamp
                //Get timestamp
                ts = reverse_bytes((byte *)&tagheader.Timestamp, sizeof(tagheader.Timestamp));
                ts = ts*2;
                //Writeback Timestamp
                ts_new = reverse_bytes((byte *)&ts, sizeof(ts));
                memcpy(&tagheader.Timestamp, ((char *)&ts_new)+1, sizeof(tagheader.Timestamp));
                
#endif
                
                //TagData + Previous Tag Size
                int data_size = reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize))+4;
                if (output_v!=0)
                {
                    //TagHeader
                    fwrite((char *)&tagheader, 1, sizeof(tagheader), vfh);
                    //TagData
                    for (int i = 0; i < data_size; i++) {
                        fputc(fgetc(ifh), vfh);
                    }
                }else{
                    for (int i = 0; i < data_size; i ++) {
                        fgetc(ifh);
                    }
                }
                
                
                //rewind 4 bytes, because we need  to read the previoustagsize again for the loop's sake
                fseek(ifh, -4, SEEK_CUR);
                
                break;
            }
                
            default:
                fseek(ifh, reverse_bytes((byte *)&tagheader.DataSize, sizeof(tagheader.DataSize)), SEEK_CUR);
                fprintf(myout, "\n");
                break;
                
        }
    }while (!feof(ifh));
    
    fclose(ifh);
    
    
    return 0;
}





#pragma mark - strcat(dest, ...)
/**
 char *strcat(char *dest, const char *src)
 把 src 所指向的字符串追加到 dest 所指向的字符串的结尾。
 
 @param dest 指向目标数组，该数组包含了一个 C 字符串，且足够容纳追加后的字符串。
 @param ... 指向要追加的字符串，该字符串不会覆盖目标字符串
 @return 该函数返回一个指向最终的目标字符串 dest 的指针。
 */
