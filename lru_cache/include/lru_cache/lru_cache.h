/**
 * @file
 * LRU cache
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#ifndef LRU_CACHE_H
#define LRU_CACHE_H

struct lru_cache;

/**
 * Create the LRU cache.
 *
 * @param capacity The numter of frames within the cache.
 */
extern struct lru_cache *lru_cache_alloc(unsigned capacity);
extern void lru_cache_free(struct lru_cache *cache);

/** Cache a value with a specific key. */
extern void lru_cache_put(struct lru_cache *cache, int key, int value);

/**
 * Retrieve the value for a specific key.
 *
 * @retval The value or -1 if there is no value for the key.
 */
extern int lru_cache_get(struct lru_cache *cache, int key);

#endif /* LRU_CACHE_H */
