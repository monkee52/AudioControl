#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ NameChangedEventArgs::NameChangedEventArgs(String^ oldName, String^ newName) {
			this->_previousName = oldName;
			this->_name = newName;
		}
		/* public */ String^ NameChangedEventArgs::PreviousName::get() {
			return this->_previousName;
		}

		/* public */ String^ NameChangedEventArgs::Name::get() {
			return this->_name;
		}
	}
}
