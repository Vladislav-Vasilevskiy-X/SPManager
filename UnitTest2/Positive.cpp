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
	};
}