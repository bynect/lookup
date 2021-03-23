#ifndef LOOKUP_H_
#define LOOKUP_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/*
Hashed keys declarations
*/
typedef struct
{
    uint8_t *raw;
    size_t len;
    uint32_t hash;
} lookup_key_t;

void lookup_key_init(lookup_key_t *key, uint8_t *raw, size_t len);

static inline bool
lookup_key_same(const lookup_key_t *key1, const lookup_key_t *key2)
{
    return (key1->hash == key2->hash)
        && (key1->len == key2->len)
        && (memcmp(key1->raw, key2->raw, key1->len) == 0);
}

/*
Lookup entry declarations
*/
typedef struct
{
    lookup_key_t *key;
    intptr_t ptr;
} lookup_entry_t;

/*
Lookup table declarations
*/
typedef struct
{
    uint32_t size, count;
    float load, grow;
    lookup_entry_t *entries;
} lookup_table_t;

void lookup_table_init(lookup_table_t *table, float load, float grow);

void lookup_table_free(lookup_table_t *table);

bool lookup_table_insert(lookup_table_t *table, lookup_key_t *key, intptr_t ptr);

bool lookup_table_find(lookup_table_t *table, const lookup_key_t *key, intptr_t *ptr);

bool lookup_table_remove(lookup_table_t *table, const lookup_key_t *key);

void lookup_table_merge(lookup_table_t *dest, const lookup_table_t *src);

/*
Convenience helpers
*/
#define DEFAULT_LOAD 0.75
#define DEFAULT_GROW 1.5
#define lookup_table_default(table) lookup_table_init(table, DEFAULT_LOAD, DEFAULT_GROW)

static inline bool
lookup_table_insert_raw(lookup_table_t *table, uint8_t *raw, size_t len, intptr_t ptr)
{
    lookup_key_t key;
    lookup_key_init(&key, raw, len);
    return lookup_table_insert(table, &key, ptr);
}

static inline bool
lookup_table_find_raw(lookup_table_t *table, uint8_t *raw, size_t len, intptr_t *ptr)
{
    lookup_key_t key;
    lookup_key_init(&key, raw, len);
    return lookup_table_find(table, &key, ptr);
}

static inline bool
lookup_table_remove_raw(lookup_table_t *table, uint8_t *raw, size_t len)
{
    lookup_key_t key;
    lookup_key_init(&key, raw, len);
    return lookup_table_remove(table, &key);
}

#endif /* LOOKUP_H_ */
