#include "lookup.h"
#include <stdio.h>

int
main(int argc, char **argv)
{
    lookup_table_t t, t2, t3;
    lookup_table_init(&t, 0.75, 1.5);
    lookup_table_init(&t2, 0.75, 1.5);
    lookup_table_init(&t3, 0.75, 1.5);

    lookup_key_t keys[argc];

    for (int i = 0; i < argc; ++i)
    {
        lookup_key_init(keys + i, argv[i], strlen(argv[i]));
        lookup_table_insert(&t, keys + i, i + 100);
        lookup_table_insert(&t2, keys + i, i + 200);
    }

    lookup_table_merge(&t3, &t);
    lookup_table_merge(&t3, &t2);

    for (int i = 0; i < argc; ++i)
    {
        intptr_t v1, v2;
        lookup_table_find(&t, keys + i, &v1);
        lookup_table_find(&t2, keys + i, &v2);
        printf("%s == %ld && %ld\n", argv[i], v1, v2);
    }

    for (int i = 0; i < t3.count; ++i)
    {
        intptr_t val;
        lookup_table_find(&t3, keys + i, &val);
        printf("%s == %ld\n", argv[i], val);
    }
}
