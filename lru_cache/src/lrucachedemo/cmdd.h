/**
 * @file
 *
 * LRU cache command driver
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#ifndef CMDD_H
#define CMDD_H

#include <stdio.h>

struct cmdd;

/** Create cmdd for commands on the command line */
extern struct cmdd *cmdd_parse(int argc, char **argv);
extern void cmdd_free(struct cmdd *cmdd);

extern void cmdd_run(struct cmdd *cmdd);

/** Print a cmdd results. */
extern void cmdd_print(struct cmdd *cmdd, FILE *fp);

#endif /* CMDD_H */
