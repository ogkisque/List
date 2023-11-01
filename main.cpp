#include "list.h"

void print_list (List* list);

int main ()
{
    List* list = NULL;
    MAKE_LIST(&list);
    int pos_elem = 0;

    list_push_begin (list, 55,  &pos_elem);
    list_push_begin (list, 78,  &pos_elem);
    list_push_end   (list, 123, &pos_elem);
    list_push_begin (list, 69,  &pos_elem);
    print_list (list);

    Iterator it_end   = end_it   (list);
    Iterator it_begin = begin_it (list);
    Elemt val = 0;
    get_value (&it_end,   &val);
    set_value (&it_begin, val);
    list_pop_end (list, &pos_elem);
    print_list (list);

    for (int i = 1; i <= 10; i++)
        list_push_end (list, i, &pos_elem);
    print_list (list);

    for (int i = 0; i < 10; i++)
        list_pop_begin (list, &pos_elem);
    print_list (list);

    list_dtor (list);
    return 0;
}

void print_list (List* list)
{
    for (Iterator it = begin_it (list); it.index != 0; it = next_it (it))
    {
        Elemt val = 0;
        get_value (&it, &val);
        printf ("%d ", val);
    }
    printf ("\n");
}
