#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* public */ CAudioEndpointVolumeCallback::CAudioEndpointVolumeCallback(void* pDevice) {
			this->_cRef = 1;

			// Get reference back to managed device
			IntPtr mpDevice(pDevice);
			GCHandle hController = GCHandle::FromIntPtr(mpDevice);

			this->hDevice = hDevice;

			this->bCurrMuted = ((AudioDevice^)this->hDevice.Target)->IsMuted;
			//this->fCurrMasterVolume = ((AudioDevice^)this->hDevice.Target)->MasterVolume;
		}

		/* public */ CAudioEndpointVolumeCallback::~CAudioEndpointVolumeCallback() {
			// Free weak GCHandle when this class is no longer needed
			this->hDevice.Free();
		}

		/* public */ ULONG CAudioEndpointVolumeCallback::AddRef() {
			// Increment internal reference count
			return InterlockedIncrement(&this->_cRef);
		}

		/* public */ ULONG CAudioEndpointVolumeCallback::Release() {
			// Decrement internal reference count
			ULONG ulRef = InterlockedDecrement(&this->_cRef);

			// Cleanup if necessary
			if (ulRef == 0) {
				delete this;
			}

			return ulRef;
		}

		/* public */ HRESULT CAudioEndpointVolumeCallback::QueryInterface(REFIID riid, VOID** ppvInterface) {
			if (riid == IID_IUnknown) { // All COM classes need to implement IUnknown
				this->AddRef();

				*ppvInterface = (IUnknown*)this;
			} else if (riid == __uuidof(IAudioEndpointVolumeCallback)) {
				this->AddRef();

				*ppvInterface = (IAudioEndpointVolumeCallback*)this;
			} else {
				*ppvInterface = nullptr;

				return E_NOINTERFACE;
			}

			return S_OK;
		}

		/* public */ HRESULT CAudioEndpointVolumeCallback::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
			if (pNotify == nullptr) {
				return E_INVALIDARG;
			}

			AudioDevice^ controller = (AudioDevice^)this->hDevice.Target;
			Guid guidEventContext = Utilities::ConvertNativeGuidToGuid(pNotify->guidEventContext);

			// Handle mute status changes
			if (pNotify->bMuted != this->bCurrMuted) {
				if (controller != nullptr) {
					MuteStatusChangedEventArgs^ args = gcnew MuteStatusChangedEventArgs(guidEventContext, this->bCurrMuted, pNotify->bMuted);

					controller->OnMuteStatusChanged(args);
				}

				this->bCurrMuted = pNotify->bMuted;
			}

			// Handle master volume changes
			if (pNotify->fMasterVolume != this->fCurrMasterVolume) {
				if (controller != nullptr) {
					VolumeChangedEventArgs^ args = gcnew VolumeChangedEventArgs(guidEventContext, this->fCurrMasterVolume, pNotify->fMasterVolume);

					controller->OnMasterVolumeChanged(args);
				}

				this->fCurrMasterVolume = pNotify->fMasterVolume;
			}

			return S_OK;
		}
	}
}
