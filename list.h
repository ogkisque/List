#ifndef LIST_HEADER
#define LIST_HEADER

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"

#define LIST_CTOR(list) \
        list_ctor (list, #list, __FILE__, __func__, __LINE__)

const int REALLOC_STEP = 2;
const int START_SIZE   = 10;
const int NAME_SIZE    = 50;

enum Error_codes
{
    CORRECT =           0,
    NULL_POINTER =      1,
    MEM_ALLOC =         2,
    INCOR_POS =         3,
    INCOR_ZERO_ELEM =   4
};

struct List
{
    int* data;
    int* next;
    int* prev;
    int head;
    int tail;
    int free;
    int size;
    const char* name;
    const char* file;
    const char* func;
    int line;
};

struct Error
{
    Error_codes code;
    int         line;
    const char* file;
    const char* func;
    const char* message;
};

Error list_insert (List* list, int value, int pos, int* pos_real);
Error print_real_list (List* list);
void print_error (Error error);
Error list_ctor (List* list, const char* name, const char* file, const char* func, int line);
Error list_dtor (List* list);
Error list_verify (List* list);
void list_dump (List* list, Error error);

#endif //LIST_HEADER
