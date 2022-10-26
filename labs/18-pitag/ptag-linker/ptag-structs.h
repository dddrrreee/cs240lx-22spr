#ifndef __PTAG_STRUCTS_H__
#define __PTAG_STRUCTS_H__

// have them add ARGV, and SYMTAB
#define PTAG_BIN_FILE_TAG 0x12345678

// these first three fields are the same in every ptag record.
typedef struct ptag_hdr {
    uint32_t tag;               // tag of this record.
    uint32_t tot_nbytes;        // total bytes of the record including 
                                // everything [code, string, everything]
    uint32_t expected_off;      // expected offset in the .bin file.

    // EXTENSION: add crc over everything so we can detect 
    // nasty corruption bugs.
    // uint32_t crc;               // crc of entire record [everything]

    // struct hack: takes no space, lets us easily grab the byte
    // after this structure with the right type.
    uint8_t data[0];
} ptag_hdr_t;
_Static_assert(sizeof(ptag_hdr_t) == 4*3, "wrong size of header\n");

typedef struct ptag_prog {
    char prog_name[128];  // name of .bin program.
    uint32_t code_addr;         // where the code should run.
    uint32_t code_nbytes;       // code size in bytes.
    uint32_t code_crc;
    // we use the "struct hack" so that it will be laid out
    // right after this record.  codesize must be multiple
    // of 4.
    uint32_t code[0];      // rest of the actual code.
} ptag_prog_t;

#if 0
// used on pi side for the linked list.
typedef struct ptag {
    struct ptag *next;
    ptag_hdr_t *hdr;
    union {
        ptag_prog_t *p;
    } u;
} ptag_t;
#endif

#endif
