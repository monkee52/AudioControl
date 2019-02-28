#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ DeviceStateChangedEventArgs::DeviceStateChangedEventArgs(String^ deviceId, DeviceState newState) {
			this->_deviceId = deviceId;
			this->_state = newState;
		}

		/* internal */ String^ DeviceStateChangedEventArgs::DeviceId::get() {
			return this->_deviceId;
		}

		/* public */ DeviceState DeviceStateChangedEventArgs::PreviousState::get() {
			return this->_previousState;
		}

		/* internal */ void DeviceStateChangedEventArgs::PreviousState::set(DeviceState oldState) {
			this->_previousState = oldState;
		}

		/* public */ DeviceState DeviceStateChangedEventArgs::State::get() {
			return this->_state;
		}
	}
}
