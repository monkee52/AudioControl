#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* private */ AudioDevice::AudioDevice(IMMDevice* pDevice) {
			this->pDevice = pDevice;
			this->pDevice->AddRef();

			// Cache ID
			LPWSTR pwszId = nullptr;

			HRESULT hr = this->pDevice->GetId(&pwszId);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			String^ id = gcnew String(pwszId);

			CoTaskMemFree(pwszId);

			this->_id = id;

			// Get endpoint for device
			IMMEndpoint* pEndpoint = nullptr;

			hr = this->pDevice->QueryInterface(__uuidof(IMMEndpoint), (void**)&pEndpoint);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Cache role
			EDataFlow eFlow;

			hr = pEndpoint->GetDataFlow(&eFlow);

			if (FAILED(hr)) {
				// Cleanup
				if (pEndpoint != nullptr) {
					pEndpoint->Release();
					pEndpoint = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Cleanup
			if (pEndpoint != nullptr) {
				pEndpoint->Release();
				pEndpoint = nullptr;
			}

			this->_type = (DeviceType)eFlow;

			// Init properties
			IPropertyStore* pProps = nullptr;

			hr = this->pDevice->OpenPropertyStore(STGM_READ, &pProps);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->pProps = pProps;

			// Cache properties
			this->_name = this->GetPropertyAsString(PKEY_DeviceInterface_FriendlyName);
			this->_friendlyName = this->GetPropertyAsString(PKEY_Device_FriendlyName);
			this->_description = this->GetPropertyAsString(PKEY_Device_DeviceDesc);
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

		/* private */ String^ AudioDevice::GetPropertyAsString(const PROPERTYKEY key) {
			// Prepare to get value
			PROPVARIANT varProperty;

			PropVariantInit(&varProperty);

			HRESULT hr = this->pProps->GetValue(key, &varProperty);

			if (FAILED(hr)) {
				// Cleanup
				PropVariantClear(&varProperty);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Convert to managed and cleanup
			String^ value = gcnew String(varProperty.pwszVal);

			PropVariantClear(&varProperty);

			return value;
		}

		/* public */ String^ AudioDevice::Id::get() {
			return this->_id;
		}

		/* public */ DeviceState AudioDevice::State::get() {
			DWORD dwState = 0;

			HRESULT hr = this->pDevice->GetState(&dwState);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			return (DeviceState)dwState;
		}

		/* public */ DeviceType AudioDevice::Type::get() {
			return this->_type;
		}

		/* public */ String^ AudioDevice::Name::get() {
			return this->_name;
		}

		/* public */ String^ AudioDevice::FriendlyName::get() {
			return this->_friendlyName;
		}

		/* public */ String^ AudioDevice::Description::get() {
			return this->_description;
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
