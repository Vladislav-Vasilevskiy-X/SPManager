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

		TEST_METHOD(PosInitTest)
		{
			Assert::AreEqual(0, _init(10, 2), L"Init is succesful", LINE_INFO());
		}

		TEST_METHOD(PosMallocTest)
		{
			VA adr;
			_init(10, 2);
			Assert::AreEqual(0, _malloc(&adr, 10), L"Success", LINE_INFO());
		}

		TEST_METHOD(PosFreeTest)
		{
			VA adr;
			int result;
			_init(10, 2);
			_malloc(&adr, 10);
			result = _free(adr);
			Assert::AreEqual(0, result, L"Success", LINE_INFO());
		}

		TEST_METHOD(PosWriteAndReadTest)
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
	TEST_CLASS(NegativeTests)
	{
	public:

		TEST_METHOD(NegInitTest)
		{
			Assert::AreEqual(-1, _init(10, 3), L"Page size must be 2^k", LINE_INFO());
		}

		TEST_METHOD(NegMallocTest)
		{
			VA adr;
			_init(10, 2);
			Assert::AreEqual(-2, _malloc(&adr, 30), L"Out of memory", LINE_INFO());
		}

		TEST_METHOD(NegFreeTest)
		{
			VA adr = NULL;
			int result;
			_init(10, 2);
			result = _free(adr);
			Assert::AreEqual(-1, result, L"can't free nothing", LINE_INFO());
		}

		TEST_METHOD(NegWriteTest)
		{
			VA adr;
			_init(10, 2);
			_malloc(&adr, 10);
			char srcArray[11] = { 't' ,'e','s','t',' ','a','r','r','a','y','!' };
			char resArray[10];
			int result = _write(adr, srcArray, 11);
			Assert::AreEqual(-2, result, L"out of block", LINE_INFO());
		}

		TEST_METHOD(NegReadTest)
		{
			VA adr;
			_init(10, 2);
			_malloc(&adr, 10);
			char srcArray[10] = { 't' ,'e','s','t',' ','a','r','r','a','y' };
			char resArray[9];
			_write(adr, srcArray, 10);
			int result = _read(adr, resArray, 9);
			Assert::AreEqual(-2, result, L"out of buffer", LINE_INFO());
		}
	};
}