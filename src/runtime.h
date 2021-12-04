#pragma once

/**
 *
 * The goal of this structure is to contain
 * all the global data needed during the runtime
 *
 */

struct runtime
{
    int last_status; // is the return of the last execution
};

struct runtime *runtime_init();
void runtime_free(struct runtime *rt);
void runtime_set_status(struct runtime *rt, int status);
