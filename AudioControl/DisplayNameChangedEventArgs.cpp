#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ DisplayNameChangedEventArgs::DisplayNameChangedEventArgs(Guid evContext, String^ oldDisplayName, String^ newDisplayName) {
			this->_evContext = evContext;
			this->_previousDisplayName = oldDisplayName;
			this->_displayName = newDisplayName;
		}

		/* public */ Guid DisplayNameChangedEventArgs::Context::get() {
			return this->_evContext;
		}

		/* public */ String^ DisplayNameChangedEventArgs::PreviousDisplayName::get() {
			return this->_previousDisplayName;
		}

		/* public */ String^ DisplayNameChangedEventArgs::DisplayName::get() {
			return this->_displayName;
		}
	}
}
