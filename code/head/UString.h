#pragma once

#include <string>
#include "AuxiliaryBase.h"

namespace Rendering
{
	class UString
	{
	public:
		UString() {};

		UString(const std::string&  _str)
		{
			str = _str;
		}
		UString(const char* _str)
		{
			str = std::string(_str);
		}

		UString(const std::wstring& _str)
		{
			str = WStringToAnsi(_str);
		}
		UString(const wchar_t* _str)
		{
			str = WStringToAnsi(std::wstring(_str));
		}

		UString& operator=(const std::string&  _str)
		{
			str = _str;
			return *this;
		}
		UString& operator=(const std::wstring& _str)
		{
			str = WStringToAnsi(_str);
			return *this;
		}

		UString operator+(const UString& _str)const
		{
			return (str + _str.GetString());
		}
		bool operator==(const UString& _str)const
		{
			return str == _str.GetString();
		}
		bool operator<(const UString& _str)const
		{
			return str < _str.GetString();
		}
		bool operator>(const UString& _str)const
		{
			return str > _str.GetString();
		}

		std::string& GetStr()
		{
			return str;
		}

		const std::string& GetString()const
		{
			return str;
		}
		std::wstring GetWstring()const
		{
			return AnsiToWString(str);
		}

		const char* GetC_Str()const
		{
			return str.c_str();
		}
		const wchar_t* GetWC_Str()const
		{
			return AnsiToWString(str).c_str();
		}

	private:
		std::string str;
	};
}