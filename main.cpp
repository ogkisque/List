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

    list_pop_end    (&list, &pos_elem5);
    for (Iterator it = begin_it (&list); it.index != 0; it = next_it (it))
        printf ("%d ", ((it.list)->nodes)[it.index].value);

    list_dtor (&list);
    return 0;
}
