/**
 * @file
 * LRU cache
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#include <stdlib.h>
#include "lru_cache/log.h"
#include "lru_cache/frames.h"
#include "lru_cache/lrul.h"
#include "lru_cache/hmap.h"
#include "lru_cache/lru_cache.h"

struct lru_cache {
    struct frames *frames;
    struct lrul *lrul;
    struct hmap *hmap;
};

struct lru_cache *lru_cache_alloc(unsigned capacity)
{
    struct lru_cache *cache = malloc(sizeof(*cache));

    die_on(!cache, "failed to allocate lru cache\n");

    cache->frames = frames_alloc(capacity);
    cache->lrul = lrul_alloc();
    cache->hmap = hmap_alloc(capacity);

    return cache;
}

void lru_cache_free(struct lru_cache *cache)
{
    hmap_free(cache->hmap);
    lrul_free(cache->lrul);
    frames_free(cache->frames);
    free(cache);
}

void lru_cache_put(struct lru_cache *cache, int key, int value)
{
    struct hmap_item *hmap_item = hmap_get(cache->hmap, key);
    struct lrul_item *lrul_item;

    if (hmap_item) {
        lrul_rm_item(cache->lrul, hmap_item->lrul_item);
    } else {
        if (frames_all_used(cache->frames)) {
            lrul_item = lrul_rm(cache->lrul);
            hmap_item = lrul_item->hmap_item;
            hmap_rm(cache->hmap, hmap_item);
        } else {
            hmap_item = hmap_item_alloc(cache->frames);
            hmap_item->lrul_item = lrul_item_alloc(hmap_item);
        }

        hmap_item->key = key;
        hmap_add(cache->hmap, hmap_item);
    }

    lrul_add(cache->lrul, hmap_item->lrul_item);
    *frames_ref(cache->frames, hmap_item->frame_idx) = value;
}

int lru_cache_get(struct lru_cache *cache, int key)
{
    struct hmap_item *hmap_item = hmap_get(cache->hmap, key);

    if (!hmap_item)
        return -1;

    lrul_rm_item(cache->lrul, hmap_item->lrul_item);
    lrul_add(cache->lrul, hmap_item->lrul_item);

    return *frames_ref(cache->frames, hmap_item->frame_idx);
}
