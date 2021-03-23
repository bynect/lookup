#include <lookup.h>
#include <malloc.h>

/*
Hashed keys definitions
*/
void
lookup_key_init(lookup_key_t *key, uint8_t *raw, size_t len)
{
    key->raw = raw;
    key->len = len;

    /*
    FNV1a hash algorithm
    */

    key->hash = 2166136261u;

    for (size_t i = 0; i < len; ++i)
    {
        key->hash ^= raw[i];
        key->hash *= 16777619;
    }
}

/*
Lookup table helper functions
*/
static lookup_entry_t *
lookup_table_entry(lookup_entry_t *entries, uint32_t size, const lookup_key_t *key)
{
    lookup_entry_t *removed = NULL;

    for (uint32_t i = key->hash % size; ; i = (i + 1) % size)
    {
        lookup_entry_t *entry = entries + i;

        if (entry->key == NULL)
        {
            if (entry->ptr == INTPTR_MAX)
                return removed != NULL ? removed : entry;
            else if (removed == NULL)
                removed = entry;
        }
        else if (lookup_key_same(entry->key, key))
        {
            return entry;
        }
    }
}

static void
lookup_table_grow(lookup_table_t *table, uint32_t size)
{
    lookup_entry_t *entries = malloc(size * sizeof(lookup_entry_t));

    for (uint32_t i = 0; i < size; ++i)
    {
        entries[i].key = NULL;
        entries[i].ptr = INTPTR_MAX;
    }

    table->count = 0;

    for (uint32_t i = 0; i < table->size; ++i)
    {
        lookup_entry_t *entry = table->entries + i;
        if (entry->key == NULL)
            continue;

        lookup_entry_t *dest = lookup_table_entry(
            entries, size, entry->key
        );

        dest->key = entry->key;
        dest->ptr = entry->ptr;
        ++table->count;
    }

    free(table->entries);
    table->size = size;
    table->entries = entries;
}

/*
Lookup table definitions
*/
void
lookup_table_init(lookup_table_t *table, float load, float grow)
{
    table->size = 0;
    table->count = 0;
    table->load = load;
    table->grow = grow;
    table->entries = NULL;
}

void
lookup_table_free(lookup_table_t *table)
{
    free(table->entries);
    lookup_table_init(table, table->load, table->grow);
}

bool
lookup_table_insert(lookup_table_t *table, lookup_key_t *key, intptr_t ptr)
{
    if (table->count + 1 > table->size * table->load)
    {
        uint32_t new_size = table->size == 0 ? 8 : table->size * table->grow;
        lookup_table_grow(table, new_size);
    }

    lookup_entry_t *entry = lookup_table_entry(
        table->entries, table->size, key
    );

    bool newkey = entry->key == NULL;
    if (newkey && entry->ptr == INTPTR_MAX)
        ++table->count;

    entry->key = key;
    entry->ptr = ptr;
    return newkey;
}

bool
lookup_table_find(lookup_table_t *table, const lookup_key_t *key, intptr_t *ptr)
{
    if (table->count == 0)
        return false;

    lookup_entry_t *entry = lookup_table_entry(
        table->entries, table->size, key
    );

    if (entry->key == NULL)
        return false;

    *ptr = entry->ptr;
    return true;
}

bool
lookup_table_remove(lookup_table_t *table, const lookup_key_t *key)
{
    if (table->count == 0)
        return false;

    lookup_entry_t *entry = lookup_table_entry(
        table->entries, table->size, key
    );

    if (entry->key == NULL)
        return false;

    entry->key = NULL;
    entry->ptr = INTPTR_MIN;
    return true;
}

void
lookup_table_merge(lookup_table_t *dest, const lookup_table_t *src)
{
    for (uint32_t i = 0; i < src->size; ++i)
    {
        lookup_entry_t *entry = src->entries + i;
        if (entry->key != NULL)
            lookup_table_insert(dest, entry->key, entry->ptr);
    }
}
