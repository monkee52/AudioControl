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
			CComPtr<IMMDeviceEnumerator> pEnumerator = nullptr;

			hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Get weak handle to this
			GCHandle hThis = GCHandle::Alloc(this, GCHandleType::Weak);

			// Create notification client and register it
			this->notificationClient = new CMMNotificationClient(GCHandle::ToIntPtr(hThis).ToPointer());

			hr = pEnumerator->RegisterEndpointNotificationCallback(this->notificationClient);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		Controller::~Controller() {
			// Get device enumerator
			CComPtr<IMMDeviceEnumerator> pEnumerator = nullptr;

			HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Detach notification client
			hr = pEnumerator->UnregisterEndpointNotificationCallback(this->notificationClient);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Release ref counted class
			if (this->notificationClient != nullptr) {
				this->notificationClient->Release();
			}

			// Cleanup COM
			CoUninitialize();
		}

		Controller::!Controller() {
			delete this;
		}

		/* public */ array<AudioDevice^>^ Controller::GetAudioDevices(DeviceType type, DeviceState stateMask) {
			// Get enumerator
			CComPtr<IMMDeviceEnumerator> pEnumerator = nullptr;

			HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Get all endpoints
			CComPtr<IMMDeviceCollection> pCollection = nullptr;

			hr = pEnumerator->EnumAudioEndpoints((EDataFlow)type, (DWORD)stateMask, &pCollection);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Loop through all devices
			UINT count;

			hr = pCollection->GetCount(&count);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			array<AudioDevice^>^ devices = gcnew array<AudioDevice^>(count);

			for (UINT i = 0; i < count; i++) {
				// Get device
				CComPtr<IMMDevice> pDevice = nullptr;

				hr = pCollection->Item(i, &pDevice);

				if (FAILED(hr)) {
					throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
				}

				// Get device ID
				LPWSTR pwszId = nullptr;

				hr = pDevice->GetId(&pwszId);

				if (FAILED(hr)) {
					throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
				}

				String^ id = gcnew String(pwszId);

				// Cleanup
				CoTaskMemFree(pwszId);

				devices[i] = gcnew AudioDevice(this, id, true);
			}

			return devices;
		}

		/* public */ AudioDevice^ Controller::GetAudioDevice(String^ id) {
			return gcnew AudioDevice(this, id, false);
		}

		/* public */ AudioDevice^ Controller::GetDefaultAudioDevice(DeviceType type, DeviceRole role) {
			// Get device enumerator
			CComPtr<IMMDeviceEnumerator> pEnumerator = nullptr;

			HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Get device
			CComPtr<IMMDevice> pDevice = nullptr;

			hr = pEnumerator->GetDefaultAudioEndpoint((EDataFlow)type, (ERole)role, &pDevice);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Get ID
			LPWSTR pwszId = nullptr;

			hr = pDevice->GetId(&pwszId);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			String^ id = gcnew String(pwszId);

			// Cleanup
			CoTaskMemFree(pwszId);

			return gcnew AudioDevice(this, id, true);
		}
	}
}
