/*
 * The MIT License
 *
 * Copyright 2023 Andrea Vouk.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*------------------------------------------------------------------------------
    anv_arr (https://github.com/anvouk/anv)
--------------------------------------------------------------------------------

# anv_arr

C general purpose dynamic and memory contiguous arrays.

An array self-contains within a single alloc all its elements + extra info
(like size, capacity, etc).

Once the array's reaches max initial capacity, the next item's insert will
automatically trigger an array expansion to accomodate the new item.

All array's items are fully stored inside the array as a contiguous block of
memory using memcpy/memset.

Trying to insert a NULL item in the array, will memset to zero its location.
As of now at least, there is no way to distinguish between an empty struct and a
NULL item zeroed into the array. For this reason, try to avoid relying on NULL
elements and prefer simply deleting them instead.

Assertion errors are used for invalid parameters during development and get
treated as proper return value in prod. Since most methods do not return a
status code, there is no way to determine a prod "invalid param error" from
another type of runtime error. The correct way to address this is to ensure
no asserts are triggered during development (meaning no invalid parameters are
passed to methods).

## Dependencies

- anv_metalloc.h

## Include usage

```c
// only if metalloc define is not already present somewhere else.
#define ANV_METALLOC_IMPLEMENTATION

#define ANV_ARR_IMPLEMENTATION
#include "anv_arr.h"
```

## Examples

```c
#include <stdio.h>

#define ANV_METALLOC_IMPLEMENTATION
#define ANV_ARR_IMPLEMENTATION
#include "anv_arr.h"

typedef struct item_t {
    int a;
} item_t;

int
main(void)
{
    anv_arr_t arr = anv_arr_new(10, sizeof(item_t));

    // simple push syntax
    item_t item1 = { .a = 69 };
    if (anv_arr_push(arr, &item1) != ANV_ARR_RESULT_OK) {
        fprintf(stderr, "failed inserting item\n");
    }

    // alternative push syntax
    if (anv_arr_push_new(arr, item_t, { .a = 1 }) != ANV_ARR_RESULT_OK) {
        fprintf(stderr, "failed inserting item\n");
    }

    item_t *found_item = anv_arr_get(arr, item_t, 0);
    if (!found_item) {
        fprintf(stderr, "no item at index 0 could be found\n");
    } else {
        printf("found item with value: %d\n", found_item->a);
    }

    anv_arr_destroy(arr);
}
```

------------------------------------------------------------------------------*/

#ifndef ANV_ARR_H
#define ANV_ARR_H

/*
TODO: anv_arr missing methods list
- remove_slow (delete entry at index and traslate all after to keep order)
- pop_first_slow (return and remove first entry, always uses delete_slow)
- push_first (add to head and move existing at spot to last)
- push_first_slow (add to head and traslate all to keep order)
- replace (replace item at index)
- sorting algorithms (qsort, etc)
*/

#include <stddef.h> /* for size_t */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Array status codes for operations.
 */
typedef enum anv_arr_result {
    /**
     * Operation succeeded.
     */
    ANV_ARR_RESULT_OK = 0,
    /**
     * Invalid params have been passed to method.
     * @note With debug asserts enabled, these errors always trigger an assert.
     */
    ANV_ARR_RESULT_INVALID_PARAMS = 1,
    /**
     * Memory allocation related error.
     */
    ANV_ARR_RESULT_ALLOC_ERROR = 2,
    /**
     * Array index is >= the array's current length.
     */
    ANV_ARR_RESULT_INDEX_OUT_OF_BOUNDS = 10,
    /**
     * Two indexes collide with each-other (e.g. are most likely equal).
     */
    ANV_ARR_RESULT_INDEX_COLLISION = 11,
} anv_arr_result;

/**
 * Convenience alias to better recognize an anv array from a regular void *ptr;
 */
typedef void *anv_arr_t;

/**
 * Allocator callback used to determine the new size to assign to an existing
 * array when it has reached max capacity.
 *
 * Reallocator fn example:
 * @code{.c}
 * size_t
 * my_custom_reallocator(size_t old_capacity)
 * {
 *     return old_capacity + 8;
 * }
 * @endcode
 *
 * @param old_capacity The previous array's max capacity.
 * @return The new array's max capacity, which must be > old_capacity.
 */
