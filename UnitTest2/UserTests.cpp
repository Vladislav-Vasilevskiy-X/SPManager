#pragma comment (lib,"Manager.lib")

#include "stdafx.h"

extern "C"
{
#include "..\Manager\mmemory.h"
}
//void printVolumes(int szpage)
//{
//	printf("Size of page table: %dB\n", (VIRTUAL_MEMORY_SIZE / szpage) * sizeof(page));
//	printf("Size of extra mem: %dB\n", VIRTUAL_MEMORY_SIZE % szpage);
//}

void testRandomAccess(int size) {

	

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