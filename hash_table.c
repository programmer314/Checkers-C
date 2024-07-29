#include <stdlib.h>
#include <stdbool.h>

#include "hash_table.h"

struct HashTable newHashTable()
{
	struct HashTable hashTable;
	hashTable.key_count = 0;
	
	for (size_t i = 0; i < HASH_TABLE_SIZE; i++)
		hashTable.map[i] = NULL;
	
	return hashTable;
}

int hashCode(uint32_t key)
{
	return key % HASH_TABLE_SIZE;
}

uint32_t map_get(struct HashDataItem* hashArray[], uint32_t key)
{
	//get the hash 
	int hashIndex = hashCode(key);  

	//move in array until an empty 
	while(hashArray[hashIndex] != NULL)
	{
		if(hashArray[hashIndex]->key == key)
			return hashArray[hashIndex]->data; 
					
		//go to next cell
		++hashIndex;
				
		//wrap around the table
		hashIndex %= HASH_TABLE_SIZE;
	}        
		
	return 0;        
}

void hashTable_update(struct HashTable* hashTable, uint32_t key, uint32_t data)
{
	struct HashDataItem *item = malloc(sizeof(struct HashDataItem));
	item->data = data;  
	item->key = key;

	//get the hash 
	int hashIndex = hashCode(key);
	
	struct HashDataItem** hashArray = hashTable->map;

	//move in array until an empty or deleted cell
	while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1)
	{
		if(hashArray[hashIndex]->key == key)
			break;

		//go to next cell
		++hashIndex;
				
		//wrap around the table
		hashIndex %= HASH_TABLE_SIZE;
	}
	
	if (hashArray[hashIndex] == NULL || hashArray[hashIndex]->key == -1)
	{
		hashTable->keys[hashTable->key_count] = key;
		hashTable->key_count++;
	}
	
	hashArray[hashIndex] = item;
}

void map_free(struct HashDataItem* hashArray[], uint32_t key)
{
	//get the hash 
	int hashIndex = hashCode(key);  

	//move in array until an empty 
	while(hashArray[hashIndex] != NULL)
	{
		if(hashArray[hashIndex]->key == key)
		{
			free(hashArray[hashIndex]); 
			break;
		}

		//go to next cell
		++hashIndex;

		//wrap around the table
		hashIndex %= HASH_TABLE_SIZE;
	}    
}