#pragma once

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

			array<AudioDevice^>^ GetAudioDevices(DeviceType type, DeviceState stateMask);

			AudioDevice^ GetAudioDevice(String^ id);
			AudioDevice^ GetDefaultAudioDevice(DeviceType type, DeviceRole role);
			void SetDefaultAudioDevice(AudioDevice^ device, DeviceRole role);
			void SetDefaultAudioDevice(String^ deviceId, DeviceRole role);
		};
	}
}
