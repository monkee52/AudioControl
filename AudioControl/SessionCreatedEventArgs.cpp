#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ SessionCreatedEventArgs::SessionCreatedEventArgs(AudioSession^ session) {
			this->_session = session;
		}

		/* public */ AudioSession^ SessionCreatedEventArgs::Session::get() {
			return this->_session;
		}
	}
}
