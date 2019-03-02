#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ PropertyValueChangedEventArgs::PropertyValueChangedEventArgs(String^ deviceId, PropertyKey key) {
			this->_deviceId = deviceId;
			this->_key = key;
		}

		/* internal */ String^ PropertyValueChangedEventArgs::DeviceId::get() {
			return this->_deviceId;
		}

		/* internal */ PropertyKey PropertyValueChangedEventArgs::Key::get() {
			return this->_key;
		}
	}
}
