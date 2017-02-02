#include <stdio.h>
#include "trace_log.h"
#include "cfiles_table.h"

static int comp_file_name(const void* m1, const void* m2)
{
    CFILES_DESC* c1 = (CFILES_DESC*)m1;
    CFILES_DESC* c2 = (CFILES_DESC*)m2;
    return strcmp(c1->file_name, c2->file_name);
}

void tl_trace_log(char* file_name, int linenum, void* p, int size)
{
    CFILES_DESC* res;
    CFILES_DESC key;
    key.file_name = file_name;
    res = (CFILES_DESC*)bsearch((const void *)&key, (const void *)cfiles_table, 4, sizeof(CFILES_DESC), comp_file_name);
    if (NULL == res)
    {
    printf("ERROR: [%s:%d] %p %d\n", file_name, linenum, p, size);
    }
    else
    {
    printf("[%s:%d:%d] %p %d\n", file_name, res->file_id, linenum, p, size);
    }
}

