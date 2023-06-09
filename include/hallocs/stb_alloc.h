//////////////////////////////////////////////////////////////////////////////
//
//                   stb_alloc - hierarchical allocator
//
//                                     inspired by http://swapped.cc/halloc
//
//
// When you alloc a given block through stb_alloc, you have these choices:
//
//       1. does it have a parent?
//       2. can it have children?
//       3. can it be freed directly?
//       4. is it transferrable?
//       5. what is its alignment?
//
// Here are interesting combinations of those:
//
//                              children   free    transfer     alignment
//  arena                          Y         Y         N           n/a
//  no-overhead, chunked           N         N         N         normal
//  string pool alloc              N         N         N            1
//  parent-ptr, chunked            Y         N         N         normal
//  low-overhead, unchunked        N         Y         Y         normal
//  general purpose alloc          Y         Y         Y         normal
//
// Unchunked allocations will probably return 16-aligned pointers. If
// we 16-align the results, we have room for 4 pointers. For smaller
// allocations that allow finer alignment, we can reduce the pointers.
//
// The strategy is that given a pointer, assuming it has a header (only
// the no-overhead allocations have no header), we can determine the
// type of the header fields, and the number of them, by stepping backwards
// through memory and looking at the tags in the bottom bits.
//
// Implementation strategy:
//     chunked allocations come from the middle of chunks, and can't
//     be freed. thefore they do not need to be on a sibling chain.
//     they may need child pointers if they have children.
//
// chunked, with-children
//     void *parent;
//
// unchunked, no-children -- reduced storage
//     void *next_sibling;
//     void *prev_sibling_nextp;
//
// unchunked, general
//     void *first_child;
//     void *next_sibling;
//     void *prev_sibling_nextp;
//     void *chunks;
//
// so, if we code each of these fields with different bit patterns
// (actually same one for next/prev/child), then we can identify which
// each one is from the last field.

// Note: extracted from stb.h.

#include <stddef.h>

#ifndef STB_EXTERN
#define STB_EXTERN
#endif

#ifdef __cplusplus
extern "C" {
#endif

STB_EXTERN void stb_free(void* p);
STB_EXTERN void* stb_malloc_global(size_t size);
STB_EXTERN void* stb_malloc(void* context, size_t size);
STB_EXTERN void* stb_malloc_nofree(void* context, size_t size);
STB_EXTERN void* stb_malloc_leaf(void* context, size_t size);
STB_EXTERN void* stb_malloc_raw(void* context, size_t size);
STB_EXTERN void* stb_realloc(void* ptr, size_t newsize);

STB_EXTERN void stb_reassign(void* new_context, void* ptr);
STB_EXTERN void stb_malloc_validate(void* p, void* parent);

extern int stb_alloc_chunk_size;
extern int stb_alloc_count_free;
extern int stb_alloc_count_alloc;
extern int stb_alloc_alignment;

#ifdef __cplusplus
}
#endif

#ifdef STB_ALLOC_IMPLEMENTATION

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int stb_alloc_chunk_size = 65536;
int stb_alloc_count_free = 0;
int stb_alloc_count_alloc = 0;
int stb_alloc_alignment = -16;

typedef struct stb__chunk {
    struct stb__chunk* next;
    int data_left;
    int alloc;
} stb__chunk;

typedef struct {
    void* next;
    void** prevn;
} stb__nochildren;

typedef struct {
    void** prevn;
    void* child;
    void* next;
    stb__chunk* chunks;
} stb__alloc;

typedef struct {
    stb__alloc* parent;
} stb__chunked;

#define STB__PARENT 1
#define STB__CHUNKS 2

typedef enum {
    STB__nochildren = 0,
    STB__chunked = STB__PARENT,
    STB__alloc = STB__CHUNKS,

    STB__chunk_raw = 4,
} stb__alloc_type;

// these functions set the bottom bits of a pointer efficiently
// anvouk: added missing size_t conversion since otherwise all hell would break
// loose. FFS took me 4h to get here...
// FIXME: take change in upstream stb?
#define STB__DECODE(x, v) ((void*)((char*)((size_t)(x) - (v))))
#define STB__ENCODE(x, v) ((void*)((char*)((size_t)(x) + (v))))

#define stb__parent(z) (stb__alloc*)STB__DECODE((z)->parent, STB__PARENT)
#define stb__chunks(z) (stb__chunk*)STB__DECODE((z)->chunks, STB__CHUNKS)

