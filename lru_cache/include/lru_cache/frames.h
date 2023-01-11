/**
 * @file
 * LRU frames
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#ifndef FRAMES_H
#define FRAMES_H

struct frames;

extern struct frames *frames_alloc(unsigned capacity);
extern void frames_free(struct frames *frames);

/**
 * Check all frames are used.
 *
 * @retval 1 if all frames are used or 0 otherwise.
 */
extern int frames_all_used(struct frames *frames);

/**
 * Make the next unused frame to be used.
 *
 * @retval The index of the frame.
 */
extern unsigned frames_reserve(struct frames *frames);

/**
 * Get the address of a frame.
 *
 * @param idx The index of the frame.
 */
extern int *frames_ref(struct frames *frames, unsigned idx);

#endif
