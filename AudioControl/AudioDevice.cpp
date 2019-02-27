#include "stdafx.h"

#include "AudioControl.h"
#include "Utilities.h"
#include "AudioDevice.h"

namespace AydenIO {
	namespace AudioControl {
		/* private */ AudioDevice::AudioDevice(IMMDevice* pDevice) {
			this->pDevice = pDevice;
			this->pDevice->AddRef();

			IPropertyStore* pProps = nullptr;

			HRESULT hr = this->pDevice->OpenPropertyStore(STGM_READ, &pProps);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->pProps = pProps;
		}

		/* private */ AudioDevice::~AudioDevice() {
			if (this->pProps != nullptr) {
				this->pProps->Release();
				this->pProps = nullptr;
			}

			if (this->pDevice != nullptr) {
				this->pDevice->Release();
				this->pDevice = nullptr;
			}
		}

		/* public */ AudioDevice::!AudioDevice() {
			delete this;
		}

		/* public */ String^ AudioDevice::Id::get() {
			LPWSTR pwszId = nullptr;

			HRESULT hr = this->pDevice->GetId(&pwszId);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			String^ id = gcnew String(pwszId);

			CoTaskMemFree(pwszId);

			return id;
		}

		/* public */ String^ AudioDevice::Name::get() {
			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_DeviceInterface_FriendlyName, &varName);

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

		/* public */ String^ AudioDevice::FriendlyName::get() {
			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_Device_FriendlyName, &varName);

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

		/* public */ String^ AudioDevice::Description::get() {
			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_Device_DeviceDesc, &varName);

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
