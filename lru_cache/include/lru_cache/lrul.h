/**
 * @file
 * LRUL list for LRU cache
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#ifndef LRUL_H
#define LRUL_H

#include <sys/queue.h>

struct hmap_item;
struct lrul;

/** The LRU information on a frame. */
struct lrul_item {
    CIRCLEQ_ENTRY(lrul_item) next;
    /** The HMap item associated with the frame. */
    struct hmap_item *hmap_item;
};

extern struct lrul_item *lrul_item_alloc(struct hmap_item *hmap_item);
extern void lrul_item_free(struct lrul_item *item);

extern struct lrul *lrul_alloc(void);
extern void lrul_free(struct lrul *lrul);

/** Add the item to be the most recently used. */
extern void lrul_add(struct lrul *lrul, struct lrul_item *item);

/** Extract the last recently used item. */
extern struct lrul_item *lrul_rm(struct lrul *lrul);

/** Remove a specific item. */
extern void lrul_rm_item(struct lrul *lrul, struct lrul_item *item);

#endif /* LRUL_H */
