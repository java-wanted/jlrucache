/**
 * @file
 * LRU frames
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#include <stdlib.h>
#include "lru_cache/log.h"
#include "lru_cache/frames.h"

struct frames {
    int *values;
    unsigned capacity;
    unsigned size;
};

struct frames *frames_alloc(unsigned capacity)
{
    struct frames *frames;

    ASSERT(capacity > 0);

    frames = malloc(sizeof(*frames));
    die_on(!frames, "failed to allocate frames: capacity %u\n", capacity);

    frames->values = malloc(capacity * sizeof(frames->values[0]));
    die_on(!frames, "failed to allocate frames: capacity %u\n", capacity);
    frames->capacity = capacity;
    frames->size = 0;

    return frames;
}

void frames_free(struct frames *frames)
{
    free(frames->values);
    free(frames);
}

int frames_all_used(struct frames *frames)
{
    return frames->size == frames->capacity;
}

unsigned frames_reserve(struct frames *frames)
{
    ASSERT(frames->size < frames->capacity);

    return frames->size++;
}

int *frames_ref(struct frames *frames, unsigned idx)
{
    ASSERT(idx < frames->size);

    return frames->values + idx;
}
