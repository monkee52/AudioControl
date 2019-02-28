#include "stdafx.h"

#include "AudioControl.h"

namespace AydenIO {
	namespace AudioControl {
		/* internal */ VolumeChangedEventArgs::VolumeChangedEventArgs(Guid evContext, float oldVolume, float newVolume) {
			this->_evContext = evContext;
			this->_previousVolume = oldVolume;
			this->_volume = newVolume;
		}

		/* public */ Guid VolumeChangedEventArgs::Context::get() {
			return this->_evContext;
		}

		/* public */ float VolumeChangedEventArgs::PreviousVolume::get() {
			return this->_previousVolume;
		}

		/* public */ float VolumeChangedEventArgs::Volume::get() {
			return this->_volume;
		}
	}
}
