typedef union kr_header ck_blk_t;

#include "src-loc.h"

typedef struct ck_blk {
    struct ck_blk *next;
    unsigned n;
    src_loc_t loc;
} ck_blk_t;
struct ck_blk ck_blk_t;

// returns pointer to the first header block on allocated list
ck_blk_t *ck_first_hdr(void);
// returns pointer to next hdr or 0 if none on free list.
ck_blk_t *ck_next_hdr(ck_blk_t *p);

// returns pointer to block data for block <p>
void *ck_blk_data(ck_blk_t *p);
// returns size of data in block <p>
void *ck_blk_nbytes(ck_blk_t *p);

