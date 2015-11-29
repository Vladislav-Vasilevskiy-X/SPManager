#ifndef MMEMORY_H_
#define MMEMORY_H_

#include<stdlib.h>
#include<stdio.h>

#define PAGE(X) ((unsigned int)X >> bitsForPageSize)
#define OFFSET(X) ((unsigned int)X & ((1 << bitsForPageSize)- 1)

const int TRUE = 0;
const int FALSE = 1;
const int PHYSICAL_PAGES_NUMBER = 14;
const int MAX_VIRTUAL_MEMORY_SIZE = 128;

typedef struct block {
	struct block * next;
	struct block * prev;
	size_t size;
	unsigned int address;
	char isUse;
} block;

typedef struct page {
	unsigned long offset;
	char isUse;
	unsigned long timesUsed;
} page;

//Virtual address
typedef char * VA;

char*memory = NULL;

unsigned int bitsForPageSize;

unsigned int numberOfPages;
size_t pageSize;
unsigned int bitsForPageSize;

page * pageTable = NULL;
block * firstBlock = NULL;

FILE * swap = NULL;

int _init(int n, int szPage);
int _malloc(VA* pointer, size_t size);
int _read(VA pointer, void * ptrBuffer, size_t bufferSize);
int _write(VA pointer, void * ptrBuffer, size_t bufferSize);
int _free(VA pointer);

#endif