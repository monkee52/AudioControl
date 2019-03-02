#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ SessionDisconnectedEventArgs::SessionDisconnectedEventArgs(SessionDisconnectReason reason) {
			this->_reason = reason;
		}

		/* public */ SessionDisconnectReason SessionDisconnectedEventArgs::Reason::get() {
			return this->_reason;
		}
	}
}
