/**
 * @file
 * LRU list for LRU cache
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#include <stdlib.h>
#include "lru_cache/log.h"
#include "lru_cache/lrul.h"

struct lrul {
    CIRCLEQ_HEAD(, lrul_item) head;
};

struct lrul_item *lrul_item_alloc(struct hmap_item *hmap_item)
{
    struct lrul_item *item = malloc(sizeof(*item));

    die_on(!item, "failed to allocate LRU list item\n");
    item->hmap_item = hmap_item;

    return item;
}

void lrul_item_free(struct lrul_item *item)
{
    free(item);
}

struct lrul *lrul_alloc(void)
{
    struct lrul *lrul = malloc(sizeof(*lrul));

    die_on(!lrul, "failed to allocate LRU list\n");

    CIRCLEQ_INIT(&lrul->head);

    return lrul;
}

void lrul_free(struct lrul *lrul)
{
    free(lrul);
}

void lrul_add(struct lrul *lrul, struct lrul_item *item)
{
    CIRCLEQ_INSERT_HEAD(&lrul->head, item, next);
}

struct lrul_item *lrul_rm(struct lrul *lrul)
{
    struct lrul_item *item = CIRCLEQ_LAST(&lrul->head);

    CIRCLEQ_REMOVE(&lrul->head, item, next);

    return item;
}

void lrul_rm_item(struct lrul *lrul, struct lrul_item *item)
{
    CIRCLEQ_REMOVE(&lrul->head, item, next);
}
