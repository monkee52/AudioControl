#pragma once

#include "stdafx.h"
#include "AudioControl.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace AydenIO {
	namespace AudioControl {
		ref class Controller;

		/// <summary>
		/// Represents an audio endpoint
		/// </summary>
		public ref class AudioDevice : public IDisposable {
		private:
			IMMDevice* pDevice;
			IPropertyStore* pProps;

			~AudioDevice();
		internal:
			AudioDevice(IMMDevice* pDevice);
		public:
			!AudioDevice();

			/// <summary>
			/// Gets the system defined identifier for the endpoint
			/// </summary>
			property String^ Id {
				String^ get();
			}

			/// <summary>
			/// Gets the name of the endpoint. e.g. "XYZ Audio Adapter"
			/// </summary>
			property String^ Name {
				String^ get();
			}

			/// <summary>
			/// Gets the description of the endpoint. e.g. "Speakers (XYZ Audio Adapter)"
			/// </summary>
			property String^ FriendlyName {
				String^ get();
			}

			/// <summary>
			/// Gets the description of the endpoint. e.g. "Speakers"
			/// </summary>
			property String^ Description {
				String^ get();
			}

			/// <summary>
			/// Gets the default number of channels
			/// </summary>

			virtual bool Equals(Object^ otherDevice) override;
			virtual bool Equals(AudioDevice^ otherDevice);
			virtual int GetHashCode() override;
			static bool operator!= (AudioDevice^ device1, AudioDevice^ device2);
			static bool operator== (AudioDevice^ device1, AudioDevice^ device2);
			virtual String^ ToString() override;
		};
	}
}
