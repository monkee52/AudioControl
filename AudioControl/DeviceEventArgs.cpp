#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ DeviceEventArgs::DeviceEventArgs(AudioDevice^ device) {
			this->_device = device;
		}

		/* public */ AudioDevice^ DeviceEventArgs::Device::get() {
			return this->_device;
		}
	}
}
