#include "mmemory.h"

void initManagerParams(int n, size_t size)
{
	numberOfPages = n;
	pageSize = size;
	size_t tmp = size;
	bitsForPageSize = 0;

	numberOfBlocks = 0;
	currentBlocksSize = 0;
	interruptsNumber = 0;

	while (tmp > 1)
	{
		tmp >>= 1;
		bitsForPageSize++;
	}
}

void setInterrupts(unsigned int number)
{
	interruptsNumber = number;
}

unsigned int getInterrupts()
{
	return interruptsNumber;
}

int checkInitParameters(int n, size_t size)
{
	if (size & (size - 1)){
		return FALSE;
	}
	else return TRUE;
}

void initMemory()
{
	memory = (char*)malloc(sizeof(char)*PHYSICAL_PAGES_NUMBER*pageSize);
}

void initPageTable()
{
	pageTable = (page*)malloc(sizeof(page)*numberOfPages);

	for (int i = 0; i < numberOfPages; i++)
	{
		pageTable[i].offset = pageSize*i;
		pageTable[i].timesUsed = 0;

		if (i < PHYSICAL_PAGES_NUMBER)
		{
			pageTable[i].isUse = TRUE;
		}
		else pageTable[i].isUse = FALSE;
	}
}

void initSwap()
{
	swap = fopen("./swap.txt", "w+");
}

int _init(int n, size_t size)
{
	if (checkInitParameters(n, size) != TRUE) return -1;
	initManagerParams(n, size);
	initMemory();
	initPageTable();
	initSwap();

	return 0;
}

block * addSegmentToTable(size_t size)
{
	block * temp = NULL;
	currentBlocksSize += size;
	numberOfBlocks++;

	blockTable = (block*)realloc(blockTable, sizeof(block)*numberOfBlocks);

	temp = blockTable + numberOfBlocks - 1;
	temp->isUse = FALSE;
	temp->size = size;
	temp->number = numberOfBlocks - 1;
	temp->address = currentBlocksSize - size;

	return temp;
}

int _free(VA  * pointer)
{
	block * temp = (block*)pointer;
	if (temp == NULL || numberOfBlocks == 0 || pointer == NULL) return -1;
	temp->isUse = FALSE;

	blockTable[temp->number].isUse = FALSE;

	for (int i = temp->number; i < numberOfBlocks - 1; i++)
	{
		blockTable[i].address = blockTable[i + 1].address;
		blockTable[i].isUse = blockTable[i + 1].isUse;
		blockTable[i].size = blockTable[i + 1].size;
	}

	numberOfBlocks--;
	currentBlocksSize -= temp->size;

	blockTable = (block*)realloc(blockTable, numberOfBlocks*sizeof(block));
	temp = NULL;
	*pointer = (VA)temp;

	return 0;
}

int _malloc(VA * pointer, size_t size)
{
	if (currentBlocksSize + size <= MAX_VIRTUAL_MEMORY_SIZE)
	{
		block * temp = NULL;
		currentBlocksSize += size;
		numberOfBlocks++;

		blockTable = (block*)realloc(blockTable, sizeof(block)*numberOfBlocks);

		temp = blockTable + numberOfBlocks - 1;
		temp->isUse = FALSE;
		temp->size = size;
		temp->number = numberOfBlocks - 1;
		temp->address = currentBlocksSize - size;

		*pointer = (VA)temp;

		return 0;
	}
	else return -2;
}

int preparePage(int pageNumber)
{
	pageTable[pageNumber].timesUsed++;
	if (pageTable[pageNumber].isUse == TRUE) return;

	interruptsNumber++;

	pageTable[pageNumber].timesUsed = 0;

	int pageForSwapNumber = -2;
	unsigned long minTimes = -1;

	for (int i = 0; i < numberOfPages; i++)
	{
		if (minTimes > pageTable[i].timesUsed && pageTable[i].isUse == TRUE)
		{
			minTimes = pageTable[i].timesUsed;
			pageForSwapNumber = i;
			break;
		}
	}

	pageTable[pageNumber].offset = pageTable[pageForSwapNumber].offset;
	pageTable[pageForSwapNumber].offset = pageForSwapNumber * pageSize;

	fseek(swap, pageForSwapNumber*pageSize, SEEK_SET);
	fwrite(&memory[pageTable[pageNumber].offset], sizeof(char), pageSize, swap);

	fseek(swap, pageNumber*pageSize, SEEK_SET);
	fread(&memory[pageTable[pageNumber].offset], sizeof(char), pageSize, swap);

	pageTable[pageNumber].isUse = TRUE;
	pageTable[pageForSwapNumber].isUse = FALSE;
}

unsigned int pageVA(VA ptr)
{
	return ((unsigned int)ptr >> bitsForPageSize);
}

unsigned int offsetVA(VA ptr)
{
	return ((unsigned int)ptr & ((1 << bitsForPageSize) - 1));
}

void write(VA ptr, char value)
{
	preparePage(pageVA(ptr));
	memory[pageTable[pageVA(ptr)].offset + offsetVA(ptr)] = value;
}

int __write(int address, void * buffer, size_t size)
{
	char * cBuffer = (char*)buffer;

	for (int i = 0; i < size; i++)
	{
		write(address + i, cBuffer[i]);
	}

	return 0;
}

int _write(VA pointer, void * buffer, size_t size)
{
	char * cBuffer = (char*)buffer;
	unsigned long address = ((block*)pointer)->address;

	if (((block*)pointer)->size < size) return -2;

	for (int i = 0; i < size; i++)
	{
		write(address + i, cBuffer[i]);
	}

	return 0;
}

char read(VA ptr)
{
	preparePage(pageVA(ptr));
	return memory[pageTable[pageVA(ptr)].offset + offsetVA(ptr)];
}

int __read(int address, void * buffer, size_t size)
{
	char * cBuffer = (char*)buffer;

	for (int i = 0; i < size; i++)
	{
		cBuffer[i] = read(address + i);
	}

	return 0;
}

int _read(VA pointer, void * buffer, size_t size)
{
	char * cBuffer = (char*)buffer;
	unsigned long address = ((block*)pointer)->address;

	if (((block*)pointer)->size > size) return -2;

	for (int i = 0; i < size; i++){
		cBuffer[i] = read(address + i);
	}

	return 0;
}