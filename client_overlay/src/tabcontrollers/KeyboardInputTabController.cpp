#include "KeyboardInputTabController.h"
#include <QQuickWindow>
#include <QApplication>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtCore/QDebug>
#include <QtCore/QtMath>
#include "../overlaycontroller.h"
#include <openvr_math.h>
#include <chrono>

// application namespace
namespace keyboardinput {

	KeyboardInputTabController::~KeyboardInputTabController() {
		if (identifyThread.joinable()) {
			identifyThread.join();
		}
	}


	void KeyboardInputTabController::initStage1() {
		reloadProfiles();
		reloadSettings();
	}


	void KeyboardInputTabController::initStage2(OverlayController * parent, QQuickWindow * widget) {
		this->parent = parent;
		this->widget = widget;
	}


	void KeyboardInputTabController::eventLoopTick() {
		if (kiEnabled) {
			if (!initializedDriver) {
				if (hasTwoControllers) {
					try {
						vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
						vrkeyboardinput.connect();
						vrkeyboardinput.openvrEnableDriver(true);
						initializedDriver = true;
					}
					catch (std::exception& e) {
						LOG(INFO) << "Exception caught while adding initializing driver: " << e.what();
					}
				}
			}
			else {
				keyboardInput();
			}
		}
		if (identifyControlTimerSet) {
			auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			auto tdiff = ((double)(now - identifyControlLastTime));
			//LOG(INFO) << "DT: " << tdiff;
			if (tdiff >= identifyControlTimeOut) {
				identifyControlTimerSet = false;
				try {
					int model_count = vr::VRRenderModels()->GetRenderModelCount();
					for (int model_index = 0; model_index < model_count; model_index++) {
						char buffer[vr::k_unMaxPropertyStringSize];
						vr::VRRenderModels()->GetRenderModelName(model_index, buffer, vr::k_unMaxPropertyStringSize);
						if ((std::string(buffer).compare("vr_controller_vive_1_5")) == 0) {
							vive_controller_model_index = model_index;
							break;
						}
					}
				}
				catch (std::exception& e) {
					LOG(INFO) << "Exception caught while finding vive controller model: " << e.what();
				}
				setDeviceRenderModel(controlSelectOverlayHandle, 0, 1, 1, 1, 1, 1, 1);
			}
		}
		if (!initializedDriver || !kiEnabled) {
			if (hmdID == vr::k_unTrackedDeviceIndexInvalid ||
				(controller1ID == vr::k_unTrackedDeviceIndexInvalid || controller2ID == vr::k_unTrackedDeviceIndexInvalid) ||
				(useTrackers && (tracker1ID == vr::k_unTrackedDeviceIndexInvalid || tracker2ID == vr::k_unTrackedDeviceIndexInvalid))) {
				bool newDeviceAdded = false;
				for (uint32_t id = maxValidDeviceId; id < vr::k_unMaxTrackedDeviceCount; id++) {
					auto deviceClass = vr::VRSystem()->GetTrackedDeviceClass(id);
					if (deviceClass != vr::TrackedDeviceClass_Invalid) {
						if (deviceClass == vr::TrackedDeviceClass_HMD || deviceClass == vr::TrackedDeviceClass_Controller || deviceClass == vr::TrackedDeviceClass_GenericTracker) {
							auto info = std::make_shared<DeviceInfo>();
							info->openvrId = id;
							info->deviceClass = deviceClass;
							char buffer[vr::k_unMaxPropertyStringSize];
							vr::ETrackedPropertyError pError = vr::TrackedProp_Success;
							vr::VRSystem()->GetStringTrackedDeviceProperty(id, vr::Prop_SerialNumber_String, buffer, vr::k_unMaxPropertyStringSize, &pError);
							if (pError == vr::TrackedProp_Success) {
								info->serial = std::string(buffer);
							}
							else {
								info->serial = std::string("<unknown serial>");
								LOG(ERROR) << "Could not get serial of device " << id;
							}
							deviceInfos.push_back(info);
							if (deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_HMD) {
								if (hmdID == vr::k_unTrackedDeviceIndexInvalid) {
									hmdID = info->openvrId;
								}
							}
							LOG(INFO) << "Found device: id " << info->openvrId << ", class " << info->deviceClass << ", serial " << info->serial;
						}
						maxValidDeviceId = id + 1;
					}
				}
				int cntrlIdx = 0;
				int trkrIdx = 0;
				int cntrlCount = 0;
				for (auto dev : deviceInfos) {
					if ((dev->deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_Controller) 
						|| ( useTrackers && (dev->deviceClass == vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker))) {
						cntrlCount++;
						if (cntrlCount >= 2) {
							hasTwoControllers = true;
						}
					}
				}
			}
		}
	}

