#include "stdafx.h"

#include "AudioControl.h"
#include "Utilities.h"
#include "AudioDevice.h"

namespace AydenIO {
	namespace AudioControl {
		/* private */ String^ AudioDevice::GetPropertyAsString(const PROPERTYKEY key) {
			// Get device enumerator
			CComPtr<IMMDeviceEnumerator> pEnumerator = nullptr;

			HRESULT hr = pEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator));

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Convert ID to native string
			IntPtr hId = Marshal::StringToHGlobalUni(this->Id);
			LPWSTR wszId = (LPWSTR)hId.ToPointer();

			// Get device
			CComPtr<IMMDevice> pEndpoint = nullptr;

			hr = pEnumerator->GetDevice(wszId, &pEndpoint);

			// Cleanup native string
			Marshal::FreeHGlobal(hId);
			wszId = nullptr;

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			if (pEndpoint == nullptr) {
				throw gcnew ApplicationException(String::Format(gcnew String(_T("Unable to find device {0}")), this->Id));
			}

			// Get property store
			CComPtr<IPropertyStore> pProps = nullptr;

			hr = pEndpoint->OpenPropertyStore(STGM_READ, &pProps);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			hr = pProps->GetValue(key, &varName);

			if (FAILED(hr)) {
				// Cleanup
				PropVariantClear(&varName);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
			
			// Convert to managed and cleanup
			String^ name = gcnew String(varName.pwszVal);

			PropVariantClear(&varName);

			return name;
		}

		/* internal */ AudioDevice::AudioDevice(Controller^ controller, String^ id, bool skipCheck) {
			this->_controller = controller;
			this->_id = id;
		}

		/* public */ String^ AudioDevice::Id::get() {
			return this->_id;
		}

		/* private */ void AudioDevice::Id::set(String^ id) {
			this->_id = id;
		}

		/* public */ String^ AudioDevice::Name::get() {
			return this->GetPropertyAsString(PKEY_DeviceInterface_FriendlyName);
		}

		/* public */ String^ AudioDevice::FriendlyName::get() {
			return this->GetPropertyAsString(PKEY_Device_FriendlyName);
		}

		/* public */ String^ AudioDevice::Description::get() {
			return this->GetPropertyAsString(PKEY_Device_DeviceDesc);
		}

		/* public */ bool AudioDevice::Equals(Object^ otherDevice) {
			if (otherDevice == nullptr) {
				return false;
			}

			AudioDevice^ that = dynamic_cast<AudioDevice^>(otherDevice);

			return this->Id == that->Id;
		}

		/* public */ bool AudioDevice::Equals(AudioDevice^ otherDevice) {
			if (otherDevice == nullptr) {
				return false;
			}

			return this->Id == otherDevice->Id;
		}

		/* public */ bool AudioDevice::operator!= (AudioDevice^ device1, AudioDevice^ device2) {
			return !(device1 == device2);
		}

		/* public */ bool AudioDevice::operator== (AudioDevice^ device1, AudioDevice^ device2) {
			if (Object::ReferenceEquals(device1, device2)) {
				return true;
			}

			if ((dynamic_cast<Object^>(device1) == nullptr) || (dynamic_cast<Object^>(device2) == nullptr)) {
				return false;
			}

			return device1->Id == device2->Id;
		}

		/* public */ int AudioDevice::GetHashCode() {
			return this->ToString()->GetHashCode();
		}

		/* public */ String^ AudioDevice::ToString() {
			return String::Format(gcnew String(_T("<#Device({0})>")), this->Id);
		}
	}
}
