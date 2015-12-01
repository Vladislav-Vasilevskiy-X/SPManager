#pragma comment (lib,"Manager.lib")
#include <stdlib.h>

extern "C"
{
#include "..\Manager\mmemory.h"
}


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
	startTests();
}