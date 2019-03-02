#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* public */ CAudioSessionEvents::CAudioSessionEvents(void* pSession) {
			this->_cRef = 1;

			// Get reference back to managed session
			IntPtr mpSession(pSession);
			GCHandle hSession = GCHandle::FromIntPtr(mpSession);

			this->hSession = hSession;
		}

		/* public */ CAudioSessionEvents::~CAudioSessionEvents() {
			// Free weak GCHandle when this class is no longer needed
			this->hSession.Free();
		}

		/* public */ ULONG CAudioSessionEvents::AddRef() {
			// Increment internal reference count
			return InterlockedIncrement(&this->_cRef);
		}

		/* public */ ULONG CAudioSessionEvents::Release() {
			// Decrement internal reference count
			ULONG ulRef = InterlockedDecrement(&this->_cRef);

			// Cleanup if necessary
			if (ulRef == 0) {
				delete this;
			}

			return ulRef;
		}

		/* public */ HRESULT CAudioSessionEvents::QueryInterface(REFIID riid, VOID** ppvInterface) {
			if (riid == IID_IUnknown) { // All COM classes need to implement IUnknown
				this->AddRef();

				*ppvInterface = (IUnknown*)this;
			}
			else if (riid == __uuidof(IAudioSessionEvents)) {
				this->AddRef();

				*ppvInterface = (IAudioSessionEvents*)this;
			}
			else {
				*ppvInterface = nullptr;

				return E_NOINTERFACE;
			}

			return S_OK;
		}

		/* public */ HRESULT CAudioSessionEvents::OnChannelVolumeChanged(DWORD channelCount, float newChannelVolumes[], DWORD changedChannel, LPCGUID pGuidEvContext) {
			return S_OK;
		}

		/* public */ HRESULT CAudioSessionEvents::OnDisplayNameChanged(LPCWSTR newDisplayName, LPCGUID pGuidEvContext) {
			if (newDisplayName == nullptr) {
				return E_INVALIDARG;
			}

			AudioSession^ session = (AudioSession^)this->hSession.Target;
			Guid evContext = Utilities::ConvertNativeGuidToGuid(pGuidEvContext);

			if (session != nullptr) {
				session->OnDisplayNameChanged(evContext, gcnew String(newDisplayName));
			}

			return S_OK;
		}

		/* public */ HRESULT CAudioSessionEvents::OnGroupingParamChanged(LPCGUID newGroupingParam, LPCGUID pGuidEvContext) {
			return S_OK;
		}

		/* public */ HRESULT CAudioSessionEvents::OnIconPathChanged(LPCWSTR newIconPath, LPCGUID pGuidEvContext) {
			return S_OK;
		}

		/* public */ HRESULT CAudioSessionEvents::OnSessionDisconnected(AudioSessionDisconnectReason disconnectReason) {
			AudioSession^ session = (AudioSession^)this->hSession.Target;

			if (session != nullptr) {
				session->OnSessionDisconnected((SessionDisconnectReason)disconnectReason);
			}

			return S_OK;
		}

		/* public */ HRESULT CAudioSessionEvents::OnSimpleVolumeChanged(float newVolume, BOOL newMute, LPCGUID pGuidEvContext) {
			AudioSession^ session = (AudioSession^)this->hSession.Target;
			Guid evContext = Utilities::ConvertNativeGuidToGuid(pGuidEvContext);

			if (session != nullptr) {
				session->OnVolumeControlChanged(evContext, newMute, newVolume);
			}

			return S_OK;
		}

		/* public */ HRESULT CAudioSessionEvents::OnStateChanged(AudioSessionState newState) {
			AudioSession^ session = (AudioSession^)this->hSession.Target;
			
			if (session != nullptr) {
				session->OnSessionStateChanged((SessionState)newState);
			}

			return S_OK;
		}
	}
}
