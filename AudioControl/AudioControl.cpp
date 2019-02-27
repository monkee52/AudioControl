#include "stdafx.h"

#include "AudioControl.h"
#include "Utilities.h"
#include "CMMNotificationClient.h"

namespace AydenIO {
	namespace AudioControl {
		Controller::Controller() {
			HRESULT hr = CoInitialize(NULL);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(String::Format(gcnew String(_T("Could not initialize COM: {0}")), Utilities::ConvertHrToString(hr)));
			}

			CComPtr<IMMDeviceEnumerator> pEnumerator = nullptr;

			hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			GCHandle hThis = GCHandle::Alloc(this, GCHandleType::Weak);

			this->notificationClient = new CMMNotificationClient(GCHandle::ToIntPtr(hThis).ToPointer());

			hr = pEnumerator->RegisterEndpointNotificationCallback(this->notificationClient);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		Controller::~Controller() {
			CComPtr<IMMDeviceEnumerator> pEnumerator = nullptr;

			HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			hr = pEnumerator->UnregisterEndpointNotificationCallback(this->notificationClient);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			if (this->notificationClient != nullptr) {
				this->notificationClient->Release();
			}

			CoUninitialize();
		}

		Controller::!Controller() {
			delete this;
		}
	}
}
