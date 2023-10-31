#include "list.h"

#define RETURN_ERROR(code, message) \
        return Error {code, __LINE__, __FILE__, __func__, message}

#define PARSE_ERROR(error)          \
        if (error.code != CORRECT)  \
        {                           \
            print_error (error);    \
            return error;           \
        }

Error list_insert (List* list, Elemt value, int pos, int* pos_real)
{
    Error error = list_verify (list);
    PARSE_ERROR(error);

    if (!pos_real)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pos_real.");

    if (pos < 0 || pos > list->size - 1 || ((list->nodes[pos].prev == -1) && pos != 0))
        RETURN_ERROR(INCOR_POS, "Incorrect position.");

    int new_next = (list->nodes)[list->free].next;

    list->nodes[list->free].value = value;

    list->nodes[list->free].next = pos;
    list->nodes[list->free].prev = list->nodes[pos].prev;
    list->nodes[list->nodes[pos].prev].next = list->free;
    list->nodes[pos].prev = list->free;

    *pos_real = list->free;
    list->free = new_next;
    list->num_elems++;

    if (list->num_elems == list->size - 1)
    {
        error = list_realloc (list);
        PARSE_ERROR(error);
    }

    RETURN_ERROR(CORRECT, "");
}

Error list_erase (List* list, int pos, int* pos_real)
{
    Error error = list_verify (list);
    PARSE_ERROR(error);

    if (!pos_real)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pos_real.");

    if (pos <= 0 || list->nodes[pos].prev == -1 || pos >= list->size)
        RETURN_ERROR(INCOR_POS, "Incorrect position.");

    *pos_real = list->nodes[pos].next;
    list->nodes[pos].value = 0;
    list->nodes[list->nodes[pos].prev].next = list->nodes[pos].next;
    list->nodes[list->nodes[pos].next].prev = list->nodes[pos].prev;

    list->nodes[pos].next = list->free;
    list->nodes[pos].prev = -1;
    list->free = pos;
    list->num_elems--;
    RETURN_ERROR(CORRECT, "");
}

Error list_realloc (List* list)
{
    Node* new_nodes = NULL;
    new_nodes = (Node*) realloc (list->nodes, list->size * REALLOC_STEP * sizeof (Node));
    if (!list->nodes)
        RETURN_ERROR(MEM_ALLOC, "Error of reallocation memory of nodes.");

    list->nodes = new_nodes;
    list->size = list->size * REALLOC_STEP;
    for (int i = list->size / REALLOC_STEP; i < list->size; i++)
    {
        list->nodes[i].prev = -1;
        list->nodes[i].next = i + 1;
        list->nodes[i].value = 0;
    }

    RETURN_ERROR(CORRECT, "");
}

Iterator prev_it (Iterator it)
{
    return Iterator {((it.list)->nodes)[it.index].prev, it.list};
}

Iterator next_it (Iterator it)
{
    return Iterator {((it.list)->nodes)[it.index].next, it.list};
}

Iterator begin_it (List* list)
{
    return Iterator {list->nodes[0].next, list};
}

Iterator end_it (List* list)
{
    return Iterator {list->nodes[0].prev, list};
}

Error get_value (Iterator* it, Elemt* value)
{
    if (it->index <= 0 || it->index >= it->list->size || it->list->nodes[it->index].prev == -1)
        RETURN_ERROR(INCOR_POS, "Incorrect index.");

    if (!value)
        RETURN_ERROR(NULL_POINTER, "Null pointer of value.");

    if (!it)
        RETURN_ERROR(NULL_POINTER, "Null pointer of iterator.");

    *value = it->list->nodes[it->index].value;
    RETURN_ERROR(CORRECT, "");
}

Error set_value (Iterator* it, Elemt value)
{
    if (it->index <= 0 || it->index >= it->list->size || it->list->nodes[it->index].prev == -1)
        RETURN_ERROR(INCOR_POS, "Incorrect index.");

    if (!it)
        RETURN_ERROR(NULL_POINTER, "Null pointer of iterator.");

    it->list->nodes[it->index].value = value;
    RETURN_ERROR(CORRECT, "");
}

Error list_push_begin (List* list, Elemt value, int* pos_real)
{
    if (list->num_elems == 0)
        return list_insert (list, value, 0, pos_real);

    return list_insert (list, value, list->nodes[0].next, pos_real);
}

Error list_push_end (List* list, Elemt value, int* pos_real)
{
    return list_insert (list, value, 0, pos_real);
}

Error list_pop_begin (List* list, int* pos_real)
{
    return list_erase (list, list->nodes[0].next, pos_real);
}

Error list_pop_end (List* list, int* pos_real)
{
    return list_erase (list, list->nodes[0].prev, pos_real);
}

Error list_ctor (List* list, const char* name, const char* file, const char* func, int line)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");

    list->size = START_SIZE;
    list->nodes = (Node*) calloc (list->size, sizeof (Node));

    if (!(list->nodes))
        RETURN_ERROR(MEM_ALLOC, "Failed memory allocation of nodes");

    for (int i = 1; i < list->size; i++)
    {
        list->nodes[i].next = i + 1;
        list->nodes[i].prev = -1;
    }

    list->nodes[0].value =  INT_MAX;
    list->nodes[0].next =   0;
    list->nodes[0].prev =   0;
    list->free =            1;
    list->num_elems =       0;
    list->name =            name;
    list->file =            file;
    list->func =            func;
    list->line =            line;
    RETURN_ERROR(CORRECT, "");
}

Error list_dtor (List* list)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");

    free (list->nodes);
    list->nodes =       NULL;
    list->size =        -1;
    list->free =        -1;
    list->num_elems =   -1;
    RETURN_ERROR(CORRECT, "");
}

Error list_verify (List* list)
{
    if (!list)                                          RETURN_ERROR(NULL_POINTER,      "Null pointer of list.");
    if (!(list->nodes))                                 RETURN_ERROR(NULL_POINTER,      "Null pointer of nodes.");
    if (list->nodes[0].value != INT_MAX)                RETURN_ERROR(INCOR_ZERO_ELEM,   "Zero element in nodes is not INT_MAX.");

    RETURN_ERROR(CORRECT, "");
}

void list_dump (List* list, Error error)
{
    printf (RED_COL);
    printf ("-------------------------------------\n");
    if (error.code != CORRECT)
    {
        printf ("Error in list: %s\n"
                "Called from file: %s, func: %s, line: %d\n",
                list->name, list->file, list->func, list->line);
        print_error (error);
    }
    printf ("Size - %d\n", list->size);
    printf (YELLOW_COL);
    printf ("Data:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%d [%d] ", list->nodes[i].value, i);
    printf ("\n");
    printf (BLUE_COL);
    printf ("Next:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%d [%d] ", list->nodes[i].next, i);
    printf ("\n");
    printf (GREEN_COL);
    printf ("Prev:\n");
    for (int i = 0; i < list->size; i++)
        printf ("%d [%d] ", list->nodes[i].prev, i);
    printf ("\n");
    printf (RED_COL);
    printf ("Free = %d\n", list->free);
    printf ("-------------------------------------\n");
    printf (OFF_COL);
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
