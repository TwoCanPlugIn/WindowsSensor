# ---------------------------------------------------------------------------
# Author:      Steven Adler based on (jongough aka. Jon Gough) Copyright: License:     wxWidgets License
# ---------------------------------------------------------------------------

## ----- When changing this file do NOT change the order in which sections occur        ----- ##
## ----- Changes should only be made between the section blocks that identify where     ----- ##
## ----- these changes should be. The whole configuration process relies on this        ----- ##
## ----- sequence to be successful                                                      ----- ##
##

##
## ----- Modify section below to include all the details for your plugin ----- ##
##

set(CMLOC "CMakeLists: ")

# define minimum cmake version
cmake_minimum_required(VERSION 3.1.1)
if(COMMAND cmake_policy)
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif(POLICY CMP0043)
    cmake_policy(SET CMP0048 NEW)
    if(POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
    endif(POLICY CMP0077)
endif(COMMAND cmake_policy)


# define plugin name, owner and versions
set(VERBOSE_NAME "Windows_Sensor")
set(COMMON_NAME "Windows Sensor")
set(TITLE_NAME "WINDOWSSENSOR")
set(PACKAGE_CONTACT "twocanplugin@hotmail.com")
set(PACKAGE "windows_sensor")
set(SHORT_DESCRIPTION "Windows Sensor plugin, uses Windows Sensor API to obtain a position fix.")
set(LONG_DESCRIPTION "Windows Sensor plugin, uses Windows Sensor API to obtain a position fix. Designed for Microsoft Surface computers with a GPS chip.") 
set(VERSION_MAJOR "1")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION_TWEAK "0")
set(VERSION_DATE "10/03/2024")
set(OCPN_MIN_VERSION "ov50")
set(OCPN_API_VERSION_MAJOR "1")
set(OCPN_API_VERSION_MINOR "16")
set(TP_COMMENT "Release for O5 using CI")

set(PARENT "opencpn")

# The next line allows for setting up your own local webserver with git on it for testing purposes, the default is github.com
set(GIT_REPOSITORY_SERVER "github.com")

# Specifies cloudsmith user and repository names (note suffixes for repositories production, beta & alpha)
set(PROD "prod") # Standard Repos
set(BETA "beta") # Standard Repos
set(ALPHA "alpha") # Standard Repos

set(CLOUDSMITH_USER "steven-adler")
# set this if your cloudsmith repository name does not match your git repository name 
set(CLOUDSMITH_BASE_REPOSITORY "windows_sensor")

set(XML_INFO_URL "https://docs.microsoft.com/en-us/windows/win32/sensorsapi/portal")
set(XML_SUMMARY ${SHORT_DESCRIPTION})
set(XML_DESCRIPTION ${LONG_DESCRIPTION})

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

## We don't have any toolbar buttons etc.'
## option(PLUGIN_USE_SVG "Use SVG graphics" ON)

set(CMAKE_CXX_STANDARD 11)

# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")
# Don't use local version of GLU library
set(USE_LOCAL_GLU FALSE)
option(USE_GL "Enable OpenGL support" OFF)
message(STATUS "${CMLOC}USE_GL: ${USE_GL}")

## Define the build type
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE
        "Release"
        CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")


project(${PACKAGE})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")
include("PluginSetup")

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

set(CMAKE_VERBOSE_MAKEFILE ON)

option(Plugin_CXX11 "Use c++11" OFF)


##
## ----- do not change next section - needed to configure build process ----- ##
##

include("PluginConfigure")

##
## ----- Change below to match project requirements for source, headers, etc. ----- ##
##

## We don't understand/use this definition
## add_definitions(-DUSE_S57)

## We don't install any SVG for toolbar icons
## add_definitions(-DPLUGIN_USE_SVG)

## Need api-16 for ocpn_plugin.h
INCLUDE_DIRECTORIES(BEFORE ${PROJECT_SOURCE_DIR}/libs/ocpn-api)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc)

SET(SOURCE src/sensor_plugin.cpp
		   src/sensor_plugin_settings.cpp
		   src/sensor_plugin_settings_base.cpp
           src/sensor_plugin_icons.cpp)

SET(INCLUDE inc/sensor_plugin.h
			inc/sensor_plugin_settings.h
		    inc/sensor_plugin_settings_base.h
            inc/sensor_plugin_icons.h)

TARGET_SOURCES(${PACKAGE_NAME} PUBLIC ${SOURCE})

# Link to these Windows libraries
TARGET_LINK_LIBRARIES(${PACKAGE_NAME} sensorsapi comsuppwd)

##
## ----- do not change next section - needed to configure build process ----- ##
##

include("PluginInstall")
include("PluginLocalization")
include("PluginPackage")