#define stb__setparent(z, p) (z)->parent = (stb__alloc*)STB__ENCODE((p), STB__PARENT)
#define stb__setchunks(z, c) (z)->chunks = (stb__chunk*)STB__ENCODE((c), STB__CHUNKS)

static stb__alloc stb__alloc_global = { NULL, NULL, NULL, (stb__chunk*)STB__ENCODE(NULL, STB__CHUNKS) };

static stb__alloc_type stb__identify(void* p)
{
    void** q = (void**)p;
    return (stb__alloc_type)((uint64_t)q[-1] & 3);
}

static void*** stb__prevn(void* p)
{
    if (stb__identify(p) == STB__alloc) {
        stb__alloc* s = (stb__alloc*)p - 1;
        return &s->prevn;
    } else {
        stb__nochildren* s = (stb__nochildren*)p - 1;
        return &s->prevn;
    }
}

void stb_free(void* p)
{
    if (p == NULL) {
        return;
    }

    // count frees so that unit tests can see what's happening
    ++stb_alloc_count_free;

    switch (stb__identify(p)) {
        case STB__chunked:
            // freeing a chunked-block with children does nothing;
            // they only get freed when the parent does
            // surely this is wrong, and it should free them immediately?
            // otherwise how are they getting put on the right chain?
            return;
        case STB__nochildren: {
            stb__nochildren* s = (stb__nochildren*)p - 1;
            // unlink from sibling chain
            *(s->prevn) = s->next;
            if (s->next) {
                *stb__prevn(s->next) = s->prevn;
            }
            free(s);
            return;
        }
        case STB__alloc: {
            stb__alloc* s = (stb__alloc*)p - 1;
            stb__chunk *c, *n;
            void* q;

            // unlink from sibling chain, if any
            *(s->prevn) = s->next;
            if (s->next) {
                *stb__prevn(s->next) = s->prevn;
            }

            // first free chunks
            c = (stb__chunk*)stb__chunks(s);
            if (c) {
                while (c != NULL) {
                    n = c->next;
                    stb_alloc_count_free += c->alloc;
                    free(c);
                    c = n;
                }
            }

            // validating
            stb__setchunks(s, NULL);
            s->prevn = NULL;
            s->next = NULL;

            // now free children
            while ((q = s->child) != NULL) {
                stb_free(q);
            }

            // now free self
            free(s);
            return;
        }
        default:
            assert(0); /* NOTREACHED */
    }
}

void stb_malloc_validate(void* p, void* parent)
{
    if (p == NULL) {
        return;
    }

    switch (stb__identify(p)) {
        case STB__chunked:
            return;
        case STB__nochildren: {
            stb__nochildren* n = (stb__nochildren*)p - 1;
            if (n->prevn) {
                assert(*n->prevn == p);
            }
            if (n->next) {
                assert(*stb__prevn(n->next) == &n->next);
                stb_malloc_validate(n, parent);
            }
            return;
        }
        case STB__alloc: {
            stb__alloc* s = (stb__alloc*)p - 1;

            if (s->prevn) {
                assert(*s->prevn == p);
            }

            if (s->child) {
                assert(*stb__prevn(s->child) == &s->child);
                stb_malloc_validate(s->child, p);
            }

            if (s->next) {
                assert(*stb__prevn(s->next) == &s->next);
                stb_malloc_validate(s->next, parent);
            }
            return;
        }
        default:
            assert(0); /* NOTREACHED */
    }
}

static void* stb__try_chunk(stb__chunk* c, int size, int align, int pre_align)
{
    char *memblock = (char*)(c + 1), *q;
    int64_t iq;
    int start_offset;

    // we going to allocate at the end of the chunk, not the start. confusing,
    // but it means we don't need both a 'limit' and a 'cur', just a 'cur'.
    // the block ends at: p + c->data_left
    //   then we move back by size
    start_offset = c->data_left - size;

    // now we need to check the alignment of that
    q = memblock + start_offset;
    iq = (int64_t)q;
    assert(sizeof(q) == sizeof(iq));

    // suppose align = 2
    // then we need to retreat iq far enough that (iq & (2-1)) == 0
    // to get (iq & (align-1)) = 0 requires subtracting (iq & (align-1))

    start_offset -= iq & (align - 1);
    assert(((uint64_t)(memblock + start_offset) & (align - 1)) == 0);

    // now, if that + pre_align works, go for it!
    start_offset -= pre_align;

    if (start_offset >= 0) {
        c->data_left = start_offset;
        return memblock + start_offset;
    }

    return NULL;
}

