#include "smart_enum.h"

#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace smart_enum
{
	template<typename SV>
	struct stringview_mapper;

	template<> struct stringview_mapper<std::string_view>
	{
		static constexpr char whitespace[] = " \t\r\n";
		static constexpr char comma[] = ",";
		static constexpr char equal[] = "=";
		static constexpr char empty[] = "";

		using type = std::string;
		using stream = std::stringstream;
	};

	template<> struct stringview_mapper<std::wstring_view>
	{
		static constexpr wchar_t whitespace[] = L" \t\r\n";
		static constexpr wchar_t comma[] = L",";
		static constexpr wchar_t equal[] = L"=";
		static constexpr wchar_t empty[] = L"";

		using type = std::wstring;
		using stream = std::wstringstream;
	};

	template <typename S>
	struct string_mapper;

	template<> struct string_mapper<std::string>
	{
		using view_type = std::string_view;
	};

	template<> struct string_mapper<std::wstring>
	{
		using view_type = std::wstring_view;
	};
			
	template <typename SV>
	static SV trim(SV text)
	{
		SV whitespace{ stringview_mapper<SV>::whitespace };

		size_t length = text.length();
		if (length == 0) return stringview_mapper<SV>::empty;

		size_t lpos = 0;
		size_t rpos = length - 1;
		
		for (; whitespace.find(text[lpos]) != SV::npos && lpos < length; lpos++);
		for (; whitespace.find(text[rpos]) != SV::npos && rpos >= 0; rpos--);

		return text.substr(lpos, rpos - lpos + 1);
	}

	template <typename SV>
	static std::vector<SV> split(SV text, SV delim)
	{
		std::vector<SV> out;
		size_t prev_pos = 0;
		size_t next_pos = 0;
		while ((next_pos = text.find(delim, prev_pos)) != SV::npos)
		{
			out.emplace_back(text.substr(prev_pos, next_pos - prev_pos));
			prev_pos = next_pos + 1;
		}
		out.emplace_back(text.substr(prev_pos));

		return out;
	}
	
	template <typename SV, typename ValueTy>
	static ValueTy parse_from(SV text)
	{
		using stream = typename stringview_mapper<SV>::stream;
		
		stream ss(text.data());
		ss.exceptions(std::ios::failbit | std::ios::badbit);

		ValueTy value;
		ss >> value;

		return value;
	}

	template<>
	static char parse_from<std::wstring_view, char>(std::wstring_view text)
	{
		std::wstringstream ss(text.data());
		ss.exceptions(std::ios::failbit | std::ios::badbit);

		int value;
		ss >> value;

		if (value < -128 || value > 127)
		{
			throw std::wstringstream::failure("ParseEnum<char>(...) value range over.");
		}

		return value;
	}

	template<>
	static unsigned char parse_from<std::wstring_view, unsigned char>(std::wstring_view text)
	{
		std::wstringstream ss(text.data());
		ss.exceptions(std::ios::failbit | std::ios::badbit);

		unsigned int value;
		ss >> value;

		if (value > 255)
		{
			throw std::wstringstream::failure("ParseEnum<char>(...) value range over.");
		}

		return value;
	}

	template<typename SV, typename ValueTy>
	static  std::vector<std::pair<typename stringview_mapper<SV>::type, ValueTy>> parse_enum(SV text)
	{
		using string_type = typename stringview_mapper<SV>::type;
		using trait_type = std::pair<string_type, ValueTy>;
		
		SV comma{ stringview_mapper<SV>::comma };
		SV equal{ stringview_mapper<SV>::equal };

		std::vector<trait_type> enums;
		std::map<SV, ValueTy> parsed_enums;

		ValueTy enum_value = 0;
		for (SV raw_item : split(text, comma))
		{
			SV item = trim(raw_item);
			std::vector<SV> tokens = split(item, equal);

			SV item_name = trim(tokens[0]);

			// enum 항목이 없다면 제외한다.
			if (item_name.empty()) break;

			// = 으로 값을 지정하고 있다면 값을 다시 설정한다.
			if (tokens.size() == 2)
			{
				SV item_value = trim(tokens[1]);
				try
				{
					enum_value = parse_from<SV, ValueTy>(item_value);
				}
				catch (...)
				{
					auto found = parsed_enums.find(item_value);
					if (found != parsed_enums.end())
					{
						enum_value = found->second;
					}
					else
					{
						throw parsing_failed_exception<string_type>(string_type{ item_name }, string_type{ item_value });
					}
				}
			}

			enums.emplace_back(make_pair(string_type{ item_name }, enum_value));
			parsed_enums.emplace(make_pair(item_name, enum_value));
			enum_value++;
		}

		return enums;
	}

	
	//
	// Multibyte characters
	//
	template<>
	std::vector<std::pair<std::string, char>> ParseEnum<std::string, char>(const typename std::string::value_type* text)
	{
		return std::move(parse_enum<std::string_view, char>(text));
	}

	template<>
	std::vector<std::pair<std::string, short>> ParseEnum<std::string, short>(const typename std::string::value_type* text)
	{
		return std::move(parse_enum<std::string_view, short>(text));
	}

	template<>
	std::vector<std::pair<std::string, int>> ParseEnum<std::string, int>(const typename std::string::value_type* text)
	{
		return std::move(parse_enum<std::string_view, int>(text));
	}

	template<>
	std::vector<std::pair<std::string, long long>> ParseEnum<std::string, long long>(const typename std::string::value_type* text)
	{
		return std::move(parse_enum<std::string_view, long long>(text));
	}

	template<>
	std::vector<std::pair<std::string, unsigned char>> ParseEnum<std::string, unsigned char>(const typename std::string::value_type* text)
	{
		return std::move(parse_enum<std::string_view, unsigned char>(text));
	}

	template<>
	std::vector<std::pair<std::string, unsigned short>> ParseEnum<std::string, unsigned short>(const typename std::string::value_type* text)
	{
		return std::move(parse_enum<std::string_view, unsigned short>(text));
	}

	template<>
	std::vector<std::pair<std::string, unsigned int>> ParseEnum<std::string, unsigned int>(const typename std::string::value_type* text)
	{
		return std::move(parse_enum<std::string_view, unsigned int>(text));
	}

	template<>
	std::vector<std::pair<std::string, unsigned long long>> ParseEnum<std::string, unsigned long long>(const typename std::string::value_type* text)
	{
		return std::move(parse_enum<std::string_view, unsigned long long>(text));
	}

	//
	// Unicode characters (wchar_t base)
	//
	template<>
	std::vector<std::pair<std::wstring, char>> ParseEnum<std::wstring, char>(const typename std::wstring::value_type* text)
	{
		return std::move(parse_enum<std::wstring_view, char>(text));
	}

	template<>
	std::vector<std::pair<std::wstring, short>> ParseEnum<std::wstring, short>(const typename std::wstring::value_type* text)
	{
		return std::move(parse_enum<std::wstring_view, short>(text));
	}

	template<>
	std::vector<std::pair<std::wstring, int>> ParseEnum<std::wstring, int>(const typename std::wstring::value_type* text)
	{
		return std::move(parse_enum<std::wstring_view, int>(text));
	}

	template<>
	std::vector<std::pair<std::wstring, long long>> ParseEnum<std::wstring, long long>(const typename std::wstring::value_type* text)
	{
		return std::move(parse_enum<std::wstring_view, long long>(text));
	}

	template<>
	std::vector<std::pair<std::wstring, unsigned char>> ParseEnum<std::wstring, unsigned char>(const typename std::wstring::value_type* text)
	{
		return std::move(parse_enum<std::wstring_view, unsigned char>(text));
	}

	template<>
	std::vector<std::pair<std::wstring, unsigned short>> ParseEnum<std::wstring, unsigned short>(const typename std::wstring::value_type* text)
	{
		return std::move(parse_enum<std::wstring_view, unsigned short>(text));
	}

	template<>
	std::vector<std::pair<std::wstring, unsigned int>> ParseEnum<std::wstring, unsigned int>(const typename std::wstring::value_type* text)
	{
		return std::move(parse_enum<std::wstring_view, unsigned int>(text));
	}

	template<>
	std::vector<std::pair<std::wstring, unsigned long long>> ParseEnum<std::wstring, unsigned long long>(const typename std::wstring::value_type* text)
	{
		return std::move(parse_enum<std::wstring_view, unsigned long long>(text));
	}

	//
	// ignore_case_comparator
	//
	template<>
	bool ignore_case_comparator<std::string>::operator ()(const std::string& lhs, const std::string& rhs) const
	{
		return _stricmp(lhs.c_str(), rhs.c_str()) < 0;
	}

	template<>
	bool ignore_case_comparator<std::wstring>::operator ()(const std::wstring& lhs, const std::wstring& rhs) const
	{
		return _wcsicmp(lhs.c_str(), rhs.c_str()) < 0;
	}
}