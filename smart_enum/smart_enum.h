#ifndef SMART_ENUM_H_INCLUDED
#define SMART_ENUM_H_INCLUDED

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <map>

#ifdef _HAS_CXX17
#include <optional>
#endif

#ifdef _UNICODE
	#define SMART_ENUM_QUOTE(x)			L# x
	#define SMART_ENUM_CHARACTER_TYPE	wchar_t
	#define SMART_ENUM_STRING_TYPE		std::wstring
#else
	#define SMART_ENUM_QUOTE(x)			# x
	#define SMART_ENUM_CHARACTER_TYPE	char
	#define SMART_ENUM_STRING_TYPE		std::string	
#endif

#define ENUM_DEFINITION_NAME(enum_name)			EnumDefinition_## enum_name
#define DEFINE_SMART_ENUM(enum_name, type, ...) DEFINE_SMART_ENUM_RAW(enum_name, type, __VA_ARGS__)
#define DEFINE_SMART_ENUM_RAW(enum_name, type, ...)													\
	enum class enum_name : type																		\
	{																								\
		__VA_ARGS__																					\
	};																								\
																									\
	struct ENUM_DEFINITION_NAME(enum_name)															\
	{																								\
		using trait_type = enum_name;																\
		using underlying_type = type;																\
																									\
		static constexpr SMART_ENUM_CHARACTER_TYPE definition[] = SMART_ENUM_QUOTE(__VA_ARGS__);	\
	};																								\
																									\
	template<> struct smart_enum_def_map<enum_name>													\
	{																								\
		using enum_type = enum_name;																\
		using def_type = ENUM_DEFINITION_NAME(enum_name);											\
	};

template<typename _EnumTy>
struct smart_enum_def_map;

namespace smart_enum
{
	using character_type	= SMART_ENUM_CHARACTER_TYPE;
	using string_type		= SMART_ENUM_STRING_TYPE;
	
	template <typename StringTy = string_type>
	struct parsing_failed_exception : public std::exception
	{
		parsing_failed_exception(StringTy name, StringTy value)
			: enum_name(name), parsing_value(value)
		{
		}

		const StringTy enum_name;
		const StringTy parsing_value;
	};

	template <typename StringTy>
	struct ignore_case_comparator
	{
		bool operator() (const StringTy&, const StringTy&) const;
	};

	template <typename StringTy, typename ValueTy>
	std::vector<std::pair<StringTy, ValueTy>> ParseEnum(const typename StringTy::value_type *);
	
	template <typename _EnumTy>
	class Enum
	{
		static_assert(std::is_enum_v<_EnumTy>, "_EnumTy must be enum type.");
		using def_type = typename smart_enum_def_map<_EnumTy>::def_type;

	public:		
		using value_type		= _EnumTy;
		using underlying_type	= std::underlying_type_t<_EnumTy>;
		
	public:
		// return value set of enum.
		static const std::set<value_type>& Values()
		{
			container_type meta = GetMetaContainer();
			return meta->values;
		}

		// return names of enum.
		static const std::vector<string_type>& Names()
		{
			container_type meta = GetMetaContainer();
			return meta->names;
		}

		// convert enum value to enum name.
		static string_type ToString(value_type value)
		{
			container_type meta = GetMetaContainer();

			auto found = meta->value_to_name.find(value);
			if (found != meta->value_to_name.end())
			{
				return found->second;
			}

			return string_type{ SMART_ENUM_QUOTE(undefined) };
		}

		#ifdef _HAS_CXX17
		// convert enum name to enum value.
		// if the name is wrong, return nullopt. this works since C++17.
		static std::optional<value_type> ParseFrom(string_type name, bool ignoreCase = true)
		{
			container_type meta = GetMetaContainer();

			auto found = meta->name_to_value.find(name);
			if (found != meta->name_to_value.end())
			{
				value_type& value = found->second;
				if (ignoreCase || (meta->value_to_name[value] == name))
				{
					return value;
				}
			}

			return std::nullopt;
		}
		#endif

		// convert enum name to enum value.
		// if the name is wrong, return default value.
		static value_type ParseFrom(string_type name, value_type defaultValue, bool ignoreCase = true)
		{
			container_type meta = GetMetaContainer();

			auto found = meta->name_to_value.find(name);
			if (found != meta->name_to_value.end())
			{
				value_type& value = found->second;
				if (ignoreCase || (meta->value_to_name[value] == name))
				{
					return value;
				}
			}

			return defaultValue;
		}

	private:
		struct EnumMetaContainer
		{
			std::set<value_type> values;
			std::vector<string_type> names;
			std::map<value_type, string_type> value_to_name;
			std::map<string_type, value_type, ignore_case_comparator<string_type>> name_to_value;

			EnumMetaContainer()
			{
				using enum_pair = std::pair<string_type, underlying_type>;
				for (enum_pair& ep : ParseEnum<string_type, underlying_type>(def_type::definition))
				{
					values.emplace(static_cast<value_type>(ep.second));
					names.emplace_back(ep.first);
					value_to_name.emplace(make_pair(static_cast<value_type>(ep.second), ep.first));
					name_to_value.emplace(make_pair(ep.first, static_cast<value_type>(ep.second)));
				}
			}
		};
		using container_type = std::shared_ptr<EnumMetaContainer>;

		static container_type GetMetaContainer()
		{
			static container_type metaContainer{ new EnumMetaContainer{} };			
			return metaContainer;
		}
	};
}

#endif