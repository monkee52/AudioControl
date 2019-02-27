#include "stdafx.h"

#include "AudioControl.h"
#include "AudioDevice.h"
#include "Utilities.h"
#include "CMMNotificationClient.h"

namespace AydenIO {
	namespace AudioControl {
		Controller::Controller() {
			// Init COM
			HRESULT hr = CoInitialize(NULL);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(String::Format(gcnew String(_T("Could not initialize COM: {0}")), Utilities::ConvertHrToString(hr)));
			}

			// Get device enumerator
			IMMDeviceEnumerator* pEnumerator = nullptr;

			hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));

			if (FAILED(hr)) {
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
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		Controller::~Controller() {
			// Detach notification client
			HRESULT hr = this->deviceEnumerator->UnregisterEndpointNotificationCallback(this->notificationClient);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
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

		Controller::!Controller() {
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
				if (pCollection != nullptr) {
					pCollection->Release();
					pCollection = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			array<AudioDevice^>^ devices = gcnew array<AudioDevice^>(count);

			for (UINT i = 0; i < count; i++) {
				// Get device
				IMMDevice* pDevice = nullptr;

				hr = pCollection->Item(i, &pDevice);

				if (FAILED(hr)) {
					if (pCollection != nullptr) {
						pCollection->Release();
						pCollection = nullptr;
					}

					throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
				}

				devices[i] = gcnew AudioDevice(pDevice);

				if (pDevice != nullptr) {
					pDevice->Release();
					pDevice = nullptr;
				}
			}

			// Cleanup
			if (pCollection != nullptr) {
				pCollection->Release();
				pCollection = nullptr;
			}

			return devices;
		}

		/* public */ AudioDevice^ Controller::GetAudioDevice(String^ id) {
			// Convert ID to native string
			IntPtr hId = Marshal::StringToHGlobalAnsi(id);
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

			AudioDevice^ device = gcnew AudioDevice(pDevice);

			if (pDevice != nullptr) {
				pDevice->Release();
				pDevice = nullptr;
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

			AudioDevice^ device = gcnew AudioDevice(pDevice);

			if (pDevice != nullptr) {
				pDevice->Release();
				pDevice = nullptr;
			}
		}
	}
}
