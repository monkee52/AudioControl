#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ AudioSession::AudioSession(AudioDevice^ device, IAudioSessionControl2* pControl) {
			// Init
			this->pVolume = nullptr;
			this->events = nullptr;

			this->device = device;

			this->pControl = pControl;
			this->pControl->AddRef();

			// Cache if system sounds
			HRESULT hr = this->pControl->IsSystemSoundsSession();

			this->_isSystemSounds = hr == S_OK;

			// Get volume control
			ISimpleAudioVolume* pVolume = nullptr;

			hr = this->pControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pVolume);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->pVolume = pVolume;

			// Cache mute status
			BOOL bMuted;

			hr = this->pVolume->GetMute(&bMuted);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_currMuteStatus = bMuted;

			// Cache volume
			float fVolume;

			hr = this->pVolume->GetMasterVolume(&fVolume);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_currVolume = fVolume;

			// Cache process id
			DWORD dProcessId;

			hr = this->pControl->GetProcessId(&dProcessId);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_processId = dProcessId;

			// Cache session id
			LPWSTR pwszId = nullptr;

			hr = this->pControl->GetSessionIdentifier(&pwszId);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_id = gcnew String(pwszId);

			CoTaskMemFree(pwszId);

			// Cache session instance id
			hr = this->pControl->GetSessionInstanceIdentifier(&pwszId);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_instanceId = gcnew String(pwszId);

			CoTaskMemFree(pwszId);

			// Cache display name
			hr = this->pControl->GetDisplayName(&pwszId);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_displayName = gcnew String(pwszId);

			CoTaskMemFree(pwszId);

			// Cache state
			AudioSessionState state;

			hr = this->pControl->GetState(&state);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_state = (SessionState)state;

			// Get weak handle to this
			GCHandle hThis = GCHandle::Alloc(this, GCHandleType::Weak);

			// Register events
			this->events = new CAudioSessionEvents(GCHandle::ToIntPtr(hThis).ToPointer());

			hr = this->pControl->RegisterAudioSessionNotification(this->events);

			if (FAILED(hr)) {
				this->Cleanup();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		/* private */ void AudioSession::Cleanup() {
			if (this->pVolume != nullptr) {
				this->pVolume->Release();
				this->pVolume = nullptr;
			}

			if (this->pControl != nullptr) {
				if (this->pControl != nullptr) {
					this->pControl->UnregisterAudioSessionNotification(this->events);
				}

				this->events->Release();
				this->events = nullptr;
			}

			if (this->pControl != nullptr) {
				this->pControl->Release();
				this->pControl = nullptr;
			}
		}

		/* private */ AudioSession::~AudioSession() {
			this->Cleanup();
		}

		/* public */ AudioSession::!AudioSession() {
			delete this;
		}

		/* public */ bool AudioSession::IsSystemSoundsSession::get() {
			return this->_isSystemSounds;
		}

		/* public */ bool AudioSession::IsMuted::get() {
			return this->_currMuteStatus;
		}

		/* public */ void AudioSession::Mute() {
			HRESULT hr = this->pVolume->SetMute(TRUE, NULL);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		/* public */ void AudioSession::Unmute() {
			HRESULT hr = this->pVolume->SetMute(TRUE, NULL);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		/* public */ float AudioSession::MasterVolume::get() {
			return this->_currVolume;
			
		}

		/* public */ void AudioSession::MasterVolume::set(float newVolume) {
			HRESULT hr = this->pVolume->SetMasterVolume(newVolume, NULL);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		/* public */ String^ AudioSession::Id::get() {
			return this->_id;
		}

		/* public */ String^ AudioSession::InstanceId::get() {
			return this->_instanceId;
		}

		/* public */ int AudioSession::ProcessId::get() {
			return this->_processId;
		}

		/* public */ String^ AudioSession::DisplayName::get() {
			return this->_displayName;
		}

		/* public */ void AudioSession::DisplayName::set(String^ newDisplayName) {
			IntPtr hDisplayName = Marshal::StringToHGlobalUni(newDisplayName);
			LPWSTR wszDisplayName = (LPWSTR)hDisplayName.ToPointer();

			HRESULT hr = this->pControl->SetDisplayName(wszDisplayName, NULL);

			// Cleanup native string
			Marshal::FreeHGlobal(hDisplayName);
			wszDisplayName = nullptr;

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		/* public */ SessionState AudioSession::State::get() {
			return this->_state;
		}

		/* internal */ void AudioSession::OnDisplayNameChanged(Guid evContext, String^ newDisplayName) {
			if (newDisplayName != this->_displayName) {
				String^ oldDisplayName = this->_displayName;

				this->_displayName = newDisplayName;

				this->DisplayNameChanged(this, gcnew DisplayNameChangedEventArgs(evContext, oldDisplayName, newDisplayName));
			}
		}

		/* internal */ void AudioSession::OnVolumeControlChanged(Guid evContext, bool newMuteStatus, float newVolume) {
			// Handle different mute
			if (newMuteStatus != this->_currMuteStatus) {
				bool oldMuteStatus = this->_currMuteStatus;

				this->_currMuteStatus = newMuteStatus;

				this->MuteStatusChanged(this, gcnew MuteStatusChangedEventArgs(evContext, oldMuteStatus, newMuteStatus));
			}

			// Handle different volume
			if (newVolume != this->_currVolume) {
				float oldVolume = this->_currVolume;

				this->_currVolume = newVolume;

				this->MasterVolumeChanged(this, gcnew VolumeChangedEventArgs(evContext, oldVolume, newVolume));
			}
		}

		/* internal */ void AudioSession::OnSessionStateChanged(SessionState newState) {
			if (newState != this->_state) {
				SessionState oldState = this->_state;

				this->_state = newState;

				this->StateChanged(this, gcnew SessionStateChangedEventArgs(oldState, newState));
			}
		}
	}
}
