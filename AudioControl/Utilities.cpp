#include "stdafx.h"

#include "AudioControl.h"
#include "Utilities.h"

namespace AydenIO {
	namespace AudioControl {
		/* public static */ String^ Utilities::ConvertHrToString(HRESULT hr) {
			// Convert error into string
			_com_error err(hr);

			// Include error code in message
			LPTSTR text = (LPTSTR)err.ErrorMessage();
			size_t len = 13 + _tcslen(text); // 13 is the length of the string below when formatted
			LPTSTR result = new TCHAR[len];

			_sntprintf_s(result, len, _TRUNCATE, _T("%#010x: %s"), hr, text);

			// Convert to .NET String
			String^ managedResult = gcnew String(result);

			// Free memory
			delete result;

			return managedResult;
		}
	}
}
