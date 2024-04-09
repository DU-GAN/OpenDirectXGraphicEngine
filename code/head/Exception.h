#pragma once

#include <string>
#include <exception>
#include <windows.h>

#include "AuxiliaryBase.h"
#include "UString.h"

namespace Rendering
{
	enum ExceptionLevel
	{
		LEVEL_Prompt = 1,
		LEVEL_Warning = 0,
		LEVEL_Error = -1,
	};

	class Exception : public std::exception
	{
	public:
		Exception() = default;
		Exception(ExceptionLevel _level, UString _info)
			:level(_level), info(_info) {}

		__forceinline static void CheckResult(HRESULT x)
		{
			if (FAILED(x))
			{
				ThrowException(LEVEL_Warning, "Direct3D result value is failed");
			}
		}

		__forceinline static void ThrowException(ExceptionLevel level, UString reason)
		{
			UString errorInfo = reason
				+ (L" Function:" + std::wstring(__FUNCTIONW__)
				+ L" File:" + std::wstring(__FILEW__)
				+ L" Line:" + AnsiToWString(std::to_string(__LINE__)));

			throw Exception(level, errorInfo);
		}

		__forceinline void ShowException()const
		{
#ifdef _DEBUG
			std::cout << std::endl << info.GetC_Str();
#else
			MessageBox(NULL, info.GetWC_Str(), L"Exception", MB_OK);
#endif
		}

		UString info;
		ExceptionLevel level;
	};
}