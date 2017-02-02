#include "stdio.h"
#include "fibonacci.h"
#include "trace_log.h"

typedef struct _tc_1
{
    int id;
    char tag;
    char* name;
} TC_1;

struct file_table
{
    char* name;
    int     id;
};

struct file_table tbl[]={
    { "main.c", 0 },
    { "../../src/fibonacci.c", 1}
};

int get_file_id(char* name)
{
    int i;
    for(i = 0; i < sizeof(tbl) / sizeof(struct file_table); i++)
    {
        if (0 == strcmp(tbl[i].name, name))
        {
            return tbl[i].id;
        }
    }
    return -1;
}

int main(int argc, char* argv[])
{
    TC_1 tc1 = { 100, 2, NULL};

    printf("hello world! %s id %d\n", __BASE_FILE__, get_file_id(__FILE__));
    printf("fibonacci file %s id %d\n", fibonacci_get_file_name(), get_file_id(fibonacci_get_file_name()));
    TestFibonacciSeq();

    TRACE_LOG(&tc1, sizeof(TC_1), TC_1, id,  "try to output tc1");
    return 0;
}
