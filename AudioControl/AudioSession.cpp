#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ AudioSession::AudioSession(AudioDevice^ device, IAudioSessionControl2* pControl) {
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
				// Cleanup
				if (this->pControl != nullptr) {
					this->pControl->Release();
					this->pControl = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->pVolume = pVolume;

			// Cache process id
			DWORD dProcessId;

			hr = this->pControl->GetProcessId(&dProcessId);

			if (FAILED(hr)) {
				// Cleanup
				if (this->pVolume != nullptr) {
					this->pVolume->Release();
					this->pVolume = nullptr;
				}

				if (this->pControl != nullptr) {
					this->pControl->Release();
					this->pControl = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_processId = dProcessId;

			// Cache session id
			LPWSTR pwszId = nullptr;

			hr = this->pControl->GetSessionIdentifier(&pwszId);

			if (FAILED(hr)) {
				// Cleanup
				if (this->pVolume != nullptr) {
					this->pVolume->Release();
					this->pVolume = nullptr;
				}

				if (this->pControl != nullptr) {
					this->pControl->Release();
					this->pControl = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_id = gcnew String(pwszId);

			CoTaskMemFree(pwszId);

			// Cache session instance id
			hr = this->pControl->GetSessionInstanceIdentifier(&pwszId);

			if (FAILED(hr)) {
				// Cleanup
				if (this->pVolume != nullptr) {
					this->pVolume->Release();
					this->pVolume = nullptr;
				}

				if (this->pControl != nullptr) {
					this->pControl->Release();
					this->pControl = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_instanceId = gcnew String(pwszId);

			CoTaskMemFree(pwszId);

			// Cache display name
			hr = this->pControl->GetDisplayName(&pwszId);

			if (FAILED(hr)) {
				// Cleanup
				if (this->pVolume != nullptr) {
					this->pVolume->Release();
					this->pVolume = nullptr;
				}

				if (this->pControl != nullptr) {
					this->pControl->Release();
					this->pControl = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->_displayName = gcnew String(pwszId);

			CoTaskMemFree(pwszId);
		}

		/* private */ AudioSession::~AudioSession() {
			if (this->pVolume != nullptr) {
				this->pVolume->Release();
				this->pVolume = nullptr;
			}

			if (this->pControl != nullptr) {
				this->pControl->Release();
				this->pControl = nullptr;
			}
		}

		/* public */ AudioSession::!AudioSession() {
			delete this;
		}

		/* public */ bool AudioSession::IsSystemSoundsSession::get() {
			return this->_isSystemSounds;
		}

		/* public */ bool AudioSession::IsMuted::get() {
			BOOL bMuted;

			HRESULT hr = this->pVolume->GetMute(&bMuted);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			return (bool)bMuted;
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
			float fVolume;

			HRESULT hr = this->pVolume->GetMasterVolume(&fVolume);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			return fVolume;
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
	}
}
