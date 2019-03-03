#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ DefaultDeviceChangedEventArgs::DefaultDeviceChangedEventArgs(AudioDevice^ device, DeviceType type, DeviceRole role) {
			this->_device = device;
			this->_type = type;
			this->_role = role;
		}

		/* public */ AudioDevice^ DefaultDeviceChangedEventArgs::Device::get() {
			return this->_device;
		}

		/* public */ DeviceType DefaultDeviceChangedEventArgs::Type::get() {
			return this->_type;
		}

		/* public */ DeviceRole DefaultDeviceChangedEventArgs::Role::get() {
			return this->_role;
		}
	}
}
