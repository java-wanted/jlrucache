/**
 * @file
 * Hash Map for LRU list
 *
 * @author Boris.Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#ifndef HMAP_H
#define HMAP_H

#include <sys/queue.h>

struct frames;
struct lrul_item;

/** HMap item */
struct hmap_item {
    CIRCLEQ_ENTRY(hmap_item) next;
    /** The index of the associated frame. */
    unsigned frame_idx;
    /** The information on the frame usage. */
    struct lrul_item *lrul_item;
    /**
     * The key.
     *
     * It must be set before the item is added into the map and
     * must not be until the item is unmapped.
     */
    int key;
    /** The index within the internal storage implementation. */
    unsigned hmap_idx;
};

/** Allocate hmap item and associate it with an unused frame. */
extern struct hmap_item *hmap_item_alloc(struct frames *frames);

/**
 * Allocate hmap for a specific number of frames.
 *
 * @param capacity The numer of frames to map.
 */
extern struct hmap *hmap_alloc(unsigned capacity);
extern void hmap_free(struct hmap *hmap);

/**
 * Get the hmap item for a key.
 *
 * @retval The item or NULL if there is no item for the key.
 */
extern struct hmap_item *hmap_get(struct hmap *hmap, int key);

/** Unmap an hmap item. */
extern void hmap_rm(struct hmap *hmap, struct hmap_item *item);

/**
 * Map an hmap item.
 *
 * The item has to be set with a key.
 */
extern void hmap_add(struct hmap *hmap, struct hmap_item *item);

#endif /* HMAP_H */
