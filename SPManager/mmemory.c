#include "mmemory.h";


/*typedef struct block {
	struct block* next;
	struct block* previous;
	int adress;
	unsigned int number;
	unsigned int size;
}block;

typedef struct page {
	struct page* next;
	struct page* previous;
	unsigned int number;
	char here;
	char free;
}page;

struct block * segmentsTable;
struct page * pagesTable;

unsigned int page_size;
unsigned int pages_number;

const int default_block_size;
const int default_blocks_number = 5;

void _printStructures();

void main() {
	_init(10, 4);
	_printStructures();
}

int _init(int n, int szPage){

	//create/clear swap file

	//check input data for correct
	page_size = szPage;
	pages_number = n;

	segmentsTable = (block*)malloc(sizeof(block)* default_blocks_number);
	pagesTable = (page*)malloc(sizeof(page)*pages_number);

	block * prev_block = NULL;

	for (int i = 0; i < default_blocks_number; i++){

		//init every block;
		(segmentsTable + i)->previous = prev_block;
		(segmentsTable + i)->number = i;
		(segmentsTable + i)->size = page_size;
		(segmentsTable + i)->adress = (int)(segmentsTable + i);

		if (i + 1 < default_blocks_number) {
			(segmentsTable + i)->next = (segmentsTable + i + 1);
		}
		else {
			(segmentsTable + i)->next = NULL;
		}

		prev_block = (segmentsTable + i);
	}

	page * prev_page = NULL;

	for (int i = 0; i < pages_number; i++){

		//init every page;
		(pagesTable + i)->here = TRUE;
		(pagesTable + i)->previous = prev_page;
		(pagesTable + i)->number = i;
		(pagesTable + i)->free = TRUE;

		if (i + 1 < pages_number) {
			(pagesTable + i)->next = (pagesTable + i + 1);
		}
		else {
			(pagesTable + i)->next = NULL;
		}

		prev_page = (pagesTable + i);
	}
}
*/

void initManagerParams(int n, size_t size) {

	numberOfPages = n;
	pageSize = size;
	size_t tmp = size;
	bitsForPageSize = 0;

	while (tmp > 1) {
		tmp >>= 1;
		bitsForPageSize++;
	}
}

void initMemory() {
	memory = (char*)malloc(sizeof(char)*PHYSICAL_PAGES_NUMBER*pageSize);
}

void initPageTable() {

	pageTable = (page*)malloc(sizeof(page)*numberOfPages);

	for (int i = 0; i < numberOfPages; i++) {
		pageTable[i].offset = pageSize*i;
		pageTable[i].timesUsed = 0;

		if (i < PHYSICAL_PAGES_NUMBER) {
			pageTable[i].isUse = TRUE;
		}
		else pageTable[i].isUse = FALSE;

	}
}

void initSwap() {
	fopen_s(&swap, "./swap.dat", "wb+");
}

void initFirstBlock() {
	firstBlock = (block*)malloc(sizeof(block));

	firstBlock->next = NULL;
	firstBlock->prev = NULL;
	firstBlock->isUse = FALSE;
	firstBlock->size = MAX_VIRTUAL_MEMORY_SIZE;
	firstBlock->address = 0;
}

/*int validateParams() {

}*/

int _init(int n, size_t size) {

	//validate params


	//manager params
	initManagerParams(n, size);

	//init mem
	initMemory();

	//init pageTable
	initPageTable();
	
	//swap
	initSwap();

	//block
	initFirstBlock();
}

int _malloc(VA * pointer, size_t size) {
	block * temp = firstBlock;

	while (temp != NULL) {
		if (temp->size >= size && temp->isUse == FALSE) break;
		temp = temp->next;
	}

	if (temp == NULL) return -2;

	int part = temp->size - size;
	temp->size = size;
	temp->isUse = TRUE;

	block * newBlock = (block*)malloc(sizeof(block));
	newBlock->address = temp->address + size;
	newBlock->isUse = FALSE;
	newBlock->next = temp->next;
	newBlock->prev = temp;
	temp->next = newBlock;

	*pointer = temp->address;

	return 0;
}