typedef size_t (*anv_arr_reallocator_fn)(size_t old_capacity);

/**
 * Set custom reallocator callback called to determine the new array's capacity
 * to expand the array to.
 * @note A default reallocator is present by default and can be re-assigned by
 *       passing NULL as a parameter.
 * @param fn Custom reallocator fn to assign.
 */
void anv_arr_config_reallocator_fn(anv_arr_reallocator_fn fn);

/**
 * Allocate a new array with initial max capacity.
 * @param arr_capacity Initial array's max capacity, always > 0.
 * @param item_sz Size of an item to be inserted in the array.
 * @return New array, NULL on invalid params or internal alloc errors.
 */
anv_arr_t anv_arr_new(size_t arr_capacity, size_t item_sz);

/**
 * Destroy an array.
 */
void anv_arr_destroy(anv_arr_t arr);

/**
 * Get number of items currently present in the array.
 * @return 0 for empty array or if cannot determine array length.
 */
size_t anv_arr_length(anv_arr_t arr);

anv_arr_result anv_arr__insert(anv_arr_t *refarr, size_t index, void *item);

/**
 * Insert item at index and move old item at the end of the array.
 * This method does not guarantee items order.
 * @param item Object to insert, can be NULL but it's not recommended.
 * @return Status code.
 */
#define anv_arr_insert(arr, index, item)                                       \
    anv_arr__insert((void *)&(arr), index, item)

/**
 * Syntactic sugar for inserting a new-struct like object inside the array.
 * Works same as anv_arr_insert.
 * @return Status code.
 */
#define anv_arr_insert_new(arr, index, type, fields)                           \
    anv_arr_insert(arr, index, &(type)fields)

anv_arr_result anv_arr__push(anv_arr_t *refarr, void *item);

/**
 * Push a new item to the end of the array.
 * @param item Object to insert, can be NULL but it's not recommended.
 * @return Status code.
 */
#define anv_arr_push(arr, item) anv_arr__push((void *)&(arr), item)

/**
 * Create a temporary struct-like item on the stack and immediately push it to
 * the end of the array.
 *
 * Example:
 * @code{.c}
 * typedef struct item_t {
 *     int a;
 * } item_t;
 *
 * anv_arr_t arr = anv_arr_new(10, sizeof(item_t));
 * anv_arr_push_new(arr, item_t, { .a = 10 });
 * anv_arr_destroy(arr);
 * @endcode
 *
 * @param type The struct-like type which will be inserted.
 * @param fields A C initialization list of the struct fields.
 * @return Status code.
 */
#define anv_arr_push_new(arr, type, fields) anv_arr_push(arr, &(type)fields)

void *anv_arr__pop(anv_arr_t arr);

/**
 * Get and remove last item from array.
 * @return NULL, if the array is empty.
 */
#define anv_arr_pop(arr, type) ((type *)anv_arr__pop(arr))

void *anv_arr__get(anv_arr_t arr, size_t index);

/**
 * Get an item from the array at the specific index.
 *
 * Example:
 * @code{.c}
 * typedef struct item_t {
 *     int a;
 * } item_t;
 *
 * anv_arr_t arr = anv_arr_new(10, sizeof(item_t));
 * item_t *it = anv_arr_get(arr, item_t, 2);
 * anv_arr_destroy(arr);
 * @endcode
 *
 * @return NULL if no item is present at index, index exceeded array's capacity
 *         or an internal error happened.
 */
#define anv_arr_get(arr, type, index) ((type *)anv_arr__get(arr, index))

/**
 * Swap the 2 items found at the 2 passed indexes.
 * This method performs no allocations during the swap.
 * @return Status code.
 */
anv_arr_result anv_arr_swap(anv_arr_t arr, size_t index_a, size_t index_b);

/**
 * Delete an item from the array at the specified index.
 * This method does not guarantee items order.
 * This method performs no allocations during the swap.
 * @return Status code.
 */
