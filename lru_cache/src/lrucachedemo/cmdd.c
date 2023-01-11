/**
 * @file
 *
 * LRU cache command driver
 *
 * @author Boris Stankevich <microsoft-wanted@yandex.ru>
 * @copyright GPL-3.0+
 */
#include <stdlib.h>
#include <stdarg.h>
#include <sys/queue.h>
#include "lru_cache/log.h"
#include "lru_cache/lru_cache.h"
#include "cmdd.h"

#ifndef UNUSED
#define UNUSED(_x) (void)(_x)
#endif

enum cmdd_op {
    CMDD_OP_ALLOC = 0,
    CMDD_OP_PUT,
    CMDD_OP_GET,
};

enum cmdd_print_flag {
    CMDD_PRINT_NAME = 0,
    CMDD_PRINT_ARGV,
};

struct cmdd_cmd;
struct cmdd_out;

typedef struct cmdd_out *(*cmdd_run_cmd_t)(struct cmdd *cmdd,
                                           struct cmdd_cmd *cmd);
typedef void (*cmdd_print_cmd_t)(struct cmdd *cmdd, struct cmdd_cmd *cmd,
                                 FILE *fp, enum cmdd_print_flag flag);

struct cmdd_cmd_ops {
    cmdd_run_cmd_t run;
    cmdd_print_cmd_t print;
};

struct cmdd_cmd {
    SIMPLEQ_ENTRY(cmdd_cmd) next;
    struct cmdd_cmd_ops const *ops;
    union {
        struct {
            unsigned capacity;
        } a;
        struct {
            int key;
            int value;
        } p;
        struct {
            int key;
        } g;
    };
};

struct cmdd_out {
    SIMPLEQ_ENTRY(cmdd_out) next;
    char *out;
};

struct cmdd {
    SIMPLEQ_HEAD(, cmdd_cmd) cmds;
    SIMPLEQ_HEAD(, cmdd_out) outs;
    struct lru_cache *cache;
};

static struct cmdd_out *cmdd_run_alloc(struct cmdd *cmdd,
                                       struct cmdd_cmd *cmd);
static struct cmdd_out *cmdd_run_put(struct cmdd *cmdd,
                                     struct cmdd_cmd *cmd);
static struct cmdd_out *cmdd_run_get(struct cmdd *cmdd,
                                     struct cmdd_cmd *cmd);

static void cmdd_print_alloc(struct cmdd *cmdd, struct cmdd_cmd *cmd,
                             FILE *fp, enum cmdd_print_flag flag);
static void cmdd_print_put(struct cmdd *cmdd, struct cmdd_cmd *cmd,
                           FILE *fp, enum cmdd_print_flag flag);
static void cmdd_print_get(struct cmdd *cmdd, struct cmdd_cmd *cmd,
                           FILE *fp, enum cmdd_print_flag flag);

static struct cmdd_cmd_ops const cmdd_cmd_ops[] = {
    [CMDD_OP_ALLOC] = {
        .run = cmdd_run_alloc,
        .print = cmdd_print_alloc,
    },
    [CMDD_OP_PUT] = {
        .run = cmdd_run_put,
        .print = cmdd_print_put,
    },
    [CMDD_OP_GET] = {
        .run = cmdd_run_get,
        .print = cmdd_print_get,
    },
};

static struct cmdd_cmd *cmdd_cmd_alloc(enum cmdd_op op, ...)
{
    struct cmdd_cmd *cmd = malloc(sizeof(*cmd));
    va_list ap;

    die_on(!cmd, "failed to allocate command\n");

    va_start(ap, op);
    switch (op) {
    case CMDD_OP_ALLOC:
        cmd->ops = &cmdd_cmd_ops[CMDD_OP_ALLOC];
        cmd->a.capacity = va_arg(ap, unsigned);
        break;
    case CMDD_OP_PUT:
        cmd->ops = &cmdd_cmd_ops[CMDD_OP_PUT];
        cmd->p.key = va_arg(ap, int);
        cmd->p.value = va_arg(ap, int);
        break;
    case CMDD_OP_GET:
        cmd->ops = &cmdd_cmd_ops[CMDD_OP_GET];
        cmd->g.key = va_arg(ap, int);
        break;
    default:
        ASSERT(0);
    }
    va_end(ap);

    return cmd;
}

static void cmdd_cmd_free(struct cmdd_cmd *cmd)
{
    free(cmd);
}

static struct cmdd_out *cmdd_out_alloc(char const *fmt, ...)
{
    struct cmdd_out *out = malloc(sizeof(*out));
    va_list ap;
    int n;

    die_on(!out, "failed to allocate memory to save result\n");

    va_start(ap, fmt);
    n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    out->out = malloc(n + 1);
    die_on(!out, "failed to allocate memory to save result string\n");

    va_start(ap, fmt);
    vsnprintf(out->out, n + 1, fmt, ap);
    va_end(ap);

    return out;
}

static void cmdd_out_free(struct cmdd_out *out)
{
    free(out->out);
    free(out);
}

static struct cmdd *cmdd_alloc(void)
{
    struct cmdd *cmdd = malloc(sizeof(*cmdd));

    die_on(!cmdd, "failed to allocate cmdd\n");

    SIMPLEQ_INIT(&cmdd->cmds);
    SIMPLEQ_INIT(&cmdd->outs);
    cmdd->cache = NULL;

    return cmdd;
}

