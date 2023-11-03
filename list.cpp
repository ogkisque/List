#include "list.h"
#include "Dotter.h"

#define RETURN_ERROR(code, message) \
        return Error {code, __LINE__, __FILE__, __func__, message}

#define RETURN_ERROR_AND_DUMP(list, code, message)                                          \
        {                                                                                   \
            list_dump (list, Error {code, __LINE__, __FILE__, __func__, message});          \
            list_graph_dump (list, Error {code, __LINE__, __FILE__, __func__, message});    \
            return Error {code, __LINE__, __FILE__, __func__, message};                     \
        }

#define PARSE_ERROR(list, error)            \
        if (error.code != CORRECT)          \
        {                                   \
            list_dump (list, error);        \
            list_graph_dump (list, error);  \
            return error;                   \
        }

const char* NAME_DOT        = "pic.dot";
const int   MAX_TEXT_SIZE   = 200;
const int   REALLOC_STEP    = 2;
const int   START_SIZE      = 8;
const int   NAME_SIZE       = 50;

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

Error   list_realloc_increase   (List* list);
Error   list_realloc_decrease   (List* list);
Error   list_verify             (List* list);
void    list_dump               (List* list, Error error);
void    print_error             (Error error);
void    list_graph_dump         (List* list, Error error);
void    dump_nodes              (List* list);
void    dump_links              (List* list);
void    dump_error              (List* list, Error error);
bool    prev_of_next            (List* list);
bool    is_cycles               (List* list);

Error list_insert (List* list, Elemt value, int pos, int* pos_real)
{
    Error error = list_verify (list);
    PARSE_ERROR(list, error);

    if (!pos_real)
        RETURN_ERROR_AND_DUMP(list, NULL_POINTER, "Null pointer of pos_real.");

    if (pos < 0 || pos > list->size - 1 || ((list->nodes[pos].prev == -1) && pos != 0))
        RETURN_ERROR_AND_DUMP(list, INCOR_POS, "Incorrect position.");

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
        error = list_realloc_increase (list);
        PARSE_ERROR(list, error);
    }

    RETURN_ERROR(CORRECT, "");
}

Error list_erase (List* list, int pos, int* pos_real)
{
    Error error = list_verify (list);
    PARSE_ERROR(list, error);

    if (!pos_real)
        RETURN_ERROR_AND_DUMP(list, NULL_POINTER, "Null pointer of pos_real.");

    if (pos <= 0 || list->nodes[pos].prev == -1 || pos >= list->size)
        RETURN_ERROR_AND_DUMP(list, INCOR_POS, "Incorrect position.");

    *pos_real = list->nodes[pos].next;
    list->nodes[pos].value = 0;
    list->nodes[list->nodes[pos].prev].next = list->nodes[pos].next;
    list->nodes[list->nodes[pos].next].prev = list->nodes[pos].prev;

    list->nodes[pos].next = list->free;
    list->nodes[pos].prev = -1;
    list->free = pos;
    list->num_elems--;

    if (list->num_elems < list->size / REALLOC_STEP / REALLOC_STEP)
    {
        error = list_realloc_decrease (list);
        PARSE_ERROR(list, error);
    }
    RETURN_ERROR(CORRECT, "");
}

Error list_realloc_increase (List* list)
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

Error list_realloc_decrease (List* list)
{
    List* old_list = (List*) calloc (1, sizeof (List));
    old_list->nodes = (Node*) calloc (list->size, sizeof (Node));
    old_list->size = list->size;
    memcpy (old_list->nodes, list->nodes, list->size * sizeof (Node));

    list->size =  list->size / REALLOC_STEP;
    list->nodes = (Node*) realloc (list->nodes, list->size * sizeof (Node));
    int num = 0;
    for (int i = old_list->nodes[0].next; i != 0; i = old_list->nodes[i].next)
    {
        num++;
        list->nodes[num].value =    old_list->nodes[i].value;
        list->nodes[num].prev =     num - 1;
        list->nodes[num].next =     num + 1;
    }

    list->nodes[num].next = 0;
    list->nodes[0].next   = 1;
    list->nodes[0].prev   = num;
    list->free            = num + 1;

    for (int i = num + 1; i < list->size; i++)
    {
        list->nodes[i].next = i + 1;
        list->nodes[i].prev = -1;
    }

    free (old_list);
    RETURN_ERROR(CORRECT, "");
}

Iterator search_value (List* list, Elemt value)
{
    Iterator ans = {-1, list};
    for (Iterator it = begin_it (list); it.index != 0; it = next_it (it))
    {
        Elemt val = 0;
        get_value (&it, &val);
        if (val == value)
        {
            ans = it;
            break;
        }
    }
    return ans;
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
        RETURN_ERROR_AND_DUMP(it->list, INCOR_POS, "Incorrect index.");

    if (!value)
        RETURN_ERROR_AND_DUMP(it->list, NULL_POINTER, "Null pointer of value.");

    if (!it)
        RETURN_ERROR_AND_DUMP(it->list, NULL_POINTER, "Null pointer of iterator.");

    *value = it->list->nodes[it->index].value;
    RETURN_ERROR(CORRECT, "");
}

