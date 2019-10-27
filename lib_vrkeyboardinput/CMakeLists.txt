cmake_minimum_required (VERSION 3.0.0)

project(VRKeyboardInput)

# The version number.
set (VRKeyboardInput_VERSION_MAJOR 1)
set (VRKeyboardInput_VERSION_MINOR 0)
 
 
include_directories ("../openvr/headers/")
include_directories ("../third-party/boost_1_65_1/")
include_directories ("../third-party/easylogging++/")

include_directories ("include")

add_library(VRKeyboardInput src/vrkeyboardinput.cpp)

