#include "list.h"

int main ()
{
    List list = {};
    int pos_elem = 0;
    LIST_CTOR(&list);
    Error error = list_verify (&list);
    if (error.code != CORRECT)
        list_dump (&list, error);
    list_insert (&list, 55, 1, &pos_elem);
    list_insert (&list, 66, 1, &pos_elem);
    list_insert (&list, 77, 1, &pos_elem);
    list_dump (&list, error);
    print_real_list (&list);

    list_dtor (&list);
    return 0;
}
