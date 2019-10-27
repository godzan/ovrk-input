#include "WatchdogProvider.h"

#include "../logging.h"


// driver namespace
namespace vrkeyboardinput {
namespace driver {


vr::EVRInitError WatchdogProvider::Init(vr::IVRDriverContext * pDriverContext) {
	LOG(TRACE) << "WatchdogProvider::Init()";
	return vr::EVRInitError::VRInitError_None;
}


void WatchdogProvider::Cleanup() {
	LOG(TRACE) << "WatchdogProvider::Cleanup()";
}


}
}
