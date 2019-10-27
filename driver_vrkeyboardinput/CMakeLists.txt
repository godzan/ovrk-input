cmake_minimum_required (VERSION 3.0.0)

project(DriverKeyboardInput)

# The version number.
set (DriverKeyboardInput_VERSION_MAJOR 1)
set (DriverKeyboardInput_VERSION_MINOR 0)
 
 
include_directories ("src/com/shm/")
include_directories ("src/virtualdevice/")
include_directories ("src/driver/")
include_directories ("../openvr/headers/")
include_directories ("../third-party/boost_1_65_1/")
include_directories ("../third-party/easylogging++/")
include_directories ("../lib_vrkeyboardinput/include/")

set(SOURCES
	src/com/shm/driver_ipc_shm.cpp 
	src/virtualdevice/VirtualController.cpp 
	src/driver/ServerDriver.cpp 
	src/driver/WatchdogProvider.cpp
	)
	
add_library(DriverKeyboardInput SHARED ${SOURCES})