static void stb__sort_chunks(stb__alloc* src)
{
    // of the first two chunks, put the chunk with more data left in it first
    stb__chunk *c = stb__chunks(src), *d;
    if (c == NULL) {
        return;
    }
    d = c->next;
    if (d == NULL) {
        return;
    }
    if (c->data_left > d->data_left) {
        return;
    }

    c->next = d->next;
    d->next = c;
    stb__setchunks(src, d);
}

static void* stb__alloc_chunk(stb__alloc* src, int size, int align, int pre_align)
{
    void* p;
    stb__chunk* c = stb__chunks(src);

    if (c && size <= stb_alloc_chunk_size) {

        p = stb__try_chunk(c, size, align, pre_align);
        if (p) {
            ++c->alloc;
            return p;
        }

        // try a second chunk to reduce wastage
        if (c->next) {
            p = stb__try_chunk(c->next, size, align, pre_align);
            if (p) {
                ++c->alloc;
                return p;
            }

            // put the bigger chunk first, since the second will get buried
            // the upshot of this is that, until it gets allocated from, chunk
            // #2 is always the largest remaining chunk. (could formalize this
            // with a heap!)
            stb__sort_chunks(src);
            c = stb__chunks(src);
        }
    }

    // allocate a new chunk
    {
        stb__chunk* n;

        int chunk_size = stb_alloc_chunk_size;
        // we're going to allocate a new chunk to put this in
        if (size > chunk_size) {
            chunk_size = size;
        }

        assert(sizeof(*n) + pre_align <= 16);

        // loop trying to allocate a large enough chunk
        // the loop is because the alignment may cause problems if it's big...
        // and we don't know what our chunk alignment is going to be
        while (1) {
            n = (stb__chunk*)malloc(16 + chunk_size);
            if (n == NULL) {
                return NULL;
            }

            n->data_left = chunk_size - sizeof(*n);

            p = stb__try_chunk(n, size, align, pre_align);
            if (p != NULL) {
                n->next = c;
                stb__setchunks(src, n);

                // if we just used up the whole block immediately,
                // move the following chunk up
                n->alloc = 1;
                if (size == chunk_size) {
                    stb__sort_chunks(src);
                }

                return p;
            }

            free(n);
            chunk_size += 16 + align;
        }
    }
}

static stb__alloc* stb__get_context(void* context)
{
    if (context == NULL) {
        return &stb__alloc_global;
    } else {
        int u = stb__identify(context);
        // if context is chunked, grab parent
        if (u == STB__chunked) {
            stb__chunked* s = (stb__chunked*)context - 1;
            return stb__parent(s);
        } else {
            return (stb__alloc*)context - 1;
        }
    }
}

static void stb__insert_alloc(stb__alloc* src, stb__alloc* s)
{
    s->prevn = &src->child;
    s->next = src->child;
    src->child = s + 1;
    if (s->next) {
        *stb__prevn(s->next) = &s->next;
    }
}

static void stb__insert_nochild(stb__alloc* src, stb__nochildren* s)
{
    s->prevn = &src->child;
    s->next = src->child;
    src->child = s + 1;
    if (s->next) {
        *stb__prevn(s->next) = &s->next;
    }
}

static int stb_lowbit8(unsigned int n)
{
    static signed char lowbit4[16] = { -1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };
    int k = lowbit4[n & 15];
    if (k >= 0) {
        return k;
    }
    k = lowbit4[(n >> 4) & 15];
    if (k >= 0) {
        return k + 4;
    }
    return k;
}

static int stb_is_pow2(size_t n)
{
    return (n & (n - 1)) == 0;
}

typedef union {
    unsigned int i;
    void* p;
} stb_uintptr;

