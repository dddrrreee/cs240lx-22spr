#include "kr-malloc.h"

#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))


static Header base;
static Header *freep = NULL;

#define NALLOC 1024


static Header *kr_morecore(unsigned inc) {
	char *cp;
	Header *up;

	if (inc < NALLOC) {
		inc = NALLOC;
	}

	cp = sbrk(inc * sizeof(Header));
	if (cp ==  (char *) -1) {
		return NULL;
	}

	up = (Header *)cp;
	up->s.size = inc;
	kr_free((void *)(up + 1));
	return freep;
}


void *kr_malloc(unsigned nbytes) {
	Header *p, *prevp;
	unsigned nunits;
	Header *kr_morecore(unsigned);

	nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
	if ((prevp = freep) == NULL) {
		// no free list yet
		base.s.ptr = freep = prevp = &base;
		base.s.size = 0;
	}

	for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
		if (p->s.size >= nunits) {
			// big enough
			if (p->s.size == nunits) {
				//fits exactly
				prevp->s.ptr = p->s.ptr;
			} else {
				p->s.size -= nunits;
				p += p->s.size;
				p->s.size = nunits;
			}
			freep = prevp;
			return (void *)(p + 1);
		}

		if (p == freep) {
			// wrapped aroundd freelist
			if ((p = kr_morecore(nunits)) == NULL) {
				return NULL;
			}
		}
	}

	return NULL;
}

void kr_free(void* ptr) {
	Header *bp, *p;
	bp = (Header *)ptr - 1;
	for (p = freep; !(bp > p && bp <p->s.ptr); p = p->s.ptr) {
		if (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
			break; //freed block at start of arena
	}

	if (bp + bp->s.size == p->s.ptr) {    /* join to upper nbr */
           bp->s.size += p->s.ptr->s.size;
           bp->s.ptr = p->s.ptr->s.ptr;
       } else
           bp->s.ptr = p->s.ptr;
       if (p + p->s.size == bp) {            /* join to lower nbr */
           p->s.size += bp->s.size;
           p->s.ptr = bp->s.ptr;
       } else {
           p->s.ptr = bp;
	   }
	freep = p;
}

