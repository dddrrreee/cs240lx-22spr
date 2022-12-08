//https://github.com/microsoft/uf2 -> implement our own uf2conv
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include "crcpico.h"

#define MAXSIZE 0x1E8480 /* 2 Megabytes */

struct UF2_Block {
    // 32 byte header
    uint32_t magicStart0;
    uint32_t magicStart1;
    uint32_t flags;
    uint32_t targetAddr;
    uint32_t payloadSize;
    uint32_t blockNo;
    uint32_t numBlocks;
    uint32_t familyID; // or fileSize;
    uint8_t data[476];
    uint32_t magicEnd;
} UF2_Block;

// Max bytes should be 252, pico uses CRC-32 by default
unsigned int compute_crc(uint8_t const message[], int nBytes) {
    unsigned crc = 0xFFFFFFFF;
    unsigned data;
    for (int byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (crc >> 24);
        crc = crc_pico[data] ^ (crc << 8);
    }
    return crc;
}

void write_block(struct UF2_Block block, FILE* uf2) {
    fwrite(&block.magicStart0, sizeof(uint32_t), 1, uf2);
    fwrite(&block.magicStart1, sizeof(uint32_t), 1, uf2);
    fwrite(&block.flags, sizeof(uint32_t), 1, uf2);
    fwrite(&block.targetAddr, sizeof(uint32_t), 1, uf2);
    fwrite(&block.payloadSize, sizeof(uint32_t), 1, uf2);
    fwrite(&block.blockNo, sizeof(uint32_t), 1, uf2);
    fwrite(&block.numBlocks, sizeof(uint32_t), 1, uf2);
    fwrite(&block.familyID, sizeof(uint32_t), 1, uf2);
    fwrite(&block.data, 476, 1, uf2);
    fwrite(&block.magicEnd, sizeof(uint32_t), 1, uf2);
}

// .uf2conv myfile.bin myfile.uf2
int main ( int argc, char *argv[] ) {
    if(argc<3)
    {
        printf("input and output files not specified\n");
        return(1);
    }
    FILE *fp = fopen(argv[1],"rb");
    if(fp==NULL)
    {
        printf("Error opening file [%s]\n",argv[1]);
        return(1);
    }

    struct stat file_stats;
    stat(argv[1], &file_stats);
    uint32_t len = file_stats.st_size;
    if (len > MAXSIZE) {
        printf("File size is greater than 2 Megabytes\n");
        return(1);
    }
    // 252 byte sized blocks because we must also hold the 4 byte crc
    uint32_t num_blocks = (len + 251)/ 252;
    // setup struct values correctly
    struct UF2_Block block;
    block.magicStart0 = 0x0A324655;
    block.magicStart1 = 0x9E5D5157;
    // Page 172 rp2040 datasheet is expecting family ID to be set
    block.flags = 0x00002000;
    // Placing in flash
    block.targetAddr = 0x10000000;
    // I think we might be limited to 256
    block.payloadSize = 0x00000100;
    block.blockNo = 0x00000000;
    block.numBlocks = num_blocks; 
    block.familyID = 0xe48bff56;
    block.magicEnd = 0x0AB16F30;
    size_t size;
    uint32_t crc;
    FILE* uf2 = fopen(argv[2], "wb");
    printf("num blocks: %d\n", num_blocks);
    if(uf2==NULL)
    {
        printf("Error opening file [%s]\n",argv[2]);
        return(1);
    }
    
    for (int i = 0; i < num_blocks; i++) {
        memset(block.data, 0x00, 476);
        // fread will advance the file pointer for you
        size = fread(block.data, sizeof(uint8_t), 256, fp);

        // First block includes a crc pg 172
        if ( i == 0) {
            crc = compute_crc(block.data, 252);
            // !!!!! Tricky the crc goes at the end even if we have 0 padding
            block.data[252] = (crc>> 0)&0xFF;
            block.data[253] = (crc>> 8)&0xFF;
            block.data[254] = (crc>>16)&0xFF;
            block.data[255] = (crc>>24)&0xFF;
        }
        write_block(block, uf2);
        // !!!! Big bug Need to increment location by 256
        block.targetAddr += 0x100;
        block.blockNo += 1;
    }

    fclose(uf2);
    fclose(fp);

    stat(argv[2], &file_stats);
    len = file_stats.st_size; 
    printf("Successfully wrote %u bytes to %s\n", len, argv[2]);

    return(0);
}