cmake_minimum_required (VERSION 3.0.0)

project(OpenVRKeyboardInputOverlay)

# The version number.
set (OpenVRKeyboardInputOverlay_VERSION_MAJOR 1)
set (OpenVRKeyboardInputOverlay_VERSION_MINOR 0)
 
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Quick REQUIRED)
 
include_directories ("src/")
include_directories ("src/tabcontrollers/")
include_directories ("../openvr/headers/")
include_directories ("../third-party/boost_1_65_1/")
include_directories ("../third-party/easylogging++/")
include_directories ("../lib_vrkeyboardinput/include/")

add_library(openvr_api SHARED IMPORTED)

set_target_properties(openvr_api PROPERTIES
  IMPORTED_LOCATION "../openvr/lib/linux64/openvr_api.so"
)

set(SOURCES
	src/main.cpp 
	src/overlaycontroller.cpp 
	src/tabcontrollers/KeyboardInputTabController.cpp 
   )

add_executable(OpenVRKeyboardInputOverlay ${SOURCES})

qt5_use_modules(OpenVRKeyboardInputOverlay Core Gui Widgets Quick) # This macro depends from Qt version

target_link_libraries(OpenVRKeyboardInputOverlay openvr_api VRKeyboardInput -lpthread -lm -lrt)





