# This is used to test the installation process

cmake_minimum_required(VERSION 3.10)
project(lvgl_example)

set(CMAKE_C_STANDARD 99) # LVGL officially supports C99 and above
set(CMAKE_CXX_STANDARD 17) #C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Uncomment if the program needs debugging
set(CMAKE_BUILD_TYPE Debug)

# Optional path to a local LVGL build directory (contains lv_conf.h and lvgl/lib)
set(LVGL_LOCAL_BUILD_DIR "" CACHE PATH "LVGL build directory for in-tree linking")
set(_lvgl_local_build "")
if(LVGL_LOCAL_BUILD_DIR)
	set(_lvgl_local_build "${LVGL_LOCAL_BUILD_DIR}")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/../build/lv_conf.h" AND EXISTS "${CMAKE_SOURCE_DIR}/../build/lvgl/lib/liblvgl.a")
	set(_lvgl_local_build "${CMAKE_SOURCE_DIR}/../build")
endif()

if(_lvgl_local_build)
	set(LVGL_INCLUDE_DIRS
		"${CMAKE_SOURCE_DIR}/../lvgl"
		"${CMAKE_SOURCE_DIR}/../lvgl/examples"
		"${_lvgl_local_build}")
	set(LVGL_LIBRARIES "${_lvgl_local_build}/lvgl/lib/liblvgl.a")
	if(EXISTS "${_lvgl_local_build}/lvgl/lib/liblvgl_examples.a")
		list(APPEND LVGL_LIBRARIES "${_lvgl_local_build}/lvgl/lib/liblvgl_examples.a")
	endif()
else()
	# Fallback to system-installed LVGL via pkg-config
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(LVGL REQUIRED lvgl)
endif()


find_package(PkgConfig REQUIRED)

# Find GStreamer packages
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-1.0)
pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)

# SDL dependencies
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(main main.c)
target_include_directories(main PUBLIC 
	${LVGL_INCLUDE_DIRS} 
	${GSTREAMER_INCLUDE_DIRS} 
	${GSTREAMER_VIDEO_INCLUDE_DIRS} 
	${GSTREAMER_APP_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS})
target_link_libraries(main PUBLIC 
	${LVGL_LIBRARIES} 
	${GSTREAMER_LIBRARIES} 
	${GSTREAMER_VIDEO_LIBRARIES} 
	${GSTREAMER_APP_LIBRARIES}
	${SDL2_LIBRARIES}
	m stdc++)

add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main DEPENDS main)
