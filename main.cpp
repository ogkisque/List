#include "list.h"

int main ()
{
    List list = {};
    int pos_elem1 = 0;
    int pos_elem2 = 0;
    int pos_elem3 = 0;
    int pos_elem4 = 0;
    int pos_elem5 = 0;
    LIST_CTOR(&list);

    list_push_begin (&list, 55,  &pos_elem1);
    list_push_begin (&list, 78,  &pos_elem2);
    list_push_end   (&list, 123, &pos_elem3);
    list_push_begin (&list, 69,  &pos_elem4);
    for (Iterator it = begin_it (&list); it.index != 0; it = next_it (it))
        printf ("%d ", ((it.list)->nodes)[it.index].value);
    printf ("\n");

    Iterator it_end   = end_it   (&list);
    Iterator it_begin = begin_it (&list);
    Elemt val = 0;
    get_value (&it_end,   &val);
    set_value (&it_begin, val);

    list_pop_end    (&list, &pos_elem5);
    for (Iterator it = begin_it (&list); it.index != 0; it = next_it (it))
        printf ("%d ", ((it.list)->nodes)[it.index].value);
    printf ("\n");

    list_push_end (&list, 1,  &pos_elem1);
    list_push_end (&list, 2,  &pos_elem1);
    list_push_end (&list, 3,  &pos_elem1);
    list_push_end (&list, 4,  &pos_elem1);
    list_push_end (&list, 5,  &pos_elem1);
    Error error = list_push_end (&list, 6,  NULL);
    for (Iterator it = begin_it (&list); it.index != 0; it = next_it (it))
        printf ("%d ", ((it.list)->nodes)[it.index].value);
    printf ("\n");

    list_graph_dump (&list, error);
    list_dtor (&list);
    return 0;
}
