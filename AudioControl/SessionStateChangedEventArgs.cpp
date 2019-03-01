#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ SessionStateChangedEventArgs::SessionStateChangedEventArgs(SessionState oldState, SessionState newState) {
			this->_previousState = oldState;
			this->_state = newState;
		}

		/* public */ SessionState SessionStateChangedEventArgs::PreviousState::get() {
			return this->_previousState;
		}

		/* public */ SessionState SessionStateChangedEventArgs::State::get() {
			return this->_state;
		}
	}
}
