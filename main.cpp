#include "list.h"

void print_list (List* list);

int main ()
{
    List* list = NULL;
    MAKE_LIST(&list, 8, true, true);
    Iterator ret_it = {};

    list_push_begin (list, 55,  &ret_it);
    list_push_begin (list, 78,  &ret_it);
    list_push_end   (list, 123, &ret_it);
    list_push_begin (list, 69,  &ret_it);
    print_list (list);

    Iterator it_end   = end_it     (list);
    Iterator it_prev_end = prev_it (it_end);
    Iterator it_begin = begin_it   (list);
    Elemt val = 0;
    get_value (&it_prev_end,   &val);
    set_value (&it_begin, val);
    list_pop_end (list, &ret_it);
    print_list (list);

    for (int i = 1; i <= 10; i++)
        list_push_end (list, i, &ret_it);
    print_list (list);

    Iterator iter = search_value (list, 55);
    printf ("%lld\n", iter.index);

    for (int i = 0; i < 10; i++)
        list_pop_begin (list, &ret_it);
    print_list (list);

    list_dtor (list);
    return 0;
}

void print_list (List* list)
{
    for (Iterator it1 = begin_it (list), it2 = end_it (list);
        it1.index != it2.index;
        it1 = next_it (it1))
    {
        Elemt val = 0;
        get_value (&it1, &val);
        printf ("%d ", val);
    }
    printf ("\n");
}
