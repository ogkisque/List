#include "list.h"

#define RETURN_ERROR(code, message) \
        return Error {code, __LINE__, __FILE__, __func__, message}

#define PARSE_ERROR(error)          \
        if (error.code != CORRECT)  \
        {                           \
            print_error (error);    \
            return error;           \
        }

Error list_insert (List* list, int value, int pos, int* pos_real)
{
    Error error = list_verify (list);
    PARSE_ERROR(error);

    if (pos < 1 || list->prev[pos] == -1)
        RETURN_ERROR(INCOR_POS, "Incorrect position of inserted value.");

    list->data[list->free] = value;

    if (pos == list->tail)
    {
        int free_next = list->next[list->free];
        list->next[list->free] = 0;
        list->next[pos] = list->free;

        list->prev[list->free] = pos;

        list->tail = list->free;
        *pos_real = list->free;
        list->free = free_next;
        RETURN_ERROR(CORRECT, "");
    }

    if (pos == list->head)
    {
        int free_next = list->next[list->free];
        list->next[list->free] = 0;
        list->next[pos] = list->free;

        list->prev[list->free] = pos;

        list->head = list->free;
        *pos_real = list->free;
        list->free = free_next;
        RETURN_ERROR(CORRECT, "");
    }

    int free_next = list->next[list->free];
    list->next[list->free] = list->next[pos];
    list->next[pos] = list->free;

    list->prev[list->free] = pos;
    list->prev[list->next[list->free]] = list->free;

    *pos_real = list->free;
    list->free = free_next;
    RETURN_ERROR(CORRECT, "");
}

void print_error (Error error)
{
    printf ("Error: %s\n"
            "Code: %d\n"
            "File: %s, function: %s, line: %d\n",
            error.message,
            error.code,
            error.file, error.func, error.line);
}

Error print_real_list (List* list)
{
    Error error = list_verify (list);
    PARSE_ERROR(error);

    printf ("%d ", list->data[list->head]);
    for (int i = list->head; list->next[i] != 0; i++)
        printf ("%d ", list->data[list->next[i]]);
    printf ("\n");

    RETURN_ERROR(CORRECT, "");
}

Error list_ctor (List* list, const char* name, const char* file, const char* func, int line)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");

    list->size = START_SIZE;
    list->data = (int*) calloc (list->size, sizeof (int));
    list->next = (int*) calloc (list->size, sizeof (int));
    list->prev = (int*) calloc (list->size, sizeof (int));

    if (!(list->data) || !(list->next) ||!(list->prev))
        RETURN_ERROR(MEM_ALLOC, "Failed memory allocation.");

    for (int i = 1; i < list->size; i++)
    {
        list->next[i] = i + 1;
        list->prev[i] = -1;
    }

    list->data[0] = INT_MAX;
    list->prev[1] = 0;
    list->prev[list->size - 1] =
    list->free = 1;
    list->head = 1;
    list->tail = 1;
    list->name = name;
    list->file = file;
    list->func = func;
    list->line = line;
    RETURN_ERROR(CORRECT, "");
}

Error list_dtor (List* list)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");

    list->size = -1;
    free (list->data);
    free (list->next);
    free (list->prev);
    list->data = NULL;
    list->next = NULL;
    list->prev = NULL;
    list->free = -1;
    list->head = -1;
    list->tail = -1;
    RETURN_ERROR(CORRECT, "");
}

Error list_verify (List* list)
{
    if (!list)                                          RETURN_ERROR(NULL_POINTER,      "Null pointer of list.");
    if (!(list->data) || !(list->next) ||!(list->prev)) RETURN_ERROR(NULL_POINTER,      "Null pointer of arrays in list.");
    if (list->data[0] != INT_MAX)                       RETURN_ERROR(INCOR_ZERO_ELEM,   "Zero element in data is not INT_MAX.");

    RETURN_ERROR(CORRECT, "");
}

void list_dump (List* list, Error error)
{
    printf (RED_COL);
    printf ("-------------------------------------\n");
    printf ("Error in list: %s\n"
            "Called from file: %s, func: %s, line: %d\n",
            list->name, list->file, list->func, list->line);
    print_error (error);
    printf ("Size - %d\n", list->size);
    printf (YELLOW_COL);
    printf ("Data:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%d [%d] ", list->data[i], i);
    printf ("\n");
    printf (BLUE_COL);
    printf ("Next:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%d [%d] ", list->next[i], i);
    printf ("\n");
    printf (GREEN_COL);
    printf ("Prev:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%d [%d] ", list->prev[i], i);
    printf ("\n");
    printf (RED_COL);
    printf ("Head = %d, tail = %d, free = %d\n", list->head, list->tail, list->free);
    printf ("-------------------------------------\n");
    printf (OFF_COL);
}
