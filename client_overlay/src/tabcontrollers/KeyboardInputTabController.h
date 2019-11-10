
#pragma once

#include <QObject>
#include <memory>
#include <openvr.h>
#include <vector>
#include <vrkeyboardinput.h>

class QQuickWindow;

// application namespace
namespace keyboardinput {

	// forward declaration
	class OverlayController;
	 
	struct greater
	{
		template<class T>
		bool operator()(T const &a, T const &b) const { return a > b; }
	};

	struct KeyboardInputMapping {
		int keyboardKey = 0x57;
		int vrButton = vr::EVRButtonId::k_EButton_DPad_Up;
		bool isPress = true;
		bool isTouch = false;
		bool wasDown = false;
		double timeLastDown = 0;
	};

	struct KeyboardInputProfile {
		std::string profileName;

		std::string mappingString;

		bool kiEnabled = false;
		bool useTrackers = false;
	};


	struct DeviceInfo {
		std::string serial;
		vr::ETrackedDeviceClass deviceClass = vr::TrackedDeviceClass_Invalid;
		uint32_t openvrId = 0;
		uint32_t renderModelIndex = 0;
		vr::VROverlayHandle_t renderModelOverlay = vr::k_ulOverlayHandleInvalid;
		std::string renderModelOverlayName;
	};


	class KeyboardInputTabController : public QObject {
		Q_OBJECT

	private:
		OverlayController * parent;
		QQuickWindow* widget;
		vrkeyboardinput::VRKeyboardInput vrkeyboardinput;

		std::vector<std::shared_ptr<DeviceInfo>> deviceInfos;
		uint32_t maxValidDeviceId = 0;

		std::thread identifyThread;

		std::vector<KeyboardInputProfile> keyboardInputProfiles;
		std::vector< std::shared_ptr<KeyboardInputMapping>> inputMappings;

		int currentProfileIdx = -1;

		vr::VROverlayHandle_t overlayHandle;

		bool kiEnabled = false;
		bool hasTwoControllers = false;
		bool useTrackers = false;
		bool initializedProfile = false;
		bool initializedDriver = false;
		bool identifyControlTimerSet = true;
		int initNewProfile = -1;

		bool pressedFlag = false;
		bool inputStateChanged = false;

		int controlSelectOverlayHandle = -1;
		int vive_controller_model_index = -1;
		int unnTouchedCount = 0;
		int stopCallCount = 0;

		uint64_t hmdID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t controller1ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t controller2ID = vr::k_unTrackedDeviceIndexInvalid;		
		uint64_t tracker1ID = vr::k_unTrackedDeviceIndexInvalid;
		uint64_t tracker2ID = vr::k_unTrackedDeviceIndexInvalid;

		double timeStep = 1.0 / 40.0;
		double dT = timeStep * 1000;
		double timeLastTick = 0.0;
		double kiReleaseTO = 3300;

		double identifyControlLastTime = 99999;
		double identifyControlTimeOut = 6000;

		double currentRunTimeInSec = 0.0;

	public:
		~KeyboardInputTabController();
		void initStage1();
		void initStage2(OverlayController* parent, QQuickWindow* widget);

		void eventLoopTick();
		void keyboardInput();
		void handleEvent(const vr::VREvent_t& vrEvent);

		Q_INVOKABLE unsigned getDeviceCount();
		Q_INVOKABLE QString getDeviceSerial(unsigned index);
		Q_INVOKABLE unsigned getDeviceId(unsigned index);
		Q_INVOKABLE int getDeviceClass(unsigned index);

		Q_INVOKABLE bool isKIEnabled();
		Q_INVOKABLE bool getUseTrackers();

		Q_INVOKABLE unsigned getProfileCount();
		Q_INVOKABLE QString getProfileName(unsigned index);

	public slots:
		void enableKI(bool enable);

		void setDeviceRenderModel(unsigned deviceIndex, unsigned renderModelIndex, float r, float g, float b, float sx, float sy, float sz);

		void stopButtonPress(int buttonId);
		void applyButtonPress(int buttonId);

		void addProfile(QString name);
		void applyProfile(unsigned index);
		void deleteProfile(unsigned index);

		void reloadSettings();
		void reloadProfiles();
		void saveSettings();
		void saveProfiles();


	};

} // namespace keyboardinput