anv_arr_result anv_arr_remove(anv_arr_t arr, size_t index);

anv_arr_result anv_arr__shrink_to_fit(anv_arr_t *refarr);

/**
 * Reallocates the array such as the array's capacity is equal to its length.
 * @return Status code.
 */
#define anv_arr_shrink_to_fit(arr) anv_arr__shrink_to_fit((void *)&(arr))

#ifdef __cplusplus
}
#endif

#ifdef ANV_ARR_IMPLEMENTATION

#include <stddef.h> /* for size_t, ptrdiff_t */

#ifndef ANV_METALLOC_METASIZE
#define ANV_METALLOC_METASIZE unsigned char
#endif

typedef ANV_METALLOC_METASIZE anv_meta_size_t;

/**
 * Metalloc status codes for operations.
 */
typedef enum anv_meta_result {
    /**
     * Operation succeeded.
     */
    ANV_META_RESULT_OK = 0,
    /**
     * Invalid params have been passed to method.
     * @note With debug asserts enabled, these errors always trigger an assert.
     */
    ANV_META_RESULT_INVALID_PARAMS = 1,
} anv_meta_result;

/**
 * Check whether mem object is a valid metallocated object and not null.
 * @param mem Metallocated memory block to check.
 * @return 1 if valid, 0 otherwise.
 */
int anv_meta_isvalid(void *mem);

/**
 * Get metadata size for passed memory object.
 * @param mem Metallocated memory block.
 * @return metadata size, always > 0.
 */
anv_meta_size_t anv_meta_getsz(void *mem);

/**
 * Get metadata for passes memory object.
 * @param mem Metallocated memory block.
 * @return Fully zeroed metadata object if metadata was set to NULL.
 */
void *anv_meta_get(void *mem);

/**
 * Change metadata for passed memory object.
 * @param mem Metallocated memory block.
 * @param metadata New metadata value. Must be of correct size specified at
 *        malloc time.
 * @return Status code.
 */
anv_meta_result anv_meta_set(void *mem, void *metadata);

/**
 * Get offset between real memory allocated on heap and first data byte after
 * the metadata.
 * @note subtracting this size from mem you get the pointer to the heap object
 *       passed to regular malloc and thus freeable with regular free.
 * @param mem Metallocated memory block.
 */
ptrdiff_t anv_meta_get_offset(void *mem);

/**
 * Allocate on the heap a new matallocated object.
 *
 * A metallocated object is malloc'd object plus a metadata part before. There
 * is small memory overhead for each object but this allows the storage of
 * custom data for the object while allowing the same usages as a regular
 * malloc'd object (for the most part).
 *
 * @param metadata Optional metadata value to store. Can always be set later.
 * @param meta_sz Size of the metadata value to store. Can not be zero and must
 *                not exceed sizeof(anv_meta_size_t).
 * @param data_sz Size of the data portion to allocated
 * @return A pointer to the data portion of the object and after the metadata
 *         part. To manage the metadata part, use the corresponding methods in
 *         this lib.
 */
void *anv_meta_malloc(void *metadata, anv_meta_size_t meta_sz, size_t data_sz);

/**
 * Free metallocated object memory.
 * @param mem Metallocated memory block, passing NULL is safe and does nothing.
 */
void anv_meta_free(void *mem);

/**
 * Reallocate metallocated data memory part.
 * @note This method does not change size of the metadata portion.
 * @param mem Metallocated memory block.
 * @param new_sz New size of the data portion.
 * @return Pointer to data portion if succeeded.
 */
void *anv_meta_realloc(void *mem, size_t new_sz);

#include <stdint.h> /* for uint32_t */
#include <stdlib.h> /* for malloc(), ... */
#include <string.h> /* for memset() */

#ifndef anv_meta__assert
#include <assert.h>
#define anv_meta__assert(cond, errmsg) assert((cond) && (errmsg))
#endif

#ifdef __GNUC__
#define ANV_META__LIKELY(x)   __builtin_expect((x), 1)
#define ANV_META__UNLIKELY(x) __builtin_expect((x), 0)
#else
#define ANV_META__LIKELY(x)   (x)
#define ANV_META__UNLIKELY(x) (x)
#endif

