#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* public */CMMNotificationClient::CMMNotificationClient(void* pController) {
			this->_cRef = 1;

			// Get reference back to managed Controller
			IntPtr mpController(pController);
			GCHandle hController = GCHandle::FromIntPtr(mpController);

			this->hController = hController;
		}

		/* public */CMMNotificationClient::~CMMNotificationClient() {
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
			Controller^ controller = (Controller^)this->hController.Target;

			if (controller != nullptr) {
				controller->OnDeviceAdded(gcnew String(pwstrDeviceId));
			}

			return S_OK;
		}

		/* public */ HRESULT CMMNotificationClient::OnDeviceRemoved(LPCWSTR pwstrDeviceId) {
			Controller^ controller = (Controller^)this->hController.Target;

			if (controller != nullptr) {
				controller->OnDeviceRemoved(gcnew String(pwstrDeviceId));
			}

			return S_OK;
		}

		/* public */ HRESULT CMMNotificationClient::OnDeviceStateChanged(LPCWSTR pwstrDeviceId, DWORD dwNewState) {
			Controller^ controller = (Controller^)this->hController.Target;

			if (controller != nullptr) {
				controller->OnDeviceStateChanged(gcnew String(pwstrDeviceId), (DeviceState)dwNewState);
			}

			return S_OK;
		}

		/* public */ HRESULT CMMNotificationClient::OnPropertyValueChanged(LPCWSTR pwstrDeviceId, const PROPERTYKEY key) {
			Controller^ controller = (Controller^)this->hController.Target;

			if (controller != nullptr) {
				String^ deviceId = gcnew String(pwstrDeviceId);

				if (key == PKEY_DeviceInterface_FriendlyName) {
					controller->OnPropertyValueChanged(deviceId, PropertyKey::Name);
				} else if (key == PKEY_Device_FriendlyName) {
					controller->OnPropertyValueChanged(deviceId, PropertyKey::FriendlyName);
				} else if (key == PKEY_Device_DeviceDesc) {
					controller->OnPropertyValueChanged(deviceId, PropertyKey::Description);
				} else if (key == PKEY_AudioEngine_OEMFormat) {
					controller->OnPropertyValueChanged(deviceId, PropertyKey::DefaultFormat);
				} else if (key == PKEY_AudioEngine_DeviceFormat) {
					controller->OnPropertyValueChanged(deviceId, PropertyKey::CurrentFormat);
				}
			}

			return S_OK;
		}
	}
}
