# smart_enum
C++ does not support some features like string to enum value, enum value to string, etc.
This smart enum implement these features by parsing enum definition text.


# usage
// enum class UserType
DEFINE_SMART_ENUM(UserType, int,
	Guest,
	Individual,
	Business,
	Admin
);

using namespace smart_enum;

// Values()
const std::set<UserType>& values = Enum<UserType>::Values();

// Names()
// You can use std::string or std::wstring.
const std::vector<std::string>& names = Enum<UserType>::Names();

// ToString(enum value)
// Invalid enum value return "undefined"
std::cout << "You are " << Enum<UserType>::ToString(UserType::Admin) << std::endl;

// ParseFrom(string, defaultValue, ignoreCase = true)
// if Smart enum cannot found enum value, it return defaultValue.
UserType myType = Enum<UserType>::ParseFrom("individual", UserType::Guest);

// for C++17
// optional<enum_type> ParseFrom(string, ignoreCase = true)
std::optional<UserType> myType = Enum<UserType>::ParseFrom("inDividual", false);


# limitation
Smart enum cannot recognize const variables, values of other enums.
so, These are wrong. (just example)

1)
constexpr int MAX_USER_TYPE_COUNT = 4;
DEFINE_SMART_ENUM(UserType, int,
	Guest,
	Individual,
	Business,
	Admin,
	MaxUserType = MAX_USER_TYPE_COUNT
);

parser cannot recognize MAX_USER_TYPE_COUNT, so it throw parsing_failed_exception at MaxUserType.

2)
DEFINE_SMART_ENUM(EnumType, int,
	Normal = 100,
	Smart = 200
);

DEFINE_SMART_ENUM(FooType, int,
	Why = EnumType::Smart
);

parser cannot recognize EnumType::Smart, so it throw parsing_failed_exception at Why.

but, belows are enable.

1)
#define MY_DEBUG_VALUE	200
DEFINE_SMART_ENUM(FooType, int,
	Why = MY_DEBUG_VALUE
);

2)
DEFINE_SMART_ENUM(FooType, int,
	A,
	B,
	C,
	HEY = C
);

case 2, FooType::C == FooType::Hey, Enum<FooType>::ToString(Foo::HEY) returns "C".
but Enum<FooType>::ParseFrom("hey") returns Foo::HEY.