typedef uint32_t chkb_t;

#define CHKB ((chkb_t)0x696941469)

#define METASZ_SZ sizeof(anv_meta_size_t)
#define CHKB_SZ   sizeof(chkb_t)

int
anv_meta_isvalid(void *mem)
{
    if (!mem) {
        return 0;
    }
    return *(chkb_t *)((size_t)mem - CHKB_SZ) == CHKB ? 1 : 0;
}

static anv_meta_size_t
anv_meta__getsz(void *mem)
{
    return *(anv_meta_size_t *)((size_t)mem - METASZ_SZ - CHKB_SZ);
}

anv_meta_size_t
anv_meta_getsz(void *mem)
{
    if (ANV_META__UNLIKELY(!anv_meta_isvalid(mem))) {
        anv_meta__assert(0, "not a valid metallocated object");
        return 0;
    }
    return anv_meta__getsz(mem);
}

void *
anv_meta_get(void *mem)
{
    if (ANV_META__UNLIKELY(!anv_meta_isvalid(mem))) {
        anv_meta__assert(0, "not a valid metallocated object");
        return NULL;
    }
    return (void *)((size_t)mem - anv_meta__getsz(mem) - METASZ_SZ - CHKB_SZ);
}

static void
anv_meta__set(void *full_mem, void *metadata, size_t meta_sz)
{
    if (metadata) {
        memcpy(full_mem, metadata, meta_sz);
    } else {
        memset(full_mem, 0, meta_sz);
    }
}

anv_meta_result
anv_meta_set(void *mem, void *metadata)
{
    if (ANV_META__UNLIKELY(!anv_meta_isvalid(mem))) {
        anv_meta__assert(0, "not a valid metallocated object");
        return ANV_META_RESULT_INVALID_PARAMS;
    }

    anv_meta_size_t meta_sz = anv_meta__getsz(mem);
    void *full_mem = (void *)((size_t)mem - meta_sz - METASZ_SZ - CHKB_SZ);

    anv_meta__set(full_mem, metadata, meta_sz);
    return ANV_META_RESULT_OK;
}

static ptrdiff_t
anv_meta_get__offset(void *mem)
{
    return (ptrdiff_t)(anv_meta__getsz(mem) + METASZ_SZ + CHKB_SZ);
}

ptrdiff_t
anv_meta_get_offset(void *mem)
{
    if (ANV_META__UNLIKELY(!anv_meta_isvalid(mem))) {
        anv_meta__assert(0, "not a valid metallocated object");
        return 0;
    }
    return anv_meta_get__offset(mem);
}

void *
anv_meta_malloc(void *metadata, anv_meta_size_t meta_sz, size_t data_sz)
{
    if (ANV_META__UNLIKELY(data_sz <= 0)) {
        anv_meta__assert(0, "trying to allocate 0 bytes is not supported");
        return NULL;
    }
    if (ANV_META__UNLIKELY(meta_sz <= 0)) {
        anv_meta__assert(0, "metadata allocation size cannot be 0");
        return NULL;
    }

    void *full_mem = malloc(data_sz + meta_sz + METASZ_SZ + CHKB_SZ);
    if (ANV_META__UNLIKELY(!full_mem)) {
        return NULL;
    }

    anv_meta__set(full_mem, metadata, meta_sz);

    *(anv_meta_size_t *)((size_t)full_mem + meta_sz) = meta_sz;
    *(chkb_t *)((size_t)full_mem + meta_sz + METASZ_SZ) = CHKB;
    return (void *)((size_t)full_mem + meta_sz + METASZ_SZ + CHKB_SZ);
}

void
anv_meta_free(void *mem)
{
    if (ANV_META__UNLIKELY(!anv_meta_isvalid(mem))) {
        anv_meta__assert(0, "not a valid metallocated object");
        return;
    }
    free((void *)((size_t)mem - (anv_meta__getsz(mem) + METASZ_SZ + CHKB_SZ)));
}

