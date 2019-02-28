#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ MuteStatusChangedEventArgs::MuteStatusChangedEventArgs(Guid evContext, bool oldMuteStatus, bool newMuteStatus) {
			this->_evContext = evContext;
			this->_previousMuteStatus = oldMuteStatus;
			this->_muteStatus = newMuteStatus;
		}

		/* public */ Guid MuteStatusChangedEventArgs::Context::get() {
			return this->_evContext;
		}

		/* public */ bool MuteStatusChangedEventArgs::PreviousMuteStatus::get() {
			return this->_previousMuteStatus;
		}

		/* public */ bool MuteStatusChangedEventArgs::MuteStatus::get() {
			return this->_muteStatus;
		}
	}
}
