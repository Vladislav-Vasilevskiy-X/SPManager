#pragma comment (lib,"Manager.lib")
#include <stdlib.h>
#include <stdio.h>

extern "C"
{
#include "..\Manager\mmemory.h"
}

#define VIRTUAL_MEMORY_SIZE 10000000

void testRandomAccess(int size)
{
	int i, result;
	int n = VIRTUAL_MEMORY_SIZE / size;

	result = _init(n, size);
	
	char* buffer = (char*)malloc(size);
	
	VA * addresses = (VA*)malloc(sizeof(VA)*n);
	
	for (i = 0; i < n; i++)
	{
		_malloc(&addresses[i], size);
	}

	struct timeb startTime;
	struct timeb endTime;
	ftime(&startTime);

	for (i = 0; i < 100; i++)
	{
		int index = abs(rand() % n);
		_write(addresses[index], buffer, size);
	}

	ftime(&endTime);
	int diff = 0;

	diff = (int)(1000.0 * (endTime.time - startTime.time) + (endTime.millitm - startTime.millitm));

	printf("Perf time: %dms\n", diff);
}

void interruptsTest(int szPage)
{
	int i, result;
	const int n = VIRTUAL_MEMORY_SIZE / szPage;
	_init(n, szPage);

	char* buffer = (char*)malloc(szPage);

	VA * addresses = (VA*)malloc(sizeof(VA)*n);

	for (i = 0; i < n; i++)
	{
		_malloc(&addresses[i], szPage);
	}

	for (i = 0; i < n; i++)
	{
		_write(addresses[i], buffer, szPage);
	}

	setInterrupts(0);
	for (i = 0; i < n; i++)
	{
		_read(addresses[i], buffer, szPage);
	}
	printf("Interrupts : %d\n", getInterrupts());
}

void startTests()
{
	int size = 1<<15;
	while (size > 1<<8)
	{
		printf("\nPage size: %dB\n", size);
		testRandomAccess(size);
		interruptsTest(size);
		size /= 2;
	}
}

void main()
{
	VA adr;
	_init(10, 2);
	_malloc(&adr, 10);

	char srcArray[10] = { 't' ,'e','s','t',' ','a','r','r','a','y' };
	char resArray[10];

	char buf[4] = "www";
	char buf2[4];

	block * temp = (block*)adr;
	int address = temp->address;


	_write(adr, srcArray, 10);


	__write(address + 2, buf, 3);
		
	_read(adr, resArray, 10);

	for (int i = 0; i < 10; i++)
	{
		printf("%c\n", resArray[i]);
	}

	__read(address + 2, buf2, 4);

	puts("line");

	for (int i = 0; i < 4; i++)
	{
		printf("%c\n", buf2[i]);
	}

	//startTests();
}