	void KeyboardInputTabController::handleEvent(const vr::VREvent_t&) {
		/*switch (vrEvent.eventType) {
		default:
		break;
		}*/
	}

	unsigned  KeyboardInputTabController::getDeviceCount() {
		return (unsigned)deviceInfos.size();
	}

	QString KeyboardInputTabController::getDeviceSerial(unsigned index) {
		if (index < deviceInfos.size()) {
			return QString::fromStdString(deviceInfos[index]->serial);
		}
		else {
			return QString("<ERROR>");
		}
	}

	unsigned KeyboardInputTabController::getDeviceId(unsigned index) {
		if (index < deviceInfos.size()) {
			return (int)deviceInfos[index]->openvrId;
		}
		else {
			return vr::k_unTrackedDeviceIndexInvalid;
		}
	}

	int KeyboardInputTabController::getDeviceClass(unsigned index) {
		if (index < deviceInfos.size()) {
			return (int)deviceInfos[index]->deviceClass;
		}
		else {
			return -1;
		}
	}

	bool KeyboardInputTabController::isKIEnabled() {
		return kiEnabled;
	}


	bool KeyboardInputTabController::getUseTrackers() {
		return useTrackers;
	}

	void KeyboardInputTabController::reloadSettings() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("keyboardInputSettings");
		settings->endGroup();
	}

	void KeyboardInputTabController::reloadProfiles() {
		keyboardInputProfiles.clear();
		auto settings = OverlayController::appSettings();
		settings->beginGroup("keyboardInputSettings");
		auto profileCount = settings->beginReadArray("keyboardInputProfiles");
		for (int i = 0; i < profileCount; i++) {
			settings->setArrayIndex(i);
			keyboardInputProfiles.emplace_back();
			auto& entry = keyboardInputProfiles[i];
			entry.profileName = settings->value("profileName").toString().toStdString();
			entry.kiEnabled = settings->value("kiEnabled", false).toBool();
			entry.useTrackers = settings->value("useTrackers", false).toBool();
		}
		settings->endArray();
		settings->endGroup();
	}

	void KeyboardInputTabController::saveSettings() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("keyboardInputSettings");
		settings->endGroup();
		settings->sync();
	}


	void KeyboardInputTabController::saveProfiles() {
		auto settings = OverlayController::appSettings();
		settings->beginGroup("keyboardInputSettings");
		settings->beginWriteArray("keyboardInputProfiles");
		unsigned i = 0;
		for (auto& p : keyboardInputProfiles) {
			settings->setArrayIndex(i);
			settings->setValue("profileName", QString::fromStdString(p.profileName));
			settings->setValue("kiEnabled", p.kiEnabled);
			settings->setValue("useTrackers", p.useTrackers);
			i++;
		}
		settings->endArray();
		settings->endGroup();
		settings->sync();
	}

	unsigned KeyboardInputTabController::getProfileCount() {
		return (unsigned)keyboardInputProfiles.size();
	}

	QString KeyboardInputTabController::getProfileName(unsigned index) {
		if (index >= keyboardInputProfiles.size()) {
			return QString();
		}
		else {
			return QString::fromStdString(keyboardInputProfiles[index].profileName);
		}
	}

	void KeyboardInputTabController::addProfile(QString name) {
		KeyboardInputProfile* profile = nullptr;
		for (auto& p : keyboardInputProfiles) {
			if (p.profileName.compare(name.toStdString()) == 0) {
				profile = &p;
				break;
			}
		}
		if (!profile) {
			auto i = keyboardInputProfiles.size();
			keyboardInputProfiles.emplace_back();
			profile = &keyboardInputProfiles[i];
		}
		profile->profileName = name.toStdString();
		profile->kiEnabled = isKIEnabled();
		profile->useTrackers = useTrackers;

		saveProfiles();
		OverlayController::appSettings()->sync();
	}

	void KeyboardInputTabController::applyProfile(unsigned index) {
		if (index < keyboardInputProfiles.size()) {
			currentProfileIdx = index;
			auto& profile = keyboardInputProfiles[index];
			useTrackers = profile.useTrackers;

			enableKI(profile.kiEnabled);

			initializedProfile = true;

			auto settings = OverlayController::appSettings();
			settings->beginGroup("keyboardInputSettings");
			auto profileCount = settings->beginReadArray("keyboardInputProfiles");
			for (int i = 0; i < profileCount; i++) {
				settings->setArrayIndex(i);
				if (index == i) {
				}
			}
			settings->endArray();
			settings->endGroup();

		}
	}

	void KeyboardInputTabController::deleteProfile(unsigned index) {
		if (index < keyboardInputProfiles.size()) {
			initializedProfile = true;
			auto pos = keyboardInputProfiles.begin() + index;
			keyboardInputProfiles.erase(pos);
			saveProfiles();
			OverlayController::appSettings()->sync();
		}
	}

	void KeyboardInputTabController::enableKI(bool enable) {
		kiEnabled = enable;
		if (!enable && initializedDriver) {
			stopMovement();
		}
	}

	void KeyboardInputTabController::setDeviceRenderModel(unsigned deviceIndex, unsigned renderModelIndex, float r, float g, float b, float sx, float sy, float sz) {
		if (deviceIndex < deviceInfos.size()) {
			try {
				if (renderModelIndex == 0) {
					for (auto dev : deviceInfos) {
						if (dev->renderModelOverlay != vr::k_ulOverlayHandleInvalid) {
							vr::VROverlay()->DestroyOverlay(dev->renderModelOverlay);
							dev->renderModelOverlay = vr::k_ulOverlayHandleInvalid;
						}
					}
				}
				else {
					vr::VROverlayHandle_t overlayHandle = deviceInfos[deviceIndex]->renderModelOverlay;
					if (overlayHandle == vr::k_ulOverlayHandleInvalid) {
						std::string overlayName = std::string("RenderModelOverlay_") + std::string(deviceInfos[deviceIndex]->serial);
						auto oerror = vr::VROverlay()->CreateOverlay(overlayName.c_str(), overlayName.c_str(), &overlayHandle);
						if (oerror == vr::VROverlayError_None) {
							overlayHandle = deviceInfos[deviceIndex]->renderModelOverlay = overlayHandle;
						}
						else {
							LOG(INFO) << "Could not create render model overlay: " << vr::VROverlay()->GetOverlayErrorNameFromEnum(oerror);
						}
					}
					if (overlayHandle != vr::k_ulOverlayHandleInvalid) {
						std::string texturePath = QApplication::applicationDirPath().toStdString() + "\\res\\transparent.png";
						if (QFile::exists(QString::fromStdString(texturePath))) {
							vr::VROverlay()->SetOverlayFromFile(overlayHandle, texturePath.c_str());
							char buffer[vr::k_unMaxPropertyStringSize];
							vr::VRRenderModels()->GetRenderModelName(renderModelIndex, buffer, vr::k_unMaxPropertyStringSize);
							vr::VROverlay()->SetOverlayRenderModel(overlayHandle, buffer, nullptr);
							vr::HmdMatrix34_t trans = {
								sx, 0.0f, 0.0f, 0.0f,
								0.0f, sy, 0.0f, 0.0f,
								0.0f, 0.0f, sz, 0.0f
							};
							vr::VROverlay()->SetOverlayTransformTrackedDeviceRelative(overlayHandle, deviceInfos[deviceIndex]->openvrId, &trans);
							vr::VROverlay()->ShowOverlay(overlayHandle);
							vr::VROverlay()->SetOverlayColor(overlayHandle, r, g, b);
							identifyControlTimerSet = true;
						}
						else {
							LOG(INFO) << "Could not find texture \"" << texturePath << "\"";
						}
					}
					//LOG(INFO) << "Successfully created control select Overlay for device: " << deviceInfos[deviceIndex]->openvrId << " Index: " << deviceIndex;
				}
			}
			catch (std::exception& e) {
				LOG(INFO) << "Exception caught while updating control select overlay: " << e.what();
			}
		}
	}

	void KeyboardInputTabController::keyboardInput() {
		if (kiEnabled && initializedDriver) {
			auto now = std::chrono::duration_cast <std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
			double tdiff = ((double)(now - timeLastTick));
			//LOG(INFO) << "DT: " << tdiff;
			if (tdiff >= dT) {
				timeLastTick = now;
			}
		}
	}

	void KeyboardInputTabController::clearClickedFlag() {
		try {
			if (gameType->useAxis) {
				vr::VRControllerAxis_t axisState;
				axisState.x = 0;
				axisState.y = 0;
				try {
					vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
					vrkeyboardinput.connect();
					if (gameType->inputType == InputType::touchpad) {
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
						pressedFlag = false;
						vrkeyboardinput.openvrAxisEvent(deviceId, vr::k_EButton_SteamVR_Touchpad, axisState);
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					} else {
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
						pressedFlag = false;
						vrkeyboardinput.openvrAxisEvent(deviceId, vr::k_EButton_IndexController_JoyStick, axisState);
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
					}
				}
				catch (std::exception& e) {
					//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
				}
			}
		}
		catch (std::exception& e) {
			//LOG(INFO) << "Exception caught while applying virtual axis movement: " << e.what();
		}
	}

	void KeyboardInputTabController::stopMovement() {
		try {
			if (stopCallCount < 20) {
				stopCallCount++;
				if (gameType->useAxis) {
					vr::VRControllerAxis_t axisState;
					axisState.x = 0;
					axisState.y = 0;
					try {
						vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
						vrkeyboardinput.connect();
						if (gameType->inputType == InputType::touchpad) {
							if (gameType->useClick) {
								vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
								pressedFlag = false;
							}
							vrkeyboardinput.openvrAxisEvent(deviceId, vr::k_EButton_SteamVR_Touchpad, axisState);
							vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
						}
						else {
							if (gameType->useClick) {
								vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
								pressedFlag = false;
							}
							vrkeyboardinput.openvrAxisEvent(deviceId, vr::k_EButton_IndexController_JoyStick, axisState);
							vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUntouched, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);

						}
					}
					catch (std::exception& e) {
						//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
					}
				}
				else if (gameType->inputType == InputType::grip) {
					try {
						vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
						vrkeyboardinput.connect();
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_Grip, 0.0);
					}
					catch (std::exception& e) {
						//LOG(INFO) << "Exception caught while applying virtual grip movement: " << e.what();
					}
				}
				/*else if (gameType == 9999) { //click only disabled atm
					try {
						vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
						vrkeyboardinput.connect();
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
					catch (std::exception& e) {
						//LOG(INFO) << "Exception caught while stopping virtual teleport movement: " << e.what();
					}
				}*/
				else if (gameType->inputType == InputType::keyWASD) {
#if defined _WIN64 || defined _LP64
					INPUT input[2];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x57, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(2, input, sizeof(INPUT));
#else
#endif
				}
				else if (gameType->inputType == InputType::keyArrow) {
#if defined _WIN64 || defined _LP64
					INPUT input[2];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wVk = 0;
					input[0].ki.wScan = MapVirtualKey(0x26, 0);
					input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = 0;
					SendInput(2, input, sizeof(INPUT));
#else
#endif
				}
			}
		}
		catch (std::exception &e) {
			LOG(INFO) << "error when attempting to stop movement: " << e.what();
		}
	}

	void KeyboardInputTabController::stopClickMovement() {
		if (gameType == 0) {
			try {
				vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
				vrkeyboardinput.connect();
				vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				pressedFlag = true;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while stopping virtual step movement: " << e.what();
			}
		}
	}

	void KeyboardInputTabController::applyAxisMovement(vr::VRControllerAxis_t axisState) {
		try {
			vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
			vrkeyboardinput.connect();
			if (gameType->inputType == InputType::touchpad) {
				vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonTouched, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				vrkeyboardinput.openvrAxisEvent(deviceId, vr::k_EButton_SteamVR_Touchpad, axisState);
			}
			else {
				vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonTouched, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
				vrkeyboardinput.openvrAxisEvent(deviceId, vr::k_EButton_IndexController_JoyStick, axisState);
			}
			if (gameType->useClick) {
				if (pressedFlag || gameType->alwaysHeld) {
					if (gameType->inputType == InputType::touchpad) {
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
					else {
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
					}
				}
				else {
					if (gameType->inputType == InputType::touchpad) {
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
					}
					else {
						vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_IndexController_JoyStick, 0.0);
					}
				}

			}
		}
		catch (std::exception& e) {
			//LOG(INFO) << "Exception caught while applying virtual axis movement: " << e.what();
		}
	}

	void KeyboardInputTabController::applyClickMovement() {
		if (pressedFlag) {
			try {
				vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
				vrkeyboardinput.connect();
				vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonPressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				pressedFlag = false;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while applying virtual telport movement: " << e.what();
			}
		}
		else {
			try {
				vrkeyboardinput::VRKeyboardInput vrkeyboardinput;
				vrkeyboardinput.connect();
				vrkeyboardinput.openvrButtonEvent(vrkeyboardinput::ButtonEventType::ButtonUnpressed, deviceId, vr::k_EButton_SteamVR_Touchpad, 0.0);
				pressedFlag = true;
			}
			catch (std::exception& e) {
				//LOG(INFO) << "Exception caught while resetting virtual telport movement: " << e.what();
			}
		}
		unnTouchedCount = 0;
	}


} // namespace keyboardinput