void *
anv_meta_realloc(void *mem, size_t new_sz)
{
    if (ANV_META__UNLIKELY(!anv_meta_isvalid(mem))) {
        anv_meta__assert(0, "not a valid metallocated object");
        return NULL;
    }

    ptrdiff_t padd = anv_meta_get__offset(mem);
    void *full_mem = (void *)((size_t)mem - padd);

    anv_meta_size_t meta_sz = anv_meta__getsz(mem);

    void *reallocated_mem
        = realloc(full_mem, new_sz + meta_sz + METASZ_SZ + CHKB_SZ);
    if (ANV_META__UNLIKELY(!reallocated_mem)) {
        free(full_mem);
        return NULL;
    }

    return (void *)((size_t)reallocated_mem + padd);
}

#ifndef anv_arr__assert
#include <assert.h>
#define anv_arr__assert(cond, msg) assert((cond) && (msg))
#endif

#ifdef __GNUC__
#define ANV_ARR__LIKELY(x)   __builtin_expect((x), 1)
#define ANV_ARR__UNLIKELY(x) __builtin_expect((x), 0)
#else
#define ANV_ARR__LIKELY(x)   (x)
#define ANV_ARR__UNLIKELY(x) (x)
#endif

typedef struct anv_arr__metadata {
    size_t arr_sz;
    size_t arr_capacity;
    size_t item_sz;
    // This is used to perform swaps without having to allocate extra memory.
    // Always leave as last element in the struct.
    // Always access with ANV_ARR__TMP_ITEM(metadata).
    void *tmp_item;
} anv_arr__metadata;

#define ANV_ARR__TMP_ITEM_OFFSET    offsetof(anv_arr__metadata, tmp_item)
#define ANV_ARR__TMP_ITEM(metadata) ((metadata) + ANV_ARR__TMP_ITEM_OFFSET)

static size_t
anv_arr__reallocator_default(size_t old_capacity)
{
    return old_capacity + 8;
}

static anv_arr_reallocator_fn anv_arr__reallocator
    = anv_arr__reallocator_default;

void
anv_arr_config_reallocator_fn(anv_arr_reallocator_fn fn)
{
    if (!fn) {
        anv_arr__reallocator = anv_arr__reallocator_default;
    } else {
        anv_arr__reallocator = fn;
    }
}

anv_arr_t
anv_arr_new(size_t arr_capacity, size_t item_sz)
{
    anv_arr__metadata metadata = {
        .arr_sz = 0,
        .arr_capacity = arr_capacity,
        .item_sz = item_sz,
        .tmp_item = NULL,
    };
    // Note: the weird metadata size calculation is to make tmp_item of the
    // exact size of an item to insert. This empty spot is used to perform swaps
    // and such operations without having to allocate extra memory.
    // Always access tmp_item with ANV_ARR__TMP_ITEM(metadata).
    void *arr = anv_meta_malloc(
        &metadata,
        sizeof(anv_arr__metadata) + item_sz - sizeof(void *),
        item_sz * arr_capacity
    );
    return arr;
}

void
anv_arr_destroy(anv_arr_t arr)
{
    anv_meta_free(arr);
}

size_t
anv_arr_length(anv_arr_t arr)
{
    if (ANV_ARR__UNLIKELY(!arr)) {
        anv_arr__assert(0, "invalid null array");
        return 0;
    }
    anv_arr__metadata *metadata = (anv_arr__metadata *)anv_meta_get(arr);
    if (ANV_ARR__UNLIKELY(!metadata)) {
        anv_arr__assert(0, "cannot find metadata, is arr a valid meta obj?");
        return 0;
    }
    return metadata->arr_sz;
}

static void *
anv_arr__get_internal(anv_arr_t arr, size_t index, anv_arr__metadata *metadata)
{
    return (void *)((size_t)arr + metadata->item_sz * index);
}

static void
anv_arr__set_internal(
    anv_arr_t arr, size_t index, anv_arr__metadata *metadata, void *item
)
{
    void *spot = (void *)((size_t)arr + metadata->item_sz * index);
    if (item) {
        memcpy(spot, item, metadata->item_sz);
    } else {
        memset(spot, 0, metadata->item_sz);
    }
}

