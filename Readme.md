![language](https://img.shields.io/badge/Language-C%2B%2B11-green.svg)  ![dependencies](https://img.shields.io/badge/Dependencies-Boost%201.63-green.svg)  ![license_gpl3](https://img.shields.io/badge/License-GPL%203.0-green.svg)

# OpenVR-WalkInPlace

An OpenVR driver that applies virtual movement using a pedometer

The OpenVR driver hooks into the HTC Vive lighthouse driver and tracks movement of the HMD and Vive Controllers. It then applies movement into the virtual envrionment.

# Current Games that Work Best with OpenVR-WalkInPlace

- Fallout 4 VR
- Arizona Sunshine
- Onward
- Gorn
- DOOM VFR (with Keyboard (WASD))
- Any other games with Keyboard or Touchpad locomotion controls

Other games may not have touchpad movement options however this driver will 
also activate teleport if you'd like.

# Features

- Change Step Thresholds for Walk / Jog / Run in Place to fit different games
- Configuration for "Arm Swinging" Locomotion
- Change speed of movement applied in game 
- Profiles for different games
- graph of velocity values for step configuration

# Upcoming

- Optional Beta Algorithm in UI and algorithm improvements
- Tracker support (for feet)
- Options for emulating other input methods
- Jumping support
- Fixes for teleport games

## Installer

Download the newest installer from the [release section](https://github.com/pottedmeat7/OpenVR-WalkInPlace/releases) and then execute it. Don't forget to exit SteamVR before installing/de-installing.

# Documentation

## Configuration Examples

![Example Screenshot](docs/screenshots/walkinplace_default.png)
*This is settings, which allows for all speeds (walk/jog/run) for Fallout 4 VR, this may cause walking when just nodding (see arm-swingin config below). You can use "Button for movement" options if you prefer.*

![Example Screenshot](docs/screenshots/walkinplace_armswing.png)
*This type of "arm-swing" settings works well in FO4VR but only allows you to move at two speeds (jog/run, the actual speed of jog can be lowered if you want it to feel slower)*

### WalkInPlace Overlay
Just "Enable WIP" in the UI.

Enable the "analog" locomotion in the games settings this is the input method that uses touching the touch-pad (not clicking)
Then you simply walk in place to emulate walking in the VR world.

### Game Type
These options are for different combinations of controls some games use the touch-pad for movement and then a click to sprint, some use the click to engage teleport. If you dont want to trigger teleport use the second option.

### Controller selection (which controller is used for virtual input)
Some games only use one controller for locomotion while the other touchpad is used for different functions.
This menu allows you to select which controller should be used for virtual input.
The 1st and 2nd option will just switch between two controllers without identification.
There is no distinction just switch until its the right one. In some games you can just use the "both" option

### HMD Thresholds
The Y value is the Up and Down movement of your head to trigger a step, in order to trigger the real time HMD values have to be greater than the Y threshold.

The XZ value is the Side to Side movement that will disable triggering a step (if over the threshold), in order to trigger a step the HMD values have to be less than the XZ threshold.

### Hand Jog / Run
These values are for the magnitude of movement (X, Y and Z movement) of the arms. 
The real time Controller values have to be greater then these values in order to Jog / Run.

### Button for Movement
These options can be used to disable virtual movement when your not holding the button selected.

### Button as Toggle
This will change the default behavior for the "Button for Movement" so that you click the button once and then virtual movement will be enabled. Click the button again to disable virtual movement and so on.

### Flip Button Use
This will change the "Button for Movement" behavior instead of allowing virtual movement when the button is held, it will only allow virtual movement when the button is not held and visa versa.

### Step Time
This is how long each "step" lasts. If 1 step is detected it will last this amount of time in seconds. As you repeat steps this time is reset for every step detected. 

### Touch Options
These values control the degree of movement applied in game.
Some games will use the touchpad axis differntly, for slow games sometimes there is only 1 degree of movment.
Some games use the entire axis from the center, 0, to 1

If you find the walking with just the HMD is too sensitive you can set the "Walk Touch" to 0 this will require you HMD and arms to move in order to trigger a step.

### Profiles
If you like your current settings for a game and want to save them you can click "New Profile" it will take the current settings and save them with the profile name of your choice. 

After re-opening SteamVR you can reload your saved profiles by first clicking "Load Profiles" then selecting the profile you want from the drop down menu, and click "Apply".

If you want to update a profile with new settings you need to select the profile and delete it and re-create a "New Profile".

If you name a profile with the name "default" it will be the initially loaded profile once you "load profiles".

## Initial Setup
### Boost
1. Goto https://sourceforge.net/projects/boost/files/boost-binaries/1.65.1/
2. Download Boost 1.63 Binaries (boost_1_65_1-msvc-14.0-64.exe)
3. Install Boost into `OpenVR-WalkInPlace/third-party/boost_1_65_1`
4. Open Developer Command Prompt enter `OpenVR-WalkInPlace/third-party/boost_1_65_1` and run bootstrap.bat
5. then run `b2 toolset=msvc-14.0 address-model=64`
  
### Qt
1. Goto https://download.qt.io/official_releases/qt/5.9/5.9.0/
2. Download Qt 5.9.0
3. Run the Qt installer (I installed it to "c:\Qt")
4. Goto `OpenVR-WalkInPlace\client_overlay`
5. Create `client_overlay.vcxproj.user` and paste the following into it:

```
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="14.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <QTDIR>C:\Qt\Qt5.9.0\5.9\msvc2015_64</QTDIR>
    <LocalDebuggerEnvironment>PATH=$(QTDIR)\bin%3b$(PATH)</LocalDebuggerEnvironment>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <QTDIR>C:\Qt\Qt5.9.0\5.9\msvc2015_64</QTDIR>
    <LocalDebuggerEnvironment>PATH=$(QTDIR)\bin%3b$(PATH)</LocalDebuggerEnvironment>
  </PropertyGroup>
</Project>
```

NOTE: Adjust the path the `msvc2017_64` folder in Qt to match your installation

Then run the `windeployqt.bat` if your system doesn't find the exe update the batch to the absolute path
in `{QT_INSTLATION_PATH}\5.9\msvc2017_64\bin\windeployqt.exe`

## Building
1. Open *'VRWalkInPlace.sln'* in Visual Studio 2017.
2. Build Solution

## Uninstall
1. Run "C:\Program Files\OpenVR-WalkInPlace\Uninstall.exe"

# Known Bugs

- The shared-memory message queue is prone to deadlock the driver when the client crashes or is exited ungracefully.

# License

This software is released under GPL 3.0.