#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ FriendlyNameChangedEventArgs::FriendlyNameChangedEventArgs(String^ oldFriendlyName, String^ newFriendlyName) {
			this->_previousFriendlyName = oldFriendlyName;
			this->_friendlyName = newFriendlyName;
		}
		/* public */ String^ FriendlyNameChangedEventArgs::PreviousFriendlyName::get() {
			return this->_previousFriendlyName;
		}

		/* public */ String^ FriendlyNameChangedEventArgs::FriendlyName::get() {
			return this->_friendlyName;
		}
	}
}