static anv_arr_result
anv_arr__reallocate(
    anv_arr_t *refarr, anv_arr__metadata **refmetadata, size_t new_capacity
)
{
    void *resized_arr
        = anv_meta_realloc(*refarr, (*refmetadata)->item_sz * new_capacity);
    if (ANV_ARR__UNLIKELY(!resized_arr)) {
        return ANV_ARR_RESULT_ALLOC_ERROR;
    }
    // during reallocations the metadata might be moved somewhere else, we
    // retrieve it again and propagate it upwards where needed.
    anv_arr__metadata *new_metadata_loc
        = (anv_arr__metadata *)anv_meta_get(resized_arr);
    *refmetadata = new_metadata_loc;
    new_metadata_loc->arr_capacity = new_capacity;
    *refarr = resized_arr;
    return ANV_ARR_RESULT_OK;
}

static anv_arr_result
anv_arr__push_internal(
    anv_arr_t *refarr, anv_arr__metadata **refmetadata, void *item
)
{
    if ((*refmetadata)->arr_sz >= (*refmetadata)->arr_capacity) {
        size_t new_capacity
            = anv_arr__reallocator((*refmetadata)->arr_capacity);
        anv_arr_result res
            = anv_arr__reallocate(refarr, refmetadata, new_capacity);
        if (res != ANV_ARR_RESULT_OK) {
            return res;
        }
    }

    anv_arr__set_internal(
        *refarr, (*refmetadata)->arr_sz++, (*refmetadata), item
    );
    return ANV_ARR_RESULT_OK;
}