static void* malloc_base(void* context, size_t size, stb__alloc_type t, int align)
{
    void* p;

    stb__alloc* src = stb__get_context(context);

    if (align <= 0) {
        // compute worst-case C packed alignment
        // e.g. a 24-byte struct is 8-aligned
        int align_proposed = 1 << stb_lowbit8((unsigned int)size);

        if (align_proposed < 0) {
            align_proposed = 4;
        }

        if (align_proposed == 0) {
            if (size == 0) {
                align_proposed = 1;
            } else {
                align_proposed = 256;
            }
        }

        // a negative alignment means 'don't align any larger
        // than this'; so -16 means we align 1,2,4,8, or 16

        if (align < 0) {
            if (align_proposed > -align) {
                align_proposed = -align;
            }
        }

        align = align_proposed;
    }

    assert(stb_is_pow2(align));

    // don't cause misalignment when allocating nochildren
    if (t == STB__nochildren && align > 8) {
        t = STB__alloc;
    }

    switch (t) {
        case STB__alloc: {
            stb__alloc* s = (stb__alloc*)malloc(size + sizeof(*s));
            if (s == NULL) {
                return NULL;
            }
            p = s + 1;
            s->child = NULL;
            stb__insert_alloc(src, s);

            stb__setchunks(s, NULL);
            break;
        }

        case STB__nochildren: {
            stb__nochildren* s = (stb__nochildren*)malloc(size + sizeof(*s));
            if (s == NULL) {
                return NULL;
            }
            p = s + 1;
            stb__insert_nochild(src, s);
            break;
        }

        case STB__chunk_raw: {
            p = stb__alloc_chunk(src, (int)size, align, 0);
            if (p == NULL) {
                return NULL;
            }
            break;
        }

        case STB__chunked: {
            stb__chunked* s;
            if (align < sizeof(stb_uintptr)) {
                align = sizeof(stb_uintptr);
            }
            s = (stb__chunked*)stb__alloc_chunk(src, (int)size, align, sizeof(*s));
            if (s == NULL) {
                return NULL;
            }
            stb__setparent(s, src);
            p = s + 1;
            break;
        }

        default:
            p = NULL;
            assert(0); /* NOTREACHED */
    }

    ++stb_alloc_count_alloc;
    return p;
}

void* stb_malloc_global(size_t size)
{
    return malloc_base(NULL, size, STB__alloc, stb_alloc_alignment);
}

void* stb_malloc(void* context, size_t size)
{
    return malloc_base(context, size, STB__alloc, stb_alloc_alignment);
}

void* stb_malloc_nofree(void* context, size_t size)
{
    return malloc_base(context, size, STB__chunked, stb_alloc_alignment);
}

void* stb_malloc_leaf(void* context, size_t size)
{
    return malloc_base(context, size, STB__nochildren, stb_alloc_alignment);
}

void* stb_malloc_raw(void* context, size_t size)
{
    return malloc_base(context, size, STB__chunk_raw, stb_alloc_alignment);
}

char* stb_malloc_string(void* context, size_t size)
{
    return (char*)malloc_base(context, size, STB__chunk_raw, 1);
}

void* stb_realloc(void* ptr, size_t newsize)
{
    stb__alloc_type t;

    if (ptr == NULL) {
        return stb_malloc(NULL, newsize);
    }
    if (newsize == 0) {
        stb_free(ptr);
        return NULL;
    }

    t = stb__identify(ptr);
    assert(t == STB__alloc || t == STB__nochildren);

    if (t == STB__alloc) {
        stb__alloc* s = (stb__alloc*)ptr - 1;

        s = (stb__alloc*)realloc(s, newsize + sizeof(*s));
        if (s == NULL) {
            return NULL;
        }

        ptr = s + 1;

        // update pointers
        (*s->prevn) = ptr;
        if (s->next) {
            *stb__prevn(s->next) = &s->next;
        }

        if (s->child) {
            *stb__prevn(s->child) = &s->child;
        }

        return ptr;
    } else {
        stb__nochildren* s = (stb__nochildren*)ptr - 1;

        s = (stb__nochildren*)realloc(ptr, newsize + sizeof(s));
        if (s == NULL) {
            return NULL;
        }

        // update pointers
        (*s->prevn) = s + 1;
        if (s->next) {
            *stb__prevn(s->next) = &s->next;
        }

        return s + 1;
    }
}

void* stb_realloc_c(void* context, void* ptr, size_t newsize)
{
    if (ptr == NULL) {
        return stb_malloc(context, newsize);
    }
    if (newsize == 0) {
        stb_free(ptr);
        return NULL;
    }
    // @TODO: verify you haven't changed contexts
    return stb_realloc(ptr, newsize);
}

void stb_reassign(void* new_context, void* ptr)
{
    stb__alloc* src = stb__get_context(new_context);

    stb__alloc_type t = stb__identify(ptr);
    assert(t == STB__alloc || t == STB__nochildren);

    if (t == STB__alloc) {
        stb__alloc* s = (stb__alloc*)ptr - 1;

        // unlink from old
        *(s->prevn) = s->next;
        if (s->next) {
            *stb__prevn(s->next) = s->prevn;
        }

        stb__insert_alloc(src, s);
    } else {
        stb__nochildren* s = (stb__nochildren*)ptr - 1;

        // unlink from old
        *(s->prevn) = s->next;
        if (s->next) {
            *stb__prevn(s->next) = s->prevn;
        }

        stb__insert_nochild(src, s);
    }
}

#endif /* STB_ALLOC_IMPLEMENTATION */

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