Error set_value (Iterator* it, Elemt value)
{
    if (it->index <= 0 || it->index >= it->list->size || it->list->nodes[it->index].prev == -1)
        RETURN_ERROR_AND_DUMP(it->list, INCOR_POS, "Incorrect index.");

    if (!it)
        RETURN_ERROR_AND_DUMP(it->list, NULL_POINTER, "Null pointer of iterator.");

    it->list->nodes[it->index].value = value;
    RETURN_ERROR(CORRECT, "");
}

Error list_push_begin (List* list, Elemt value, int* pos_real)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");
    return list_insert (list, value, list->nodes[0].next, pos_real);
}

Error list_push_end (List* list, Elemt value, int* pos_real)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");
    return list_insert (list, value, 0, pos_real);
}

Error list_pop_begin (List* list, int* pos_real)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");
    return list_erase (list, list->nodes[0].next, pos_real);
}

Error list_pop_end (List* list, int* pos_real)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of list.");
    return list_erase (list, list->nodes[0].prev, pos_real);
}

Error make_list (List** list, const char* name, const char* file, const char* func, int line)
{
    if (!list)
        RETURN_ERROR(NULL_POINTER, "Null pointer of pointer of list.");

    *list = (List*) calloc (1, sizeof (List));

    if (!list)
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory of list.");

    Error error = list_ctor (*list, name, file, func, line);
    return error;
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
    if (prev_of_next (list))                            RETURN_ERROR(PREV_OF_NEXT,      "Previous element of next element is not equal to the current one");
    if (is_cycles (list))                               RETURN_ERROR(CYCLES,            "There are cycles in list.");
    if (list->num_elems >= list->size)                  RETURN_ERROR(ELEMS_MORE_SIZE,   "There are more elements in the list than its size");
    RETURN_ERROR(CORRECT, "");
}

bool prev_of_next (List* list)
{
    for (int i = 0; i < list->size; i++)
        if (list->nodes[i].prev != -1)
            if (list->nodes[list->nodes[i].next].prev != i)
                return true;
    return false;
}

bool is_cycles (List* list)
{
    int kol = 0;
    for (int i = list->nodes[0].next; i != 0; i = list->nodes[i].next)
        kol++;
    return kol != list->num_elems;
}

void list_dump (List* list, Error error)
{
    printf (RED_COL);
    printf ("-------------------------------------\n");
    if (error.code != CORRECT)
    {
        print_error (error);
        printf ("Error in list: %s\n"
                "Called from file: %s, func: %s, line: %d\n",
                list->name, list->file, list->func, list->line);
    }
    printf ("Size - %d\n"
            "Number of elements - %d\n",
            list->size, list->num_elems);
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

void list_graph_dump (List* list, Error error)
{
    dtBegin (NAME_DOT);
    dump_nodes (list);
    dump_links (list);
    if (error.code != CORRECT)
        dump_error (list, error);
    dtEnd ();
    dtRender (NAME_DOT);
}

void dump_nodes (List* list)
{
    char text[MAX_TEXT_SIZE] = "";
    dtNodeStyle ().shape        ("box")
                  .style        ("rounded, filled")
                  .fontcolor    ("black");

    for (int i = 0; i < list->size; i++)
    {
        sprintf (text, "Index: [%d]\n"
                       "Value: %d\n"
                       "Prev:  [%d]\n"
                       "Next:  [%d]\n",
                       i, list->nodes[i].value, list->nodes[i].prev, list->nodes[i].next);
        if (list->nodes[i].prev == -1)
            dtNodeStyle ().fillcolor ("#FFA07A");
        else if (i == 0)
            dtNodeStyle ().fillcolor ("#FFD700");
        else
            dtNodeStyle ().fillcolor ("#90EE90");

        dtNode (i, text);
    }
}

void dump_links (List* list)
{
    dtLinkStyle ().style ("bold")
                  .color ("#4682B4");
    for (int i = 0; i < list->size; i++)
    {
        dtLink (i, list->nodes[i].next, "");
    }

    dtLinkStyle ().style ("dashed")
                  .color ("#00FFFF");
    for (int i = 0; i < list->size; i++)
    {
        if (list->nodes[i].prev != -1)
        {
            dtLink (i, list->nodes[i].prev, "");
        }
    }
}

void dump_error (List* list, Error error)
{
    char text[MAX_TEXT_SIZE] = "";
    dtNodeStyle ().shape        ("box")
                  .style        ("rounded, filled")
                  .fontcolor    ("black")
                  .fillcolor    ("#FFFF00");

    sprintf (text,
            "Error in list: %s\n"
            "Called from file: %s, func: %s, line: %d\n"
            "Error: %s\n"
            "Code: %d\n"
            "File: %s, function: %s, line: %d\n",
            list->name, list->file, list->func, list->line,
            error.message, error.code, error.file, error.func, error.line);

    dtNode (list->size + 1, text);
}
