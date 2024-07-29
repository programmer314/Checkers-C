// https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>
#include <stddef.h>

#define HASH_TABLE_SIZE 32

struct HashTable {
	size_t key_count;
	uint32_t keys[HASH_TABLE_SIZE];
	struct HashDataItem* map[HASH_TABLE_SIZE];
};

struct HashDataItem {
   uint32_t data;   
   uint32_t key;
};

struct HashTable newHashTable();

uint32_t map_get(struct HashDataItem* hashTable[], uint32_t key);

void hashTable_update(struct HashTable* hashTable, uint32_t key, uint32_t data);

void map_free(struct HashDataItem* hashArray[], uint32_t key);

#endif