#include "stdafx.h"

#include "AudioControl.h"
#include "CMMNotificationClient.h"

namespace AydenIO {
	namespace AudioControl {
		CMMNotificationClient::CMMNotificationClient(void* pController) {
			this->_cRef = 1;

			// Get reference back to managed Controller
			IntPtr mpController(pController);
			GCHandle hController = GCHandle::FromIntPtr(mpController);

			this->hController = hController;
		}

		CMMNotificationClient::~CMMNotificationClient() {
			// Free weak GCHandle when this class is no longer needed
			this->hController.Free();
		}

		/* public */ ULONG CMMNotificationClient::AddRef() {
			// Increment internal reference count
			return InterlockedIncrement(&this->_cRef);
		}

		/* public */ ULONG CMMNotificationClient::Release() {
			 // Decrement internal reference count
			ULONG ulRef = InterlockedDecrement(&this->_cRef);

			// Cleanup if necessary
			if (ulRef == 0) {
				delete this;
			}

			return ulRef;
		}

		/* public */ HRESULT CMMNotificationClient::QueryInterface(REFIID riid, VOID** ppvInterface) {
			if (riid == IID_IUnknown) { // All COM classes need to implement IUnknown
				this->AddRef();

				*ppvInterface = (IUnknown*)this;
			} else if (riid == __uuidof(IMMNotificationClient)) {
				this->AddRef();

				*ppvInterface = (IMMNotificationClient*)this;
			} else {
				*ppvInterface = nullptr;

				return E_NOINTERFACE;
			}

			return S_OK;
		}

		/* public */ HRESULT CMMNotificationClient::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDevice) {
			//((Controller^)this->hController.Target)->

			return S_OK;
		}

		/* public */ HRESULT CMMNotificationClient::OnDeviceAdded(LPCWSTR pwstrDeviceId) {
			//((Controller^)this->hController.Target)->

			return S_OK;
		}

		/* public */ HRESULT CMMNotificationClient::OnDeviceRemoved(LPCWSTR pwstrDeviceId) {
			//((Controller^)this->hController.Target)->

			return S_OK;
		}

		/* public */ HRESULT CMMNotificationClient::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) {
			//((Controller^)this->hController.Target)->

			return S_OK;
		}

		/* public */ HRESULT CMMNotificationClient::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) {
			//((Controller^)this->hController.Target)->

			return S_OK;
		}
	}
}
