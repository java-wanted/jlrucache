/**
 * @file
 * Hash Map for LRU list
 *
 * @author Boris.Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#include <stdlib.h>
#include "lru_cache/log.h"
#include "lru_cache/frames.h"
#include "lru_cache/hmap.h"

#ifndef UNUSED
#define UNUSED(_x) (void) (_x)
#endif

#ifndef UINT_WIDTH
#define UINT_WIDTH 32
#endif

/*
 * Maximum number of bits in the number of buckets.
 *
 * The number of buckets is derived from the number of frames:
 *   Number of buckets = MIN(1 << FLS(Number of frames),
 *                           1 << HMAP_BUCKETS_BITS)
 */
#define HMAP_BUCKETS_BITS 12U

CIRCLEQ_HEAD(hmap_bucket, hmap_item);

typedef unsigned (*hmap_h_func_t)(struct hmap *hmap, unsigned key);

struct hmap {
    struct hmap_bucket *buckets;
    unsigned n_bits;
    unsigned n_mask;
    hmap_h_func_t h_func;
};

struct hmap_item *hmap_item_alloc(struct frames *frames)
{
    struct hmap_item *item = calloc(1, sizeof(*item));

    ASSERT(!frames_all_used(frames));

    item->frame_idx = frames_reserve(frames);

    return item;
}

static void hmap_item_free(struct hmap_item *item)
{
    free(item);
}

static void hmap_bucket_init(struct hmap_bucket *bucket)
{
    CIRCLEQ_INIT(bucket);
}

static void hmap_bucket_free(struct hmap_bucket *bucket)
{
    struct hmap_item *item;

    while (!CIRCLEQ_EMPTY(bucket)) {
        item = CIRCLEQ_FIRST(bucket);
        CIRCLEQ_REMOVE(bucket, item, next);
        hmap_item_free(item);
    }
}

void hmap_bucket_add(struct hmap_bucket *bucket, struct hmap_item *item)
{
    CIRCLEQ_INSERT_TAIL(bucket, item, next);
}

void hmap_bucket_rm(struct hmap_bucket *bucket, struct hmap_item *item)
{
    CIRCLEQ_REMOVE(bucket, item, next);
}

static struct hmap_item *hmap_bucket_get(struct hmap_bucket *bucket, int key)
{
    struct hmap_item *item;

    CIRCLEQ_FOREACH(item, bucket, next) {
        if (item->key == key)
            return item;
    }

    return NULL;
}

static unsigned hmap_n_bits(unsigned capacity)
{
    unsigned n = UINT_WIDTH - __builtin_clz(capacity);

    BUILD_ASSERT(HMAP_BUCKETS_BITS < UINT_WIDTH);

    if (n > HMAP_BUCKETS_BITS)
        n = HMAP_BUCKETS_BITS;

    return n;
}

static unsigned hmap_h_func_8(struct hmap *hmap, unsigned key)
{
    unsigned h = key ^ (key >> 8) ^ (key >> 16) ^ (key >> 24);

    h = h ^ (h >> hmap->n_bits);

    return h & hmap->n_mask;
}

static unsigned hmap_h_func_4(struct hmap *hmap, unsigned key)
{
    unsigned h = key ^ (key >> 16);

    h = h ^ (h >> hmap->n_bits);

    return h & hmap->n_mask;
}

static unsigned hmap_h_func_2(struct hmap *hmap, unsigned key)
{
    unsigned h = key;

    h = h ^ (h >> hmap->n_bits);

    return h & hmap->n_mask;
}

static hmap_h_func_t hmap_h_func(unsigned bits)
{
    if (bits <= 4)
        return hmap_h_func_8;
    else if (bits <= 8)
        return hmap_h_func_4;
    else
        return hmap_h_func_2;
}

struct hmap *hmap_alloc(unsigned capacity)
{
    struct hmap *hmap;
    unsigned i;

    ASSERT(capacity > 0);

    hmap = malloc(sizeof(*hmap));
    die_on(!hmap, "failed to allocate hmap\n");

    hmap->n_bits = hmap_n_bits(capacity);
    hmap->n_mask = (1U << hmap->n_bits) - 1U;

    hmap->buckets = malloc((hmap->n_mask + 1) * sizeof(*hmap->buckets));
    die_on(!hmap->buckets, "failed to allocate hmap storage: capacity %u\n",
                           capacity);

    hmap->h_func = hmap_h_func(hmap->n_bits);

    for (i = 0; i < hmap->n_mask + 1; ++i)
        hmap_bucket_init(hmap->buckets + i);

    return hmap;
}

void hmap_free(struct hmap *hmap)
{
    unsigned i;

    for (i = 0; i < hmap->n_mask + 1; ++i)
        hmap_bucket_free(hmap->buckets + i);

    free(hmap);
}

void hmap_add(struct hmap *hmap, struct hmap_item *item)
{
    unsigned i = hmap->h_func(hmap, item->key);
    struct hmap_bucket *bucket = hmap->buckets + i;

    DPRINT(0, "hmap: add key %u with idx %u (frame %u)\n",
           item->key, i, item->frame_idx);
    item->hmap_idx = i;
    hmap_bucket_add(bucket, item);
}

void hmap_rm(struct hmap *hmap, struct hmap_item *item)
{
    struct hmap_bucket *bucket = hmap->buckets + item->hmap_idx;

    ASSERT(item->hmap_idx == hmap->h_func(hmap, item->key));

    DPRINT(0, "hmap: rm key %u with idx %u (frame %u)\n",
           item->key, item->hmap_idx, item->frame_idx);
    hmap_bucket_rm(bucket, item);
}

struct hmap_item *hmap_get(struct hmap *hmap, int key)
{
    unsigned i = hmap->h_func(hmap, key);
    struct hmap_bucket *bucket = hmap->buckets + i;

    return hmap_bucket_get(bucket, key);
}
