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

		public enum class SessionState {
			Inactive = AudioSessionStateInactive, // The audio session is inactive. (It contains at least one stream, but none of the streams in the session is currently running.)
			Active = AudioSessionStateActive, // The audio session is active. (At least one of the streams in the session is running.)
			Expired = AudioSessionStateExpired // The audio session has expired. (It contains no streams.)
		};

		enum class PropertyKey {
			Name,
			FriendlyName,
			Description,
			DefaultFormat,
			CurrentFormat
		};

		public enum class SessionDisconnectReason {
			Removal = DisconnectReasonDeviceRemoval, // The user removed the audio endpoint device.
			ServerShutdown = DisconnectReasonServerShutdown, // The Windows audio service has stopped.
			FormatChanged = DisconnectReasonFormatChanged, // The stream format changed for the device that the audio session is connected to.
			SessionLogoff = DisconnectReasonSessionLogoff, // The user logged off the Windows Terminal Services (WTS) session that the audio session was running in.
			SessionDisconnected = DisconnectReasonSessionDisconnected, // The WTS session that the audio session was running in was disconnected.
			ExclusiveModeOverride = DisconnectReasonExclusiveModeOverride // The (shared-mode) audio session was disconnected to make the audio endpoint device available for an exclusive-mode connection.
		};

		ref class Controller;
		ref class AudioDevice;
		ref class AudioSession;

		private ref class Utilities {
		public:
			static String^ ConvertHrToString(HRESULT hr);
			static Guid ConvertNativeGuidToGuid(const GUID g);
			static Guid ConvertNativeGuidToGuid(LPCGUID g);
			static void SafeRelease(IUnknown** ppT);
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

		public ref class DeviceStateChangedEventArgs : public EventArgs {
		private:
			String^ _deviceId;
			DeviceState _previousState;
			DeviceState _state;
		internal:
			DeviceStateChangedEventArgs(String^ id, DeviceState newState);

			property String^ DeviceId {
				String^ get();
			}
		public:
			property DeviceState PreviousState {
				DeviceState get();

			internal:
				void set(DeviceState oldState);
			}

			property DeviceState State {
				DeviceState get();
			}
		};

		public ref class DisplayNameChangedEventArgs : public EventArgs {
		private:
			Guid _evContext;
			String^ _previousDisplayName;
			String^ _displayName;
		internal:
			DisplayNameChangedEventArgs(Guid evContext, String^ oldDisplayName, String^ newDisplayName);
		public:
			property Guid Context {
				Guid get();
			}

			property String^ PreviousDisplayName {
				String^ get();
			}

			property String^ DisplayName {
				String^ get();
			}
		};

		public ref class SessionStateChangedEventArgs : public EventArgs {
		private:
			SessionState _previousState;
			SessionState _state;
		internal:
			SessionStateChangedEventArgs(SessionState oldState, SessionState newState);
		public:
			property SessionState PreviousState {
				SessionState get();
			}

			property SessionState State {
				SessionState get();
			}
		};

		public ref class PropertyValueChangedEventArgs : public EventArgs {
		private:
			String^ _deviceId;
			PropertyKey _key;
		internal:
			PropertyValueChangedEventArgs(String^ deviceId, PropertyKey key);

			property String^ DeviceId {
				String^ get();
			}

			property PropertyKey Key {
				PropertyKey get();
			}
		};

		public ref class NameChangedEventArgs : public EventArgs {
		private:
			String^ _previousName;
			String^ _name;
		internal:
			NameChangedEventArgs(String^ oldName, String^ newName);
		public:
			property String^ PreviousName {
				String^ get();
			}

			property String^ Name {
				String^ get();
			}
		};

		public ref class FriendlyNameChangedEventArgs : public EventArgs {
		private:
			String^ _previousFriendlyName;
			String^ _friendlyName;
		internal:
			FriendlyNameChangedEventArgs(String^ oldFriendlyName, String^ newFriendlyName);
		public:
			property String^ PreviousFriendlyName {
				String^ get();
			}

			property String^ FriendlyName {
				String^ get();
			}
		};

		public ref class DescriptionChangedEventArgs : public EventArgs {
		private:
			String^ _previousDescription;
			String^ _description;
		internal:
			DescriptionChangedEventArgs(String^ oldDescription, String^ newDescription);
		public:
			property String^ PreviousDescription {
				String^ get();
			}

			property String^ Description {
				String^ get();
			}
		};

		public ref class SessionCreatedEventArgs : public EventArgs {
		private:
			AudioSession^ _session;
		internal:
			SessionCreatedEventArgs(AudioSession^ session);
		public:
			property AudioSession^ Session {
				AudioSession^ get();
			}
		};

		public ref class SessionDisconnectedEventArgs : public EventArgs {
		private:
			SessionDisconnectReason _reason;
		internal:
			SessionDisconnectedEventArgs(SessionDisconnectReason reason);
		public:
			property SessionDisconnectReason Reason {
				SessionDisconnectReason get();
			}
		};

		public ref class DefaultDeviceChangedEventArgs : public EventArgs {
		private:
			AudioDevice^ _device;
			DeviceType _type;
			DeviceRole _role;
		internal:
			DefaultDeviceChangedEventArgs(AudioDevice^ device, DeviceType type, DeviceRole role);
		public:
			property AudioDevice^ Device {
				AudioDevice^ get();
			}

			property DeviceType Type {
				DeviceType get();
			}

			property DeviceRole Role {
				DeviceRole get();
			}
		};

		private class CAudioSessionEvents : public IAudioSessionEvents {
		private:
			LONG _cRef;
			GCHandle hSession;
		public:
			CAudioSessionEvents(void* pSession);
			~CAudioSessionEvents();

			ULONG STDMETHODCALLTYPE AddRef();
			ULONG STDMETHODCALLTYPE Release();
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);

			HRESULT STDMETHODCALLTYPE OnChannelVolumeChanged(DWORD channelCount, float newChannelVolumes[], DWORD changedChannel, LPCGUID pGuidEvContext);
			HRESULT STDMETHODCALLTYPE OnDisplayNameChanged(LPCWSTR newDisplayName, LPCGUID pGuidEvContext);
			HRESULT STDMETHODCALLTYPE OnGroupingParamChanged(LPCGUID newGroupingParam, LPCGUID pGuidEvContext);
			HRESULT STDMETHODCALLTYPE OnIconPathChanged(LPCWSTR newIconPath, LPCGUID pGuidEvContext);
			HRESULT STDMETHODCALLTYPE OnSessionDisconnected(AudioSessionDisconnectReason disconnectReason);
			HRESULT STDMETHODCALLTYPE OnSimpleVolumeChanged(float newVolume, BOOL newMute, LPCGUID pGuidEvContext);
			HRESULT STDMETHODCALLTYPE OnStateChanged(AudioSessionState newState);
		};

		private class CAudioSessionNotification : public IAudioSessionNotification {
		private:
			LONG _cRef;
			GCHandle hDevice;
		public:
			CAudioSessionNotification(void* pDevice);
			~CAudioSessionNotification();

			ULONG STDMETHODCALLTYPE AddRef();
			ULONG STDMETHODCALLTYPE Release();
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);

			HRESULT STDMETHODCALLTYPE OnSessionCreated(IAudioSessionControl* pNewSession);
		};

		public ref class AudioSession : public IDisposable {
		private:
			AudioDevice^ device;

			IAudioSessionControl2* pControl;
			ISimpleAudioVolume* pVolume;

			IAudioSessionEvents* events;
			bool _isSystemSounds;
			int _processId;
			String^ _id;
			String^ _instanceId;
			String^ _displayName;
			float _currVolume;
			bool _currMuteStatus;
			SessionState _state;

			~AudioSession();

			void Cleanup();
		internal:
			AudioSession(AudioDevice^ device, IAudioSessionControl2* pControl);

			void OnDisplayNameChanged(Guid evContext, String^ newDisplayName);
			void OnVolumeControlChanged(Guid evContext, bool newMuteStatus, float newVolume);
			void OnSessionStateChanged(SessionState newState);
			void OnSessionDisconnected(SessionDisconnectReason reason);
		public:
			!AudioSession();

			event EventHandler<DisplayNameChangedEventArgs^>^ DisplayNameChanged;
			event EventHandler<MuteStatusChangedEventArgs^>^ MuteStatusChanged;
			event EventHandler<VolumeChangedEventArgs^>^ MasterVolumeChanged;
			event EventHandler<SessionStateChangedEventArgs^>^ StateChanged;
			event EventHandler<SessionDisconnectedEventArgs^>^ Disconnected;

			property String^ Id {
				String^ get();
			}

			property String^ InstanceId {
				String^ get();
			}

			property int ProcessId {
				int get();
			}

			property String^ DisplayName {
				String^ get();
				void set(String^ newDisplayName);
			}

			property SessionState State {
				SessionState get();
			}

			property bool IsSystemSoundsSession {
				bool get();
			}

			property float MasterVolume {
				float get();
				void set(float newVolume);
			}

			property bool IsMuted {
				bool get();
			}

			void Mute();
			void Unmute();
		};

		/// <summary>
		/// Represents an audio endpoint
		/// </summary>
		public ref class AudioDevice : public IDisposable {
		private:
			Controller^ controller;

			IMMDevice* pDevice;
			IPropertyStore* pProps;
			IAudioEndpointVolume* pVolume;
			IAudioSessionManager2* pSessMgr;

			IAudioEndpointVolumeCallback* volumeCallback;
			IAudioSessionNotification* sessionNotification;
			DeviceState _currState;
			bool _currMuteStatus;
			float _currMasterVolume;

			String^ _id;
			DeviceType _type;
			String^ _name;
			String^ _friendlyName;
			String^ _description;

			~AudioDevice();

			String^ GetPropertyAsString(const PROPERTYKEY key);
			void Cleanup();
		internal:
			AudioDevice(Controller^ controller, IMMDevice* pDevice);

			void OnMuteStatusChanged(Guid evContext, bool newMuteStatus);
			void OnMasterVolumeChanged(Guid evContext, float newMasterVolume);
			void OnSessionCreated(AudioSession^ session);

			void OnDeviceStateChanged(DeviceStateChangedEventArgs^ e);
			void OnDeviceStateChanged(Object^ sender, DeviceStateChangedEventArgs^ e);

			void OnPropertyValueChanged(PropertyValueChangedEventArgs^ e);
			void OnPropertyValueChanged(Object^ sender, PropertyValueChangedEventArgs^ e);
		public:
			!AudioDevice();

			event EventHandler<MuteStatusChangedEventArgs^>^ MuteStatusChanged;
			event EventHandler<VolumeChangedEventArgs^>^ MasterVolumeChanged;
			event EventHandler<DeviceStateChangedEventArgs^>^ StateChanged;
			event EventHandler<NameChangedEventArgs^>^ NameChanged;
			event EventHandler<FriendlyNameChangedEventArgs^>^ FriendlyNameChanged;
			event EventHandler<DescriptionChangedEventArgs^>^ DescriptionChanged;

			event EventHandler<SessionCreatedEventArgs^>^ SessionCreated;

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

				/// <summary>
				/// Sets whether the device is muted.
				/// </summary>
				void set(bool newMuteStatus);
			}

			/// <summary>
			/// Mutes the device
			/// </summary>
			void Mute();

			/// <summary>
			/// Unmutes the device
			/// </summary>
			void Unmute();

			property float MasterVolume {
				/// <summary>
				/// Returns the current master volume
				/// </summary>
				float get();
				
				/// <summary>
				/// Sets the current master volume
				/// </summary>
				void set(float newVolume);
			}

			property bool SystemEffectsEnabled {
				/// <summary>
				/// Returns whether system fx are enabled
				/// </summary>
				bool get();

				/// <summary>
				/// Sets whether system fx are enabled. Requires administrator privileges
				/// </summary>
				void set(bool newFxEnabled);
			}

			array<AudioSession^>^ GetSessions();

			virtual bool Equals(Object^ otherDevice) override;
			virtual bool Equals(AudioDevice^ otherDevice);
			virtual int GetHashCode() override;
			static bool operator!= (AudioDevice^ device1, AudioDevice^ device2);
			static bool operator== (AudioDevice^ device1, AudioDevice^ device2);
			virtual String^ ToString() override;
		};

		public ref class DeviceEventArgs : public EventArgs {
		private:
			AudioDevice^ _device;
		internal:
			DeviceEventArgs(AudioDevice^ device);
		public:
			property AudioDevice^ Device {
				AudioDevice^ get();
			}
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
		public:
			CAudioEndpointVolumeCallback(void* pDevice);
			~CAudioEndpointVolumeCallback();

			ULONG STDMETHODCALLTYPE AddRef();
			ULONG STDMETHODCALLTYPE Release();
			HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, VOID** ppvInterface);

			HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
		};

		public ref class Controller : public IDisposable {
		private:
			IMMNotificationClient* notificationClient;
			IMMDeviceEnumerator* deviceEnumerator;

			~Controller();
		internal:
			event EventHandler<DeviceStateChangedEventArgs^>^ DeviceStateChanged;
			void OnDeviceStateChanged(String^ deviceId, DeviceState newState);

			event EventHandler<PropertyValueChangedEventArgs^>^ PropertyValueChanged;
			void OnPropertyValueChanged(String^ deviceId, PropertyKey key);

			void OnDeviceAdded(String^ deviceId);
			void OnDeviceRemoved(String^ deviceId);
			void OnDefaultDeviceChanged(String^ deviceId, DeviceType type, DeviceRole role);
		public:
			event EventHandler<DeviceEventArgs^>^ DeviceAdded;
			event EventHandler<DeviceEventArgs^>^ DeviceRemoved;
			event EventHandler<DefaultDeviceChangedEventArgs^>^ DefaultDeviceChanged;

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
			void SetDefaultAudioDevice(AudioDevice^ device, DeviceRole role);
			
			/// <summary>
			/// Sets the default render endpoint
			/// </summary>
			/// <param name="endpoint">The endpoint</param>
			void SetDefaultAudioDevice(String^ deviceId, DeviceRole role);
		};
	}
}
