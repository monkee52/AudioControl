#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* public */ CAudioSessionNotification::CAudioSessionNotification(void* pDevice) {
			this->_cRef = 1;

			// Get reference back to managed device
			IntPtr mpDevice(pDevice);
			GCHandle hDevice = GCHandle::FromIntPtr(mpDevice);

			this->hDevice = hDevice;
		}

		/* public */ CAudioSessionNotification::~CAudioSessionNotification() {
			// Free weak GCHandle when this class is no longer needed
			this->hDevice.Free();
		}

		/* public */ ULONG CAudioSessionNotification::AddRef() {
			// Increment internal reference count
			return InterlockedIncrement(&this->_cRef);
		}

		/* public */ ULONG CAudioSessionNotification::Release() {
			// Decrement internal reference count
			ULONG ulRef = InterlockedDecrement(&this->_cRef);

			// Cleanup if necessary
			if (ulRef == 0) {
				delete this;
			}

			return ulRef;
		}

		/* public */ HRESULT CAudioSessionNotification::QueryInterface(REFIID riid, VOID** ppvInterface) {
			if (riid == IID_IUnknown) { // All COM classes need to implement IUnknown
				this->AddRef();

				*ppvInterface = (IUnknown*)this;
			}
			else if (riid == __uuidof(IAudioSessionNotification)) {
				this->AddRef();

				*ppvInterface = (IAudioSessionNotification*)this;
			}
			else {
				*ppvInterface = nullptr;

				return E_NOINTERFACE;
			}

			return S_OK;
		}

		/* public */ HRESULT CAudioSessionNotification::OnSessionCreated(IAudioSessionControl* pSession) {
			IAudioSessionControl2* pSession2 = nullptr;
			
			HRESULT hr = pSession->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSession2);

			if (FAILED(hr)) {
				return hr;
			}

			AudioDevice^ device = (AudioDevice^)this->hDevice.Target;

			if (device != nullptr) {
				AudioSession^ session = gcnew AudioSession(device, pSession2);

				device->OnSessionCreated(session);
			}

			Utilities::SafeRelease((IUnknown**)&pSession2);

			return S_OK;
		}
	}
}
