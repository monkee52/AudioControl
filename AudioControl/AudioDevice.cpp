#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* private */ AudioDevice::AudioDevice(IMMDevice* pDevice) {
			this->pDevice = pDevice;
			this->pDevice->AddRef();

			// Init properties
			IPropertyStore* pProps = nullptr;

			HRESULT hr = this->pDevice->OpenPropertyStore(STGM_READ, &pProps);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->pProps = pProps;

			// Cache ID
			LPWSTR pwszId = nullptr;

			hr = this->pDevice->GetId(&pwszId);

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

		/* public */ int AudioDevice::DefaultChannelCount::get() {
			if (this->State == DeviceState::NotPresent) {
				return -1;
			}

			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_AudioEngine_OEMFormat, &varName);

			if (FAILED(hr)) {
				// Cleanup
				PropVariantClear(&varName);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			WAVEFORMATEX* deviceInfo = (WAVEFORMATEX*)varName.blob.pBlobData;

			int channelCount = deviceInfo->nChannels;

			// Cleanup
			deviceInfo = nullptr;
			PropVariantClear(&varName);

			return channelCount;
		}

		/* public */ int AudioDevice::DefaultSampleRate::get() {
			if (this->State == DeviceState::NotPresent) {
				return -1;
			}

			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_AudioEngine_OEMFormat, &varName);

			if (FAILED(hr)) {
				// Cleanup
				PropVariantClear(&varName);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			WAVEFORMATEX* deviceInfo = (WAVEFORMATEX*)varName.blob.pBlobData;

			int sampleRate = deviceInfo->nSamplesPerSec;

			// Cleanup
			deviceInfo = nullptr;
			PropVariantClear(&varName);

			return sampleRate;
		}

		/* public */ int AudioDevice::DefaultBitDepth::get() {
			if (this->State == DeviceState::NotPresent) {
				return -1;
			}

			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_AudioEngine_OEMFormat, &varName);

			if (FAILED(hr)) {
				// Cleanup
				PropVariantClear(&varName);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			WAVEFORMATEX* deviceInfo = (WAVEFORMATEX*)varName.blob.pBlobData;

			int bitDepth = deviceInfo->wBitsPerSample;

			// Cleanup
			deviceInfo = nullptr;
			PropVariantClear(&varName);

			return bitDepth;
		}

		/* public */ int AudioDevice::CurrentChannelCount::get() {
			if (this->State == DeviceState::NotPresent || this->State == DeviceState::Unplugged) {
				return this->DefaultChannelCount;
			}

			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_AudioEngine_DeviceFormat, &varName);

			if (FAILED(hr)) {
				// Cleanup
				PropVariantClear(&varName);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			WAVEFORMATEX* deviceInfo = (WAVEFORMATEX*)varName.blob.pBlobData;

			int channelCount = deviceInfo->nChannels;

			// Cleanup
			deviceInfo = nullptr;
			PropVariantClear(&varName);

			return channelCount;
		}

		/* public */ int AudioDevice::CurrentSampleRate::get() {
			if (this->State == DeviceState::NotPresent || this->State == DeviceState::Unplugged) {
				return this->DefaultSampleRate;
			}

			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_AudioEngine_DeviceFormat, &varName);

			if (FAILED(hr)) {
				// Cleanup
				PropVariantClear(&varName);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			WAVEFORMATEX* deviceInfo = (WAVEFORMATEX*)varName.blob.pBlobData;

			int sampleRate = deviceInfo->nSamplesPerSec;

			// Cleanup
			deviceInfo = nullptr;
			PropVariantClear(&varName);

			return sampleRate;
		}

		/* public */ int AudioDevice::CurrentBitDepth::get() {
			if (this->State == DeviceState::NotPresent || this->State == DeviceState::Unplugged) {
				return this->DefaultBitDepth;
			}

			// Prepare to get value
			PROPVARIANT varName;

			PropVariantInit(&varName);

			HRESULT hr = this->pProps->GetValue(PKEY_AudioEngine_DeviceFormat, &varName);

			if (FAILED(hr)) {
				// Cleanup
				PropVariantClear(&varName);

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			WAVEFORMATEX* deviceInfo = (WAVEFORMATEX*)varName.blob.pBlobData;

			int bitDepth = deviceInfo->wBitsPerSample;

			// Cleanup
			deviceInfo = nullptr;
			PropVariantClear(&varName);

			return bitDepth;
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
