#include "stdio.h"
#include "fibonacci.h"

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
    printf("hello world! %s id %d\n", __BASE_FILE__, get_file_id(__FILE__));
    printf("fibonacci file %s id %d\n", fibonacci_get_file_name(), get_file_id(fibonacci_get_file_name()));
    return 0;
}