anv_arr_result
anv_arr__insert(anv_arr_t *refarr, size_t index, void *item)
{
    if (ANV_ARR__UNLIKELY(!refarr || !*refarr)) {
        anv_arr__assert(0, "invalid null array");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    anv_arr__metadata *metadata = (anv_arr__metadata *)anv_meta_get(*refarr);
    if (ANV_ARR__UNLIKELY(!metadata)) {
        anv_arr__assert(0, "cannot find metadata, is refarr a valid meta obj?");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    // Inserting at index 0 for empty arrays is a supported special case.
    if (index != 0 && index >= metadata->arr_sz) {
        return ANV_ARR_RESULT_INDEX_OUT_OF_BOUNDS;
    }

    // Empty array have no existing item to move.
    if (metadata->arr_sz == 0) {
        return anv_arr__push_internal(refarr, &metadata, item);
    } else {
        // Here it's import that we push the new item to last before
        // updating the current item. This ensure the old item value is memcpyed
        // to the new location.
        void *old_item = anv_arr__get_internal(*refarr, index, metadata);
        anv_arr_result res
            = anv_arr__push_internal(refarr, &metadata, old_item);
        if (res == ANV_ARR_RESULT_OK) {
            anv_arr__set_internal(*refarr, index, metadata, item);
        }
        return res;
    }
}

anv_arr_result
anv_arr__push(anv_arr_t *refarr, void *item)
{
    if (ANV_ARR__UNLIKELY(!refarr || !*refarr)) {
        anv_arr__assert(0, "invalid null array");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    anv_arr__metadata *metadata = (anv_arr__metadata *)anv_meta_get(*refarr);
    if (ANV_ARR__UNLIKELY(!metadata)) {
        anv_arr__assert(0, "cannot find metadata, is refarr a valid meta obj?");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    return anv_arr__push_internal(refarr, &metadata, item);
}

void *
anv_arr__pop(anv_arr_t arr)
{
    if (ANV_ARR__UNLIKELY(!arr)) {
        anv_arr__assert(0, "invalid null array");
        return NULL;
    }

    anv_arr__metadata *metadata = (anv_arr__metadata *)anv_meta_get(arr);
    if (ANV_ARR__UNLIKELY(!metadata)) {
        anv_arr__assert(0, "cannot find metadata, is arr a valid meta obj?");
        return NULL;
    }

    if (metadata->arr_sz == 0) {
        return NULL;
    }
    void *item = anv_arr__get_internal(arr, metadata->arr_sz - 1, metadata);
    metadata->arr_sz--;
    return item;
}

void *
anv_arr__get(anv_arr_t arr, size_t index)
{
    if (ANV_ARR__UNLIKELY(!arr)) {
        anv_arr__assert(0, "invalid null array");
        return NULL;
    }

    anv_arr__metadata *metadata = (anv_arr__metadata *)anv_meta_get(arr);
    if (ANV_ARR__UNLIKELY(!metadata)) {
        anv_arr__assert(0, "cannot find metadata, is arr a valid meta obj?");
        return NULL;
    }

    if (index >= metadata->arr_sz) {
        return NULL;
    }
    return anv_arr__get_internal(arr, index, metadata);
}

static void
anv_arr__swap_internal(
    anv_arr_t arr, anv_arr__metadata *metadata, size_t index_a, size_t index_b
)
{
    void *tmp_item = ANV_ARR__TMP_ITEM(metadata);
    void *item_a = anv_arr__get_internal(arr, index_a, metadata);
    void *item_b = anv_arr__get_internal(arr, index_b, metadata);

    memcpy(tmp_item, item_a, metadata->item_sz);
    memcpy(item_a, item_b, metadata->item_sz);
    memcpy(item_b, tmp_item, metadata->item_sz);
}

anv_arr_result
anv_arr_swap(anv_arr_t arr, size_t index_a, size_t index_b)
{
    if (ANV_ARR__UNLIKELY(!arr)) {
        anv_arr__assert(0, "invalid null array");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    anv_arr__metadata *metadata = (anv_arr__metadata *)anv_meta_get(arr);
    if (ANV_ARR__UNLIKELY(!metadata)) {
        anv_arr__assert(0, "cannot find metadata, is arr a valid meta obj?");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    if (index_a == index_b) {
        return ANV_ARR_RESULT_INDEX_COLLISION;
    }
    if (index_a >= metadata->arr_sz || index_b >= metadata->arr_sz) {
        return ANV_ARR_RESULT_INDEX_OUT_OF_BOUNDS;
    }

    anv_arr__swap_internal(arr, metadata, index_a, index_b);
    return ANV_ARR_RESULT_OK;
}

static void
anv_arr__copy_internal(
    anv_arr_t arr, anv_arr__metadata *metadata, size_t from_idx, size_t to_idx
)
{
    void *from_item = anv_arr__get_internal(arr, from_idx, metadata);
    void *to_item = anv_arr__get_internal(arr, to_idx, metadata);
    memcpy(to_item, from_item, metadata->item_sz);
}

anv_arr_result
anv_arr_remove(anv_arr_t arr, size_t index)
{
    if (ANV_ARR__UNLIKELY(!arr)) {
        anv_arr__assert(0, "invalid null array");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    anv_arr__metadata *metadata = (anv_arr__metadata *)anv_meta_get(arr);
    if (ANV_ARR__UNLIKELY(!metadata)) {
        anv_arr__assert(0, "cannot find metadata, is arr a valid meta obj?");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    if (index >= metadata->arr_sz) {
        return ANV_ARR_RESULT_INDEX_OUT_OF_BOUNDS;
    }

    anv_arr__copy_internal(arr, metadata, metadata->arr_sz - 1, index);
    metadata->arr_sz--;
    return ANV_ARR_RESULT_OK;
}

anv_arr_result
anv_arr__shrink_to_fit(anv_arr_t *refarr)
{
    if (ANV_ARR__UNLIKELY(!refarr || !*refarr)) {
        anv_arr__assert(0, "invalid null array");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    anv_arr__metadata *metadata = (anv_arr__metadata *)anv_meta_get(*refarr);
    if (ANV_ARR__UNLIKELY(!metadata)) {
        anv_arr__assert(0, "cannot find metadata, is refarr a valid meta obj?");
        return ANV_ARR_RESULT_INVALID_PARAMS;
    }

    return anv_arr__reallocate(refarr, &metadata, metadata->arr_sz);
}

#endif /* ANV_ARR_IMPLEMENTATION */

#endif /* ANV_ARR_H */
