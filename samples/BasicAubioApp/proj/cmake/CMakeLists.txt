cmake_minimum_required( VERSION 3.2 FATAL_ERROR )
set( CMAKE_VERBOSE_MAKEFILE ON )
set( CMAKE_CXX_STANDARD 17 )

project( AubioInputAnalyzerApp )

get_filename_component( CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../work/Cinder" ABSOLUTE )
get_filename_component( APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE )
get_filename_component( ARG_ASSETS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../assets" ABSOLUTE )

include( "${CINDER_PATH}/proj/cmake/modules/cinderMakeApp.cmake" )

set( SRC_FILES
	${APP_PATH}/src/AubioInputAnalyzerApp.cpp
	${CINDER_PATH}/samples/_audio/common/AudioDrawUtils.h
	${CINDER_PATH}/samples/_audio/common/AudioDrawUtils.cpp
)

set( RESOURCE_FILES

)

function(makeGroups fileList)
	foreach(FILE ${fileList}) 
	    # Get the directory of the source file
	    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
	    # Remove common directory prefix to make the group
	    string(REPLACE "${APP_PATH}" "" GROUP "${PARENT_DIR}")
	    # Make sure we are using windows slashes
	    string(REPLACE "/" "\\" GROUP "${GROUP}")
	    source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endfunction()

makeGroups("${SRC_FILES}")
makeGroups("${RESOURCE_FILES}")

set( INC_PATHS
	${APP_PATH}
	${APP_PATH}/src
	${APP_PATH}/AudioBeatAnalyzer/src
	${CINDER_PATH}/samples/_audio/common
)

set( BLOCK_FILES
    ../../../../../Cinder-Aubio
)

if( APPLE )
	FIND_LIBRARY(COCOA_LIBRARY Cocoa)

	set( LIB_FILES
		${COCOA_LIBRARY}
	)

	if( TARGET_OS_IPHONE OR TARGET_IPHONE_SIMULATOR )
		find_library(UIKIT UIKit)
		if(NOT UIKIT)
			message(FATAL_ERROR "UIKit library not found")
		endif()
		set( LIB_FILES
			"-framework UIKit"
		)
	endif()
endif( APPLE )

ci_make_app(
    INCLUDES    ${INC_PATHS}
	SOURCES     ${SRC_FILES}
	LIBRARIES   ${LIB_FILES}
	BLOCKS      ${BLOCK_FILES}
	RESOURCES   ${RESOURCE_FILES}
	CINDER_PATH ${CINDER_PATH}
)
