#pragma once

using namespace System;
using namespace System::Runtime::InteropServices;

namespace AydenIO {
	namespace AudioControl {
		private ref class Utilities {
		public:
			static String^ ConvertHrToString(HRESULT hr);
		};
	}
}
