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
			Unplugged = DEVICE_STATE_UNPLUGGED,
			All = DEVICE_STATEMASK_ALL
		};

		public enum class DeviceType {
			Render = eRender,
			Capture = eCapture,
			All = eAll
		};

		public enum class DeviceRole {
			Console = eConsole, // Games, system notification sounds, and voice commands.
			Multimedia = eMultimedia, // Music, movies, narration, and live music recording.
			Communications = eCommunications // Voice communications (talking to another person).
		};

		private ref class Utilities {
		public:
			static String^ ConvertHrToString(HRESULT hr);
			static Guid ConvertNativeGuidToGuid(const GUID g);
		};

		public ref class MuteStatusChangedEventArgs : public EventArgs {
		private:
			Guid _evContext;
			bool _previousMuteStatus;
			bool _muteStatus;
		internal:
			MuteStatusChangedEventArgs(Guid evContext, bool oldMuteStatus, bool newMuteStatus);
		public:
			property Guid Context {
				Guid get();
			}

			property bool PreviousMuteStatus {
				bool get();
			}

			property bool MuteStatus {
				bool get();
			}
		};

		public ref class VolumeChangedEventArgs : public EventArgs {
		private:
			Guid _evContext;
			float _previousVolume;
			float _volume;
		internal:
			VolumeChangedEventArgs(Guid evContext, float oldVolume, float newVolume);
		public:
			property Guid Context {
				Guid get();
			}

			property float PreviousVolume {
				float get();
			}

			property float Volume {
				float get();
			}
		};

		/// <summary>
		/// Represents an audio endpoint
		/// </summary>
		public ref class AudioDevice : public IDisposable {
		private:
			IMMDevice* pDevice;
			IPropertyStore* pProps;
			IAudioEndpointVolume* pVolume;

			IAudioEndpointVolumeCallback* volumeCallback;

			String^ _id;
			DeviceType _type;
			String^ _name;
			String^ _friendlyName;
			String^ _description;

			~AudioDevice();

			String^ GetPropertyAsString(const PROPERTYKEY key);
		internal:
			AudioDevice(IMMDevice* pDevice);

			void OnMuteStatusChanged(MuteStatusChangedEventArgs^ e);
			void OnMasterVolumeChanged(VolumeChangedEventArgs^ e);
		public:
			!AudioDevice();

			event EventHandler<MuteStatusChangedEventArgs^>^ MuteStatusChanged;
			event EventHandler<VolumeChangedEventArgs^>^ MasterVolumeChanged;

			/// <summary>
			/// Gets the system defined identifier for the endpoint
			/// </summary>
			property String^ Id {
				String^ get();
			}

			/// <summary>
			/// Gets the current device state
			/// </summary>
			property DeviceState State {
				DeviceState get();
			}

			/// <summary>
			/// Gets the current device role
			/// </summary>
			property DeviceType Type {
				DeviceType get();
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

			property bool IsMuted {
				/// <summary>
				/// Returns whether the device is muted.
				/// </summary>
				bool get();
			}

			/// <summary>
			/// Mutes the device
			/// </summary>
			void Mute();

			/// <summary>
			/// Unmutes the device
			/// </summary>
			void Unmute();

			virtual bool Equals(Object^ otherDevice) override;
			virtual bool Equals(AudioDevice^ otherDevice);
			virtual int GetHashCode() override;
			static bool operator!= (AudioDevice^ device1, AudioDevice^ device2);
			static bool operator== (AudioDevice^ device1, AudioDevice^ device2);
			virtual String^ ToString() override;
		};

		private class CMMNotificationClient : public IMMNotificationClient {
		private:
			LONG _cRef;
			GCHandle hController;
		public:
			CMMNotificationClient(void* pController);
			~CMMNotificationClient();

			ULONG STDMETHODCALLTYPE AddRef();
			ULONG STDMETHODCALLTYPE Release();
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);

			HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDevice);
			HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR pwstrDeviceId);
			HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR pwstrDeviceId);
			HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState);
			HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key);
		};

		private class CAudioEndpointVolumeCallback : public IAudioEndpointVolumeCallback {
		private:
			LONG _cRef;
			GCHandle hDevice;

			BOOL bCurrMuted;
			float fCurrMasterVolume;
		public:
			CAudioEndpointVolumeCallback(void* pDevice);
			~CAudioEndpointVolumeCallback();

			ULONG STDMETHODCALLTYPE AddRef();
			ULONG STDMETHODCALLTYPE Release();
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);

			HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify)
		};

		public ref class Controller : public IDisposable {
		private:
			IMMNotificationClient* notificationClient;
			IMMDeviceEnumerator* deviceEnumerator;

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
