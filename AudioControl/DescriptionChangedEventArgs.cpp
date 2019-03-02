#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ DescriptionChangedEventArgs::DescriptionChangedEventArgs(String^ oldDescription, String^ newDescription) {
			this->_previousDescription = oldDescription;
			this->_description = newDescription;
		}
		/* public */ String^ DescriptionChangedEventArgs::PreviousDescription::get() {
			return this->_previousDescription;
		}

		/* public */ String^ DescriptionChangedEventArgs::Description::get() {
			return this->_description;
		}
	}
}
