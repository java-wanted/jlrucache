/**
 * @file
 *
 * LRU cache demonstration task
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#include <stdio.h>
#include "cmdd.h"

int main(int argc, char **argv)
{
    struct cmdd *cmdd = cmdd_parse(argc, argv);

    cmdd_run(cmdd);
    cmdd_print(cmdd, stdout);
    cmdd_free(cmdd);

    return 0;
}
