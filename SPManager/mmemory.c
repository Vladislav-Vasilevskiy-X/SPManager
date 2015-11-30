#include "mmemory.h"

void initManagerParams(int n, size_t size)
{
	numberOfPages = n;
	pageSize = size;
	size_t tmp = size;
	bitsForPageSize = 0;

	while (tmp > 1)
	{
		tmp >>= 1;
		bitsForPageSize++;
	}
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

void initFirstBlock()
{
	blockTable = (block*)malloc(sizeof(block));


	//firstBlock = (block*)malloc(sizeof(block));
	//
	////Инициализация
	//firstBlock->next = NULL;
	//firstBlock->prev = NULL;
	//firstBlock->isUse = FALSE;
	//firstBlock->size = MAX_VIRTUAL_MEMORY_SIZE;
	//firstBlock->address = 0;
}

MMEMORY_API int _init(int n, size_t size)
{
	//Проверка входных параметров
	if (checkInitParameters(n, size) != TRUE) return -1;

	//manager params
	initManagerParams(n, size);

	//init mem
	initMemory();

	//init pageTable
	initPageTable();

	//swap
	initSwap();

	//block
	//initFirstBlock();

	return 0;
}

block * addSegmentToTable(size_t size)
{
	block * temp = NULL;
	currentBlocksSize += size;
	numberOfBlocks++;

	//TEMP AND NULL
	blockTable = (block*)realloc(blockTable, sizeof(block)*numberOfBlocks);

	temp = blockTable + numberOfBlocks - 1;

	temp->isUse = FALSE;
	temp->size = size;
	temp->number = numberOfBlocks - 1;
	temp->address = currentBlocksSize - size;

	return temp;
}


MMEMORY_API int _free(VA pointer)
{

	block * temp = (block*)pointer;
	if (temp == NULL) return -1;
	temp->isUse = FALSE;

	if (numberOfBlocks == 0)
	{
		return -1;
	}

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

	//block * temp = (block*)pointer;

	//if (temp == NULL || temp->isUse == FALSE) return -1;
	//temp->isUse = FALSE;

	////Если след блок пустой - слияние
	//if (temp->next != NULL && temp->next->isUse == FALSE)
	//{
	//	temp->size += temp->next->size;
	//	temp->next = temp->next->next;
	//}
	//
	////Если пред блок пустой - слияние
	//if (temp->prev != NULL && temp->prev->isUse == FALSE)
	//{
	//	temp->prev->size += temp->size;
	//	temp->prev->next = temp->next;
	//}

	//return 0;
}


MMEMORY_API int _malloc(VA * pointer, size_t size)
{
	block * temp = NULL;

	for (int i = 0; i < numberOfBlocks; i++)
	{
		if (blockTable[i].isUse = FALSE && blockTable[i].size > size)
		{
			_free((VA)(blockTable+i));
			*pointer = (VA)addSegmentToTable(size);
			return 0;
		}
	}

	if (currentBlocksSize + size <= MAX_VIRTUAL_MEMORY_SIZE)
	{
		*pointer = (VA)addSegmentToTable(size);
		return 0;
	}
	else return -2;

//	block * temp = blockTable[0];
//
//	//Поиск свободного блока с нужным размером
//	while (temp != NULL)
//	{
//		if (temp->size >= size && temp->isUse == FALSE) break;
//		temp = temp->next;
//	}
//
//	//Недостаточно памяти
//	if (temp == NULL)
//	{
//		return -2;
//	}
//
//	int part = temp->size - size;
//	temp->size = size;
//	temp->isUse = TRUE;
//	
//	if (size == 0)
//	{
//		return 0;
//	}
//
//	//Текущий блок - блок, на которого указывает
//	//виртуальный адрес, а следующий за 
//	//ним - остатки исходного блока
//	block * newBlock = (block*)malloc(sizeof(block));
//	newBlock->size = part;
//	newBlock->address = temp->address + size;
//	newBlock->isUse = FALSE;
//	newBlock->next = temp->next;
//	newBlock->prev = temp;
//	temp->next = newBlock;
//
//	*pointer = (VA)temp;
//
//	return 0;
}

int preparePage(int pageNumber)
{
	//Если страница уже в фищической памяти, возвращаемся
	pageTable[pageNumber].timesUsed++;
	if (pageTable[pageNumber].isUse == TRUE) return;

	pageTable[pageNumber].timesUsed = 0;

	int pageForSwapNumber = -2;
	unsigned long minTimes = -1;

	//Определяем страницу для выгрузки
	for (int i = 0; i < numberOfPages; i++)
	{
		if (minTimes > pageTable[i].timesUsed && pageTable[i].isUse == TRUE)
		{
			minTimes = pageTable[i].timesUsed;
			pageForSwapNumber = i;
			break;
		}
	}

	//Задание смещение страниц
	pageTable[pageNumber].offset = pageTable[pageForSwapNumber].offset;
	pageTable[pageForSwapNumber].offset = pageForSwapNumber * pageSize;

	//Задание смещения и запись страрицы на данную позицию в файле подкачки
	fseek(swap, pageForSwapNumber*pageSize, SEEK_SET);
	fwrite(&memory[pageTable[pageNumber].offset], sizeof(char), pageSize, swap);

	//Задание смещения и чтение страрицы из данной позиции в файле подкачки
	fseek(swap, pageNumber*pageSize, SEEK_SET);
	fread(&memory[pageTable[pageNumber].offset], sizeof(char), pageSize, swap);

	//Флаги
	pageTable[pageNumber].isUse = TRUE;
	pageTable[pageForSwapNumber].isUse = FALSE;
}

unsigned int pageVA(VA ptr)
{
	//Возвращение номера страницы по ее виртуальному адресу
	return ((unsigned int)ptr >> bitsForPageSize);
}

unsigned int offsetVA(VA ptr)
{
	//Возвращение смещения страницы по ее виртуальному адресу
	return ((unsigned int)ptr & ((1 << bitsForPageSize) - 1));
}

void write(VA ptr, char value)
{
	//Подготовка страницы и запись value в физ. память
	preparePage(pageVA(ptr));
	memory[pageTable[pageVA(ptr)].offset + offsetVA(ptr)] = value;
}

MMEMORY_API int _write(VA pointer, void * buffer, size_t size)
{
	char * cBuffer = (char*)buffer;
	unsigned long address = ((block*)pointer)->address;

	if (((block*)pointer)->size < size) return -2;

	//Каждый байт в блоке записываем по адресу
	for (int i = 0; i < size; i++)
	{
		write(address + i, cBuffer[i]);
	}

	return 0;
}

char read(VA ptr)
{
	//Подготовка страницы и чтение из памяти
	preparePage(pageVA(ptr));
	return memory[pageTable[pageVA(ptr)].offset + offsetVA(ptr)];
}

MMEMORY_API int _read(VA pointer, void * buffer, size_t size)
{
	char * cBuffer = (char*)buffer;
	unsigned long address = ((block*)pointer)->address;

	//Выход за пределы блока
	if (((block*)pointer)->size < size) return -2;

	for (int i = 0; i < size; i++){
		cBuffer[i] = read(address + i);
	}

	return 0;
}


//void printVolumes(int szpage)
//{
//	printf("Size of page table: %dB\n", (VIRTUAL_MEMORY_SIZE / szpage) * sizeof(page));
//	printf("Size of extra mem: %dB\n", VIRTUAL_MEMORY_SIZE % szpage);
//}

void testRandomAccess(int size) {

	int i, result;
	int n = VIRTUAL_MEMORY_SIZE / size;

	result = _init(n, size);

	char* buffer = (char*)malloc(size);
	VA* addresses = (VA*)malloc(sizeof(VA)*n);

	for (i = 0; i < n; i++)
		_malloc(&addresses[i], size);

	struct timeb startTime;
	struct timeb endTime;
	ftime(&startTime);

	for (i = 0; i < 100; i++)
		_write(addresses[abs(rand() % n)], buffer, size);
	ftime(&endTime);
	int diff = 0;

	diff = (int)(1000.0 * (endTime.time - startTime.time) + (endTime.millitm - startTime.millitm));

	printf("Perf time: %dms\n", diff);

	for (i = 0; i < n; i++)
		_free(addresses[i]);

}

void interruptsTimeTest(int szPage)
{
	int i, result;
	int n = VIRTUAL_MEMORY_SIZE / szPage;
	_init(n, szPage);

	char* buffer = (char*)malloc(szPage);
	VA* addresses = (VA*)malloc(sizeof(VA)*n);

	for (i = 0; i < n; i++)
	{
		_malloc(&addresses[i], szPage);
	}
	struct timeb startTime, endTime, breakTime;
	ftime(&startTime);

	for (i = 0; i < n; i++)
	{
		result = _write(addresses[i], buffer, szPage);
	}
	ftime(&breakTime);

	for (i = 0; i < n; i++)
	{
		result = _write(addresses[0], buffer, szPage);
	}
	ftime(&endTime);

	int delta1 = (int)(1000.0 * (breakTime.time - startTime.time) + (breakTime.millitm - startTime.millitm));
	int delta2 = (int)(1000.0 * (endTime.time - breakTime.time) + (endTime.millitm - breakTime.millitm));

	printf("Interrupts time: %dms\n", abs(delta1 - delta2));
	for (i = 0; i < n; i++)
		_free(addresses[i]);

}

void startTests()
{
	int size = 1 << 15;
	while (size > 64)
	{
		printf("\nPage size: %dB\n", size);
		testRandomAccess(size);
		//printVolumes(size);
		interruptsTimeTest(size);
		size /= 2;
	}
}

void testInit() {
	int result;
	result = _init(10, 10);

	if (result == -1) printf("Size of page %d != 2^k\n", 10);
	result = _init(2, 8);

	if (result != 0) printf("Initialization, pages: 2, pageSize: 8 is not successful\n");
	if (result == 0) printf("Initialization is successful\n");
}

void testMalloc() {
	int result;
	VA address;
	result = _malloc(&address, 5000);

	if (result == -2) printf("Can't allocate size %d > size of all memory %d\n", 5000, 4096);
	
	result = _malloc(&address, 4);
	if (result != 0) printf("Cannot allocate block of memory\n");
	else if (result == 0) printf("Malloc is successful\n");

	_free(address);
}

void testFree() {
	int result;
	VA address;
	_malloc(&address, 4);

	result = _free(address);
	if (result != 0) printf("Can't free allocated block of memory\n");

	result = _free(address);
	if (result == -1) printf("Can't free deleted block of memory\n");
	if (result == 0) printf("Free is successful\n");
}

void testWrite() {
	int result;
	VA address;
	_malloc(&address, 4);

	char* buffer = (char*)malloc(5);
	result = _write(address, buffer, 5);

	if (result == -2) printf("Can't write outside\n");

	result = _write(address, buffer, 4);

	if (result != 0) printf("Can't write in block\n");
	else if (result == 0) printf("Write is successful\n");
	_free(address);
}

void testRead() {
	int result;
	VA address;
	_malloc(&address, 4);

	char* buffer = (char*)malloc(5);
	result = _read(address, buffer, 5);

	if (result == -2) printf("Can't read outside\n");
	result = _read(address, buffer, 4);

	if (result != 0) printf("Cannot read in block\n");
	else if (result == 0) printf("Read is successful\n");

	_free(address);
}

void testLoading() {
	int result, i;
	VA address;
	_malloc(&address, 11);

	char* buffer = (char*)malloc(11);
	for (i = 0; i < 10; i++)
		buffer[i] = '0' + i;

	buffer[10] = '\0';

	_write(address, buffer, 11);
	_read(address, buffer, 11);

	if (strcmp(buffer, "0123456789")) printf("Wrong result: %s\n", buffer);
	_free(address);
}

void runTests() {
	testInit();
	testMalloc();
	testFree();
	testWrite();
	testRead();
	testLoading();
}

void main() {}

//int main()
//{
//	/*VA adr, adr2, adr3;
//	_init(10, 2);
//	_malloc(&adr, 5);
//	_malloc(&adr3, 10);
//	_malloc(&adr2, 5);
//
//	char buf[5] = { 'a','v','a','d','a' };
//	char buf2[15];
//
//	_write(adr, buf, 5);
//	_write(adr2, buf, 5);
//	_write(adr3, buf, 5);
//	_read(adr, buf2, 5);
//	for (int i = 0; i < 5; i++)
//		printf("%c", buf2[i]);
//
//	_free(adr2);
//	_free(adr3);
//
//	_malloc(&adr2, 13);
//	char buf3[13] = { 'a','v','a','d','a',' ', 'k','e', 'd','a','v','r','a' };
//	_write(adr2, buf3, 13);
//	_read(adr2, buf2, 13);
//
//	for (int i = 0; i < 13; i++)
//		printf("%c", buf2[i]);*/
//
//	runTests();
////	startTests();
//	//getch();
//	return 0;
//}