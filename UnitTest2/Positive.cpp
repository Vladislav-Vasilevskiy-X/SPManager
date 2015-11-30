#pragma comment (lib,"Manager.lib")

#include "stdafx.h"
#include "CppUnitTest.h"
#include <stdlib.h>

extern "C"
{
#include "..\Manager\mmemory.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(PositiveTests)
	{
	public:

		TEST_METHOD(InitTest)
		{
			Assert::AreEqual(0, _init(10, 2), L"Init is succesful", LINE_INFO());
		}

		TEST_METHOD(MallocTest)
		{
			VA adr;
			_init(10, 2);
			Assert::AreEqual(0, _malloc(&adr, 10), L"Success", LINE_INFO());
		}

		TEST_METHOD(FreeTest)
		{
			VA adr;
			int result;
			_init(10, 2);
			_malloc(&adr, 10);
			result = _free(adr);
			Assert::AreEqual(0, result, L"Success", LINE_INFO());
		}

		TEST_METHOD(WriteAndReadTest)
		{
			VA adr;
			_init(10, 2);
			_malloc(&adr, 10);

			char srcArray[10] = { 't' ,'e','s','t',' ','a','r','r','a','y' };
			char resArray[10];
			
			
			_write(adr, srcArray, 10);
			_read(adr, resArray, 10);

			int counter = 0;
			for (int i = 0; i < 10; i++)
			{
				if (srcArray[i] == resArray[i]) counter++;
			}

			Assert::AreEqual(10, counter, L"Success", LINE_INFO());
		}

		TEST_METHOD(RandomAccessTest)
		{
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
	};
}