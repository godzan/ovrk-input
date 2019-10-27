;--------------------------------
;Include Modern UI

	!include "MUI2.nsh"

	!define MUI_ICON "..\client_overlay\bin\x64\res\OVRKI.ico"
	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP "..\client_overlay\bin\x64\res\OVRKI.ico"
	!define MUI_HEADERIMAGE_RIGHT

;--------------------------------
;General

	!define OPENVR_BASEDIR "..\openvr\"
	!define OVERLAY_BASEDIR "..\client_overlay\bin\x64"
	!define DRIVER_BASEDIR "..\driver_vrkeyboardinput"

	;Name and file
	Name "OpenVR Keyboard Input"
	OutFile "OpenVR-KeyboardInput-Installer.exe"
	
	;Default installation folder
	InstallDir "$PROGRAMFILES64\OpenVR-KeyboardInput"
	
	;Get installation folder from registry if available
	InstallDirRegKey HKLM "Software\OpenVR-KeyboardInput\Overlay" ""
	
	;Request application privileges for Windows Vista
	RequestExecutionLevel admin
	
;--------------------------------
;Variables

VAR upgradeInstallation

;--------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Pages

	!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
	!define MUI_PAGE_CUSTOMFUNCTION_PRE dirPre
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
  
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Macros

;--------------------------------
;Functions

Function dirPre
	StrCmp $upgradeInstallation "true" 0 +2 
		Abort
FunctionEnd

Function .onInit
	StrCpy $upgradeInstallation "false"

	ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRKeyboardInput" "UninstallString"
	StrCmp $R0 "" done	
	
	; If SteamVR is already running, display a warning message and exit
	FindWindow $0 "Qt5QWindowIcon" "SteamVR Status"
	StrCmp $0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION \
			"SteamVR is still running. Cannot install this software.$\nPlease close SteamVR and try again."
		Abort
 
	IfFileExists $INSTDIR\OpenVR-KeyboardInputOverlay.exe 0 +5
		MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
			"OpenVR Keyboard Input is already installed. $\n$\nClick `OK` to upgrade the \
			existing installation or `Cancel` to cancel this upgrade." \
			IDOK upgrade
		Abort
 
	upgrade:
		StrCpy $upgradeInstallation "true"

	done:
FunctionEnd

;--------------------------------
;Installer Sections

Section "Install" SecInstall
	
	StrCmp $upgradeInstallation "true" 0 noupgrade 
		DetailPrint "Uninstall previous version..."
		ExecWait '"$INSTDIR\Uninstall.exe" /S _?=$INSTDIR'
		Delete $INSTDIR\Uninstall.exe
		Goto afterupgrade
		
	noupgrade:

	afterupgrade:

	SetOutPath "$INSTDIR"


	;ADD YOUR OWN FILES HERE...
	File "${OVERLAY_BASEDIR}\LICENSE"
	File "${OVERLAY_BASEDIR}\*.exe"
	File "${OVERLAY_BASEDIR}\*.dll"
	File "${OVERLAY_BASEDIR}\*.bat"
	File "${OVERLAY_BASEDIR}\*.vrmanifest"
	File "/oname=qt.conf" "${OVERLAY_BASEDIR}\qt_win.conf"
	File "${OVERLAY_BASEDIR}\..\logging.conf"
	File /r "${OVERLAY_BASEDIR}\res"
	File /r "${OVERLAY_BASEDIR}\qtdata"
	File "${OPENVR_BASEDIR}\bin\win64\*.dll"



	; Install redistributable
	ExecWait '"$INSTDIR\vcredist_x64.exe" /install /quiet'
	
	Var /GLOBAL vrRuntimePath
	nsExec::ExecToStack '"$INSTDIR\OpenVR-KeyboardInputOverlay.exe" -openvrpath'
	Pop $0
	Pop $vrRuntimePath
	DetailPrint "VR runtime path: $vrRuntimePath"

	SetOutPath "$vrRuntimePath\drivers\00vrkeyboardinput"
	File "${DRIVER_BASEDIR}\driver.vrdrivermanifest"
	SetOutPath "$vrRuntimePath\drivers\00vrkeyboardinput\resources"
	File "${DRIVER_BASEDIR}\resources\driver.vrresources"
	File /r "${DRIVER_BASEDIR}\resources\icons"
	File /r "${DRIVER_BASEDIR}\resources\input"
	File /r "${DRIVER_BASEDIR}\resources\settings"
	SetOutPath "$vrRuntimePath\drivers\00vrkeyboardinput\bin\win64"
	File "${DRIVER_BASEDIR}\bin\x64\driver_00vrkeyboardinput.dll"

	; Install the vrmanifest
	nsExec::ExecToLog '"$INSTDIR\OpenVR-KeyboardInputOverlay.exe" -installmanifest'
	
	; Post-installation step
	nsExec::ExecToLog '"$INSTDIR\OpenVR-KeyboardInputOverlay.exe" -postinstallationstep'
  
	;Store installation folder
	WriteRegStr HKLM "Software\OpenVR-KeyboardInput\Overlay" "" $INSTDIR
	WriteRegStr HKLM "Software\OpenVR-KeyboardInput\Driver" "" $vrRuntimePath
  
	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRKeyboardInput" "DisplayName" "OpenVR Keyboard Input"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRKeyboardInput" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
	; If SteamVR is already running, display a warning message and exit
	FindWindow $0 "Qt5QWindowIcon" "SteamVR Status"
	StrCmp $0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION \
			"SteamVR is still running. Cannot uninstall this software.$\nPlease close SteamVR and try again."
		Abort

	; Remove the vrmanifest
	nsExec::ExecToLog '"$INSTDIR\OpenVR-KeyboardInputOverlay.exe" -removemanifest'

	; Delete installed files
	Var /GLOBAL vrRuntimePath2
	ReadRegStr $vrRuntimePath2 HKLM "Software\OpenVR-KeyboardInpute\Driver" ""
	DetailPrint "VR runtime path: $vrRuntimePath2"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\driver.vrdrivermanifest"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\driver.vrresources"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\settings\default.vrsettings"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\bin\win64\driver_00vrkeyboardinput.dll"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\bin\win64\driver_vrkeyboardinput.log"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\bin\win64\error.log"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\bin\win64\OpenVR-KeyboardInputOverlay.log"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\icons\ovrki_wand.svg"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\input\ovrki_controller_profile.json"
	Delete "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\input\legacy_bindings_ovrki_controller.json"
	RMdir "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\input"
	RMdir "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\icons"
	RMdir "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\settings"
	RMdir "$vrRuntimePath2\drivers\00vrkeyboardinput\resources\"
	RMdir "$vrRuntimePath2\drivers\00vrkeyboardinput\bin\win64\"
	RMdir "$vrRuntimePath2\drivers\00vrkeyboardinput\bin\"
	RMdir "$vrRuntimePath2\drivers\00vrkeyboardinput\"
	
	!include uninstallFiles.list

	DeleteRegKey HKLM "Software\OpenVR-KeyboardInput\Overlay"
	DeleteRegKey HKLM "Software\OpenVR-KeyboardInput\Driver"
	DeleteRegKey HKLM "Software\OpenVR-KeyboardInput"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRKeyboardInput"
SectionEnd

