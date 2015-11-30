#pragma comment (lib,"Manager.lib")

#include "stdafx.h"
#include "CppUnitTest.h"

extern "C"
{
	#include "..\Manager\mmemory.h"
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(InitTest)
		{
			Assert::AreEqual(0, _init(10,2), L"Init is succesful", LINE_INFO());
		}

	};
}
