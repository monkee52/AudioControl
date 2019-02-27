#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace AydenIO {
	namespace AudioControl {
		ref class Utilities {
		public:
			static String^ ConvertHrToString(HRESULT hr);
		};
	}
}
