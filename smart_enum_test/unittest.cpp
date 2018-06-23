#include "stdafx.h"
#include "CppUnitTest.h"

#include <sstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace smart_enum;

namespace smart_enum_test
{
	DEFINE_SMART_ENUM(Fruit, int,
		Invalid = -1,
		Apple,
		Orange,
		Kiwi,
		Banana,
		Mango,
		Strawberry,
		Grapes,
		Lemon,
		Pineapple,
		RedPineapple = Pineapple
	);

	constexpr int DEFINE_VALUE = 10;
	DEFINE_SMART_ENUM(MakeError, int,
		Enum_0,
		Enum_1 = DEFINE_VALUE
	);

	TEST_CLASS(UnitTest)
	{
	public:		
		TEST_METHOD(Enum_Values)
		{
			const std::set<Fruit>& values = Enum<Fruit>::Values();
			Assert::IsTrue(values.find(Fruit::Apple) != values.end());
			Assert::IsTrue(values.find(Fruit::Orange) != values.end());
			Assert::IsTrue(values.find(Fruit::Kiwi) != values.end());
			Assert::IsTrue(values.find(Fruit::Banana) != values.end());
			Assert::IsTrue(values.find(Fruit::Mango) != values.end());
			Assert::IsTrue(values.find(Fruit::Strawberry) != values.end());
			Assert::IsTrue(values.find(Fruit::Grapes) != values.end());
			Assert::IsTrue(values.find(Fruit::Lemon) != values.end());
			Assert::IsTrue(values.find(Fruit::Pineapple) != values.end());
		}

		TEST_METHOD(Enum_Names)
		{
			const std::vector<std::wstring>& names = Enum<Fruit>::Names();
			Assert::IsTrue(names[0] == L"Invalid");
			Assert::IsTrue(names[1] == L"Apple");
			Assert::IsTrue(names[2] == L"Orange");
			Assert::IsTrue(names[3] == L"Kiwi");
			Assert::IsTrue(names[4] == L"Banana");
			Assert::IsTrue(names[5] == L"Mango");
			Assert::IsTrue(names[6] == L"Strawberry");
			Assert::IsTrue(names[7] == L"Grapes");
			Assert::IsTrue(names[8] == L"Lemon");
			Assert::IsTrue(names[9] == L"Pineapple");
		}

		TEST_METHOD(Enum_ToString)
		{
			Assert::AreEqual(std::wstring{ L"Apple" },		Enum<Fruit>::ToString(Fruit::Apple));
			Assert::AreEqual(std::wstring{ L"Orange" },		Enum<Fruit>::ToString(Fruit::Orange));
			Assert::AreEqual(std::wstring{ L"Kiwi" },		Enum<Fruit>::ToString(Fruit::Kiwi));
			Assert::AreEqual(std::wstring{ L"Banana" },		Enum<Fruit>::ToString(Fruit::Banana));
			Assert::AreEqual(std::wstring{ L"Mango" },		Enum<Fruit>::ToString(Fruit::Mango));
			Assert::AreEqual(std::wstring{ L"Strawberry" }, Enum<Fruit>::ToString(Fruit::Strawberry));
			Assert::AreEqual(std::wstring{ L"Grapes" },		Enum<Fruit>::ToString(Fruit::Grapes));
			Assert::AreEqual(std::wstring{ L"Lemon" },		Enum<Fruit>::ToString(Fruit::Lemon));
			Assert::AreEqual(std::wstring{ L"Pineapple" },	Enum<Fruit>::ToString(Fruit::Pineapple));
			Assert::AreEqual(std::wstring{ L"undefined" },	Enum<Fruit>::ToString(static_cast<Fruit>(23423)));
		}

		TEST_METHOD(Enum_ToString_SameValue)
		{
			Assert::IsTrue(Fruit::Pineapple == Fruit::RedPineapple);
			Assert::AreNotEqual(std::wstring{ L"RedPineapple" }, Enum<Fruit>::ToString(Fruit::RedPineapple));			
		}

		TEST_METHOD(Enum_ParseFrom_Default)
		{
			Assert::IsTrue(Fruit::Apple		== Enum<Fruit>::ParseFrom(L"Apple",		Fruit::Invalid));
			Assert::IsTrue(Fruit::Orange	== Enum<Fruit>::ParseFrom(L"Orange",	Fruit::Invalid));
			Assert::IsTrue(Fruit::Kiwi		== Enum<Fruit>::ParseFrom(L"Kiwi",		Fruit::Invalid));
			Assert::IsTrue(Fruit::Banana	== Enum<Fruit>::ParseFrom(L"Banana",	Fruit::Invalid));
			Assert::IsTrue(Fruit::Mango		== Enum<Fruit>::ParseFrom(L"Mango",		Fruit::Invalid));
			Assert::IsTrue(Fruit::Strawberry== Enum<Fruit>::ParseFrom(L"Strawberry",Fruit::Invalid));
			Assert::IsTrue(Fruit::Grapes	== Enum<Fruit>::ParseFrom(L"Grapes",	Fruit::Invalid));
			Assert::IsTrue(Fruit::Lemon		== Enum<Fruit>::ParseFrom(L"Lemon",		Fruit::Invalid));
			Assert::IsTrue(Fruit::Pineapple	== Enum<Fruit>::ParseFrom(L"Pineapple", Fruit::Invalid));
		}

		TEST_METHOD(Enum_ParseFrom_CaseSensitive)
		{
			Assert::IsTrue(Fruit::Apple		== Enum<Fruit>::ParseFrom(L"apple", Fruit::Invalid, true));
			Assert::IsTrue(Fruit::Apple		== Enum<Fruit>::ParseFrom(L"aPple", Fruit::Invalid, true));
			Assert::IsTrue(Fruit::Apple		== Enum<Fruit>::ParseFrom(L"Apple", Fruit::Invalid, false));
			Assert::IsTrue(Fruit::Invalid	== Enum<Fruit>::ParseFrom(L"apple", Fruit::Invalid, false));			
		}

		TEST_METHOD(Enum_ParseFrom_Optional)
		{
			Assert::IsTrue(Fruit::Apple == Enum<Fruit>::ParseFrom(L"Apple"));
			Assert::IsTrue(Fruit::Orange == Enum<Fruit>::ParseFrom(L"Orange"));
			Assert::IsTrue(Fruit::Kiwi == Enum<Fruit>::ParseFrom(L"Kiwi"));
			Assert::IsTrue(Fruit::Banana == Enum<Fruit>::ParseFrom(L"Banana"));
			Assert::IsTrue(Fruit::Mango == Enum<Fruit>::ParseFrom(L"Mango"));
			Assert::IsTrue(Fruit::Strawberry == Enum<Fruit>::ParseFrom(L"Strawberry"));
			Assert::IsTrue(Fruit::Grapes == Enum<Fruit>::ParseFrom(L"Grapes"));
			Assert::IsTrue(Fruit::Lemon == Enum<Fruit>::ParseFrom(L"Lemon"));
			Assert::IsTrue(Fruit::Pineapple == Enum<Fruit>::ParseFrom(L"Pineapple"));
		}

		TEST_METHOD(Enum_Error)
		{
			try
			{
				Assert::AreEqual(std::wstring{ L"Enum_1" }, Enum<MakeError>::ToString(MakeError::Enum_1));
				Assert::Fail(L"not occur parsing failed exception");
			}
			catch (parsing_failed_exception<std::wstring>& e)
			{
				std::wstringstream ss;
				ss << "enum_name: " << e.enum_name << ", parsing failed value: " << e.parsing_value;

				Logger::WriteMessage(ss.str().c_str());
			}
		}
	};
}