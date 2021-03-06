#include "stdafx.h"

#include "AudioControl.h"
#include "PolicyConfig.h"

namespace AydenIO {
	namespace AudioControl {
		/* public */ Controller::Controller() {
			// Init COM
			HRESULT hr = CoInitialize(NULL);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(String::Format(gcnew String(_T("Could not initialize COM: {0}")), Utilities::ConvertHrToString(hr)));
			}

			// Get device enumerator
			IMMDeviceEnumerator* pEnumerator = nullptr;

			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));

			if (FAILED(hr)) {
				// Cleanup
				CoUninitialize();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// pEnumerator allocated on stack to avoid memory pinning
			this->deviceEnumerator = pEnumerator;

			// Get weak handle to this
			GCHandle hThis = GCHandle::Alloc(this, GCHandleType::Weak);

			// Create notification client and register it
			this->notificationClient = new CMMNotificationClient(GCHandle::ToIntPtr(hThis).ToPointer());

			hr = this->deviceEnumerator->RegisterEndpointNotificationCallback(this->notificationClient);

			if (FAILED(hr)) {
				// Cleanup
				this->deviceEnumerator->Release();
				this->deviceEnumerator = nullptr;
				
				CoUninitialize();

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		/* private */ Controller::~Controller() {
			// Detach notification client
			if (this->notificationClient != nullptr) {
				// Ignore only possible error codes E_POINTER and E_NOTFOUND
				this->deviceEnumerator->UnregisterEndpointNotificationCallback(this->notificationClient);

				this->notificationClient->Release();
				this->notificationClient = nullptr;
			}

			// Release ref counted class
			if (this->notificationClient != nullptr) {
				this->notificationClient->Release();
				this->notificationClient = nullptr;
			}

			if (this->deviceEnumerator != nullptr) {
				this->deviceEnumerator->Release();
				this->deviceEnumerator = nullptr;
			}

			// Cleanup COM
			CoUninitialize();
		}

		/* public */ Controller::!Controller() {
			delete this;
		}

		/* public */ array<AudioDevice^>^ Controller::GetAudioDevices(DeviceType type, DeviceState stateMask) {
			// Get all endpoints
			IMMDeviceCollection* pCollection = nullptr;

			HRESULT hr = this->deviceEnumerator->EnumAudioEndpoints((EDataFlow)type, (DWORD)stateMask, &pCollection);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Loop through all devices
			UINT count;

			hr = pCollection->GetCount(&count);

			if (FAILED(hr)) {
				Utilities::SafeRelease((IUnknown**)&pCollection);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			array<AudioDevice^>^ devices = gcnew array<AudioDevice^>(count);

			for (UINT i = 0; i < count; i++) {
				// Get device
				IMMDevice* pDevice = nullptr;

				hr = pCollection->Item(i, &pDevice);

				if (FAILED(hr)) {
					Utilities::SafeRelease((IUnknown**)&pCollection);

					throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
				}

				try {
					devices[i] = gcnew AudioDevice(this, pDevice);
				} finally {
					Utilities::SafeRelease((IUnknown**)&pDevice);
				}
			}

			// Cleanup
			Utilities::SafeRelease((IUnknown**)&pCollection);

			return devices;
		}

		/* public */ AudioDevice^ Controller::GetAudioDevice(String^ id) {
			// Convert ID to native string
			IntPtr hId = Marshal::StringToHGlobalUni(id);
			LPWSTR wszId = (LPWSTR)hId.ToPointer();

			// Get device
			IMMDevice* pDevice = nullptr;

			HRESULT hr = this->deviceEnumerator->GetDevice(wszId, &pDevice);

			// Cleanup native string
			Marshal::FreeHGlobal(hId);
			wszId = nullptr;

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			AudioDevice^ device;
			
			try {
				device = gcnew AudioDevice(this, pDevice);
			} finally {
				Utilities::SafeRelease((IUnknown**)&pDevice);
			}

			return device;
		}

		/* public */ AudioDevice^ Controller::GetDefaultAudioDevice(DeviceType type, DeviceRole role) {
			// Get device
			IMMDevice* pDevice = nullptr;

			HRESULT hr = this->deviceEnumerator->GetDefaultAudioEndpoint((EDataFlow)type, (ERole)role, &pDevice);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			AudioDevice^ device;
			
			try {
				device = gcnew AudioDevice(this, pDevice);
			} finally {
				Utilities::SafeRelease((IUnknown**)&pDevice);
			}

			return device;
		}

		/* internal */ void Controller::OnDeviceStateChanged(String^ deviceId, DeviceState newState) {
			this->DeviceStateChanged(this, gcnew DeviceStateChangedEventArgs(deviceId, newState));
		}

		/* internal */ void Controller::OnDeviceAdded(String^ deviceId) {
			this->DeviceAdded(this, gcnew DeviceEventArgs(this->GetAudioDevice(deviceId)));
		}

		/* internal */ void Controller::OnDeviceRemoved(String^ deviceId) {
			this->DeviceRemoved(this, gcnew DeviceEventArgs(this->GetAudioDevice(deviceId)));
		}

		/* public */ void Controller::SetDefaultAudioDevice(AudioDevice^ device, DeviceRole role) {
			this->SetDefaultAudioDevice(device->Id, role);
		}

		/* public */ void Controller::SetDefaultAudioDevice(String^ deviceId, DeviceRole role) {
			IPolicyConfigVista* pPolicyConfig = nullptr;

			HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigVistaClient), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pPolicyConfig));

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Convert managed id to native
			IntPtr hId = Marshal::StringToHGlobalUni(deviceId);
			LPWSTR pwszId = (LPWSTR)hId.ToPointer();

			hr = pPolicyConfig->SetDefaultEndpoint(pwszId, (ERole)role);

			if (FAILED(hr)) {
				Marshal::FreeHGlobal(hId);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			Marshal::FreeHGlobal(hId);
		}

		/* internal */ void Controller::OnPropertyValueChanged(String^ deviceId, PropertyKey key) {
			this->PropertyValueChanged(this, gcnew PropertyValueChangedEventArgs(deviceId, key));
		}

		/* internal */ void Controller::OnDefaultDeviceChanged(String^ deviceId, DeviceType type, DeviceRole role) {
			this->DefaultDeviceChanged(this, gcnew DefaultDeviceChangedEventArgs(this->GetAudioDevice(deviceId), type, role));
		}
	}
}
