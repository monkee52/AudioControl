#pragma once

#include "CMMNotificationClient.h"
#include "AudioDevice.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace AydenIO {
	namespace AudioControl {
		[Flags]
		public enum class DeviceState : DWORD {
			Active = DEVICE_STATE_ACTIVE,
			Disabled = DEVICE_STATE_DISABLED,
			NotPresent = DEVICE_STATE_NOTPRESENT,
			Unplugged = DEVICE_STATE_UNPLUGGED
		};

		public enum class DeviceType {
			Render = eRender,
			Capture = eCapture,
			All = eAll
		};

		public enum class DeviceRole {
			Console = eConsole,
			Multimedia = eMultimedia,
			Communications = eCommunications
		};

		public ref class Controller : public IDisposable {
		private:
			CMMNotificationClient* notificationClient;

			~Controller();
		protected:

		public:
			Controller();
			!Controller();

			/// <summary>
			/// Gets all the render audio endpoints currently enabled on the system
			/// </summary>
			/// <returns>The endpoints</returns>
			array<AudioDevice^>^ GetAudioDevices(DeviceType type, DeviceState stateMask);
			
			/// <summary>
			/// Gets an audio endpoint by id
			/// </summary>
			/// <param name="id">The endpoint id</param>
			/// <returns>The endpoint</returns>
			AudioDevice^ GetAudioDevice(String^ id);
			
			/// <summary>
			/// Gets the current default render endpoint
			/// </summary>
			/// <returns>The endpoint</returns>
			AudioDevice^ GetDefaultAudioDevice(DeviceType type, DeviceRole role);
			
			/// <summary>
			/// Sets the default render endpoint
			/// </summary>
			/// <param name="endpoint">The endpoint</param>
			//void SetDefaultAudioDevice(AudioDevice^ device, DeviceRole role);
			
			/// <summary>
			/// Sets the default render endpoint
			/// </summary>
			/// <param name="endpoint">The endpoint</param>
			//void SetDefaultAudioDevice(String^ deviceId, DeviceRole role);
		};
	}
}