int _free(VA pointer) {
	block * temp = firstBlock;

	while (temp != NULL) {
		if (temp->address == pointer) break;
		temp = temp->next;
	}

	if (temp == NULL || temp->isUse == TRUE) return -1;
	temp->isUse = FALSE;

	if (temp->next != NULL && temp->next->isUse == FALSE) {
		temp->size += temp->next->size;
		temp->next = temp->next->next;
	}

	if (temp->prev != NULL && temp->prev->isUse == FALSE) {
		temp->prev->size += temp->size;
		temp->prev->next = temp->next;
	}

	return 0;
}

int preparePage(int pageNumber) {

	pageTable[pageNumber].timesUsed++;
	if (pageTable[pageNumber].isUse == TRUE) return;

	pageTable[pageNumber].timesUsed = 0;

	int pageForSwapNumber;
	unsigned long minTimes = -1;

	for (int i = 0; i < numberOfPages; i++) {
		if (minTimes > pageTable[i].timesUsed && pageTable[i].isUse == TRUE) {
			minTimes = pageTable[i].timesUsed;
			pageForSwapNumber = i;
			break;
		}
	}
	
	pageTable[pageNumber].offset = pageTable[pageForSwapNumber].offset;
	pageTable[pageForSwapNumber].offset = pageForSwapNumber * pageSize;

	fseek(swap, pageForSwapNumber*pageSize, SEEK_SET);
	fwrite(&memory[pageTable[pageSize].offset], sizeof(char), pageSize, swap);

	fseek(swap, pageNumber*pageSize, SEEK_SET);
	fread(&memory[pageTable[pageNumber].offset], sizeof(char), pageSize, swap);

	pageTable[pageNumber].isUse = TRUE;
	pageTable[pageForSwapNumber].isUse = FALSE;
}

void write(VA ptr, char value) {
	preparePage(PAGE(ptr));
	memory[pageTable[PAGE(ptr)].offset + OFFSET(ptr))] = value;
}

int _write(VA pointer, void * buffer, size_t size) {
	char * cBuffer = (char*)buffer;
	for (int i = 0; i < size; i++){
		write(pointer + i, cBuffer[i]);
	}
}

char read(VA ptr) {
	preparePage(PAGE(ptr));
	return memory[pageTable[PAGE(ptr)].offset + OFFSET(ptr))];
}

int _read(VA pointer, void * buffer, size_t size) {
	char * cBuffer = (char*)buffer;
	for (int i = 0; i < size; i++){
		cBuffer[i] = read(pointer + i);
	}
}

void main() {
	VA address;

	const int size = 120;

	char buffer[120];
	char tBuf[120];

	_init(20, 3);
	_malloc(&address, size);

	for (int i = 0; i < size; i++) {
		buffer[i] = 'w';
	}

	_write(address, buffer, size);
	_read(address, tBuf, size);

	for (int i = 0; i < size; i++) {
		printf("%c", tBuf[i]);
	}
}

/*
void _printStructures(){

	printf("Segments table.\n");
	for (int i = 0; i < default_blocks_number; i++) {
		printf("block's adress: %d\n", segmentsTable[i]);
		printf("	next: %d\n", (segmentsTable + i)->next);
		printf("	prev: %d\n", (segmentsTable + i)->previous);
		printf("	size: %d\n", (segmentsTable + i)->size);
		printf("	number: %d\n", (segmentsTable + i)->number);

	}

	printf("Pages table.\n");
	for (int i = 0; i < pages_number; i++) {
		printf("page's adress: %d\n", (pagesTable + i));
		printf("	next: %d\n", (pagesTable + i)->next);
		printf("	prev: %d\n", (pagesTable + i)->previous);
		printf("	here: %d\n", (pagesTable + i)->here);
		printf("	free: %d\n", (pagesTable + i)->free);
		printf("	number: %d\n", (pagesTable + i)->number);
	}
}*/