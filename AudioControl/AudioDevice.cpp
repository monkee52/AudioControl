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
			pwszId = nullptr;

			this->_id = id;

			// Get endpoint for device
			IMMEndpoint* pEndpoint = nullptr;

			hr = this->pDevice->QueryInterface(__uuidof(IMMEndpoint), (void**)&pEndpoint);

			if (FAILED(hr)) {
				// Cleanup - destructor not called if exception thrown in constructor
				if (this->pDevice != nullptr) {
					this->pDevice->Release();
					this->pDevice = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			// Cache type
			EDataFlow eFlow;

			hr = pEndpoint->GetDataFlow(&eFlow);

			if (FAILED(hr)) {
				// Cleanup - destructor not called if exception thrown in constructor
				if (pEndpoint != nullptr) {
					pEndpoint->Release();
					pEndpoint = nullptr;
				}

				if (this->pDevice != nullptr) {
					this->pDevice->Release();
					this->pDevice = nullptr;
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
				// Cleanup - destructor not called if exception thrown in constructor
				if (this->pDevice != nullptr) {
					this->pDevice->Release();
					this->pDevice = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->pProps = pProps;

			// Cache properties
			try {
				this->_name = this->GetPropertyAsString(PKEY_DeviceInterface_FriendlyName);
				this->_friendlyName = this->GetPropertyAsString(PKEY_Device_FriendlyName);
				this->_description = this->GetPropertyAsString(PKEY_Device_DeviceDesc);
			} catch (Exception^ e) {
				// Cleanup - destructor not called if exception thrown in constructor
				if (this->pProps != nullptr) {
					this->pProps->Release();
					this->pProps = nullptr;
				}

				if (this->pDevice != nullptr) {
					this->pDevice->Release();
					this->pDevice = nullptr;
				}

				throw e;
			}
			
			// Init volume control
			IAudioEndpointVolume* pVolume = nullptr;

			hr = this->pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);

			if (FAILED(hr)) {
				// Cleanup - destructor not called if exception thrown in constructor
				if (this->pProps != nullptr) {
					this->pProps->Release();
					this->pProps = nullptr;
				}

				if (this->pDevice != nullptr) {
					this->pDevice->Release();
					this->pDevice = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			this->pVolume = pVolume;

			// Get weak handle to this
			GCHandle hThis = GCHandle::Alloc(this, GCHandleType::Weak);

			this->volumeCallback = new CAudioEndpointVolumeCallback(GCHandle::ToIntPtr(hThis).ToPointer());;

			hr = this->pVolume->RegisterControlChangeNotify(this->volumeCallback);

			if (FAILED(hr)) {
				// Cleanup - destructor not called if exception thrown in constructor
				if (this->pVolume != nullptr) {
					this->pVolume->Release();
					this->pVolume = nullptr;
				}

				if (this->pProps != nullptr) {
					this->pProps->Release();
					this->pProps = nullptr;
				}

				if (this->pDevice != nullptr) {
					this->pDevice->Release();
					this->pDevice = nullptr;
				}

				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		/* private */ AudioDevice::~AudioDevice() {
			// Detach volume callback
			if (this->volumeCallback != nullptr) {
				// Ignore only possible error code E_POINTER
				this->pVolume->UnregisterControlChangeNotify(this->volumeCallback);

				this->volumeCallback->Release();
				this->volumeCallback = nullptr;
			}

			if (this->pVolume != nullptr) {
				this->pVolume->Release();
				this->pVolume = nullptr;
			}

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

		/* public */ bool AudioDevice::IsMuted::get() {
			if (this->pVolume == nullptr) {
				return false;
			}

			BOOL bMuted = false;

			HRESULT hr = this->pVolume->GetMute(&bMuted);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}

			return (bool)bMuted;
		}

		/* public */ void AudioDevice::Mute() {
			HRESULT hr = this->pVolume->SetMute(TRUE, NULL);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
		}

		/* public */ void AudioDevice::Unmute() {
			HRESULT hr = this->pVolume->SetMute(FALSE, NULL);

			if (FAILED(hr)) {
				throw gcnew ApplicationException(Utilities::ConvertHrToString(hr));
			}
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

		/* internal */ void AudioDevice::OnMuteStatusChanged(MuteStatusChangedEventArgs^ e) {
			this->MuteStatusChanged(this, e);
		}

		/* internal */ void AudioDevice::OnMasterVolumeChanged(VolumeChangedEventArgs^ e) {
			this->MasterVolumeChanged(this, e);
		}
	}
}
