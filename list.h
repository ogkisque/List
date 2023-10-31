#ifndef LIST_HEADER
#define LIST_HEADER

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colors.h"

typedef int Elemt;

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
    INCOR_ZERO_ELEM =   4,
    INCOR_LAST_ELEM =   5
};

struct Node
{
    Elemt value;
    int next;
    int prev;
};

struct List
{
    Node* nodes;
    int free;
    int size;
    int num_elems;
    const char* name;
    const char* file;
    const char* func;
    int line;
};

struct Iterator
{
    int index;
    List* list;
};

struct Error
{
    Error_codes code;
    int         line;
    const char* file;
    const char* func;
    const char* message;
};

Iterator prev_it (Iterator it);
Iterator next_it (Iterator it);
Iterator begin_it (List* list);
Iterator end_it (List* list);
Error list_insert (List* list, int value, int pos, int* pos_real);
Error list_erase (List* list, int pos, int* pos_real);
Error list_push_begin (List* list, Elemt value, int* pos_real);
Error list_push_end (List* list, Elemt value, int* pos_real);
Error list_pop_begin (List* list, int* pos_real);
Error list_pop_end (List* list, int* pos_real);

void print_error (Error error);
Error list_ctor (List* list, const char* name, const char* file, const char* func, int line);
Error list_dtor (List* list);
Error list_verify (List* list);
void list_dump (List* list, Error error);

#endif //LIST_HEADER
