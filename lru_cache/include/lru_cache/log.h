/**
 * @file
 * Logging for LRU cache
 *
 * @author Boris.Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define die(_fmt, ...) \
    do { \
        fprintf(stderr, "ERROR: " _fmt, ## __VA_ARGS__); \
        exit(1); \
    } while (0)

#define die_on(_cond, _fmt, ...) \
    do { \
        if (_cond) \
            die(_fmt, ## __VA_ARGS__); \
    } while (0)

#define BUILD_ASSERT(_cond) \
    do { \
        char x[(_cond) ? 0 : -1]; \
        (void) x; \
    } while (0)

#ifdef LRU_DEBUG

#define __S(_x) # _x
#define _S(_x) __S(_x)

#define ASSERT(_cond) \
    do { \
        die_on(!(_cond), "%s.%d: assertion failed '%s'\n", \
            __FILE__, __LINE__, _S(_cond)); \
    } while(0)

#define DPRINT(_l, _fmt, ...) \
    do { \
        if (_l) \
            fprintf(stderr, "DEBUG: " _fmt, ## __VA_ARGS__); \
    } while (0)

#else

#define ASSERT(_cond)

#define DPRINT(_l, _fmt, ...)

#endif

#endif /* LOG_H */