void cmdd_free(struct cmdd *cmdd)
{
    struct cmdd_cmd *cmd;
    struct cmdd_out *out;

    if (cmdd->cache)
        lru_cache_free(cmdd->cache);

    while (!SIMPLEQ_EMPTY(&cmdd->cmds)) {
        cmd = SIMPLEQ_FIRST(&cmdd->cmds);
        SIMPLEQ_REMOVE_HEAD(&cmdd->cmds, next);
        cmdd_cmd_free(cmd);
    }

    while (!SIMPLEQ_EMPTY(&cmdd->outs)) {
        out = SIMPLEQ_FIRST(&cmdd->outs);
        SIMPLEQ_REMOVE_HEAD(&cmdd->outs, next);
        cmdd_out_free(out);
    }

    free(cmdd);
}

struct cmdd *cmdd_parse(int argc, char **argv)
{
    struct cmdd *cmdd = cmdd_alloc();
    struct cmdd_cmd *cmd;

    UNUSED(argc);
    UNUSED(argv);

    /*
     * TODO(Boris Stankevich): Read commands from the command line.
     *
     * For now, commands are preset.
     */
    cmd = cmdd_cmd_alloc(CMDD_OP_ALLOC, 2);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_PUT, 1, 1);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_PUT, 2, 2);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_GET, 1);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_PUT, 3, 3);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_GET, 2);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_PUT, 4, 4);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_GET, 1);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_GET, 3);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);
    cmd = cmdd_cmd_alloc(CMDD_OP_GET, 4);
    SIMPLEQ_INSERT_TAIL(&cmdd->cmds, cmd, next);

    return cmdd;
}

static struct cmdd_out *
cmdd_run_alloc(struct cmdd *cmdd, struct cmdd_cmd *cmd)
{
    if (cmdd->cache)
        lru_cache_free(cmdd->cache);

    cmdd->cache = lru_cache_alloc(cmd->a.capacity);

    return cmdd_out_alloc("null");
}

static struct cmdd_out *
cmdd_run_put(struct cmdd *cmdd, struct cmdd_cmd *cmd)
{
    ASSERT(cmdd->cache);

    lru_cache_put(cmdd->cache, cmd->p.key, cmd->p.value);

    return cmdd_out_alloc("null");
}

static struct cmdd_out *
cmdd_run_get(struct cmdd *cmdd, struct cmdd_cmd *cmd)
{
    int value;

    ASSERT(cmdd->cache);

    value = lru_cache_get(cmdd->cache, cmd->g.key);

    return cmdd_out_alloc("%d", value);
}

void cmdd_run(struct cmdd *cmdd)
{
    struct cmdd_cmd *cmd;
    struct cmdd_out *out;

    SIMPLEQ_FOREACH(cmd, &cmdd->cmds, next) {
        out = cmd->ops->run(cmdd, cmd);
        SIMPLEQ_INSERT_TAIL(&cmdd->outs, out, next);
    }
}

static void cmdd_print_alloc(struct cmdd *cmdd, struct cmdd_cmd *cmd,
                             FILE *fp, enum cmdd_print_flag flag)
{
    UNUSED(cmdd);

    switch (flag) {
    case CMDD_PRINT_NAME:
        fprintf(fp, "\"LRUCache\"");
        break;
    case CMDD_PRINT_ARGV:
        fprintf(fp, "[%u]", cmd->a.capacity);
        break;
    default:
        ASSERT(0);
    }
}

static void cmdd_print_put(struct cmdd *cmdd, struct cmdd_cmd *cmd,
                             FILE *fp, enum cmdd_print_flag flag)
{
    UNUSED(cmdd);

    switch (flag) {
    case CMDD_PRINT_NAME:
        fprintf(fp, "\"put\"");
        break;
    case CMDD_PRINT_ARGV:
        fprintf(fp, "[%d, %d]", cmd->p.key, cmd->p.value);
        break;
    default:
        ASSERT(0);
    }
}

static void cmdd_print_get(struct cmdd *cmdd, struct cmdd_cmd *cmd,
                             FILE *fp, enum cmdd_print_flag flag)
{
    UNUSED(cmdd);

    switch (flag) {
    case CMDD_PRINT_NAME:
        fprintf(fp, "\"get\"");
        break;
    case CMDD_PRINT_ARGV:
        fprintf(fp, "[%d]", cmd->g.key);
        break;
    default:
        ASSERT(0);
    }
}

static void
cmdd_print_cmds(struct cmdd *cmdd, FILE *fp, enum cmdd_print_flag flag)
{
    struct cmdd_cmd *cmd;
    int next;

    fprintf(fp, "[");
    next = 0;
    SIMPLEQ_FOREACH(cmd, &cmdd->cmds, next) {
        if (next)
            fprintf(fp, ", ");
        else
            next = 1;
        cmd->ops->print(cmdd, cmd, fp, flag);
    }
    fprintf(fp, "]\n");
}

static void cmdd_print_outs(struct cmdd *cmdd, FILE *fp)
{
    struct cmdd_out *out;
    int next = 0;

    fprintf(fp, "[");
    SIMPLEQ_FOREACH(out, &cmdd->outs, next) {
        if (next)
            fprintf(fp, ", ");
        else
            next = 1;
        fprintf(fp, "%s", out->out);
    }
    fprintf(fp, "]\n");
}

void cmdd_print(struct cmdd *cmdd, FILE *fp)
{
    fprintf(fp, "Input\n");
    cmdd_print_cmds(cmdd, fp, CMDD_PRINT_NAME);
    cmdd_print_cmds(cmdd, fp, CMDD_PRINT_ARGV);

    fprintf(fp, "Output\n");
    cmdd_print_outs(cmdd, fp);
}
