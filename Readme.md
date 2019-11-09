![language](https://img.shields.io/badge/Language-C%2B%2B11-green.svg)  ![dependencies](https://img.shields.io/badge/Dependencies-Boost%201.65-green.svg)  ![license_gpl3](https://img.shields.io/badge/License-GPL%203.0-green.svg)

# OpenVR-KeyboardInput 

An OpenVR client that sends input from a HID keyboard to a openvr driver into a virtual vr controller


### Profiles
You can save different settings by creating a "New Profile".
If you want to update a profile with new settings you need to select the profile and delete it and re-create a "New Profile".

If you name a profile with the name "default" it will be the initially loaded profile once you start SteamVR. If you dont want to automatically enable keyboard input on startup save the default profile with "enableKI" unchecked.


### INI settings
You can modify all of the profiles from the ini file 
 at `C:\Users\<USERNAME>\AppData\Roaming\pottedmeat7\OpenVRKeyboardInput.ini` 

You can add custom keyboard to VR input mappings as follows
find the `inputMappings` key in the correct profile your changing such as 
```
keyboardInputProfiles\<profile_index>\inputMappings="87,4:65,3:83,6:68,5:"
```
Keyboard codes are here https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes

And the available VR buttons are
```
k_EButton_System			= 0,
k_EButton_ApplicationMenu	= 1,
k_EButton_Grip				= 2,
k_EButton_DPad_Left			= 3,
k_EButton_DPad_Up			= 4,
k_EButton_DPad_Right		= 5,
k_EButton_DPad_Down			= 6,
k_EButton_A					= 7,
```

## Any Issues Check out the Logs
Overlay UI Log here `C:\Users\<USERNAME>\AppData\Roaming\pottedmeat7\OpenVRKeyboardInput\VRKeyboardInput.log`
Driver Log here `C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers\00vrwalkinplace\bin\win64\driver_vrwalkinplace.log`


## Setting up To Re-Build the project from Source
*You do not need this if you just want to use the WIP overlay*

### Boost
- WINDOWS
	1. Goto https://sourceforge.net/projects/boost/files/boost-binaries/1.65.1/
	2. Download Boost 1.65 Binaries (boost_1_65_1-msvc-14.1-64.exe)
	3. Install Boost into `OpenVR-KeyboardInput/third-party/boost_1_65_1`
- LINUX
	1. Go to https://sourceforge.net/projects/boost/files/boost/1.65.1/
	2. Download boost_1_65_1.tar.gz
	3. Extract the files into `OpenVR-KeyboardInput/third-party/boost_1_65_1`
  
### Qt
1. Goto https://download.qt.io/official_releases/qt/5.9/5.9.0/
2. Download Qt 5.9.0 (Either windows .exe or linux .run file)
3. Run the Qt installer (I installed it to `c:\Qt` or `/home/<user>/` on linux)

## Building
- WINDOWS
	Build *'VRKeyboardInput.sln'* in Visual Studio 2017
- LINUX	
	Build the OpenVR-KeyboardInputOverlay.pro project with Qt Creator 
	from a shell run
	`./build.sh`
	go to https://github.com/probonopd/linuxdeployqt/releases and download the AppImage
	then run
	`cd <OpenVR-KeyboardInput path>/client_overlay/bin/x64/
	<linuxdeployqt path>/linuxdeployqt-5-x86_64.AppImage OpenVR-KeyboardInputOverlay -qmldir=res/qml/ -no-translations -bundle-non-qt-libs -appimage -verbose=2`

### Building installer
1. go to https://sourceforge.net/projects/nsis/files/NSIS%202/2.33/
2. download and run the nsis-2.33-setup.exe
3. go to `OpenVR-KeyboardInput/installer`
4. right click the `installer.nsi` file and `Compile NSIS Script`
5. the installer exe will be built into the same directory

## Uninstall
1. Run "C:\Program Files\OpenVR-KeyboardInput\Uninstall.exe" will remove everything

# Known Bugs

- The shared-memory message queue is prone to deadlock the driver when the client crashes or is exited ungracefully.

# License

This software is released under GPL 3.0.
